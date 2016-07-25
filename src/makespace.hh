#include "header.hh"


RooArgSet* empty = new RooArgSet();
RooWorkspace* w_empty = new RooWorkspace();
double value_emtpy[9] = {0};

void NPmaker(const char* varname, double setup[5], RooArgSet *&nuispara=empty, RooArgSet *&constraints=empty , RooArgSet *&globobs=empty, RooArgSet *&expected = empty);

void signalPdfBuilder(RooWorkspace *&w, vector<double> value, const char* procname ="default");

void backgroundPdfBuilder(RooWorkspace *&w, int model ,RooArgSet *&nuispara, TString channelname);

RooWorkspace* GenerateSingleChannel( TString channelname/*, TString inputfile*/)
{

  RooWorkspace* w = new RooWorkspace(channelname);

  ifstream file_to_read("input_"+channelname+(TString)".txt");

  if(file_to_read.is_open() != 1 )
    {
      cout << "The input file " << "input_"<<channelname<<".txt" << " does not exist." << endl;
      exit(1);
    }

  //  vector<string> name ;
  //  vector<double> value ;
  map<string,double> input ;

  while(!file_to_read.eof())
    {
      string tmp_name ;
      double tmp_value ;
      file_to_read >> tmp_name >> tmp_value ;
      input[tmp_name] = tmp_value ;
      cout << tmp_name << " " <<   tmp_value << endl;
      //      name.push_back( tmp_name ) ;
      //value.push_back( tmp_value ) ; 
    }


  RooArgSet*nuispara  = new RooArgSet();
  RooArgSet*constraints  = new RooArgSet();
  RooArgSet*globobs  = new RooArgSet();
  RooArgSet*expected_sig  = new RooArgSet();
  RooArgSet*expected_bkg  = new RooArgSet();

  // array setup[5] is used to set up the configuration of a nuisance parameter                                                                                                                      
  // [0]    [1]       [2]   [3]     [4]                                                                                                                                                              
  // sigma, sigmalow, beta, nominal, nonATLAS    
  // if signmalow is nonzero, then set up a nuisance parameter with a logNormal constraint, per Statistics Forum's recommendation


  double setup_lumi[5] = { input["lumi"] , 0 , 1 , 1 , 1 } ;
  NPmaker("lumi", setup_lumi,*&nuispara , *&constraints , *&globobs, *&expected_sig);

  double setup_JESNP1[5] = { input["JESNP1"] , 0 , 1 , 1 , 1 } ;                                                                                                                                           
  NPmaker("JESNP1", setup_JESNP1,*&nuispara , *&constraints , *&globobs, *&expected_sig);    

  /*
  double setup_JESNP1[5] = { input["JESNP1"] , 0 , 1 , 1 , 1 } ;
  NPmaker("JESNP1", setup_JESNP1,*&nuispara , *&constraints , *&globobs, *&expected_sig);

  double setup_JESNP2[5] = { input["JESNP2"] , 0 , 1 , 1 , 1 } ;
  NPmaker("JESNP2", setup_JESNP2,*&nuispara , *&constraints , *&globobs, *&expected_sig);

  double setup_JESNP3[5] = { input["JESNP3"] , 0 , 1 , 1 , 1 } ;
  NPmaker("JESNP3", setup_JESNP3,*&nuispara , *&constraints , *&globobs, *&expected_sig);
  */

  double setup_bkgextrap[5] = { input["bkgextrap"] ,  input["bkgextrap"] , 1 , 1 , 1 } ; // asymmetric uncertainty implemented with lognormal constraint
  NPmaker("bkgextrap", setup_bkgextrap,*&nuispara , *&constraints , *&globobs, *&expected_bkg);

  double setup_bkgextrap_MC[5] = { input["bkgextrapMC"] , input["bkgextrapMC"] , 1 , 1 , 1 } ; // asymmetric uncertainty implemented with lognormal constraint
  NPmaker("bkgextrapMC", setup_bkgextrap_MC,*&nuispara , *&constraints , *&globobs, *&expected_bkg);

  // =========================== Declare POIs ================================                                                                                                                        
  RooRealVar *mu = new RooRealVar("mu","mu",1,-300,300);
  expected_sig->add(RooArgSet(*mu));

  // =========================== Signal expectation ===========================

  RooRealVar *nsig = new RooRealVar("nsig","nsig",input["nsig"]);
  expected_sig->add(RooArgSet(*nsig));

  RooProduct expectation_signal("expectation_signal","expectation_signal",*expected_sig);


  // =========================== Background expectation ===========================

  RooRealVar *nbkg = new RooRealVar("nbkg","nbkg",input["nbkg"]);
  expected_bkg->add(RooArgSet(*nbkg));

  RooProduct expectation_background("expectation_background","expectation_background",*expected_bkg);


  // =========================== Total expectation ===========================
  RooAddition expectation("expectation","expectation",RooArgSet(expectation_signal,expectation_background));
  w->import(expectation);


  w->factory("Nobs[0,300]");
  w->defineSet("Observables","Nobs");
  w->var("Nobs")->setVal(input["Nobs"]);
  w->var("Nobs")->setConstant();

  w->factory("RooPoisson::modelSB(Nobs,expectation)");
  w->Print();

  if(channelname.Contains("multijet")){
    RooProdPdf constraint("constraint","constraint",*constraints);
    w->import(constraint);
    w->factory("PROD::model(modelSB,constraint)"); }

  // Specify parameters that will not be renamed.
  TString correlated = "mu";
  TIterator *iter_nui = nuispara -> createIterator();
  RooRealVar* parg_nui = NULL;
  while((parg_nui=(RooRealVar*)iter_nui->Next()) )
    {
      cout << parg_nui->GetName() << endl;
      correlated = correlated +","+parg_nui->GetName()+",RNDM_"+parg_nui->GetName();
    }
  cout << " For channel " << channelname << " the following variables will not be renamed : " << correlated << endl;

  RooWorkspace* wchannel = new RooWorkspace("wchannel"+channelname);
  wchannel -> import( (*w->pdf("model")) , RenameAllNodes(channelname), RenameAllVariablesExcept(channelname,correlated), Silence());


  // ************** NP : adding nuisance parameter declared in workspace w to wchannel 

  RooArgSet* nuisance_wchannel = new RooArgSet();

  iter_nui->Reset();
  cout << " Adding correlated nuisance parameters to nuisanceParameters RooArgSet"<< endl;

  while((parg_nui=(RooRealVar*)iter_nui->Next()) )
    {
      cout <<" Adding variable : " << parg_nui->GetName() << endl;
      cout << (bool)wchannel->obj(parg_nui->GetName())<< endl;
      nuisance_wchannel -> add( *(RooRealVar*)wchannel->obj(parg_nui->GetName()) );
    }

  // Global observables:
  // Global observables only appear in the constraint terms.                                                                                                                                          
  // All constraint terms of correlated nuisance parameters are attached to the pdf of the first subchannel.                                                                                         
  // For those global observables, their names should be the same as those in the w.                                                                                                                 
  // For other subchannels, only the bias constraint term is attached.     

  RooArgSet* global_wchannel = new RooArgSet();
  TIterator *iter_global = globobs -> createIterator();
  RooRealVar* parg_global ;
  while((parg_global=(RooRealVar*)iter_global->Next()) )//&& (channelname==channel_constraints_attached) )                                                                                             
    {
      TString name_of_global = parg_global->GetName()+(TString)"_"+channelname ;
      cout << " Channel Name " << channelname << " getting global observable " << parg_global->GetName() << endl;

      if((bool)wchannel->obj(name_of_global)==true){
        global_wchannel -> add( *(RooRealVar*)wchannel->obj(name_of_global) );
        wchannel->var(name_of_global)->setConstant();
      } // below is to add global observables that are not renamed
      else if((bool)wchannel->obj(parg_global->GetName())==true){
        global_wchannel -> add( *(RooRealVar*)wchannel->obj(parg_global->GetName()) );
        wchannel->var(parg_global->GetName())->setConstant();
      }
    }


  RooArgSet* observable_wchannel = new RooArgSet();
  TIterator *iter_observable = w->set("Observables") -> createIterator();
  RooRealVar* parg_observable ;
  while((parg_observable=(RooRealVar*)iter_observable->Next()) )
    {
      TString name_of_observable = parg_observable->GetName()+(TString)"_"+channelname;
      if((bool)wchannel->obj(name_of_observable)==true)
        observable_wchannel -> add( *(RooRealVar*)wchannel->obj(name_of_observable) );
      else
        observable_wchannel -> add( *(RooRealVar*)wchannel->obj(parg_observable->GetName()) );
      // Why did I have "else" here?                                                                                                                                                                 
    }

  wchannel -> defineSet("Observables",*observable_wchannel);
  wchannel -> defineSet("nuisanceParameters",*nuisance_wchannel);
  wchannel -> defineSet("globalObservables",*global_wchannel);


  RooDataSet* obsdata = new RooDataSet("obsdata","obsdata",*observable_wchannel);
  obsdata->add( RooArgSet(*wchannel->var("Nobs_"+channelname)),1);
  wchannel ->import(*obsdata);
  RooDataSet* AsimovB = new RooDataSet("AsimovB","AsimovB",*observable_wchannel);
  wchannel->var("Nobs_"+channelname)->setVal( wchannel->var("nbkg_"+channelname)->getVal() );
  AsimovB->add( RooArgSet(*wchannel->var("Nobs_"+channelname)),1);
  wchannel ->import(*AsimovB);
  return wchannel;

}

void NPmaker(const char* varname, double setup[5] , RooArgSet *&nuispara, RooArgSet *&constraints , RooArgSet *&globobs, RooArgSet *&expected )
{
    double sigma = setup[0] ;
    double sigmalow = setup[1] ;
    double beta = setup[2] ;
    double nominal = setup[3] ;
    double nonATLAS = setup[4];
    
    RooWorkspace* w = new RooWorkspace(varname);
    if(sigmalow > 0 ) {
        cout << " Set up nuisance parameter for an asymmetric uncertainty " << endl;
        
        RooRealVar* var = new RooRealVar(varname,varname,0,-5,5);
        if( nonATLAS != 0 ){
            TString atlasNPname = (TString)"atlas_"+varname;
            var->SetName(atlasNPname);
            var->SetTitle(atlasNPname); }
        RooRealVar* beta_var = new RooRealVar((TString)"beta_"+varname,(TString)"beta_"+varname,beta);
        RooProduct* var_times_beta = new RooProduct(varname+(TString)"_times_beta",varname+(TString)"_times_beta",RooArgSet(*var,*beta_var));
        vector<double> sigma_var_high, sigma_var_low;
        sigma_var_high.push_back( 1+sigma );
        sigma_var_low.push_back( 1-sigmalow );
        RooArgList nuiList(*var_times_beta);
        RooStats::HistFactory::FlexibleInterpVar atlas_expected_var("atlas_expected_"+(TString)varname,"atlas_expected_"+(TString)varname,nuiList,nominal,sigma_var_low,sigma_var_high);
        w->import(atlas_expected_var);
        if( nonATLAS == 0 ){
            cout << " Nuisance parameter is shared between ATLAS and CMS " << endl;
            w->factory((TString)"RooGaussian::atlas_nui_"+(TString)varname+(TString)"(RNDM_"+(TString)varname+(TString)"[0,-5,5],"+(TString)varname+(TString)",1)");
        }
        else{
            w->factory((TString)"RooGaussian::atlas_nui_"+(TString)varname+(TString)"(RNDM_atlas_"+(TString)varname+(TString)"[0,-5,5],atlas_"+(TString)varname+(TString)",1)");
        }
    }
    else if( sigmalow == -999 )
    {
        cout << " Set up nuisance parameter with a Gaussian constraint term, parameter name : " << varname << endl;
        char sigma_value[9];  sprintf(sigma_value, "%4.9f", sigma);
        char beta_value[9];  sprintf(beta_value, "%4.9f", beta);
        char nominal_value[9];  sprintf(nominal_value, "%4.9f", nominal);
        w->factory((TString)"sum::atlas_expected_"+(TString)varname+(TString)"(nominal_"+(TString)varname+"["+nominal_value+(TString)"] , prod::uncer_"+(TString)varname+(TString)"( prod::"+varname+(TString)"_times_beta(atlas_"+(TString)varname+(TString)"[ 0 , -5 , 5 ] ,beta_"+varname+(TString)"["+beta_value+(TString)"]), sigma_"+(TString)varname+(TString)"["+sigma_value+(TString)" ]))");
        w->factory("RooGaussian::atlas_nui_"+(TString)varname+(TString)"(RNDM_atlas_"+(TString)varname+(TString)"[0,-5,5],atlas_"+(TString)varname+(TString)",1)");
        
    }
    else if( sigmalow<0 && sigmalow != -999)
    {
        char beta_value[9];  sprintf(beta_value, "%4.9f", beta);
        char log_kappa_value[9];  sprintf(log_kappa_value, "%4.9f", sqrt( log( 1+pow(sigma,2)) ) );
        char nominal_value[9];  sprintf(nominal_value, "%4.9f", nominal );
        char avalue[9];  sprintf(avalue, "%4.9f", fabs(sigma/sigmalow) );
        
        cout << " Set up nuisance parameter with a Bifuricated Gaussian constraint term, parameter name : " << varname << endl;
        cout << " The asymmetric factor is " << avalue<< endl;
        w->factory((TString)"atlas_log_kappa_value_"+(TString)varname+"["+(TString)log_kappa_value+(TString)"]") ;
        if( nonATLAS == 0 ){
            w->factory("RooExponential::atlas_expTerm_"+(TString)varname+"(prod::"+varname+(TString)"_times_beta("+(TString)varname+(TString)"[ 0 , -5 , 5 ], beta_"+varname+(TString)"["+beta_value+(TString)"]),atlas_log_kappa_value_"+(TString)varname+")");}
        else
            w->factory("RooExponential::atlas_expTerm_"+(TString)varname+"(prod::"+varname+(TString)"_times_beta(atlas_"+(TString)varname+(TString)"[ 0 , -5 , 5 ], beta_"+varname+(TString)"["+beta_value+(TString)"]),atlas_log_kappa_value_"+(TString)varname+")");
        
        w->factory((TString)"prod::atlas_expected_"+(TString)varname+"(atlas_expTerm_"+(TString)varname+",nominal_"+(TString)varname+"["+(TString)nominal_value+(TString)"])");
        if( nonATLAS == 0 ){
            w->factory((TString)"RooBifurGauss::atlas_nui_"+varname+(TString)"(RNDM_"+varname+(TString)"[0,-5,5],"+varname+(TString)",1,"+avalue+(TString)")");
        }
        else
            w->factory((TString)"RooBifurGauss::atlas_nui_"+varname+(TString)"(RNDM_atlas_"+varname+(TString)"[0,-5,5],"+varname+(TString)",1,"+avalue+(TString)")");
        
    }
    
    
    else
    {
        cout << " Set up a nuisance parameter with a logNormal constraint, varname: "<< varname << endl;
        char beta_value[9];  sprintf(beta_value, "%4.9f", beta);
        char log_kappa_value[9];  sprintf(log_kappa_value, "%4.9f", sqrt( log( 1+pow(sigma,2)) ) );
        char nominal_value[9];  sprintf(nominal_value, "%4.9f", nominal );
        w->factory((TString)"atlas_log_kappa_value_"+(TString)varname+"["+(TString)log_kappa_value+(TString)"]") ;
        if( nonATLAS != 0 )
            w->factory("RooExponential::atlas_expTerm_"+(TString)varname+"(prod::"+varname+(TString)"_times_beta(atlas_"+(TString)varname+(TString)"[ 0 , -5 , 5 ], beta_"+varname+(TString)"["+beta_value+(TString)"]),atlas_log_kappa_value_"+(TString)varname+")");
        else if( nonATLAS == 0 )
            w->factory("RooExponential::atlas_expTerm_"+(TString)varname+"(prod::"+varname+(TString)"_times_beta("+varname+(TString)"[ 0 , -5 , 5 ], beta_"+varname+(TString)"["+beta_value+(TString)"]),atlas_log_kappa_value_"+(TString)varname+")");
        w->factory((TString)"prod::atlas_expected_"+(TString)varname+"(atlas_expTerm_"+(TString)varname+",nominal_"+(TString)varname+"["+(TString)nominal_value+(TString)"])");
        if( nonATLAS != 0 )
            w->factory("RooGaussian::atlas_nui_"+(TString)varname+"(RNDM_atlas_"+(TString)varname+"[0,-5,5],atlas_"+(TString)varname+",1)");
        else{
            cout << " Set up constraint term for " << varname << endl;
            w->factory("RooGaussian::atlas_nui_"+(TString)varname+"(RNDM_"+(TString)varname+"[0,-5,5],"+(TString)varname+",1)");
            
        }
    }
    
    
    if( nonATLAS == 0 )
        nuispara->add(*w->var(varname));
    else
        nuispara->add(*w->var("atlas_"+(TString)varname));
    cout << " Now, adding constraint term " << "atlas_nui_"<<varname<< endl;
    constraints->add(*w->pdf("atlas_nui_"+(TString)varname));
    
    if(nonATLAS ==0 )
        globobs->add(*w->var("RNDM_"+(TString)varname));
    else
        globobs->add(*w->var("RNDM_atlas_"+(TString)varname));
    
    expected->add(*w->function("atlas_expected_"+(TString)varname));
}

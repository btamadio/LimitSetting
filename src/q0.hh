#include "header.hh"
using namespace std;
using namespace RooFit;
using namespace RooStats;

#include <sys/stat.h>

map<TString,double> generateToy( RooWorkspace* w , int seed , double mu_of_toy, double mu_of_hypothesis)
{

  RooSimultaneous* combPdf =  (RooSimultaneous*)w->pdf("CombinedPdf") ;
  RooArgSet* nuisanceParameters = (RooArgSet*)w->set("nuisanceParameters"); 
  RooArgSet* globalObservables = (RooArgSet*)w->set("globalObservables");
  RooArgSet* Observables = (RooArgSet*)w->set("Observables"); 

  // Fix nuisance parameters to the profiled values before randomize global observables.



    w->var("mu")->setVal(mu_of_toy);
    w->var("mu")->setConstant();

  TRandom3 *gRandom = new TRandom3() ;                                                                                                                    
  gRandom -> SetSeed(seed) ;                                                                                                                               
  gRandom -> Rndm() ;                                                                                                                                   
  RooRandom::randomGenerator() -> SetSeed(seed) ;                                                                                                          

  const RooArgSet* nuisanceParameters_original_values = (RooArgSet*)w->set("nuisanceParameters")->Clone();
  //  nuisanceParameters_original_values->Print("v");
  vector<double>NPvalue ;

  TIterator *iter_global_release0 = globalObservables -> createIterator();
  RooRealVar* parg_global_release0 = NULL;
  TIterator *iter_global_release02 = w->set("globalObservables") -> createIterator();
  RooRealVar* parg_global_release02 = NULL;

  double expectedSB = w->function("expectation_multijet")->getVal(); 

  while((parg_global_release0=(RooRealVar*)iter_global_release0->Next()) )
    { 
      parg_global_release02=(RooRealVar*)iter_global_release02->Next();
      double r =  gRandom->Gaus(0,1);
      if( r < -5) r = -5;
      parg_global_release0->setConstant(kFALSE); 
      parg_global_release02->setConstant(kFALSE); 
      parg_global_release0->setVal(r); 
      parg_global_release02->setVal(r);
      parg_global_release02->setConstant(); 
    }
                 

  map<string,RooDataSet*> toydatasetMap; 

  RooCategory* channellist = new RooCategory("channellist","channellist");
  TIterator* iter =  combPdf->indexCat().typeIterator() ;
  RooCatType* tt = NULL;
  // loop over all channels

  RooRealVar wt("wt","wt",1); 
  double Nobs ;
  while((tt=(RooCatType*) iter->Next())) {
    channellist->defineType(tt->GetName()) ;     
    TString channelname = tt->GetName();
    RooAbsPdf* pdftmp = combPdf->getPdf(tt->GetName()) ;
    RooArgSet* obstmp = pdftmp->getObservables( Observables ) ;  
    RooDataSet* tmpdata = pdftmp->generate(*obstmp,1);
    RooArgSet* obs2 = (RooArgSet*)tmpdata->get() ;
    RooRealVar* obsvalue = (RooRealVar*)obs2->find("Nobs_multijet") ;
    tmpdata->get(1); Nobs=obsvalue->getVal();
    RooArgSet* obs_plus_wt = new RooArgSet();
    obs_plus_wt->add(wt);
    obs_plus_wt->add(*obstmp);
    toydatasetMap[(string)tt->GetName()] =  (RooDataSet*)tmpdata->Clone() ;

    delete obs_plus_wt ;
    cout << " Finishing generating toy data in category " << channelname << endl;

  }



  RooArgSet* args = new RooArgSet();
  args->add(*Observables);
  args->add(wt);
  RooDataSet* toyData = new RooDataSet("toyData", "", *args, Index(*channellist), Import(toydatasetMap), WeightVar(wt));

  // release nuisance parameters before fit.
  TIterator *iter_nui_release = nuisanceParameters -> createIterator();
  RooRealVar* parg_nui_release = NULL;
  while((parg_nui_release=(RooRealVar*)iter_nui_release->Next()) )
    { 
      NPvalue.push_back(parg_nui_release->getVal());
      parg_nui_release->setConstant(kFALSE); }                                                                                                                                                            
  // fix global observables before fit
  TIterator *iter_global = globalObservables -> createIterator();
  RooRealVar* parg_global = NULL;
  while((parg_global=(RooRealVar*)iter_global->Next()) )
    { parg_global->setConstant(); }                                                                                                                                                            

  w->var("mu")->setVal(mu_of_hypothesis);
  w->var("mu")->setConstant();

  RooFitResult *result_datamu_fixed = combPdf->fitTo(*toyData,Constrain(*nuisanceParameters),Save(),PrintLevel(-1));
  double NLL0   = result_datamu_fixed -> minNll() ;


  nuisanceParameters = (RooArgSet*)nuisanceParameters_original_values->snapshot();
  //  cout << "Print out NP values before the 2nd fits" << endl;

  TIterator *iter_nui_release_again = nuisanceParameters -> createIterator();
  TIterator *iter_nui_resetvalue = w->set("nuisanceParameters") -> createIterator();
  RooRealVar* parg_nui_release_again = NULL;
  RooRealVar* parg_nui_resetvalue= NULL;
  int k = 0 ;
  while((parg_nui_release_again=(RooRealVar*)iter_nui_release_again->Next()) )
    { 
      parg_nui_resetvalue=(RooRealVar*)iter_nui_resetvalue->Next();
      parg_nui_resetvalue->setVal(NPvalue[k]);
      parg_nui_release_again->setConstant(kFALSE); 
      k ++;
    }

  /*
  nuisanceParameters->Print("v");
  nuisanceParameters_original_values->Print("v");
  */

  w->var("mu")->setVal(1);
  w->var("mu")->setConstant(kFALSE);

  RooFitResult *result_datamu_free = combPdf->fitTo(*toyData,Constrain(*nuisanceParameters),Save(),PrintLevel(-1));
  double NLL2   = result_datamu_free -> minNll() ;
  double muhat = (*w->var("mu")).getVal(); 


  k=0;
  TIterator *iter_nui_release3 = nuisanceParameters -> createIterator();
  TIterator *iter_nui_resetvalue3 = w->set("nuisanceParameters") -> createIterator();
  RooRealVar* parg_nui_release3 = NULL;
  RooRealVar* parg_nui_resetvalue3 = NULL;
  while((parg_nui_release3=(RooRealVar*)iter_nui_release3->Next()) )
    { 
      parg_nui_resetvalue3=(RooRealVar*)iter_nui_resetvalue3->Next();
      parg_nui_resetvalue3->setVal(NPvalue[k]);
      k++;
    }



  delete gRandom ;   
  delete args ; delete  toyData;   delete  result_datamu_fixed;
  delete channellist;



  map<TString,double> result ;
  result["NLL0"] = NLL0 ;
  result["NLL2"] = NLL2 ;
  result["muhat"] = muhat ;
  result["Nobs"] = Nobs ;
  result["Nexp"] = expectedSB ;
  //  delete w;
  //  Observables->Print"("v");
  return result ;
}



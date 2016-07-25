#include "toy.hh"

int main( int argc , char **argv)
{
  /*
  if( argc != 5 )
    {
      cout << " Input arguments are wrong " << endl;
      cout << "./toy mu_of_toy mu_of_hypothesis start Ntoy" << endl;
      exit(-1);
    }

  double mu_of_toy = atof(argv[1]);
  double mu_of_hypothesis =  atof(argv[2]) ;
  int start = atoi(argv[3]);
  int Ntoy = atoi(argv[4]);
  */

  TFile f("combinedWS.root");
  RooWorkspace *w = (RooWorkspace*)f.Get("combination");
  RooArgSet* nuisanceParameters = (RooArgSet*)w->set("nuisanceParameters");
  RooArgSet* globalObservables = (RooArgSet*)w->set("globalObservables");
  RooArgSet* Observables = (RooArgSet*)w->set("Observables");
  RooSimultaneous* combPdf =  (RooSimultaneous*)w->pdf("CombinedPdf") ;


  ofstream outputfile("output2.txt");

  double mu_obs_limit = -9 ;
  double mu_expected_CLSB = - 9;



   for( int i = 20 ; i < 3001 ; i ++ )
    {

      double mu = 0.1*i ;

      /*The first fit is corresponding to the numerator of Profile Liklihood Ratio (PLR)
        This is a mu-conditional fit, i.e., mu is fixed to the hypothetic value
      */
      w->var("mu")->setVal(mu);
      w->var("mu")->setConstant(kTRUE);
      

      /*Before a fit, set all NPs free and set their initial values to 0*/
      TIterator *iter_nui_release = nuisanceParameters -> createIterator();
      RooRealVar* parg_nui_release = NULL;
      while((parg_nui_release=(RooRealVar*)iter_nui_release->Next()) )
	{ parg_nui_release->setConstant(kFALSE); 
	  parg_nui_release->setVal(0);
	}

      RooFitResult *result_mu = w->pdf("CombinedPdf")->fitTo(*w->data("obsData"),Constrain(*nuisanceParameters),Save(),PrintLevel(1));
      double NLL_mu = result_mu-> minNll() ; // This is the Maximum Likelihood Estimator (MLE) of the mu conditional fit.
      delete result_mu;

      /*The second fit is corresponding to the denominator of the PLR*/
      w->var("mu")->setVal(0);
      w->var("mu")->setConstant(kFALSE);
      /*Reset all NPs again*/
      iter_nui_release = nuisanceParameters -> createIterator();
      parg_nui_release = NULL;
      while((parg_nui_release=(RooRealVar*)iter_nui_release->Next()) )
	{ parg_nui_release->setConstant(kFALSE); 
	  parg_nui_release->setVal(0);
	}


      RooFitResult *result_muhat = w->pdf("CombinedPdf")->fitTo(*w->data("obsData"),Constrain(*nuisanceParameters),Save(),PrintLevel(1));
      double NLL_muhat = result_muhat-> minNll() ; // MLE of an unconditional fit of mu
      delete result_muhat;

      double qmu = 2*(NLL_mu-NLL_muhat);

      /*Now, calculating qmu,A by setting the observed number of events to the expected value*/
      /*in this case, this dataset is a B-only Asimov data*/



      w->var("mu")->setVal(mu);
      w->var("mu")->setConstant(kTRUE);
      iter_nui_release = nuisanceParameters -> createIterator();
      parg_nui_release = NULL;
      while((parg_nui_release=(RooRealVar*)iter_nui_release->Next()) )
        { parg_nui_release->setConstant(kFALSE);
          parg_nui_release->setVal(0);
        }
      RooFitResult *result_muA = w->pdf("CombinedPdf")->fitTo(*w->data("AsimovB"),Constrain(*nuisanceParameters),Save(),PrintLevel(1));
      double NLL_muA = result_muA-> minNll() ; // This is the Maximum Likelihood Estimator (MLE) of the mu conditional fit.
      delete result_muA;

      w->var("mu")->setVal(0);
      w->var("mu")->setConstant(kFALSE);

      iter_nui_release = nuisanceParameters -> createIterator();
      parg_nui_release = NULL;
      while((parg_nui_release=(RooRealVar*)iter_nui_release->Next()) )
        { parg_nui_release->setConstant(kFALSE);
          parg_nui_release->setVal(0);
        }
      RooFitResult *result_muhatA = w->pdf("CombinedPdf")->fitTo(*w->data("AsimovB"),Constrain(*nuisanceParameters),Save(),PrintLevel(1));
      double NLL_muhatA = result_muhatA-> minNll() ; // MLE of an unconditional fit of mu
      delete result_muhatA;

      double qmuA = 2*(NLL_muA-NLL_muhatA);
     
      /*Now calculate the observed CLs using Eq. 12 
        of https://twiki.cern.ch/twiki/pub/AtlasProtected/StatisticsTools/Frequentist_Limit_Recommendation.pdf*/
      double cls = (1 - ROOT::Math::normal_cdf(sqrt(qmu),1) )/ ROOT::Math::normal_cdf(sqrt(qmuA) - sqrt(qmu),1);

      if( cls <= 0.05 && mu_obs_limit < 0 ){ mu_obs_limit = mu ; cout << "Found the observed limit mu < " << mu << " with CLs = " << cls << endl; };

      /*Calculate the CLs+b. When qmuA = 1.64, the mu value is the expected limit with **CLs+b** , see Eq 8 of the reference */
      double clsb_median = sqrt(qmuA);
      if( clsb_median >= 1.64 && mu_expected_CLSB < 0 ) { mu_expected_CLSB = mu ; cout << " Found the expected CLsb limit mu < " << mu << " with clsb_median = " << clsb_median << endl;}

      outputfile << mu << " " << qmuA  << " " << qmu  << " " << cls <<  " " <<        w->var("Nobs_multijet")->getVal() <<endl;
    }

   w->Print("v");
   /*Now calculate sigma using Eq 14*/
   double sigma = mu_expected_CLSB / 1.64 ;

   for( int N = -2 ; N < 3 ; N ++ )
     {
       cout << sigma*(ROOT::Math::gaussian_quantile(1-(1-0.95)*ROOT::Math::normal_cdf(N,1),1)+N)  << " ";
     }
   cout << endl;
   cout << mu_obs_limit << " " << mu_expected_CLSB << endl;


  return 0 ;

}




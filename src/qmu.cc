#include "toy.hh"

int main( int argc , char **argv)
{
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

  TFile f("combinedWS.root");
  RooWorkspace *w = (RooWorkspace*)f.Get("combination");
  RooArgSet* nuisanceParameters = (RooArgSet*)w->set("nuisanceParameters");
  RooArgSet* globalObservables = (RooArgSet*)w->set("globalObservables");
  RooArgSet* Observables = (RooArgSet*)w->set("Observables");
  RooSimultaneous* combPdf =  (RooSimultaneous*)w->pdf("CombinedPdf") ;

  ofstream outputfile("output.txt");

   for( int i = 0 ; i < 3001 ; i ++ )
    {

      double mu = 0.1*i ;


      w->var("mu")->setVal(mu);
      w->var("mu")->setConstant(kTRUE);

      w->var("atlas_JESNP1")->setVal(0);
      w->var("atlas_lumi")->setVal(0);
      w->var("atlas_bkgextrap")->setVal(0);

      w->var("atlas_JESNP1")->setConstant(kFALSE);
      w->var("atlas_lumi")->setConstant(kFALSE);
      w->var("atlas_bkgextrap")->setConstant(kFALSE);

      RooFitResult *result_mu = w->pdf("CombinedPdf")->fitTo(*w->data("obsData"),Constrain(*nuisanceParameters),Save(),PrintLevel(1));

      delete result_mu;
      double NLL_mu = result_mu-> minNll() ;

      w->var("mu")->setVal(0);
      w->var("mu")->setConstant(kFALSE);
      w->var("atlas_JESNP1")->setVal(0);
      w->var("atlas_lumi")->setVal(0);
      w->var("atlas_bkgextrap")->setVal(0);
      w->var("atlas_JESNP1")->setConstant(kFALSE);
      w->var("atlas_lumi")->setConstant(kFALSE);
      w->var("atlas_bkgextrap")->setConstant(kFALSE);

      RooFitResult *result_muhat = w->pdf("CombinedPdf")->fitTo(*w->data("obsData"),Constrain(*nuisanceParameters),Save(),PrintLevel(1));

      double NLL_muhat = result_muhat-> minNll() ;
      delete result_muhat;
      outputfile << mu << " " <<  2*(NLL_mu-NLL_muhat)<< endl;
     
    }

 w->data("obsData")->Print("v");


  return 0 ;

}




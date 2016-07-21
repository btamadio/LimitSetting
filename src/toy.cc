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

  char fname[50];
  sprintf(fname,"outcome_muSB_\%3.1f_mu_\%3.1ftoy_%d.root",mu_of_hypothesis,mu_of_toy,start);

  TFile f("combinedWS.root");
  RooWorkspace *w = (RooWorkspace*)f.Get("combination");

  TFile fOuputFile(fname,"recreate") ;
  TTree fOutputTree("likelihoodtree","likelihoodtree") ;

  double NLL0 , NLL2 ;
  double mutoy = mu_of_toy , muhypo = mu_of_hypothesis, muhat;
  double DNLL;
  double Nobs, Nexp ;
  vector<double> NPvalue0 ;
  vector<string> NPname0 ;

  vector<double> NPvalue2 ;
  vector<string> NPname2 ;

  fOutputTree.Branch("NLL0", &NLL0 , "NLL0/D") ;
  fOutputTree.Branch("NLL2", &NLL2 , "NLL2/D") ;
  fOutputTree.Branch("Nobs", &Nobs , "Nobs/D") ;
  fOutputTree.Branch("Nexp", &Nexp , "Nexp/D") ;
  fOutputTree.Branch("mutoy", &mutoy , "mutoy/D") ;
  fOutputTree.Branch("muhypo", &muhypo , "muhypo/D") ;
  fOutputTree.Branch("muhat", &muhat, "muhat/D") ;
  fOutputTree.Branch("DNLL", &DNLL , "DNLL/D") ;



   for( int i = start*Ntoy ; i < (start+1)*Ntoy ; i ++ )
    {

      map<TString,double> result = generateToy(w,i, mu_of_toy, mu_of_hypothesis ) ;

      NLL0 = result["NLL0"] ; 
      NLL2 = result["NLL2"] ;
      muhat = result["muhat"];
      Nobs=result["Nobs"];
      Nexp=result["Nexp"];
      DNLL = 2*(NLL0-NLL2) ;
      if( muhat > mu_of_hypothesis && mu_of_hypothesis >0) DNLL = 0;

      fOutputTree.Fill() ;
     
    }
  fOutputTree.Write() ;


  return 0 ;

}




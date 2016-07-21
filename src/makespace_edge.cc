#include "makespace_edge.hh"

// This is the first version of workspace after the background model is settled.
// The script generates a workspace where the observed dataset, the B-only asimov dataset
// and S+B asimov dataset are saved. 

int main( int argc , char **argv)
{



  TString CN[10] = {"bin1","bin2","bin3","bin4","bin5","bin6","bin7","bin8","bin9","bin10"};

  vector<string> catName;
  const int Nch = 10;
  RooWorkspace* w[Nch];
  RooCategory* channellist = new RooCategory("channellist","channellist");
  RooWorkspace* combination = new RooWorkspace("combination");
  combination->importClassCode();

  RooSimultaneous CombinedPdf("CombinedPdf","",*channellist) ;

  RooArgSet* nuisanceParameters = new RooArgSet();
  RooArgSet* globalObservables = new RooArgSet();
  RooArgSet* Observables = new RooArgSet();
  //  RooArgSet* constraints = new RooArgSet();

  map<string,RooDataSet*> datasetMap;

 
  for( int i = 0 ; i < Nch; i ++ )
    {
      w[i] = GenerateSingleChannel(CN[i]);
      cout << 1 << endl;
      cout << "To define type " << CN[i] << endl;
      channellist -> defineType(CN[i]) ;
      cout << 2 << endl;
      CombinedPdf.addPdf(*w[i]->pdf("model_"+CN[i]),CN[i]) ;
      nuisanceParameters->add( *w[i]->set("nuisanceParameters"));
      globalObservables->add(*w[i]->set("globalObservables"));
      Observables->add(*w[i]->set("Observables"));
      catName.push_back((string)CN[i]);
      cout << 5 << endl;
      datasetMap[catName[i]] = (RooDataSet*)w[i]->data("obsdata");
      cout << 6 << endl;
    }

  cout <<"finished the loop "<< endl;




  //  constraints->Print("v");
    combination -> import(CombinedPdf);
      cout << 6.1 << endl;
    combination -> defineSet("nuisanceParameters",*nuisanceParameters);
    combination -> defineSet("Observables",*Observables);
    combination -> defineSet("globalObservables",*globalObservables);
    combination -> defineSet("poi",RooArgSet( *w[0]->var("mu") ));   

    RooRealVar wt("wt","wt",1);//,0,10000);

    RooArgSet *args = new RooArgSet();
    args->add(*Observables);
    args->add(wt);

    RooDataSet* obsData = new RooDataSet("obsData","combined data ",*args, Index(*channellist), Import(datasetMap) ,WeightVar(wt));
    combination->import(*obsData);

    cout << 7 << endl;
    /*
    ModelConfig *mconfig = new ModelConfig("mconfig",combination);
    mconfig -> SetPdf(*combination->pdf("CombinedPdf"));
    mconfig -> SetObservables( *combination->set("Observables"));
    cout << 8 << endl;
    mconfig -> SetParametersOfInterest( (*combination->set("poi")) );
    mconfig -> SetNuisanceParameters( (*combination->set("nuisanceParameters")) );
    mconfig -> SetGlobalObservables( (*combination->set("globalObservables")) );
    cout << 9 << endl;
    combination -> import(*mconfig);
    */

    combination->writeToFile("combinedWS.root");

    cout << 10 << endl;


  return 0 ;

}





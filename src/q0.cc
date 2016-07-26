/*
 *  hcanalysis.C
 *  
 *
 *  Created by Haichen Wang on 6/6/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "plotting.hh"
//#include "AtlasStyle.h"
 

int main (int argc, char **argv)
{
  gROOT->LoadMacro("~/atlasstyle/AtlasStyle.C");
  //SetAtlasStyle();
  gROOT->ForceStyle();


		 if (argc < 2)
		 {
		    printf("\nUsage: %s *.root\n\n",argv[0]);
		    exit(0);
		 }
		 
		 int const index = argc - 1;
		
		 TString path[index];

		 /* The first argv is always the executable itself */
		 for ( int j = 0; j < argc-1; j++)
		 {
		   //		   cout <<" Add file ..." << endl;
			path[j] = (argv[j+1]);
		 }
		 
		 TChain *chain = new TChain("likelihoodtree");
		 
		 for (int k = 0; k < argc-1 ; k++)
		 {
		    chain -> Add(path[k]); // Add the root file to the TChain chain
		 }

		 // double value = 2*fitting_result(atof(argv[1])); 

		 p = new likelihoodtree(chain);

		 
		 Int_t numev = p -> fChain -> GetEntries(); // numev is the number of Events

		 const int nbin = 20000;
		 const int endpoint = 20 ;
		 //		 double binwidth = endpoint*1.0/nbin;

		 TH1F* h_B = new TH1F("h_B","h_B",2*nbin,-1*endpoint,endpoint);
		 TH1F* h_SB = new TH1F("h_SB","h_SB",nbin,0,endpoint);

                 ifstream file_to_read("output.txt");
                 if(file_to_read.is_open() != 1 )exit(1);
                 map<double,double> obsQmu;
                 while(!file_to_read.eof())
                   {
                     double muvalue , obsvalue ;
                     file_to_read >> muvalue >> obsvalue ;
                     obsQmu[muvalue]=obsvalue;
                   }

                 double obs = obsQmu[0];
		 cout<<"obs = "<<obs<<endl;

		 for( int i = 0 ; i < numev ; i ++)
		   {
		     get_event(i) ;

		     if( p->mutoy > 1 ){
		       h_SB -> Fill( p->DNLL ) ;
		     }

		     if( p->mutoy == 0 ){
		       h_B -> Fill( p->DNLL ) ;
		     }
		   }
		 TH1F* h_B0 = (TH1F*)h_B->Clone();
		 TH1F* h_SB0 = (TH1F*)h_SB->Clone();

		 h_B->Scale( 1/h_B->GetEntries() );
		 h_SB->Scale( 1/h_SB->GetEntries() );


                 double tailB = 1 , tailSB = 1;
                 double pvalue = 0;
                 for( int i = 0 ; i < h_B->GetNbinsX()+1 ; i ++ )
                   {
		     if( h_B->GetBinCenter(i+1) < obs) continue ;
		     if( h_B->GetBinCenter(i+1) >= obs)
		       {
			 pvalue = h_B->Integral(i+1,h_B->GetNbinsX()+1);
			 break;
		       }
		   }

		 cout << "p0 value " <<  pvalue << ", significance " << fabs(ROOT::Math::gaussian_quantile(pvalue,1)) <<   endl;



		 TCanvas* c = new TCanvas("c","c",800,600);
		 c->SetLogy();
		 h_B->Rebin(100);
		 h_SB->Rebin(100);
		 h_B->GetXaxis()->SetTitle("q_{0}");
		 h_B->GetYaxis()->SetTitle("Normalized to unity");

		 h_B->Draw();
		 //		 h_SB->SetLineColor(2);
		 //h_SB->Draw("same");

		 c->SaveAs("Statistics_distribution_of_profilelikelihoodratio_q0_HW.eps");
		 c->SaveAs("Statistics_distribution_of_profilelikelihoodratio_q0_HW.png");

		 return 0 ;


}

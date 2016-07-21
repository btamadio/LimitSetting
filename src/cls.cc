/*
 *  hcanalysis.C
 *  
 *
 *  Created by Haichen Wang on 6/6/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "plotting.hh"

 

int main (int argc, char **argv)
{
  gROOT->ProcessLine(".x rootlogon.C");
  gROOT->SetStyle("ATLAS");
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

		 ifstream file_to_read("output.txt");
		 if(file_to_read.is_open() != 1 )exit(1);
		 map<double,double> obsQmu;
		 while(!file_to_read.eof())
		   {
		     double muvalue , obsvalue ;
		     file_to_read >> muvalue >> obsvalue ;
		     obsQmu[muvalue]=obsvalue;
		   }
		 get_event(1);
		 double obs = obsQmu[p->muhypo];

		 TH1F* h_B = new TH1F("h_B","h_B",nbin,0,endpoint);
		 TH1F* h_SB = new TH1F("h_SB","h_SB",nbin,0,endpoint);


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

		 double tail_plus_2sigma = 1 - 0.0455/2 ;
		 double tail_plus_1sigma = 1 - 0.3173/2;
		 double tail_median = 0.5 ;
		 double tail_minus_1sigma = 0.3173/2;
		 double tail_minus_2sigma = 0.0455/2;

		 double cls_plus_2sigma = 0 , cls_plus_1sigma = 0 , cls_median = 0 , cls_minus_1sigma = 0, cls_minus_2sigma = 0;
		 double PLR_plus_2sigma = 0 , PLR_plus_1sigma = 0 , PLR_median = 0 , PLR_minus_1sigma = 0, PLR_minus_2sigma = 0;

		 double tailB = 1 , tailSB = 1;
		 double cls_obs =1000;
		 for( int i = 0 ; i < nbin+1 ; i ++ )
		   {
		     if(i==1) cout << "underflow " << h_B->GetBinContent(0) << " " << h_SB->GetBinContent(0) << endl;
		     tailB = tailB - h_B->GetBinContent(i);
		     tailSB = tailSB - h_SB->GetBinContent(i);

		     if(h_B->GetBinCenter(i)< obs && h_B->GetBinCenter(i+1)> obs && cls_obs ==1000)
		     {
		       cls_obs = tailSB/tailB;
		     } 
		     //		     cout << i << " "<< h_B->GetBinCenter(i) << " " <<   << tailB<< endl;
		     if(tailB<=tail_plus_2sigma && cls_plus_2sigma == 0 ){
		       cls_plus_2sigma = tailSB/tailB;
		       PLR_plus_2sigma = h_B->GetBinCenter(i);
		     }
		     if(tailB<=tail_plus_1sigma && cls_plus_1sigma == 0 ){
		       cls_plus_1sigma = tailSB/tailB;
                       PLR_plus_1sigma= h_B->GetBinCenter(i);
		     }
		     if(tailB<=tail_median && cls_median == 0 ){
		       cls_median = tailSB/tailB;
                       PLR_median= h_B->GetBinCenter(i);
		     }
		     if(tailB<=tail_minus_2sigma && cls_minus_2sigma == 0 ){
		       cout << h_SB0->Integral(i,nbin) << " " << h_B0->Integral(i,nbin) << endl;
		       cout << h_SB0->Integral(i+1,nbin) << " " << h_B0->Integral(i+1,nbin) << endl;
		       cout <<  tailSB << " " <<  tailB << endl;
		       cls_minus_2sigma = tailSB/tailB;
                       PLR_minus_2sigma= h_B->GetBinCenter(i);
		     }
		     if(tailB<=tail_minus_1sigma && cls_minus_1sigma == 0 ){
		       cls_minus_1sigma = tailSB/tailB;
                       PLR_minus_1sigma= h_B->GetBinCenter(i);
		     }
		   }

		 cout << h_B->GetEntries() << " " << h_SB->GetEntries() <<endl;
		 cout << h_B->GetEntries() << " " << h_SB->GetEntries() <<endl;
		 cout << "PLR " << PLR_plus_2sigma << " " << PLR_plus_1sigma << " " << PLR_median << " " << PLR_minus_1sigma << " " << PLR_minus_2sigma  << endl; 
		 cout << "CLs " << cls_plus_2sigma << " " << cls_plus_1sigma << " " << cls_median << " " << cls_minus_1sigma << " " << cls_minus_2sigma  << endl; 
		 cout << "observed CLs " << cls_obs << endl;
		 TCanvas* c = new TCanvas("c","c",800,600);
		 c->SetLogy();
		 h_B->Rebin(100);
		 h_SB->Rebin(100);
		 h_B->GetXaxis()->SetTitle("q_{#mu}");
		 h_B->GetYaxis()->SetTitle("Normalized to unity");
		 h_B->Draw();

                 TLine * l4=new TLine( PLR_median ,0, PLR_median, 1);
                 l4 -> SetLineWidth(5);
                 l4 -> SetLineColor(1);
                 l4 -> SetLineStyle(1);
                 l4->Draw("same");

		 h_SB->SetLineColor(2);
		 h_SB->Draw("same");

		 c->SaveAs("Statistics_distribution_of_profilelikelihoodratio_HW.eps");
		 c->SaveAs("Statistics_distribution_of_profilelikelihoodratio_HW.png");

		 return 0 ;


}

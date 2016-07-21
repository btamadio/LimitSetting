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
		 
		 cout << "variable argc is " << argc << endl;
		 cout << "argv[0] is " << argv[0] << endl;
		 cout << "argv[1] is " << argv[1] << endl;
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
		 cout << " Number of toys we have for this point is   " << numev << endl;

		 int nbin = 60000 ; 
		 TH1F* h_s0 = new TH1F("h_s0","h_s0",nbin,-30,30);
		 TH1F* h_s2 = new TH1F("h_s2","h_s2",nbin,-30,30);


		 for( int i = 0 ; i < numev ; i ++)
		   {
		     get_event(i) ;
		     double a , b ;

		     if( p-> alpha == 1 ){
		       h_s0 -> Fill( p->DNLL ) ;
		     }

		     if( p-> alpha == 0 ){
		       h_s2 -> Fill( p->DNLL ) ;
		     }

		   }

		 h_s2->Scale( 1/h_s2->GetEntries() );
		 h_s0->Scale( 1/h_s0->GetEntries() );

		 double mean_spin0=-99999 , mean_spin2=-99999, rsigma_spin0 = -99999, lsigma_spin2 = -99999 ;
		 double step_size = 60.0/nbin; 
		 double area_spin0 = 0 ;
		 double area_spin2 = 0 ;  
		 for( int i = 0 ; i < nbin+2 ; i ++ )
		   {
		     area_spin0 += h_s0->GetBinContent(i);
		     area_spin2 += h_s2->GetBinContent(i);

		     if( area_spin0 >= 0.5 && mean_spin0 == - 99999)
		       mean_spin0 = step_size*i ;

		     if( area_spin2 >= 0.5 && mean_spin2 == - 99999)
		       mean_spin2 = step_size*i ;

		     if( area_spin0 >= (1-0.3173/2) && rsigma_spin0 == - 99999)
		       rsigma_spin0 = step_size*i ;

		     if( area_spin2 >= (0.3173/2) && lsigma_spin2 == - 99999)
		       lsigma_spin2 = step_size*i ;

		   }


		 mean_spin0+=-30;
		 mean_spin2+=-30;
		 rsigma_spin0+=-30;
		 lsigma_spin2+=-30;

		 cout << " Median expectation for spin 0 and spin 2 are " << endl;
		 cout << mean_spin0 << "   " << mean_spin2 << endl;
		 cout << endl;
		 cout << " Expected separation " << endl;
		 cout << " Difference =  " << fabs( mean_spin2 - mean_spin0 ) << endl;
		 cout << " in the standard deviations of spin 0 hypothesis " << fabs( mean_spin2 - mean_spin0 ) / fabs( mean_spin0 - rsigma_spin0 ) << endl;
		 cout << " in the standard deviations of spin 2 hypothesis " << fabs( mean_spin2 - mean_spin0 ) / fabs( mean_spin2 - lsigma_spin2 ) << endl;
   

		 TCanvas* c = new TCanvas("c","c",800,600);
		 c->SetFillColor(0);


		 h_s0->SetLineColor(2);
		 h_s0->SetLineWidth(2);
		 h_s2->SetLineColor(4);
		 h_s2->SetLineWidth(2);
 
		 h_s0->Rebin(500);
		 h_s2->Rebin(500);

		 double max_s0 = h_s0->GetMaximum();
		 h_s0 -> GetYaxis()->SetRangeUser(0,1.5*max_s0);
		 h_s0 -> GetYaxis()->SetTitle("Fraction of Entries");
		 h_s0 -> GetYaxis()->SetTitleOffset(1.5);
 		 h_s0 -> GetXaxis()->SetTitle("-2ln(L(SM)/L(J^{P}=2^{+}))");
		 h_s0->Draw();
		 h_s2->Draw("same");


		 TLegend leg(0.25,0.78,0.395,0.93);//0.23,0.66,0.5,0.92);
		 leg.SetLineColor(0);
		 leg.SetFillStyle(0);
		 leg.SetTextSize(0.05);
		 leg.SetShadowColor(0);
		 leg.AddEntry(h_s0,"SM Higgs pseudo data","f");
		 leg.AddEntry(h_s2,"J^{P} = 2^{+} pseudo data","f");
		 leg.SetFillColor(0);
		 leg.Draw();


		 c->SaveAs("DNLLPlot.eps");

		 return 0 ;


}

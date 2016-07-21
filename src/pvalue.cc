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
		 gROOT->ProcessLine(".! clear");
		 cout << " Number of toys we have for this point is   " << numev << endl;

		 int nbin = 300000 ; 
		 TH1F* h_s0 = new TH1F("h_s0","h_s0",nbin,-15,15);
		 TH1F* h_s2 = new TH1F("h_s2","h_s2",nbin,-15,15);

		 TH1F* h_s0t = new TH1F("h_s0t","h_s0t",nbin,-15,15);
		 TH1F* h_s2t = new TH1F("h_s2t","h_s2t",nbin,-15,15);

		 ifstream file_to_read("./TS.txt");
		 double val_obs ;
		 while(!file_to_read.eof())
		   {
		     double n0 ;
		     file_to_read >> n0 ;
		     val_obs = n0 ;
		   }

		 int ctr_s0 = 0 , ctr_s2 = 0 , cp_s0 = 0 , cp_s2 = 0 ; 

		 for( int i = 0 ; i < numev ; i ++)
		   {
		     get_event(i) ;
		     double a , b ;

		     if( p-> alpha == 1 ){
		       h_s0 -> Fill( p->DNLL/(-2.0) ) ;
		       ctr_s0 ++;
		       if( p->DNLL > val_obs )  { cp_s0 ++; h_s0t -> Fill( p->DNLL/(-2.0) ) ;}
		     }

		     if( p-> alpha == 0 ){
		       h_s2 -> Fill( p->DNLL/(-2.0) ) ;
		       ctr_s2 ++;
		       if( p->DNLL < val_obs ) { cp_s2 ++; h_s2t -> Fill( p->DNLL/(-2.0) ) ;}

		     }

		   }


		 double scale0 = 1/h_s0->GetEntries();
		 double scale2 = 1/h_s2->GetEntries();

		 h_s2->Scale( 1/h_s2->GetEntries() );
		 h_s0->Scale( 1/h_s0->GetEntries() );

		 double mean_spin0=-99999 , mean_spin2=-99999, rsigma_spin0 = -99999, lsigma_spin2 = -99999 ;
		 double left_1sigma_spin0 = -99999 , left_1sigma_spin2 = -99999 , right_1sigma_spin0 = -99999 , right_1sigma_spin2 = -99999;
		 double step_size = 20.0/nbin; 
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

		     if( area_spin0 >= (1-0.3173/2) && rsigma_spin0 == - 99999){
		       rsigma_spin0 = step_size*i ;
		       right_1sigma_spin0 = step_size*i ;
		     }

		     if( area_spin0 >= (0.3173/2) && left_1sigma_spin0 == -99999){
		       left_1sigma_spin0 = step_size*i ;
		     }

		     if( area_spin2 >= (0.3173/2) && lsigma_spin2 == - 99999){
		       left_1sigma_spin2 = step_size*i ;
		       lsigma_spin2 = step_size*i ;
		     }

		     if( area_spin2 >= (1-0.3173/2) && right_1sigma_spin2 == - 99999){
		       right_1sigma_spin2 = step_size*i ;
		     }


		   }


		 cout  << left_1sigma_spin0-10 << " " << mean_spin0-10 << " " << right_1sigma_spin0-10 << endl;
		 cout  << left_1sigma_spin2-10 << " " << mean_spin2-10 << " " << right_1sigma_spin2-10 << endl;
		 double pexp0 = 0, pexp2 = 0 ;
		 double new_spin0 = 0 , new_spin2 = 0 ;

		 double p0_def_by_right_1sigma_spin2 = 0 ;
		 double p0_def_by_left_1sigma_spin2 = 0 ;
		 double p0_def_by_right_1sigma_spin0 = 0 ;
		 double p0_def_by_left_1sigma_spin0 = 0 ;
 

		 for( int i = 0 ; i < nbin+2 ; i ++ )
		   {
		     new_spin0 += h_s0->GetBinContent(i);
		     new_spin2 += h_s2->GetBinContent(i);

                     if( new_spin0 >= (0.3173/2) && p0_def_by_left_1sigma_spin0 == 0){
		       p0_def_by_left_1sigma_spin0 = new_spin2;
		       cout << p0_def_by_left_1sigma_spin0 << " " <<  p0_def_by_left_1sigma_spin0 << endl;
		     }

                     if( new_spin0 >= (1-0.3173/2) && p0_def_by_right_1sigma_spin0 == 0)
		       p0_def_by_right_1sigma_spin0 = new_spin2;

                     if( new_spin2 >= (0.3173/2) && p0_def_by_left_1sigma_spin2 == 0)
		       p0_def_by_left_1sigma_spin2 = 1-new_spin0;

                     if( new_spin2 >= (1-0.3173/2) && p0_def_by_right_1sigma_spin2 == 0)
		       p0_def_by_right_1sigma_spin2 = 1-new_spin0;


		     if( new_spin0 >= 0.5 && pexp2 == 0)
		       pexp2 = new_spin2;
		     if( new_spin2 >= 0.5 && pexp0 == 0)
		       pexp0 = 1.0 - new_spin0 ;

		   }

		 cout << "p0_def_by_left_1sigma_spin0 " << p0_def_by_left_1sigma_spin0 << endl;
		 cout << "p0_def_by_right_1sigma_spin0 " << p0_def_by_right_1sigma_spin0 << endl;
		 cout << "p0_def_by_left_1sigma_spin2 " << p0_def_by_left_1sigma_spin2 << endl;
		 cout << "p0_def_by_right_1sigma_spin2 " << p0_def_by_right_1sigma_spin2 << endl;

		 mean_spin0+=-10;
		 mean_spin2+=-10;
		 rsigma_spin0+=-10;
		 lsigma_spin2+=-10;

		 cout << " Median expectation for spin 0 and spin 2 are " << endl;
		 cout << mean_spin0 << "   " << mean_spin2 << endl;
		 cout << endl;
		 cout << " Expected separation " << endl;
		 cout << " Difference =  " << fabs( mean_spin2 - mean_spin0 ) << endl;
		 cout << " in the standard deviations of spin 0 hypothesis " << fabs( mean_spin2 - mean_spin0 ) / fabs( mean_spin0 - rsigma_spin0 ) << endl;
		 cout << " in the standard deviations of spin 2 hypothesis " << fabs( mean_spin2 - mean_spin0 ) / fabs( mean_spin2 - lsigma_spin2 ) << endl;


		 cout << " Expected separation from pvalue" << endl;
		 cout << " in the standard deviations of spin 0 hypothesis " << sqrt(TMath::ChisquareQuantile( 1 - 2*pexp0 ,1)) << endl;
		 cout << " in the standard deviations of spin 2 hypothesis " << sqrt(TMath::ChisquareQuantile( 1 - 2*pexp2 ,1)) << endl;
		 cout << " pvalues 0 " << pexp0 << " 2 " << pexp2 << endl;   

		 double Z_0 = sqrt(TMath::ChisquareQuantile( 1 - 2*pexp0 ,1));
		 double Z_2 = sqrt(TMath::ChisquareQuantile( 1 - 2*pexp2 ,1));

		 double Z_def_by_left_1sigma_spin2 = sqrt(TMath::ChisquareQuantile( 1 - 2*p0_def_by_left_1sigma_spin2 ,1)); 
		 double Z_def_by_right_1sigma_spin2 = sqrt(TMath::ChisquareQuantile( 1 - 2*p0_def_by_right_1sigma_spin2 ,1)); 
		 double Z_def_by_left_1sigma_spin0 = sqrt(TMath::ChisquareQuantile( 1 - 2*p0_def_by_left_1sigma_spin0 ,1)); 
		 double Z_def_by_right_1sigma_spin0 = sqrt(TMath::ChisquareQuantile( 1 - 2*p0_def_by_right_1sigma_spin0 ,1)); 

		 double Z_0_eleft = Z_0 - Z_def_by_left_1sigma_spin2;
		 double Z_0_eright = - (Z_0 - Z_def_by_right_1sigma_spin2);

		 double Z_2_eleft = -(Z_2 - Z_def_by_left_1sigma_spin0);
		 double Z_2_eright =  (Z_2 - Z_def_by_right_1sigma_spin0);

		 cout << " Z_0 left : right " << Z_0_eleft << " " << Z_0_eright << endl;
		 cout << " Z_2 right : left " << Z_2_eright << " " << Z_2_eleft << endl;


		 TCanvas* c = new TCanvas("c","c",800,600);
		 c->SetFillColor(0);


		 h_s0->SetLineColor(4);
		 h_s0->SetLineWidth(2);
		 h_s2->SetLineColor(2);
		 h_s2->SetLineWidth(2);
 
		 h_s0->Rebin(1000);
		 h_s2->Rebin(1000);

		 double max_s0 = h_s0->GetMaximum();
		 h_s0 -> GetYaxis()->SetRangeUser(0,1.5*max_s0);
		 h_s0 -> GetYaxis()->SetTitle("Fraction of Entries");
		 h_s0 -> GetYaxis()->SetTitleOffset(1.5);
		 // 		 h_s0 -> GetXaxis()->SetTitle("-2ln(L(SM)/L(J^{P}=2^{+}))");
 		 h_s0 -> GetXaxis()->SetTitle("ln(L(0)/L(2))");
		 h_s0->Draw();
		 h_s2->Draw("same");

		 //                 cout << " p-values spin0 " << cp_s0/(ctr_s0*1.0) << " spin2 " << cp_s2/(ctr_s2*1.0)  << endl;
		 h_s0t ->Scale( scale0);
		 h_s2t ->Scale( scale2);

		 h_s0t->SetLineColor(4);
		 h_s2t->SetLineColor(2);

                 h_s0t->Rebin(1000);
                 h_s2t->Rebin(1000);

		 h_s0t->SetFillColor(4);
		 h_s2t->SetFillColor(2);

		 h_s0t->SetFillStyle(3002);
		 h_s2t->SetFillStyle(3002);

		 h_s0t->Draw("same");
		 h_s2t->Draw("same");


		 double width = h_s0->GetBinWidth(1);
		 int n_bin = (10 + val_obs/(-2)) / width;
		 double upper = h_s0->GetBinContent( n_bin );

		 TLine * l4=new TLine( val_obs/(-2.0) ,0, val_obs/(-2.0), upper);
		 l4 -> SetLineWidth(5);
		 l4 -> SetLineColor(1);
		 l4 -> SetLineStyle(1);
		 l4->Draw("same");


		 TLegend leg(0.2,0.78,0.345,0.93);//0.23,0.66,0.5,0.92);
		 leg.SetLineColor(0);
		 leg.SetFillStyle(0);
		 leg.SetTextSize(0.04);
		 leg.SetShadowColor(0);
		 leg.AddEntry(h_s0,"J^{P} = 0^{+} hypothesis","l");
		 leg.AddEntry(h_s2,"J^{P} = 2^{+}_{m} hypothesis","l");
		 leg.AddEntry(l4,"Observed","L");
		 leg.SetFillColor(0);
		 leg.Draw();


		 TLatex l; //l.SetTextAlign(12); l.SetTextSize(tsize);
		 l.SetNDC();
		 l.SetTextSize(0.04);
		 l.SetTextColor(1);
		 l.DrawLatex(0.59,0.87,"#sqrt{s} = 8 TeV");
		 l.DrawLatex(0.59,0.78,"Data 2012, #intLdt = 20.7 fb^{-1}");

		 TLatex la; //l.SetTextAlign(12); l.SetTextSize(tsize);
		 la.SetNDC();
		 la.SetTextFont(72);
		 la.SetTextColor(1);

		 TLatex lp;
		 lp.SetNDC();
		 lp.SetTextFont(42);
		 lp.SetTextColor(1);
		 la.SetTextSize(0.04);
		 la.DrawLatex(0.59,0.72,"ATLAS");
		 lp.SetTextSize(0.04);
		 lp.DrawLatex(0.705,0.72,"Internal");


		 c->SaveAs("DNLLPlot.eps");
		 //		 c->SaveAs("~/public_html/ongoing/DNLLPlot.eps");
		 //c->SaveAs("~/public_html/ongoing/DNLLPlot.png");
		 c->SaveAs("./DNLLPlot.eps");
		 c->SaveAs("./DNLLPlot.png");

		 TFile g("output.root","recreate");
		 c->Write();
		 h_s0->Write();
		 h_s2->Write();
		 h_s0t->Write();
		 h_s2t->Write();
		 g.Close();



		 cout << " p-values spin0 " << cp_s0/(ctr_s0*1.0) << " spin2 " << cp_s2/(ctr_s2*1.0)  << endl; 

		 return 0 ;


}

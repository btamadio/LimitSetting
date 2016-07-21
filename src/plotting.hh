/*
 *  hcanalysis.h
 *  
 *
 *  Created by Haichen Wang on 6/6/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#include "TPaveStats.h"
#include "TMath.h"
#include "TStopwatch.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"
#include "TChain.h"
#include "TROOT.h"
#include "THStack.h"
#include "TLegend.h"
#include "TF1.h"
#include "TLatex.h"


#include "TString.h"
#include <sys/stat.h>
#include "header.hh"
#include "../aux/likelihoodtree.hh"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

using namespace std;
#include <vector>

likelihoodtree *p;

void get_event(int i) {
  if ( p->LoadTree(i) < 0) { 
    cout<<"\nProblem in LoadTree."
	<<"\nEntry: "<<i<<endl;
    exit(0);
  }
  p->fChain->GetEntry(i);
}


vector<double> intersection( double x1, double y1, double x2, double y2 , double x3, double y3, double x4, double y4  )
{

  // the line connecting the first two points
  double a1 = (y2-y1)/(x2-x1);
  double b1 = y1 - x1*(y2-y1)/(x2-x1);

  // the second line
  double a2 = (y4-y3)/(x4-x3);
  double b2 = y3 - x3*(y4-y3)/(x4-x3);

  double x0 = (b2 - b1)/(a1 - a2) ;
  double y0 = b1 + a1*( b2 - b1 )/( a1 - a2) ;

  vector <double > result ;
  result.push_back(x0);
  result.push_back(y0);
  return result ; 

} 

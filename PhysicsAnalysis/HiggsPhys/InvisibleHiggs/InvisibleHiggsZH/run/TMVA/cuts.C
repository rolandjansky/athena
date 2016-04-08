/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// program used to apply the final selection based on the chosen TMVA output variable. (previously called stack.C)
// 
// *************    Written by Pauline Gagnon - 14 mars6 may 2008  *********************
// 
using namespace std;

#define CHAIN

#include <fstream>
using std::ofstream;

#include <iostream>
#include <iomanip>
#include <cmath>
#include <string>
#include <string.h>
#include <vector>
#include <list>  
#include <stdlib.h>
// Regular include files to make ROOT work with our C++ program
#include "TROOT.h"
#include "TApplication.h"
#include "TCanvas.h"
#include "TPaveText.h"
#include "TLine.h"
#include "TPaveLabel.h"
#include "TNtuple.h"
#include "TH1.h"
#include "THStack.h"
#include "TPad.h"
#include "TFile.h"
#include "TLegend.h"
#include "TChain.h"
#include "TStyle.h"
#include "TGraph.h"
#include "TFrame.h"

// - - - Initiates the GUI interface of ROOT - - -
using std::string;
using std::setprecision;
Int_t places = 2;
extern void InitGui();

//============================================================================

int main(int argc, char **argv)
{
  // execute rootlogon.C to get ATLAS style setup.
#include "rootlogon.C"

  // initialization of all counters for statistics

  // enter the number of different types of events and the # of background weight files used
  Int_t const ntype = 8;
  Int_t const nvar = 8;

  // open output file to save the cutflow results
  ofstream outClientFile( "cuts.dat", ios::out );

  // exit program if unable to open file
  if ( !outClientFile ) {
    cerr << "File could not be opened" << endl;
    exit (1);
  } // end if
  
  VoidFuncPtr_t initfuncs[] = { InitGui, 0 };
  TROOT root("","", initfuncs);

  TApplication theApp("App", &argc, argv);
  gROOT->Reset();
  gROOT->SetStyle("ATLAS");
  gROOT->ForceStyle();

  Int_t nb=0;
  bool last_rec=0;

  float minValue[8] = {61.19, -1.0, -1.0, -1.0, 0.0, -1.0, -1.0, 0.0};  // 
  float maxValue[8] = {121.19, 1.0,  1.0,  1.0, 700.0, 1.0,  1.0, 50.0};  // 

  //              - - - Getting tree from the disk - - - 

  // We define the chain, which is basically a tree to add root files with the same tree name.
  // If you have two or more files, just keep going as nfchain += chain->Add("filename.root")

  float massZ;
  float cosJetMET = 0.;
  float cosll2d;
  float cosll3d;
  float mT;
  float cosMETptlep;
  float cosphi;
  float cone1;
  //  float ptlep2;
  
  int runnum;
  int evtnum;
  int type;
  float w;
  const float massZ0 = 91.190;

#ifdef CHAIN

  TChain* chain = new TChain("TreeB");
  Int_t nfchain=0;
  
  nfchain += chain->Add("/data/TMVA/preselection-HZ130-TreeB-4jun.root");
  nfchain += chain->Add("/data/TMVA/preselection-ZZ.root");
  nfchain += chain->Add("/data/TMVA/preselection-tt.root");
  nfchain += chain->Add("/data/TMVA/preselection-WW.root");
  nfchain += chain->Add("/data/TMVA/preselection-tn.root");
  nfchain += chain->Add("/data/TMVA/preselection-lt.root");
  nfchain += chain->Add("/data/TMVA/preselection-ZW.root");
  nfchain += chain->Add("/data/TMVA/preselection-Zi.root");

  chain->SetBranchAddress( "massZ", &massZ );
  chain->SetBranchAddress( "cosll2d", &cosll2d );
  chain->SetBranchAddress( "cosll3d", &cosll3d );
  chain->SetBranchAddress( "mT", &mT );
  chain->SetBranchAddress( "cosMETptlep", &cosMETptlep);
  chain->SetBranchAddress( "cosJetMET", &cosJetMET);
  chain->SetBranchAddress( "cosphi", &cosphi );
  chain->SetBranchAddress( "cone1", &cone1 );
  chain->SetBranchAddress( "runnum", &runnum );
  chain->SetBranchAddress( "evtnum", &evtnum );
  chain->SetBranchAddress( "type", &type );
  chain->SetBranchAddress( "w", &w );
  //  chain->SetBranchAddress( "ptlep2", &ptlep2 );

  Int_t nrec = static_cast<int>(0.5 + chain->GetEntries());
  cout << nrec << " records found in the chain. "<< endl;

  // The above is to find the number of events or entries

#endif // CHAIN 

  Float_t bestSoverB = 0.0;
  
  // ============  Loop over each variable 
  for (int t=0; t<nvar; t++) {
        if (t == 1) {   
    //    if (t >= 0) {   

      string varType[8]  = {("massZ"), ("cosll2d"), ("cosll3d"), ("cosphi"), ("mT"),("cosjetmet"),("cosmetptlep"), ("cone1")}; 
      //      string varType[7]  = {("massZ"), ("cosll2d"), ("cosll3d"), ("cosphi"), ("mT"),("cosjetmet"),("cosmetptlep")}; 
      
      cout << "*********************************************************************" << endl;
      cout << " variable being scanned: " << varType[t] << " between " << minValue[t] << " and " << maxValue[t] << endl;
      cout << "*********************************************************************" << endl;
      
      outClientFile << "*********************************************************************" << endl;
      outClientFile << " variable being scanned: " << varType[t] << " between " << minValue[t] << " and " << maxValue[t] << endl;
      outClientFile << "*********************************************************************" << endl;
      
      // ============  Loop over the cut values for the each background type
      Float_t SoverB;
      Float_t bestxi2[8] = {1000.,1000.,1000.,1000.,1000.,1000.,1000.,1000.};
      Float_t bestcut[8] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};

      Float_t xi2;
      Float_t cut = -1.0;
      Float_t ncut = 100.0;
      for (float c=0.0; c<ncut; c++) {
	
	//	float CutValue[8] = {50.0, -1.0, -1.0, 1.0, 0.0, -1.0, 1.0, -0.1};  // starting point 

	//         float CutValue[7] = {12.0, -1.0, -1.0, -0.98, 399.0, -1.0, 1.0};  // cross-check
	 //         float CutValue[7] = {50.0, -1.0, -1.0, 1.0, 0.0, -1.0, 1.0};  // starting point 
         float CutValue[7] = {12.0, -1.0, -1.0, -0.98, 399.0, -0.96, 1.0};  // with only halft the events - 3 jun
	//	float CutValue[7] = {12.0, -0.76, -1.0, -0.86, 378.0, -1.0, -0.72};  // final cuts

	Float_t weight[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	
	Float_t variable[8] =   {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	Float_t selected[8] =   {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	Float_t rejected[8] =   {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	Float_t Total[8]  =     {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	Float_t TotalBgnd = 0.0;
	
	float NSignal = 0.0;
	float NBgnd = 0.0;
	
	int passed[8] = {0,0,0,0,0,0,0,0};
	float failed[8][8] = {
	  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
	  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
	  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
	  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
	  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
	  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
	  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
	  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}
	};

	if (t>0 && t !=3 && t !=6) {
	  float increment=(maxValue[t]-minValue[t])/ncut;
	  cut = minValue[t] + c*increment;
	  CutValue[t] = cut;
	}

	if (t ==3 || t ==6) {
	  float increment=(maxValue[t]-minValue[t])/ncut;
	  cut = maxValue[t] - c*increment;
	  CutValue[t] = cut;
	}

	if (t==0) {
	  float increment=(maxValue[t]-massZ0)/ncut;
	  cut = (c+1.0)*increment;
	  CutValue[t] = cut;
	}

	// ============  Starting the main loop over the events  ===============
	
	for (Int_t i=0; i<nrec; i++) {        
	  
	  last_rec=(i>=nrec-1); 
	  
	  if(!last_rec)
	    {
#ifdef CHAIN
	      nb += chain->GetEvent(i+1); // Getting one event at a time
#else   /* Not CHAIN */ 
	      nb += readtree->GetEvent(i+1);
#endif   /* CHAIN */ // The next record has been read for comparison
	    }
	  // Reading the i-th (current) record:
	  
#ifdef CHAIN
	  nb += chain->GetEvent(i); 
#else   /* Not CHAIN */ 
	  nb += readtree->GetEvent(i);
#endif   /* CHAIN */
	  
// only take every other event, rejecting the half of events used in training
	  if ((i%2) != 0) {

	    // store the weight for each event type
	    weight[type] = w;
	  
	    variable[0] = massZ;
	    variable[1] = cosll2d;
	    variable[2] = cosll3d;
	    variable[3] = cosphi;
	    variable[4] = mT;
	    variable[5] = cosJetMET;
	    variable[6] = cosMETptlep;
	    variable[7] = cone1;
	  
	    int success = 1;
	  
	    for (int i = 0; i < nvar; i++) {
	      passed[i] = 0;
	    
	      if (i > 0 && i != 3 && i != 6 && i != 7 && variable[i] > CutValue[i]) {passed[i] = 1;}
	      if ((i == 3 || i == 6 || i == 7) && variable[i] < CutValue[i]) {passed[i] = 1;}
	      if (i == 0 && abs(variable[i] - massZ0) < CutValue[i]) {passed[i] = 1;}
	      success *= passed[i];
	      if (passed[i] == 0.0) failed[type][i] += 2.0;
	    }
	  
	    // count how many events pass all selections; count each event as 2 since only half the events are used
	    if (success == 1) {
	      Total[type] += 2.0*weight[type];
	      selected[type] += 2.0;
	      if (type == 0) NSignal += 2.0;
	      if (type >  0) NBgnd += 2.0;
	    }
	  
	    if (success == 0) {
	      rejected[type] += 2.0;
	    }
	  
	  } // end of if loop keeping only every other event; reject half the events used in the training
	} // end of for loop over events   
	
	for (int type=1; type<ntype; type++) {
	  TotalBgnd += Total[type];
	  //	  cout << "Total[type]: " << Total[type] << endl;
	}
	
	// get info on S/sqrt(B) and display it after doubling the numbers since only half of the events were used
	// use reweighted number
	
	SoverB = 0.0;
	xi2 = 0.0;
       	if (TotalBgnd > 0.0) SoverB = Total[0]/sqrt(TotalBgnd);  //optimize S/sqrt(all backgrounds)
	//       	if (Total[0] > 0.0) xi2 = 100.0*2.33*sqrt(0.072*TotalBgnd*0.072*TotalBgnd+TotalBgnd)/Total[0];  //optimize xi2
       	if (Total[0] > 0.0) xi2 = 100.0*1.64*sqrt(0.072*TotalBgnd*0.072*TotalBgnd+TotalBgnd)/Total[0];  //optimize xi2
        if (xi2 < bestxi2[t] && xi2 != 0.0) {
	  bestxi2[t] = xi2;
	  bestcut[t] = c;
	}

	if (SoverB > bestSoverB) {
	  bestSoverB = SoverB;
	}
	
	places = 2;
	cout <<fixed;                 
	cout.precision(places); 
	outClientFile <<fixed;                 
	outClientFile.precision(places);
	
	cout << "cut value " << cut << " SoverB: " << SoverB << " xi2: " << xi2 << endl;
	cout << "*    Total signal @ 30fb-1 with cuts = " << Total[0]  << endl;
	cout << "*    Total background @ 30fb-1       = " << TotalBgnd << "\t" << Total[1] << "\t" 
	     << Total[2] << "\t" << Total[3] << "\t" << Total[4] << "\t" << Total[5] << "\t" << Total[6] << "\t" << Total[7] << endl;

	outClientFile << "cut value " << cut << " SoverB: " << SoverB << " xi2: " << xi2 << endl;
	outClientFile << "*    Total signal @ 30fb-1 with cuts = " << Total[0]  << endl;
	outClientFile << "*    Total background @ 30fb-1       = " << TotalBgnd << "\t" << Total[1] << "\t" 
		      << Total[2] << "\t" << Total[3] << "\t" << Total[4] << "\t" << Total[5] << "\t" << Total[6] << "\t" << Total[7] << endl;
      } // end of for loop over cut value
      
      float bestcutvalue;
      if (t>0 && t !=3 && t !=6)  bestcutvalue = minValue[t] + bestcut[t]*((maxValue[t]-minValue[t])/ncut);
      if (t == 3 || t ==6)  bestcutvalue = maxValue[t] - bestcut[t]*((maxValue[t]-minValue[t])/ncut);
      if (t==0) bestcutvalue = (bestcut[t]+1.0)*((maxValue[t]-massZ0)/ncut);

      cout << "****************************************************************************" << endl;
      cout << " variable " << varType[t] << " best xi = " << bestxi2[t] << " for a cut at " << bestcutvalue << " (c = " << bestcut[t] << ")"<< endl;
      cout << "****************************************************************************" << endl;

      outClientFile << "****************************************************************************" << endl;
      outClientFile << " variable " << varType[t] << " best xi = " << bestxi2[t] << " for a cut at " << bestcutvalue << " (c = " << bestcut[t] << ")"<< endl;
      outClientFile << "****************************************************************************" << endl;

    } //end if on excluding variables that have already been fixed
  } // end of for loop over all variables
  
  // done with statistics; display the histograms
  
  
  cout << endl;
  cout << "All done!" << endl;
  
  theApp.Run();                            // concluding ROOT requirements.
  
  return 0;
} //  End of main(...)



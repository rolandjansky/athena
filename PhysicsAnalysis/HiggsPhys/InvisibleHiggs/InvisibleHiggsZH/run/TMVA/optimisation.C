/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// program used to find the optimal cut position for each variable
// 
// *************    Written by Pauline Gagnon - 31 Jan 2008  *********************
// based on selection.C
// now only counting 1.0 for each event - May 28, 2008
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
  
  // enter the number of different types of events and the # of background weight files used
  Int_t const ntype = 8;
  Int_t const nbgndw = 7;
  
  // open output file to save the cutflow results
  ofstream outClientFile( "optimisation.dat", ios::out );
  
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
  
  
  Float_t bdtZZ = 0.0;
  Float_t bdttt = 0.0;
  Float_t bdtWW = 0.0;
  Float_t bdttn = 0.0;
  Float_t bdtlt = 0.0;
  Float_t bdtZW = 0.0;
  Float_t bdtZi = 0.0;
  Float_t w = 0.0;
  
  Int_t runnum;
  Int_t evtnum, type;
  
  Int_t nb=0;
  float portion;
  bool last_rec=0;
  
  //              - - - Getting tree from the disk - - - 
  
  // We define the chain, which is basically a tree to add root files with the same tree name.
  // If you have two or more files, just keep going as nfchain += chain->Add("filename.root")
  
#ifdef CHAIN
  
  TChain* chain = new TChain("finalTree");
  Int_t nfchain=0;
  
  nfchain += chain->Add("/data/TMVA/final/final.root");
  
  
  chain->SetBranchAddress("runnum", &runnum);
  chain->SetBranchAddress("evtnum", &evtnum);
  chain->SetBranchAddress("type",  &type);   
  chain->SetBranchAddress("bdtZZ", &bdtZZ);
  chain->SetBranchAddress("bdttt", &bdttt);
  chain->SetBranchAddress("bdtWW", &bdtWW);
  chain->SetBranchAddress("bdttn", &bdttn);
  chain->SetBranchAddress("bdtlt", &bdtlt);
  chain->SetBranchAddress("bdtZW", &bdtZW);
  chain->SetBranchAddress("bdtZi", &bdtZi);
  chain->SetBranchAddress("w", &w);
  
  Int_t nrec = static_cast<int>(0.5 + chain->GetEntries());
  Int_t nticker= static_cast<int>(0.5 + nrec/20.0);
  cout << nrec << " records found in the chain. "<< endl;
  
  // The above is to find the number of events or entries
  
#endif // CHAIN 
  
  // ============  Loop over each background type except ZZ->llnunu
  for (int t=1; t<nbgndw; t++) {

  // reject types already evaluated and type 0 (ZZ) and 3 (tautaununu)
    if (t != 3 && t !=5 && t !=1 && t !=2) {   
      // && t !=5 && t !=2 && t !=6 && t !=1
      //      if (t != 3 && t !=5 && t !=4 && t !=2) {   
      //      if (t !=3 && t !=2 && t !=4) {   
      string eventType[8]  = {("HZ"), ("ZZ"), ("tt"), ("WW"), ("ZZtautaununu"),("ZZlltautau"),("ZW"),("Zincl")}; 
      
      cout << "*********************************************************************" << endl;
      cout << " background type being scanned: " << eventType[t+1] << endl;
      cout << "*********************************************************************" << endl;
      
      outClientFile << "*********************************************************************" << endl;
      outClientFile << " background type being scanned: " << eventType[t+1] << endl;
      outClientFile << "*********************************************************************" << endl;
      
      // ============  Loop over the cut values for the each background type
      Float_t SoverB;
      Float_t bestxi2[7] = {1000.,1000.,1000.,1000.,1000.,1000.,1000.};
      Float_t bestcut[7] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0};

      Float_t xi2;
      Double_t cut = -1.0;
      Double_t ncut = 100.0;
      for (double c=0.0; c<ncut; c++) {
	
	// initialization of all counters for statistics
	//	         float minCutValue[7] = {-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0};  // starting point
	//	         float minCutValue[7] = {-1.0, 0.72, 0.58, -1.0, 0.74, 0.72, 0.68};  // HZ120 -eff - 0.496 - 10 juin 99% CL

	//	         float minCutValue[7] = {-1.0, 0.66, 0.30, -1.0, 0.72, 0.24, 0.56};  // HZ110 - 10 June 95% CL
	//	         float minCutValue[7] = {-1.0, 0.72, 0.58, -1.0, 0.74, 0.72, 0.68};  // HZ120 -eff - 0.496 - 10 juin 95% CL
	//               float minCutValue[7] = {-1.0, -1.0, 0.64, -1.0, -1.0, 0.44, -1.0};  // HZ150 - 10 June 95% CL
	//               float minCutValue[7] = {-1.0, 0.50, 0.44, -1.0, 0.64, 0.46, -1.0};  // 
	         float minCutValue[7] = {-1.0, 0.72, 0.34, -1.0, -1.0, 0.62, -1.0};  // HZ250 - 10 June 95% CL

	// float minCutValue[7] = {-1.0, 0.84, 0.74, -1.0, 0.68, 0.44, 0.62};  // HZ120 with half the event - 3 june
        // float minCutValue[7] = {-1.0, 0.52, -0.08, -1.0, 0.42, 0.20, 0.62};  // HZ130 with half the event - 3 june
	// float minCutValue[7] = {-1.0, 0.62, 0.38, -1.0, 0.66, 0.24, 0.58};  // HZ140 with half the event - 3 june

//         float minCutValue[7] = {-1.0, 0.72, 0.58, -1.0, 0.74, 0.72, 0.68};  // HZ120 with half the event
//	   float minCutValue[7] = {-1.0, 0.60, 0.38, -1.0, 0.66, 0.14, 0.62};  // HZ130 with half the event
//         float minCutValue[7] = {-1.0, -1.0, -1.0, -1.0, 0.48, 0.32, 0.72};  // HZ140 with half the event

	//	   float minCutValue[7] = {-1.0, 0.70, 0.68, -1.0, 0.78, 0.84, 0.66};  // HZ120 - xi2
	//         float minCutValue[7] = {-1.0, -1.0, 0.70, -1.0, 0.66, 0.16, 0.60};  // HZ130 - xi2
	//         float minCutValue[7] = {-1.0, 0.66, 0.12, -1.0, 0.62, 0.32, 0.56};  // HZ140 - xi2

	Float_t weight[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	
	Float_t mvaBdt[7] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	Float_t selected[8] =   {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	Float_t rejected[8] =   {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	Float_t Total[8]  =     {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	Float_t TotalBgnd = 0.0;
	
	float NSignal = 0.0;
	float NBgnd = 0.0;
	
	int passed[8] = {0,0,0,0,0,0,0,0};
	float failed[8][7] = {
	  {0.0,0.0,0.0,0.0,0.0,0.0,0.0},
	  {0.0,0.0,0.0,0.0,0.0,0.0,0.0},
	  {0.0,0.0,0.0,0.0,0.0,0.0,0.0},
	  {0.0,0.0,0.0,0.0,0.0,0.0,0.0},
	  {0.0,0.0,0.0,0.0,0.0,0.0,0.0},
	  {0.0,0.0,0.0,0.0,0.0,0.0,0.0},
	  {0.0,0.0,0.0,0.0,0.0,0.0,0.0},
	  {0.0,0.0,0.0,0.0,0.0,0.0,0.0}
	};
	
	if (ncut == 100.0) cut=-1.0+c*0.02;
	if (ncut == 50.0)  cut=0.0+c*0.02;
	minCutValue[t] = cut;
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
	  
	  
	    mvaBdt[0] = bdtZZ;
	    mvaBdt[1] = bdttt;
	    mvaBdt[2] = bdtWW;
	    mvaBdt[3] = bdttn;
	    mvaBdt[4] = bdtlt;
	    mvaBdt[5] = bdtZW;
	    mvaBdt[6] = bdtZi;
	  
	    int success = 1;
	  
	    for (int i = 0; i < nbgndw; i++) {
	      passed[i] = 0;
	    
	      if (mvaBdt[i] > minCutValue[i]) {passed[i] = 1;}
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
	//       	if (Total[0] > 0.0) xi2 = 100.0*2.33*sqrt(0.071*TotalBgnd*0.071*TotalBgnd+TotalBgnd)/Total[0];  //optimize xi2 @ 99% CL
       	if (Total[0] > 0.0) xi2 = 100.0*1.64*sqrt(0.071*TotalBgnd*0.071*TotalBgnd+TotalBgnd)/Total[0];  //optimize xi2 @ 95% CL
	//	if (Total[6] > 0.0) SoverB = Total[0]/sqrt(Total[6]);   //optimize S/sqrt(ZW background only)
	//	if (Total[1] > 0.0) SoverB = Total[6]/Total[1];   //optimize error on ZZ:  ZW/ZZ 
        if (xi2 < bestxi2[t] && xi2 != 0.0) {
	  bestxi2[t] = xi2;
	  bestcut[t] = c;
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
      bestcutvalue = -1.0 + bestcut[t]*(2.0/ncut);

      cout << "****************************************************************************" << endl;
      cout << " background type " << eventType[t+1] << " best xi = " << bestxi2[t] << " for a cut at " << bestcutvalue << endl;
      cout << "****************************************************************************" << endl;

      outClientFile << "****************************************************************************" << endl;
      outClientFile << " background type " << eventType[t+1] << " best xi = " << bestxi2[t] << " for a cut at " << bestcutvalue << " (c = " << bestcut[t] << ")"<< endl;
      outClientFile << "****************************************************************************" << endl;


    } //end if on excluding ZZ->tautaununu
  } // end of for loop over background types
  
  // done with statistics; display the histograms
  
  
  cout << endl;
  cout << "All done!" << endl;
  
  theApp.Run();                            // concluding ROOT requirements.
  
  return 0;
} //  End of main(...)



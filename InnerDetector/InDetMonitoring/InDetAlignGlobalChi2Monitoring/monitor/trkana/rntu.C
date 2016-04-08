/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
TFile *f0;
TTree *RecoTrk;
TTree *RHawNtu;
TTree *RHawMod;
TTree *GlblNtu;
TTree *GlblSummary;
TTree *TrkTrackNtu;
TTree *CBNTupl;
TTree *SiClusterNtu;
TTree *TrkMatchingNtu;
TString *InputMonitorFile;
TString *DriverMacro;

void rntu(char nname[80]=NULL, bool doMonitor=false)
{

  if (nname == NULL) return;

  f0 = new TFile(nname);
  bool GoodNtu = 0;
  
  //Try to get the ntuple of TrkValidation.root
  RecoTrk = (TTree*)f0->Get("FitterValidation/OutputTrack");
  if (RecoTrk != NULL) {
    GoodNtu = 1; 
    cout << " ** loading TrkValidation ntuple ** "<< endl;
    }
  //Or release 13
  if (!GoodNtu) {
    RecoTrk = (TTree*)f0->Get("Validation/Tracks");
    if (RecoTrk != NULL) {
      GoodNtu = 1; 
      cout << " ** loading TrkValidation ntuple ** " << endl;
    }
  }
  
  // If not, try Richard's ntuple
  if(!GoodNtu) {
     RHawNtu = (TTree*)f0->Get("IDAL/100");
     if (RHawNtu != NULL) {
       GoodNtu = 1;
       cout << " ** loading Richard's ntuple ** "<< endl;
       // we get a pointer to module ntuple
       RHawMod = (TTree*)f0->Get("IDAL/110");
     }
     if (RHawMod != NULL) { 
       GoodNtu = 1; 
       cout << " ** loading idalign ntuple ** "<< endl;
     }
  } 
  
  // If not, try GlobalChi2 ntuple
   if(!GoodNtu) {
     GlblNtu = (TTree*)f0->Get("GlobalChi2/General");
     TrkTrackNtu = (TTree*)f0->Get("TrkTrack");
     SiClusterNtu = (TTree*)f0->Get("SiCluster");
     TrkMatchingNtu = (TTree*)f0->Get("TrkTrack_Matching");
     if (GlblNtu != NULL) {
       GoodNtu = 1; 
       cout << " - GlobalX2 ntuple loaded "<< endl;
     }
     GlblSummary = (TTree*)f0->Get("GlobalChi2/Summary");
     if (GlblSummary != NULL)
     cout << "  - Summary branch also loaded "<< endl;
   }
   
   // If not, try CBNT ntuple
   if(!GoodNtu) {
      CBNTupl = (TTree*)f0->Get("CBNT/tree");
      if (CBNTupl != NULL) {
	GoodNtu = 1; 
      cout << " ** loading CBNT ntuple ** "<< endl;
      }
   }

   //If not, return an error
   if (!GoodNtu) {
     cout << " ** rntu ** ERROR ** no ntuple found " << endl;
   }


   // keep the input file in a TString
   InputMonitorFile = new TString(nname);

   // driver Macro
   if (doMonitor)
     DriverMacro = new TString("monitor");
   else
     DriverMacro = new TString("rntu");

   DriverMacro->ToUpper();
}

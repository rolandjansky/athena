/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Jul 20 23:41:21 2011 by ROOT version 5.28/00e
// from TTree PixelRIOs/Clusters
// found on file: root://castoratlas//castor/cern.ch/grid/atlas/atlasscratchdisk/user.lorenzi/user.lorenzi.T1.TRKVAL.NNtrain.clustersOnTrack.110701122007_sub022943841/user.lorenzi.000160.TRKVAL._00027.root
//////////////////////////////////////////////////////////

#ifndef RIOs_h
#define RIOs_h
#include <iostream>
#include <stdio.h>
#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
using namespace std;

class RIOs {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           PixelEventNumber;
   Int_t           PixelClusNum;
   Int_t           LVL1TriggerType;
   vector<float>   *PixClusLocX;
   vector<float>   *PixClusLocY;
   vector<float>   *PixClusLocXcentroid;
   vector<float>   *PixClusLocYcentroid;
   vector<float>   *PixClusLocX_LorentzCorrection;
   vector<float>   *PixClusGloX;
   vector<float>   *PixClusGloY;
   vector<float>   *PixClusGloZ;
   vector<float>   *PixClusEta;
   vector<float>   *PixClusPhi;
   vector<float>   *PixECBarrel;
   vector<float>   *PixClusLayer;
   vector<float>   *PixEtaModule;
   vector<float>   *PixPhiModule;
   vector<float>   *PixClusGroupsize;
   vector<float>   *PixClusRow;
   vector<float>   *PixClusCol;
   vector<int>     *PixDeltaRow;
   vector<int>     *PixDeltaCol;
   vector<float>   *PixOmegaPhi;
   vector<float>   *PixOmegaEta;
   vector<int>     *PixClusToT;
   vector<float>   *PixClusCharge;
   vector<int>     *PixClusLvl1;
   vector<int>     *PixClusGanged;
   vector<int>     *PixClusFake;
   vector<int>     *PixClusAmbiguous;
   vector<int>     *PixClusIsSplit;
   vector<float>   *PixClusSplitProb1;
   vector<float>   *PixClusSplitProb2;
   vector<float>   *PixClusErrX;
   vector<float>   *PixClusErrY;
   Int_t           PixelHitsNum;
   vector<float>   *PixHitXstartpos;
   vector<float>   *PixHitYstartpos;
   vector<float>   *PixHitZstartpos;
   vector<float>   *PixHitXendpos;
   vector<float>   *PixHitYendpos;
   vector<float>   *PixHitZendpos;
   vector<int>     *PixHitBarrelEndcap;
   vector<int>     *PixHitLayerDisk;
   vector<int>     *PixHitEtaModule;
   vector<int>     *PixHitPhiModule;
   vector<int>     *PixHitEtaIndex;
   vector<int>     *PixHitPhiIndex;
   vector<float>   *PixHitAngle;
   vector<float>   *PixHitAngle2;
   vector<float>   *PixHitAngle_Lorcorr;
   vector<float>   *PixHitEnergyLoss;
   vector<float>   *PixHitTime;
   vector<int>     *PixHitPDGParticle;
   vector<int>     *PixHitFlag;

   // List of branches
   TBranch        *b_event_number;   //!
   TBranch        *b_number_cluster_per_event;   //!
   TBranch        *b_type_of_trigger;   //!
   TBranch        *b_PixClusLocX;   //!
   TBranch        *b_PixClusLocY;   //!
   TBranch        *b_PixClusLocXcentroid;   //!
   TBranch        *b_PixClusLocYcentroid;   //!
   TBranch        *b_PixClusLocX_LorentzCorrection;   //!
   TBranch        *b_PixClusGloX;   //!
   TBranch        *b_PixClusGloY;   //!
   TBranch        *b_PixClusGloZ;   //!
   TBranch        *b_PixClusEta;   //!
   TBranch        *b_PixClusPhi;   //!
   TBranch        *b_PixECBarrel;   //!
   TBranch        *b_PixClusLayer;   //!
   TBranch        *b_PixEtaModule;   //!
   TBranch        *b_PixPhiModule;   //!
   TBranch        *b_PixClusGroupsize;   //!
   TBranch        *b_PixClusRow;   //!
   TBranch        *b_PixClusCol;   //!
   TBranch        *b_PixDeltaRow;   //!
   TBranch        *b_PixDeltaCol;   //!
   TBranch        *b_PixOmegaPhi;   //!
   TBranch        *b_PixOmegaEta;   //!
   TBranch        *b_PixClusToT;   //!
   TBranch        *b_PixClusCharge;   //!
   TBranch        *b_PixClusLvl1;   //!
   TBranch        *b_PixClusGanged;   //!
   TBranch        *b_PixClusFake;   //!
   TBranch        *b_PixClusAmbiguous;   //!
   TBranch        *b_PixClusIsSplit;   //!
   TBranch        *b_PixClusSplitProb1;   //!
   TBranch        *b_PixClusSplitProb2;   //!
   TBranch        *b_PixClusErrX;   //!
   TBranch        *b_PixClusErrY;   //!
   TBranch        *b_number_hits_per_event;   //!
   TBranch        *b_PixHitXstartpos;   //!
   TBranch        *b_PixHitYstartpos;   //!
   TBranch        *b_PixHitZstartpos;   //!
   TBranch        *b_PixHitXendpos;   //!
   TBranch        *b_PixHitYendpos;   //!
   TBranch        *b_PixHitZendpos;   //!
   TBranch        *b_PixHitBarrelEndcap;   //!
   TBranch        *b_PixHitLayerDisk;   //!
   TBranch        *b_PixHitEtaModule;   //!
   TBranch        *b_PixHitPhiModule;   //!
   TBranch        *b_PixHitEtaIndex;   //!
   TBranch        *b_PixHitPhiIndex;   //!
   TBranch        *b_PixHitAngle;   //!
   TBranch        *b_PixHitAngle2;   //!
   TBranch        *b_PixHitAngle_Lorcorr;   //!
   TBranch        *b_PixHitEnergyLoss;   //!
   TBranch        *b_PixHitTime;   //!
   TBranch        *b_PixHitPDGParticle;   //!
   TBranch        *b_PixHitFlag;   //!

   RIOs(TTree *tree=0);
   virtual ~RIOs();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef RIOs_cxx
RIOs::RIOs(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("root://castoratlas//castor/cern.ch/grid/atlas/atlasscratchdisk/user.lorenzi/user.lorenzi.T1.TRKVAL.NNtrain.clustersOnTrack.110701122007_sub022943841/user.lorenzi.000160.TRKVAL._00027.root");
      if (!f) {
         f = new TFile("root://castoratlas//castor/cern.ch/grid/atlas/atlasscratchdisk/user.lorenzi/user.lorenzi.T1.TRKVAL.NNtrain.clustersOnTrack.110701122007_sub022943841/user.lorenzi.000160.TRKVAL._00027.root");
      }
      tree = (TTree*)gDirectory->Get("PixelRIOs");

   }
   Init(tree);
}

RIOs::~RIOs()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t RIOs::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t RIOs::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (!fChain->InheritsFrom(TChain::Class()))  return centry;
   TChain *chain = (TChain*)fChain;
   if (chain->GetTreeNumber() != fCurrent) {
      fCurrent = chain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void RIOs::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).
cout << "Init RIOs" << endl; 
   // Set object pointer
   PixClusLocX = 0;
   PixClusLocY = 0;
   PixClusLocXcentroid = 0;
   PixClusLocYcentroid = 0;
   PixClusLocX_LorentzCorrection = 0;
   PixClusGloX = 0;
   PixClusGloY = 0;
   PixClusGloZ = 0;
   PixClusEta = 0;
   PixClusPhi = 0;
   PixECBarrel = 0;
   PixClusLayer = 0;
   PixEtaModule = 0;
   PixPhiModule = 0;
   PixClusGroupsize = 0;
   PixClusRow = 0;
   PixClusCol = 0;
   PixDeltaRow = 0;
   PixDeltaCol = 0;
   PixOmegaPhi = 0;
   PixOmegaEta = 0;
   PixClusToT = 0;
   PixClusCharge = 0;
   PixClusLvl1 = 0;
   PixClusGanged = 0;
   PixClusFake = 0;
   PixClusAmbiguous = 0;
   PixClusIsSplit = 0;
   PixClusSplitProb1 = 0;
   PixClusSplitProb2 = 0;
   PixClusErrX = 0;
   PixClusErrY = 0;
   PixHitXstartpos = 0;
   PixHitYstartpos = 0;
   PixHitZstartpos = 0;
   PixHitXendpos = 0;
   PixHitYendpos = 0;
   PixHitZendpos = 0;
   PixHitBarrelEndcap = 0;
   PixHitLayerDisk = 0;
   PixHitEtaModule = 0;
   PixHitPhiModule = 0;
   PixHitEtaIndex = 0;
   PixHitPhiIndex = 0;
   PixHitAngle = 0;
   PixHitAngle2 = 0;
   PixHitAngle_Lorcorr = 0;
   PixHitEnergyLoss = 0;
   PixHitTime = 0;
   PixHitPDGParticle = 0;
   PixHitFlag = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("PixelEventNumber", &PixelEventNumber, &b_event_number);
   fChain->SetBranchAddress("PixelClusNum", &PixelClusNum, &b_number_cluster_per_event);
   fChain->SetBranchAddress("LVL1TriggerType", &LVL1TriggerType, &b_type_of_trigger);
   fChain->SetBranchAddress("PixClusLocX", &PixClusLocX, &b_PixClusLocX);
   fChain->SetBranchAddress("PixClusLocY", &PixClusLocY, &b_PixClusLocY);
   fChain->SetBranchAddress("PixClusLocXcentroid", &PixClusLocXcentroid, &b_PixClusLocXcentroid);
   fChain->SetBranchAddress("PixClusLocYcentroid", &PixClusLocYcentroid, &b_PixClusLocYcentroid);
   fChain->SetBranchAddress("PixClusLocX_LorentzCorrection", &PixClusLocX_LorentzCorrection, &b_PixClusLocX_LorentzCorrection);
   fChain->SetBranchAddress("PixClusGloX", &PixClusGloX, &b_PixClusGloX);
   fChain->SetBranchAddress("PixClusGloY", &PixClusGloY, &b_PixClusGloY);
   fChain->SetBranchAddress("PixClusGloZ", &PixClusGloZ, &b_PixClusGloZ);
   fChain->SetBranchAddress("PixClusEta", &PixClusEta, &b_PixClusEta);
   fChain->SetBranchAddress("PixClusPhi", &PixClusPhi, &b_PixClusPhi);
   fChain->SetBranchAddress("PixECBarrel", &PixECBarrel, &b_PixECBarrel);
   fChain->SetBranchAddress("PixClusLayer", &PixClusLayer, &b_PixClusLayer);
   fChain->SetBranchAddress("PixEtaModule", &PixEtaModule, &b_PixEtaModule);
   fChain->SetBranchAddress("PixPhiModule", &PixPhiModule, &b_PixPhiModule);
   fChain->SetBranchAddress("PixClusGroupsize", &PixClusGroupsize, &b_PixClusGroupsize);
   fChain->SetBranchAddress("PixClusRow", &PixClusRow, &b_PixClusRow);
   fChain->SetBranchAddress("PixClusCol", &PixClusCol, &b_PixClusCol);
   fChain->SetBranchAddress("PixDeltaRow", &PixDeltaRow, &b_PixDeltaRow);
   fChain->SetBranchAddress("PixDeltaCol", &PixDeltaCol, &b_PixDeltaCol);
   fChain->SetBranchAddress("PixOmegaPhi", &PixOmegaPhi, &b_PixOmegaPhi);
   fChain->SetBranchAddress("PixOmegaEta", &PixOmegaEta, &b_PixOmegaEta);
   fChain->SetBranchAddress("PixClusToT", &PixClusToT, &b_PixClusToT);
   fChain->SetBranchAddress("PixClusCharge", &PixClusCharge, &b_PixClusCharge);
   fChain->SetBranchAddress("PixClusLvl1", &PixClusLvl1, &b_PixClusLvl1);
   fChain->SetBranchAddress("PixClusGanged", &PixClusGanged, &b_PixClusGanged);
   fChain->SetBranchAddress("PixClusFake", &PixClusFake, &b_PixClusFake);
   fChain->SetBranchAddress("PixClusAmbiguous", &PixClusAmbiguous, &b_PixClusAmbiguous);
   fChain->SetBranchAddress("PixClusIsSplit", &PixClusIsSplit, &b_PixClusIsSplit);
   fChain->SetBranchAddress("PixClusSplitProb1", &PixClusSplitProb1, &b_PixClusSplitProb1);
   fChain->SetBranchAddress("PixClusSplitProb2", &PixClusSplitProb2, &b_PixClusSplitProb2);
   fChain->SetBranchAddress("PixClusErrX", &PixClusErrX, &b_PixClusErrX);
   fChain->SetBranchAddress("PixClusErrY", &PixClusErrY, &b_PixClusErrY);
   fChain->SetBranchAddress("PixelHitsNum", &PixelHitsNum, &b_number_hits_per_event);
   fChain->SetBranchAddress("PixHitXstartpos", &PixHitXstartpos, &b_PixHitXstartpos);
   fChain->SetBranchAddress("PixHitYstartpos", &PixHitYstartpos, &b_PixHitYstartpos);
   fChain->SetBranchAddress("PixHitZstartpos", &PixHitZstartpos, &b_PixHitZstartpos);
   fChain->SetBranchAddress("PixHitXendpos", &PixHitXendpos, &b_PixHitXendpos);
   fChain->SetBranchAddress("PixHitYendpos", &PixHitYendpos, &b_PixHitYendpos);
   fChain->SetBranchAddress("PixHitZendpos", &PixHitZendpos, &b_PixHitZendpos);
   fChain->SetBranchAddress("PixHitBarrelEndcap", &PixHitBarrelEndcap, &b_PixHitBarrelEndcap);
   fChain->SetBranchAddress("PixHitLayerDisk", &PixHitLayerDisk, &b_PixHitLayerDisk);
   fChain->SetBranchAddress("PixHitEtaModule", &PixHitEtaModule, &b_PixHitEtaModule);
   fChain->SetBranchAddress("PixHitPhiModule", &PixHitPhiModule, &b_PixHitPhiModule);
   fChain->SetBranchAddress("PixHitEtaIndex", &PixHitEtaIndex, &b_PixHitEtaIndex);
   fChain->SetBranchAddress("PixHitPhiIndex", &PixHitPhiIndex, &b_PixHitPhiIndex);
   fChain->SetBranchAddress("PixHitAngle", &PixHitAngle, &b_PixHitAngle);
   fChain->SetBranchAddress("PixHitAngle2", &PixHitAngle2, &b_PixHitAngle2);
   fChain->SetBranchAddress("PixHitAngle_Lorcorr", &PixHitAngle_Lorcorr, &b_PixHitAngle_Lorcorr);
   fChain->SetBranchAddress("PixHitEnergyLoss", &PixHitEnergyLoss, &b_PixHitEnergyLoss);
   fChain->SetBranchAddress("PixHitTime", &PixHitTime, &b_PixHitTime);
   fChain->SetBranchAddress("PixHitPDGParticle", &PixHitPDGParticle, &b_PixHitPDGParticle);
   fChain->SetBranchAddress("PixHitFlag", &PixHitFlag, &b_PixHitFlag);
   Notify();
}

Bool_t RIOs::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void RIOs::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t RIOs::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef RIOs_cxx

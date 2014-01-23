/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed May 19 21:21:33 2010 by ROOT version 5.26/00
// from TTree L2_MU4_DiMu_Details/L2_MU4_DiMu_Details
// found on file: user10.DanielScheirich.03.data10_7TeV_MuonswBeam.00153565.TrigDiMuonCalib.bphys.root
//////////////////////////////////////////////////////////

#ifndef Details_h
#define Details_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class Details {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           RunNumber;
   Int_t           LumiBlock;
   Int_t           EvtNumber;
   Double_t        RoiEta;
   Double_t        RoiPhi;
   Double_t        Pt;
   Double_t        Eta;
   Double_t        Phi;
   Double_t        Charge;
   Double_t        InnerBarrel_extEta;
   Double_t        InnerBarrel_extPhi;
   Double_t        InnerEndcap_extEta;
   Double_t        InnerEndcap_extPhi;
   Double_t        MiddleBarrel_extEta;
   Double_t        MiddleBarrel_extPhi;
   Double_t        MiddleEndcap_extEta;
   Double_t        MiddleEndcap_extPhi;
   Int_t           NAssociatedMdt;
   Int_t           NAssociatedRpcEta;
   Int_t           NAssociatedRpcPhi;
   Int_t           NAssociatedTgcRho;
   Int_t           NAssociatedTgcPhi;
   vector<double>  *InnerMdt_r;
   vector<double>  *InnerMdt_z;
   vector<bool>    *InnerMdt_associated;
   vector<double>  *MiddleMdt_r;
   vector<double>  *MiddleMdt_z;
   vector<bool>    *MiddleMdt_associated;
   vector<double>  *MiddleRpc_etaPad_x;
   vector<double>  *MiddleRpc_etaPad_y;
   vector<double>  *MiddleRpc_etaPad_z;
   vector<double>  *MiddleRpc_etaPad_r;
   vector<bool>    *MiddleRpc_etaPad_associated;
   vector<double>  *MiddleRpc_phiPad_x;
   vector<double>  *MiddleRpc_phiPad_y;
   vector<double>  *MiddleRpc_phiPad_z;
   vector<double>  *MiddleRpc_phiPad_r;
   vector<bool>    *MiddleRpc_phiPad_associated;
   vector<double>  *InnerTgc_phiHit_phi;
   vector<double>  *InnerTgc_phiHit_r;
   vector<double>  *InnerTgc_phiHit_z;
   vector<double>  *InnerTgc_phiHit_width;
   vector<bool>    *InnerTgc_phiHit_associated;
   vector<double>  *InnerTgc_rhoHit_phi;
   vector<double>  *InnerTgc_rhoHit_r;
   vector<double>  *InnerTgc_rhoHit_z;
   vector<double>  *InnerTgc_rhoHit_width;
   vector<bool>    *InnerTgc_rhoHit_associated;
   vector<double>  *MiddleTgc_phiHit_phi;
   vector<double>  *MiddleTgc_phiHit_r;
   vector<double>  *MiddleTgc_phiHit_z;
   vector<double>  *MiddleTgc_phiHit_width;
   vector<bool>    *MiddleTgc_phiHit_associated;
   vector<double>  *MiddleTgc_rhoHit_phi;
   vector<double>  *MiddleTgc_rhoHit_r;
   vector<double>  *MiddleTgc_rhoHit_z;
   vector<double>  *MiddleTgc_rhoHit_width;
   vector<bool>    *MiddleTgc_rhoHit_associated;
   vector<int>     *BTrigIndex;
   vector<int>     *BWhichTrack;
   vector<int>     *BTrigMatched;
   vector<int>     *BWhichTrackMatched;

   // List of branches
   TBranch        *b_RunNumber;   //!
   TBranch        *b_LumiBlock;   //!
   TBranch        *b_EvtNumber;   //!
   TBranch        *b_RoiEta;   //!
   TBranch        *b_RoiPhi;   //!
   TBranch        *b_Pt;   //!
   TBranch        *b_Eta;   //!
   TBranch        *b_Phi;   //!
   TBranch        *b_Charge;   //!
   TBranch        *b_InnerBarrel_extEta;   //!
   TBranch        *b_InnerBarrel_extPhi;   //!
   TBranch        *b_InnerEndcap_extEta;   //!
   TBranch        *b_InnerEndcap_extPhi;   //!
   TBranch        *b_MiddleBarrel_extEta;   //!
   TBranch        *b_MiddleBarrel_extPhi;   //!
   TBranch        *b_MiddleEndcap_extEta;   //!
   TBranch        *b_MiddleEndcap_extPhi;   //!
   TBranch        *b_NAssociatedMdt;   //!
   TBranch        *b_NAssociatedRpcEta;   //!
   TBranch        *b_NAssociatedRpcPhi;   //!
   TBranch        *b_NAssociatedTgcRho;   //!
   TBranch        *b_NAssociatedTgcPhi;   //!
   TBranch        *b_InnerMdt_r;   //!
   TBranch        *b_InnerMdt_z;   //!
   TBranch        *b_InnerMdt_associated;   //!
   TBranch        *b_MiddleMdt_r;   //!
   TBranch        *b_MiddleMdt_z;   //!
   TBranch        *b_MiddleMdt_associated;   //!
   TBranch        *b_MiddleRpc_etaPad_x;   //!
   TBranch        *b_MiddleRpc_etaPad_y;   //!
   TBranch        *b_MiddleRpc_etaPad_z;   //!
   TBranch        *b_MiddleRpc_etaPad_r;   //!
   TBranch        *b_MiddleRpc_etaPad_associated;   //!
   TBranch        *b_MiddleRpc_phiPad_x;   //!
   TBranch        *b_MiddleRpc_phiPad_y;   //!
   TBranch        *b_MiddleRpc_phiPad_z;   //!
   TBranch        *b_MiddleRpc_phiPad_r;   //!
   TBranch        *b_MiddleRpc_phiPad_associated;   //!
   TBranch        *b_InnerTgc_phiHit_phi;   //!
   TBranch        *b_InnerTgc_phiHit_r;   //!
   TBranch        *b_InnerTgc_phiHit_z;   //!
   TBranch        *b_InnerTgc_phiHit_width;   //!
   TBranch        *b_InnerTgc_phiHit_associated;   //!
   TBranch        *b_InnerTgc_rhoHit_phi;   //!
   TBranch        *b_InnerTgc_rhoHit_r;   //!
   TBranch        *b_InnerTgc_rhoHit_z;   //!
   TBranch        *b_InnerTgc_rhoHit_width;   //!
   TBranch        *b_InnerTgc_rhoHit_associated;   //!
   TBranch        *b_MiddleTgc_phiHit_phi;   //!
   TBranch        *b_MiddleTgc_phiHit_r;   //!
   TBranch        *b_MiddleTgc_phiHit_z;   //!
   TBranch        *b_MiddleTgc_phiHit_width;   //!
   TBranch        *b_MiddleTgc_phiHit_associated;   //!
   TBranch        *b_MiddleTgc_rhoHit_phi;   //!
   TBranch        *b_MiddleTgc_rhoHit_r;   //!
   TBranch        *b_MiddleTgc_rhoHit_z;   //!
   TBranch        *b_MiddleTgc_rhoHit_width;   //!
   TBranch        *b_MiddleTgc_rhoHit_associated;   //!
   TBranch        *b_BTrigIndex;   //!
   TBranch        *b_BWhichTrack;   //!
   TBranch        *b_BTrigMatched;   //!
   TBranch        *b_BWhichTrackMatched;   //!

   Details(TTree *tree=0);
   Int_t    GetEntry(Long64_t entry);
   void     Init(TTree *tree);
};

Details::Details(TTree *tree)
{
   Init(tree);
}

Int_t Details::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
void Details::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   InnerMdt_r = 0;
   InnerMdt_z = 0;
   InnerMdt_associated = 0;
   MiddleMdt_r = 0;
   MiddleMdt_z = 0;
   MiddleMdt_associated = 0;
   MiddleRpc_etaPad_x = 0;
   MiddleRpc_etaPad_y = 0;
   MiddleRpc_etaPad_z = 0;
   MiddleRpc_etaPad_r = 0;
   MiddleRpc_etaPad_associated = 0;
   MiddleRpc_phiPad_x = 0;
   MiddleRpc_phiPad_y = 0;
   MiddleRpc_phiPad_z = 0;
   MiddleRpc_phiPad_r = 0;
   MiddleRpc_phiPad_associated = 0;
   InnerTgc_phiHit_phi = 0;
   InnerTgc_phiHit_r = 0;
   InnerTgc_phiHit_z = 0;
   InnerTgc_phiHit_width = 0;
   InnerTgc_phiHit_associated = 0;
   InnerTgc_rhoHit_phi = 0;
   InnerTgc_rhoHit_r = 0;
   InnerTgc_rhoHit_z = 0;
   InnerTgc_rhoHit_width = 0;
   InnerTgc_rhoHit_associated = 0;
   MiddleTgc_phiHit_phi = 0;
   MiddleTgc_phiHit_r = 0;
   MiddleTgc_phiHit_z = 0;
   MiddleTgc_phiHit_width = 0;
   MiddleTgc_phiHit_associated = 0;
   MiddleTgc_rhoHit_phi = 0;
   MiddleTgc_rhoHit_r = 0;
   MiddleTgc_rhoHit_z = 0;
   MiddleTgc_rhoHit_width = 0;
   MiddleTgc_rhoHit_associated = 0;
   BTrigIndex = 0;
   BWhichTrack = 0;
   BTrigMatched = 0;
   BWhichTrackMatched = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("RunNumber", &RunNumber, &b_RunNumber);
   fChain->SetBranchAddress("LumiBlock", &LumiBlock, &b_LumiBlock);
   fChain->SetBranchAddress("EvtNumber", &EvtNumber, &b_EvtNumber);
   fChain->SetBranchAddress("RoiEta", &RoiEta, &b_RoiEta);
   fChain->SetBranchAddress("RoiPhi", &RoiPhi, &b_RoiPhi);
   fChain->SetBranchAddress("Pt", &Pt, &b_Pt);
   fChain->SetBranchAddress("Eta", &Eta, &b_Eta);
   fChain->SetBranchAddress("Phi", &Phi, &b_Phi);
   fChain->SetBranchAddress("Charge", &Charge, &b_Charge);
   fChain->SetBranchAddress("InnerBarrel_extEta", &InnerBarrel_extEta, &b_InnerBarrel_extEta);
   fChain->SetBranchAddress("InnerBarrel_extPhi", &InnerBarrel_extPhi, &b_InnerBarrel_extPhi);
   fChain->SetBranchAddress("InnerEndcap_extEta", &InnerEndcap_extEta, &b_InnerEndcap_extEta);
   fChain->SetBranchAddress("InnerEndcap_extPhi", &InnerEndcap_extPhi, &b_InnerEndcap_extPhi);
   fChain->SetBranchAddress("MiddleBarrel_extEta", &MiddleBarrel_extEta, &b_MiddleBarrel_extEta);
   fChain->SetBranchAddress("MiddleBarrel_extPhi", &MiddleBarrel_extPhi, &b_MiddleBarrel_extPhi);
   fChain->SetBranchAddress("MiddleEndcap_extEta", &MiddleEndcap_extEta, &b_MiddleEndcap_extEta);
   fChain->SetBranchAddress("MiddleEndcap_extPhi", &MiddleEndcap_extPhi, &b_MiddleEndcap_extPhi);
   fChain->SetBranchAddress("NAssociatedMdt", &NAssociatedMdt, &b_NAssociatedMdt);
   fChain->SetBranchAddress("NAssociatedRpcEta", &NAssociatedRpcEta, &b_NAssociatedRpcEta);
   fChain->SetBranchAddress("NAssociatedRpcPhi", &NAssociatedRpcPhi, &b_NAssociatedRpcPhi);
   fChain->SetBranchAddress("NAssociatedTgcRho", &NAssociatedTgcRho, &b_NAssociatedTgcRho);
   fChain->SetBranchAddress("NAssociatedTgcPhi", &NAssociatedTgcPhi, &b_NAssociatedTgcPhi);
   fChain->SetBranchAddress("InnerMdt_r", &InnerMdt_r, &b_InnerMdt_r);
   fChain->SetBranchAddress("InnerMdt_z", &InnerMdt_z, &b_InnerMdt_z);
   fChain->SetBranchAddress("InnerMdt_associated", &InnerMdt_associated, &b_InnerMdt_associated);
   fChain->SetBranchAddress("MiddleMdt_r", &MiddleMdt_r, &b_MiddleMdt_r);
   fChain->SetBranchAddress("MiddleMdt_z", &MiddleMdt_z, &b_MiddleMdt_z);
   fChain->SetBranchAddress("MiddleMdt_associated", &MiddleMdt_associated, &b_MiddleMdt_associated);
   fChain->SetBranchAddress("MiddleRpc_etaPad_x", &MiddleRpc_etaPad_x, &b_MiddleRpc_etaPad_x);
   fChain->SetBranchAddress("MiddleRpc_etaPad_y", &MiddleRpc_etaPad_y, &b_MiddleRpc_etaPad_y);
   fChain->SetBranchAddress("MiddleRpc_etaPad_z", &MiddleRpc_etaPad_z, &b_MiddleRpc_etaPad_z);
   fChain->SetBranchAddress("MiddleRpc_etaPad_r", &MiddleRpc_etaPad_r, &b_MiddleRpc_etaPad_r);
   fChain->SetBranchAddress("MiddleRpc_etaPad_associated", &MiddleRpc_etaPad_associated, &b_MiddleRpc_etaPad_associated);
   fChain->SetBranchAddress("MiddleRpc_phiPad_x", &MiddleRpc_phiPad_x, &b_MiddleRpc_phiPad_x);
   fChain->SetBranchAddress("MiddleRpc_phiPad_y", &MiddleRpc_phiPad_y, &b_MiddleRpc_phiPad_y);
   fChain->SetBranchAddress("MiddleRpc_phiPad_z", &MiddleRpc_phiPad_z, &b_MiddleRpc_phiPad_z);
   fChain->SetBranchAddress("MiddleRpc_phiPad_r", &MiddleRpc_phiPad_r, &b_MiddleRpc_phiPad_r);
   fChain->SetBranchAddress("MiddleRpc_phiPad_associated", &MiddleRpc_phiPad_associated, &b_MiddleRpc_phiPad_associated);
   fChain->SetBranchAddress("InnerTgc_phiHit_phi", &InnerTgc_phiHit_phi, &b_InnerTgc_phiHit_phi);
   fChain->SetBranchAddress("InnerTgc_phiHit_r", &InnerTgc_phiHit_r, &b_InnerTgc_phiHit_r);
   fChain->SetBranchAddress("InnerTgc_phiHit_z", &InnerTgc_phiHit_z, &b_InnerTgc_phiHit_z);
   fChain->SetBranchAddress("InnerTgc_phiHit_width", &InnerTgc_phiHit_width, &b_InnerTgc_phiHit_width);
   fChain->SetBranchAddress("InnerTgc_phiHit_associated", &InnerTgc_phiHit_associated, &b_InnerTgc_phiHit_associated);
   fChain->SetBranchAddress("InnerTgc_rhoHit_phi", &InnerTgc_rhoHit_phi, &b_InnerTgc_rhoHit_phi);
   fChain->SetBranchAddress("InnerTgc_rhoHit_r", &InnerTgc_rhoHit_r, &b_InnerTgc_rhoHit_r);
   fChain->SetBranchAddress("InnerTgc_rhoHit_z", &InnerTgc_rhoHit_z, &b_InnerTgc_rhoHit_z);
   fChain->SetBranchAddress("InnerTgc_rhoHit_width", &InnerTgc_rhoHit_width, &b_InnerTgc_rhoHit_width);
   fChain->SetBranchAddress("InnerTgc_rhoHit_associated", &InnerTgc_rhoHit_associated, &b_InnerTgc_rhoHit_associated);
   fChain->SetBranchAddress("MiddleTgc_phiHit_phi", &MiddleTgc_phiHit_phi, &b_MiddleTgc_phiHit_phi);
   fChain->SetBranchAddress("MiddleTgc_phiHit_r", &MiddleTgc_phiHit_r, &b_MiddleTgc_phiHit_r);
   fChain->SetBranchAddress("MiddleTgc_phiHit_z", &MiddleTgc_phiHit_z, &b_MiddleTgc_phiHit_z);
   fChain->SetBranchAddress("MiddleTgc_phiHit_width", &MiddleTgc_phiHit_width, &b_MiddleTgc_phiHit_width);
   fChain->SetBranchAddress("MiddleTgc_phiHit_associated", &MiddleTgc_phiHit_associated, &b_MiddleTgc_phiHit_associated);
   fChain->SetBranchAddress("MiddleTgc_rhoHit_phi", &MiddleTgc_rhoHit_phi, &b_MiddleTgc_rhoHit_phi);
   fChain->SetBranchAddress("MiddleTgc_rhoHit_r", &MiddleTgc_rhoHit_r, &b_MiddleTgc_rhoHit_r);
   fChain->SetBranchAddress("MiddleTgc_rhoHit_z", &MiddleTgc_rhoHit_z, &b_MiddleTgc_rhoHit_z);
   fChain->SetBranchAddress("MiddleTgc_rhoHit_width", &MiddleTgc_rhoHit_width, &b_MiddleTgc_rhoHit_width);
   fChain->SetBranchAddress("MiddleTgc_rhoHit_associated", &MiddleTgc_rhoHit_associated, &b_MiddleTgc_rhoHit_associated);
   fChain->SetBranchAddress("BTrigIndex", &BTrigIndex, &b_BTrigIndex);
   fChain->SetBranchAddress("BWhichTrack", &BWhichTrack, &b_BWhichTrack);
   fChain->SetBranchAddress("BTrigMatched", &BTrigMatched, &b_BTrigMatched);
   fChain->SetBranchAddress("BWhichTrackMatched", &BWhichTrackMatched, &b_BWhichTrackMatched);
}

#endif

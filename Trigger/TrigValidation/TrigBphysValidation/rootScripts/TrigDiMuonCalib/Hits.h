/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Jun 15 15:03:53 2010 by ROOT version 5.26/00
// from TTree MuidMuonCollectionHits/MuidMuonCollectionHits
// found on file: user10.DanielScheirich.03.data10_7TeV_MuonswBeam.00153565.TrigDiMuonCalib.bphys.root
//////////////////////////////////////////////////////////

#ifndef Hits_h
#define Hits_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class Hits {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           RunNumber;
   Int_t           LumiBlock;
   Int_t           EventNumber;
   Int_t           MuonIndex;
   vector<double>  *InnerMdt_eta;
   vector<double>  *InnerMdt_phi;
   vector<double>  *MiddleMdt_eta;
   vector<double>  *MiddleMdt_phi;
   vector<double>  *Csc_eta;
   vector<double>  *Csc_phi;
   vector<int>     *Csc_type;
   vector<double>  *MiddleRpc_eta;
   vector<double>  *MiddleRpc_phi;
   vector<int>     *MiddleRpc_type;
   vector<double>  *InnerTgc_eta;
   vector<double>  *InnerTgc_phi;
   vector<int>     *InnerTgc_type;
   vector<double>  *MiddleTgc_eta;
   vector<double>  *MiddleTgc_phi;
   vector<int>     *MiddleTgc_type;

   // List of branches
   TBranch        *b_RunNumber;   //!
   TBranch        *b_LumiBlock;   //!
   TBranch        *b_EventNumber;   //!
   TBranch        *b_MuonIndex;   //!
   TBranch        *b_InnerMdt_eta;   //!
   TBranch        *b_InnerMdt_phi;   //!
   TBranch        *b_MiddleMdt_eta;   //!
   TBranch        *b_MiddleMdt_phi;   //!
   TBranch        *b_Csc_eta;   //!
   TBranch        *b_Csc_phi;   //!
   TBranch        *b_Csc_type;   //!
   TBranch        *b_MiddleRpc_eta;   //!
   TBranch        *b_MiddleRpc_phi;   //!
   TBranch        *b_MiddleRpc_type;   //!
   TBranch        *b_InnerTgc_eta;   //!
   TBranch        *b_InnerTgc_phi;   //!
   TBranch        *b_InnerTgc_type;   //!
   TBranch        *b_MiddleTgc_eta;   //!
   TBranch        *b_MiddleTgc_phi;   //!
   TBranch        *b_MiddleTgc_type;   //!

   Hits(TTree *tree=0);
   virtual ~Hits();
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
};

Hits::Hits(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("user10.DanielScheirich.03.data10_7TeV_MuonswBeam.00153565.TrigDiMuonCalib.bphys.root");
      if (!f) {
         f = new TFile("user10.DanielScheirich.03.data10_7TeV_MuonswBeam.00153565.TrigDiMuonCalib.bphys.root");
      }
      tree = (TTree*)gDirectory->Get("MuidMuonCollectionHits");

   }
   Init(tree);
}

Hits::~Hits()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t Hits::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t Hits::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (!fChain->InheritsFrom(TChain::Class()))  return centry;
   TChain *chain = (TChain*)fChain;
   if (chain->GetTreeNumber() != fCurrent) {
      fCurrent = chain->GetTreeNumber();
   }
   return centry;
}

void Hits::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   InnerMdt_eta = 0;
   InnerMdt_phi = 0;
   MiddleMdt_eta = 0;
   MiddleMdt_phi = 0;
   Csc_eta = 0;
   Csc_phi = 0;
   Csc_type = 0;
   MiddleRpc_eta = 0;
   MiddleRpc_phi = 0;
   MiddleRpc_type = 0;
   InnerTgc_eta = 0;
   InnerTgc_phi = 0;
   InnerTgc_type = 0;
   MiddleTgc_eta = 0;
   MiddleTgc_phi = 0;
   MiddleTgc_type = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

    fChain->SetBranchAddress("RunNumber", &RunNumber, &b_RunNumber);
    fChain->SetBranchAddress("LumiBlock", &LumiBlock, &b_LumiBlock);
    fChain->SetBranchAddress("EventNumber", &EventNumber, &b_EventNumber);
    fChain->SetBranchAddress("MuonIndex", &MuonIndex, &b_MuonIndex);
    fChain->SetBranchAddress("InnerMdt_eta", &InnerMdt_eta, &b_InnerMdt_eta);
    fChain->SetBranchAddress("InnerMdt_phi", &InnerMdt_phi, &b_InnerMdt_phi);
    fChain->SetBranchAddress("MiddleMdt_eta", &MiddleMdt_eta, &b_MiddleMdt_eta);
    fChain->SetBranchAddress("MiddleMdt_phi", &MiddleMdt_phi, &b_MiddleMdt_phi);
    fChain->SetBranchAddress("Csc_eta", &Csc_eta, &b_Csc_eta);
    fChain->SetBranchAddress("Csc_phi", &Csc_phi, &b_Csc_phi);
    fChain->SetBranchAddress("Csc_type", &Csc_type, &b_Csc_type);
    fChain->SetBranchAddress("MiddleRpc_eta", &MiddleRpc_eta, &b_MiddleRpc_eta);
    fChain->SetBranchAddress("MiddleRpc_phi", &MiddleRpc_phi, &b_MiddleRpc_phi);
    fChain->SetBranchAddress("MiddleRpc_type", &MiddleRpc_type, &b_MiddleRpc_type);
    fChain->SetBranchAddress("InnerTgc_eta", &InnerTgc_eta, &b_InnerTgc_eta);
    fChain->SetBranchAddress("InnerTgc_phi", &InnerTgc_phi, &b_InnerTgc_phi);
    fChain->SetBranchAddress("InnerTgc_type", &InnerTgc_type, &b_InnerTgc_type);
    fChain->SetBranchAddress("MiddleTgc_eta", &MiddleTgc_eta, &b_MiddleTgc_eta);
    fChain->SetBranchAddress("MiddleTgc_type", &MiddleTgc_type, &b_MiddleTgc_type);
    fChain->SetBranchAddress("MiddleTgc_phi", &MiddleTgc_phi, &b_MiddleTgc_phi);
}

#endif

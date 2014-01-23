/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Jun 17 11:34:54 2010 by ROOT version 5.26/00
// from TTree TrigDiMuonCalibTree/TrigDiMuonCalib tree
// found on file: Calib.01.root
//////////////////////////////////////////////////////////

#ifndef Calib_h
#define Calib_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class Calib {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           RunNumber;
   Int_t           LumiBlock;
   Int_t           EventNumber;
   Double_t        Pt;
   Double_t        Eta;
   Double_t        Phi;
   Double_t        Charge;
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
   TBranch        *b_Pt;   //!
   TBranch        *b_Eta;   //!
   TBranch        *b_Phi;   //!
   TBranch        *b_Charge;   //!
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

   Calib(TTree *tree=0);
   virtual ~Calib();
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
};

Calib::Calib(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("Calib.01.root");
      if (!f) {
         f = new TFile("Calib.01.root");
      }
      tree = (TTree*)gDirectory->Get("TrigDiMuonCalibTree");

   }
   Init(tree);
}

Calib::~Calib()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t Calib::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t Calib::LoadTree(Long64_t entry)
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

void Calib::Init(TTree *tree)
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
   fChain->SetBranchAddress("Pt", &Pt, &b_Pt);
   fChain->SetBranchAddress("Eta", &Eta, &b_Eta);
   fChain->SetBranchAddress("Phi", &Phi, &b_Phi);
   fChain->SetBranchAddress("Charge", &Charge, &b_Charge);
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
   fChain->SetBranchAddress("MiddleTgc_phi", &MiddleTgc_phi, &b_MiddleTgc_phi);
   fChain->SetBranchAddress("MiddleTgc_type", &MiddleTgc_type, &b_MiddleTgc_type);
}


#endif // #ifdef Calib_cxx

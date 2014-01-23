/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Jun 21 17:13:13 2010 by ROOT version 5.26/00
// from TTree Jpsi/Jpsi tree
// found on file: L2_MU4_DiMu_FS.root
//////////////////////////////////////////////////////////

#ifndef SimpleJpsi_h
#define SimpleJpsi_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class SimpleJpsi {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           runNumber;
   Int_t           lumiBlock;
   Int_t           eventNumber;
   Double_t        mass;
   Double_t        pt;
   Double_t        eta;
   Double_t        phi;
   vector<int>     *trigIndex;
   Double_t        trk0_pt;
   Double_t        trk0_eta;
   Double_t        trk0_phi;
   Double_t        trk0_charge;
   Double_t        trk1_pt;
   Double_t        trk1_eta;
   Double_t        trk1_phi;
   Double_t        trk1_charge;
   Double_t        matchTrk0_pt;
   Double_t        matchTrk0_eta;
   Double_t        matchTrk0_phi;
   Double_t        matchTrk0_charge;
   Bool_t          matchTrk0_valid;
   Double_t        matchTrk1_pt;
   Double_t        matchTrk1_eta;
   Double_t        matchTrk1_phi;
   Double_t        matchTrk1_charge;
   Bool_t          matchTrk1_valid;

   // List of branches
   TBranch        *b_runNumber;   //!
   TBranch        *b_lumiBlock;   //!
   TBranch        *b_eventNumber;   //!
   TBranch        *b_mass;   //!
   TBranch        *b_pt;   //!
   TBranch        *b_eta;   //!
   TBranch        *b_phi;   //!
   TBranch        *b_trigIndex;   //!
   TBranch        *b_trk0_pt;   //!
   TBranch        *b_trk0_eta;   //!
   TBranch        *b_trk0_phi;   //!
   TBranch        *b_trk0_charge;   //!
   TBranch        *b_trk1_pt;   //!
   TBranch        *b_trk1_eta;   //!
   TBranch        *b_trk1_phi;   //!
   TBranch        *b_trk1_charge;   //!
   TBranch        *b_matchTrk0_pt;   //!
   TBranch        *b_matchTrk0_eta;   //!
   TBranch        *b_matchTrk0_phi;   //!
   TBranch        *b_matchTrk0_charge;   //!
   TBranch        *b_matchTrk0_valid;   //!
   TBranch        *b_matchTrk1_pt;   //!
   TBranch        *b_matchTrk1_eta;   //!
   TBranch        *b_matchTrk1_phi;   //!
   TBranch        *b_matchTrk1_charge;   //!
   TBranch        *b_matchTrk1_valid;   //!

   SimpleJpsi(TTree *tree=0);
   virtual ~SimpleJpsi();
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
};

SimpleJpsi::SimpleJpsi(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("L2_MU4_DiMu_FS.root");
      if (!f) {
         f = new TFile("L2_MU4_DiMu_FS.root");
      }
      tree = (TTree*)gDirectory->Get("Jpsi");

   }
   Init(tree);
}

SimpleJpsi::~SimpleJpsi()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t SimpleJpsi::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t SimpleJpsi::LoadTree(Long64_t entry)
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

void SimpleJpsi::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   trigIndex = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("runNumber", &runNumber, &b_runNumber);
   fChain->SetBranchAddress("lumiBlock", &lumiBlock, &b_lumiBlock);
   fChain->SetBranchAddress("eventNumber", &eventNumber, &b_eventNumber);
   fChain->SetBranchAddress("mass", &mass, &b_mass);
   fChain->SetBranchAddress("pt", &pt, &b_pt);
   fChain->SetBranchAddress("eta", &eta, &b_eta);
   fChain->SetBranchAddress("phi", &phi, &b_phi);
   fChain->SetBranchAddress("trigIndex", &trigIndex, &b_trigIndex);
   fChain->SetBranchAddress("trk0_pt", &trk0_pt, &b_trk0_pt);
   fChain->SetBranchAddress("trk0_eta", &trk0_eta, &b_trk0_eta);
   fChain->SetBranchAddress("trk0_phi", &trk0_phi, &b_trk0_phi);
   fChain->SetBranchAddress("trk0_charge", &trk0_charge, &b_trk0_charge);
   fChain->SetBranchAddress("trk1_pt", &trk1_pt, &b_trk1_pt);
   fChain->SetBranchAddress("trk1_eta", &trk1_eta, &b_trk1_eta);
   fChain->SetBranchAddress("trk1_phi", &trk1_phi, &b_trk1_phi);
   fChain->SetBranchAddress("trk1_charge", &trk1_charge, &b_trk1_charge);
   fChain->SetBranchAddress("matchTrk0_pt", &matchTrk0_pt, &b_matchTrk0_pt);
   fChain->SetBranchAddress("matchTrk0_eta", &matchTrk0_eta, &b_matchTrk0_eta);
   fChain->SetBranchAddress("matchTrk0_phi", &matchTrk0_phi, &b_matchTrk0_phi);
   fChain->SetBranchAddress("matchTrk0_charge", &matchTrk0_charge, &b_matchTrk0_charge);
   fChain->SetBranchAddress("matchTrk0_valid", &matchTrk0_valid, &b_matchTrk0_valid);
   fChain->SetBranchAddress("matchTrk1_pt", &matchTrk1_pt, &b_matchTrk1_pt);
   fChain->SetBranchAddress("matchTrk1_eta", &matchTrk1_eta, &b_matchTrk1_eta);
   fChain->SetBranchAddress("matchTrk1_phi", &matchTrk1_phi, &b_matchTrk1_phi);
   fChain->SetBranchAddress("matchTrk1_charge", &matchTrk1_charge, &b_matchTrk1_charge);
   fChain->SetBranchAddress("matchTrk1_valid", &matchTrk1_valid, &b_matchTrk1_valid);
}

#endif // #ifdef SimpleJpsi_cxx

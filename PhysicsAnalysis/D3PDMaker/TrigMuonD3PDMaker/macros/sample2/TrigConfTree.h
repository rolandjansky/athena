/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Aug 17 01:16:17 2010 by ROOT version 5.22/00j
// from TChain triggerMeta/TrigConfTree/
//////////////////////////////////////////////////////////

#ifndef TrigConfTree_h
#define TrigConfTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class TrigConfTree {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           SMK;
   Int_t           L1PSK;
   Int_t           HLTPSK;
   std::map<std::string, int>* LVL1NameMap;
   std::map<std::string, float>* LVL1PrescaleMap;
   std::map<std::string, int>* HLTNameMap;
   std::map<std::string, float>* HLTPrescaleMap;

   // List of branches
   TBranch        *b_SMK;   //!
   TBranch        *b_L1PSK;   //!
   TBranch        *b_HLTPSK;   //!


   TrigConfTree(TTree *tree=0);
   virtual ~TrigConfTree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef TrigConfTree_cxx
TrigConfTree::TrigConfTree(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) { 
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("trig_muon_d3pd_level0.root");
      if (!f) {
         f = new TFile("trig_muon_d3pd_level0.root");
      }
      tree = (TTree*)gDirectory->Get("triggerMeta/TrigConfTree");
   
   }

   Init(tree);
}

TrigConfTree::~TrigConfTree()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t TrigConfTree::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t TrigConfTree::LoadTree(Long64_t entry)
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

void TrigConfTree::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;

   LVL1NameMap = 0;
   LVL1PrescaleMap = 0;
   HLTNameMap = 0;
   HLTPrescaleMap = 0;

   fChain->SetBranchAddress("SMK", &SMK, &b_SMK);
   fChain->SetBranchAddress("L1PSK", &L1PSK, &b_L1PSK);
   fChain->SetBranchAddress("HLTPSK", &HLTPSK, &b_HLTPSK);
   fChain->SetBranchAddress("LVL1NameMap", &LVL1NameMap);
   //fChain->SetBranchAddress("LVL1NameMap", &LVL1NameMap_, &b__);
   //fChain->SetBranchAddress("LVL1NameMap.first", LVL1NameMap_first, &b_LVL1NameMap_first);
   //fChain->SetBranchAddress("LVL1NameMap.second", LVL1NameMap_second, &b_LVL1NameMap_second);
   fChain->SetBranchAddress("LVL1PrescaleMap", &LVL1PrescaleMap);
   //fChain->SetBranchAddress("LVL1PrescaleMap", &LVL1PrescaleMap_, &b__);
   //fChain->SetBranchAddress("LVL1PrescaleMap.first", LVL1PrescaleMap_first, &b_LVL1PrescaleMap_first);
   //fChain->SetBranchAddress("LVL1PrescaleMap.second", LVL1PrescaleMap_second, &b_LVL1PrescaleMap_second);
   fChain->SetBranchAddress("HLTNameMap", &HLTNameMap);
   //fChain->SetBranchAddress("HLTNameMap", &HLTNameMap_, &b__);
   //fChain->SetBranchAddress("HLTNameMap.first", HLTNameMap_first, &b_HLTNameMap_first);
   //fChain->SetBranchAddress("HLTNameMap.second", HLTNameMap_second, &b_HLTNameMap_second);
   fChain->SetBranchAddress("HLTPrescaleMap", &HLTPrescaleMap);
   //fChain->SetBranchAddress("HLTPrescaleMap", &HLTPrescaleMap_, &b__);
   //fChain->SetBranchAddress("HLTPrescaleMap.first", HLTPrescaleMap_first, &b_HLTPrescaleMap_first);
   //fChain->SetBranchAddress("HLTPrescaleMap.second", HLTPrescaleMap_second, &b_HLTPrescaleMap_second);

   Notify();
}

Bool_t TrigConfTree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void TrigConfTree::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t TrigConfTree::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef TrigConfTree_cxx

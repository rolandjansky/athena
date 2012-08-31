/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Dec  3 18:24:58 2009 by ROOT version 5.22/00d
// from TTree L1_Cluster/L1 Trigger Cluster
// found on file: trigntupleTTBAR.root
//////////////////////////////////////////////////////////

#ifndef L1_Cluster_h
#define L1_Cluster_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class L1_Cluster {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   UInt_t          L1ROI_nClus;
   vector<float>   *L1ROI_eta;
   vector<float>   *L1ROI_phi;
   vector<float>   *L1ROI_Core;
   vector<float>   *L1ROI_EMet;
   vector<float>   *L1ROI_EMiso;
   vector<float>   *L1ROI_Hadiso;
   vector<float>   *L1ROI_Hadcore;
   vector<unsigned int> *L1ROI_ROIword;
   vector<vector<string> > *L1ROI_trigChain;
   vector<vector<int> > *L1ROI_trigDec;

   // List of branches
   TBranch        *b_L1ROI_nClus;   //!
   TBranch        *b_L1ROI_eta;   //!
   TBranch        *b_L1ROI_phi;   //!
   TBranch        *b_L1ROI_Core;   //!
   TBranch        *b_L1ROI_EMet;   //!
   TBranch        *b_L1ROI_EMiso;   //!
   TBranch        *b_L1ROI_Hadiso;   //!
   TBranch        *b_L1ROI_Hadcore;   //!
   TBranch        *b_L1ROI_ROIword;   //!
   TBranch        *b_L1ROI_trigChain;   //!
   TBranch        *b_L1ROI_trigDec;   //!

   L1_Cluster(TTree *tree=0);
   virtual ~L1_Cluster();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef L1_Cluster_cxx
L1_Cluster::L1_Cluster(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("trigntupleTTBAR.root");
      if (!f) {
         f = new TFile("trigntupleTTBAR.root");
         f->cd("trigntupleTTBAR.root:/EGamma");
      }
      tree = (TTree*)gDirectory->Get("L1_Cluster");

   }
   Init(tree);
}

L1_Cluster::~L1_Cluster()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t L1_Cluster::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t L1_Cluster::LoadTree(Long64_t entry)
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

void L1_Cluster::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   L1ROI_eta = 0;
   L1ROI_phi = 0;
   L1ROI_Core = 0;
   L1ROI_EMet = 0;
   L1ROI_EMiso = 0;
   L1ROI_Hadiso = 0;
   L1ROI_Hadcore = 0;
   L1ROI_ROIword = 0;
   L1ROI_trigChain = 0;
   L1ROI_trigDec = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("L1ROI_nClus", &L1ROI_nClus, &b_L1ROI_nClus);
   fChain->SetBranchAddress("L1ROI_eta", &L1ROI_eta, &b_L1ROI_eta);
   fChain->SetBranchAddress("L1ROI_phi", &L1ROI_phi, &b_L1ROI_phi);
   fChain->SetBranchAddress("L1ROI_Core", &L1ROI_Core, &b_L1ROI_Core);
   fChain->SetBranchAddress("L1ROI_EMet", &L1ROI_EMet, &b_L1ROI_EMet);
   fChain->SetBranchAddress("L1ROI_EMiso", &L1ROI_EMiso, &b_L1ROI_EMiso);
   fChain->SetBranchAddress("L1ROI_Hadiso", &L1ROI_Hadiso, &b_L1ROI_Hadiso);
   fChain->SetBranchAddress("L1ROI_Hadcore", &L1ROI_Hadcore, &b_L1ROI_Hadcore);
   fChain->SetBranchAddress("L1ROI_ROIword", &L1ROI_ROIword, &b_L1ROI_ROIword);
   fChain->SetBranchAddress("L1ROI_trigChain", &L1ROI_trigChain, &b_L1ROI_trigChain);
   fChain->SetBranchAddress("L1ROI_trigDec", &L1ROI_trigDec, &b_L1ROI_trigDec);
   Notify();
}

Bool_t L1_Cluster::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void L1_Cluster::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t L1_Cluster::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef L1_Cluster_cxx

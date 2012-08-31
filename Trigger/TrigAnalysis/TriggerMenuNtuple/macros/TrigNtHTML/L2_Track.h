/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Dec  3 18:25:19 2009 by ROOT version 5.22/00d
// from TTree L2_Track/L2 Trigger InDetTrack
// found on file: trigntupleTTBAR.root
//////////////////////////////////////////////////////////

#ifndef L2_Track_h
#define L2_Track_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class L2_Track {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   UInt_t          TrigInDetTrack_nTrack;
   vector<int>     *TrigInDetTrack_AlgoId;
   vector<float>   *TrigInDetTrack_chi2;
   vector<string>  *TrigInDetTrack_slice;
   vector<int>     *TrigInDetTrack_NStrawHits;
   vector<int>     *TrigInDetTrack_NStraw;
   vector<int>     *TrigInDetTrack_NStrawTime;
   vector<int>     *TrigInDetTrack_NTRHits;
   vector<int>     *TrigInDetTrack_NSiSpacePoints;
   vector<float>   *TrigInDetTrack_a0;
   vector<float>   *TrigInDetTrack_z0;
   vector<float>   *TrigInDetTrack_phi0;
   vector<float>   *TrigInDetTrack_eta;
   vector<float>   *TrigInDetTrack_pT;
   vector<float>   *TrigInDetTrack_ea0;
   vector<float>   *TrigInDetTrack_ez0;
   vector<float>   *TrigInDetTrack_ephi0;
   vector<float>   *TrigInDetTrack_eeta;
   vector<float>   *TrigInDetTrack_epT;
   vector<float>   *TrigInDetTrack_end_a0;
   vector<float>   *TrigInDetTrack_end_z0;
   vector<float>   *TrigInDetTrack_end_phi0;
   vector<float>   *TrigInDetTrack_end_eta;
   vector<float>   *TrigInDetTrack_end_pT;
   vector<float>   *TrigInDetTrack_end_ea0;
   vector<float>   *TrigInDetTrack_end_ez0;
   vector<float>   *TrigInDetTrack_end_ephi0;
   vector<float>   *TrigInDetTrack_end_eeta;
   vector<float>   *TrigInDetTrack_end_epT;
   vector<vector<string> > *TrigInDetTrack_trigChain;
   vector<vector<int> > *TrigInDetTrack_trigDec;

   // List of branches
   TBranch        *b_TrigInDetTrack_nTrack;   //!
   TBranch        *b_TrigInDetTrack_AlgoId;   //!
   TBranch        *b_TrigInDetTrack_chi2;   //!
   TBranch        *b_TrigInDetTrack_slice;   //!
   TBranch        *b_TrigInDetTrack_NStrawHits;   //!
   TBranch        *b_TrigInDetTrack_NStraw;   //!
   TBranch        *b_TrigInDetTrack_NStrawTime;   //!
   TBranch        *b_TrigInDetTrack_NTRHits;   //!
   TBranch        *b_TrigInDetTrack_NSiSpacePoints;   //!
   TBranch        *b_TrigInDetTrack_a0;   //!
   TBranch        *b_TrigInDetTrack_z0;   //!
   TBranch        *b_TrigInDetTrack_phi0;   //!
   TBranch        *b_TrigInDetTrack_eta;   //!
   TBranch        *b_TrigInDetTrack_pT;   //!
   TBranch        *b_TrigInDetTrack_ea0;   //!
   TBranch        *b_TrigInDetTrack_ez0;   //!
   TBranch        *b_TrigInDetTrack_ephi0;   //!
   TBranch        *b_TrigInDetTrack_eeta;   //!
   TBranch        *b_TrigInDetTrack_epT;   //!
   TBranch        *b_TrigInDetTrack_end_a0;   //!
   TBranch        *b_TrigInDetTrack_end_z0;   //!
   TBranch        *b_TrigInDetTrack_end_phi0;   //!
   TBranch        *b_TrigInDetTrack_end_eta;   //!
   TBranch        *b_TrigInDetTrack_end_pT;   //!
   TBranch        *b_TrigInDetTrack_end_ea0;   //!
   TBranch        *b_TrigInDetTrack_end_ez0;   //!
   TBranch        *b_TrigInDetTrack_end_ephi0;   //!
   TBranch        *b_TrigInDetTrack_end_eeta;   //!
   TBranch        *b_TrigInDetTrack_end_epT;   //!
   TBranch        *b_TrigInDetTrack_trigChain;   //!
   TBranch        *b_TrigInDetTrack_trigDec;   //!

   L2_Track(TTree *tree=0);
   virtual ~L2_Track();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef L2_Track_cxx
L2_Track::L2_Track(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("trigntupleTTBAR.root");
      if (!f) {
         f = new TFile("trigntupleTTBAR.root");
         f->cd("trigntupleTTBAR.root:/EGamma");
      }
      tree = (TTree*)gDirectory->Get("L2_Track");

   }
   Init(tree);
}

L2_Track::~L2_Track()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t L2_Track::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t L2_Track::LoadTree(Long64_t entry)
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

void L2_Track::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   TrigInDetTrack_AlgoId = 0;
   TrigInDetTrack_chi2 = 0;
   TrigInDetTrack_slice = 0;
   TrigInDetTrack_NStrawHits = 0;
   TrigInDetTrack_NStraw = 0;
   TrigInDetTrack_NStrawTime = 0;
   TrigInDetTrack_NTRHits = 0;
   TrigInDetTrack_NSiSpacePoints = 0;
   TrigInDetTrack_a0 = 0;
   TrigInDetTrack_z0 = 0;
   TrigInDetTrack_phi0 = 0;
   TrigInDetTrack_eta = 0;
   TrigInDetTrack_pT = 0;
   TrigInDetTrack_ea0 = 0;
   TrigInDetTrack_ez0 = 0;
   TrigInDetTrack_ephi0 = 0;
   TrigInDetTrack_eeta = 0;
   TrigInDetTrack_epT = 0;
   TrigInDetTrack_end_a0 = 0;
   TrigInDetTrack_end_z0 = 0;
   TrigInDetTrack_end_phi0 = 0;
   TrigInDetTrack_end_eta = 0;
   TrigInDetTrack_end_pT = 0;
   TrigInDetTrack_end_ea0 = 0;
   TrigInDetTrack_end_ez0 = 0;
   TrigInDetTrack_end_ephi0 = 0;
   TrigInDetTrack_end_eeta = 0;
   TrigInDetTrack_end_epT = 0;
   TrigInDetTrack_trigChain = 0;
   TrigInDetTrack_trigDec = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("TrigInDetTrack_nTrack", &TrigInDetTrack_nTrack, &b_TrigInDetTrack_nTrack);
   fChain->SetBranchAddress("TrigInDetTrack_AlgoId", &TrigInDetTrack_AlgoId, &b_TrigInDetTrack_AlgoId);
   fChain->SetBranchAddress("TrigInDetTrack_chi2", &TrigInDetTrack_chi2, &b_TrigInDetTrack_chi2);
   fChain->SetBranchAddress("TrigInDetTrack_slice", &TrigInDetTrack_slice, &b_TrigInDetTrack_slice);
   fChain->SetBranchAddress("TrigInDetTrack_NStrawHits", &TrigInDetTrack_NStrawHits, &b_TrigInDetTrack_NStrawHits);
   fChain->SetBranchAddress("TrigInDetTrack_NStraw", &TrigInDetTrack_NStraw, &b_TrigInDetTrack_NStraw);
   fChain->SetBranchAddress("TrigInDetTrack_NStrawTime", &TrigInDetTrack_NStrawTime, &b_TrigInDetTrack_NStrawTime);
   fChain->SetBranchAddress("TrigInDetTrack_NTRHits", &TrigInDetTrack_NTRHits, &b_TrigInDetTrack_NTRHits);
   fChain->SetBranchAddress("TrigInDetTrack_NSiSpacePoints", &TrigInDetTrack_NSiSpacePoints, &b_TrigInDetTrack_NSiSpacePoints);
   fChain->SetBranchAddress("TrigInDetTrack_a0", &TrigInDetTrack_a0, &b_TrigInDetTrack_a0);
   fChain->SetBranchAddress("TrigInDetTrack_z0", &TrigInDetTrack_z0, &b_TrigInDetTrack_z0);
   fChain->SetBranchAddress("TrigInDetTrack_phi0", &TrigInDetTrack_phi0, &b_TrigInDetTrack_phi0);
   fChain->SetBranchAddress("TrigInDetTrack_eta", &TrigInDetTrack_eta, &b_TrigInDetTrack_eta);
   fChain->SetBranchAddress("TrigInDetTrack_pT", &TrigInDetTrack_pT, &b_TrigInDetTrack_pT);
   fChain->SetBranchAddress("TrigInDetTrack_ea0", &TrigInDetTrack_ea0, &b_TrigInDetTrack_ea0);
   fChain->SetBranchAddress("TrigInDetTrack_ez0", &TrigInDetTrack_ez0, &b_TrigInDetTrack_ez0);
   fChain->SetBranchAddress("TrigInDetTrack_ephi0", &TrigInDetTrack_ephi0, &b_TrigInDetTrack_ephi0);
   fChain->SetBranchAddress("TrigInDetTrack_eeta", &TrigInDetTrack_eeta, &b_TrigInDetTrack_eeta);
   fChain->SetBranchAddress("TrigInDetTrack_epT", &TrigInDetTrack_epT, &b_TrigInDetTrack_epT);
   fChain->SetBranchAddress("TrigInDetTrack_end_a0", &TrigInDetTrack_end_a0, &b_TrigInDetTrack_end_a0);
   fChain->SetBranchAddress("TrigInDetTrack_end_z0", &TrigInDetTrack_end_z0, &b_TrigInDetTrack_end_z0);
   fChain->SetBranchAddress("TrigInDetTrack_end_phi0", &TrigInDetTrack_end_phi0, &b_TrigInDetTrack_end_phi0);
   fChain->SetBranchAddress("TrigInDetTrack_end_eta", &TrigInDetTrack_end_eta, &b_TrigInDetTrack_end_eta);
   fChain->SetBranchAddress("TrigInDetTrack_end_pT", &TrigInDetTrack_end_pT, &b_TrigInDetTrack_end_pT);
   fChain->SetBranchAddress("TrigInDetTrack_end_ea0", &TrigInDetTrack_end_ea0, &b_TrigInDetTrack_end_ea0);
   fChain->SetBranchAddress("TrigInDetTrack_end_ez0", &TrigInDetTrack_end_ez0, &b_TrigInDetTrack_end_ez0);
   fChain->SetBranchAddress("TrigInDetTrack_end_ephi0", &TrigInDetTrack_end_ephi0, &b_TrigInDetTrack_end_ephi0);
   fChain->SetBranchAddress("TrigInDetTrack_end_eeta", &TrigInDetTrack_end_eeta, &b_TrigInDetTrack_end_eeta);
   fChain->SetBranchAddress("TrigInDetTrack_end_epT", &TrigInDetTrack_end_epT, &b_TrigInDetTrack_end_epT);
   fChain->SetBranchAddress("TrigInDetTrack_trigChain", &TrigInDetTrack_trigChain, &b_TrigInDetTrack_trigChain);
   fChain->SetBranchAddress("TrigInDetTrack_trigDec", &TrigInDetTrack_trigDec, &b_TrigInDetTrack_trigDec);
   Notify();
}

Bool_t L2_Track::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void L2_Track::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t L2_Track::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef L2_Track_cxx

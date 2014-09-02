/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu May 14 14:24:07 2009 by ROOT version 5.22/00a
// from TTree Vertices/Primary Vertices
// found on file: beamspot.root
//////////////////////////////////////////////////////////

#ifndef vrtTree_h
#define vrtTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class vrtTree {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   UInt_t          event_run;
   UInt_t          event_event;
   UInt_t          event_lb;
   UInt_t          event_tck;
   ULong64_t       id;
   Double_t        vrt_x;
   Double_t        vrt_y;
   Double_t        vrt_z;
   Double_t        vrt_vxx;
   Double_t        vrt_vxy;
   Double_t        vrt_vyy;
   Double_t        vrt_vxz;
   Double_t        vrt_vyz;
   Double_t        vrt_vzz;
   Double_t        vrt_chi2;
   Double_t        vrt_ndf;
   Int_t           vrt_vType;
   Int_t           qual_nTracks;
   Int_t           qual_passed;

   // List of branches
   TBranch        *b_event;   //!
   TBranch        *b_id;   //!
   TBranch        *b_vrt;   //!
   TBranch        *b_qual;   //!

   vrtTree(TTree *tree=0);
   virtual ~vrtTree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef vrtTree_cxx
vrtTree::vrtTree(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("beamspot.root");
      if (!f) {
         f = new TFile("beamspot.root");
         f->cd("beamspot.root:/Beamspot");
      }
      tree = (TTree*)gDirectory->Get("Vertices");

   }
   Init(tree);
}

vrtTree::~vrtTree()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t vrtTree::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t vrtTree::LoadTree(Long64_t entry)
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

void vrtTree::Init(TTree *tree)
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
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("event", &event_run, &b_event);
   fChain->SetBranchAddress("id", &id, &b_id);
   fChain->SetBranchAddress("vrt", &vrt_x, &b_vrt);
   fChain->SetBranchAddress("qual", &qual_nTracks, &b_qual);
   Notify();
}

Bool_t vrtTree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void vrtTree::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t vrtTree::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef vrtTree_cxx

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu May 14 14:23:52 2009 by ROOT version 5.22/00a
// from TTree Beamspots/Beamspot Solutions
// found on file: beamspot.root
//////////////////////////////////////////////////////////

#ifndef bsTree_h
#define bsTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class bsTree {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           event_run;
   Int_t           event_id;
   Int_t           event_lumiStart;
   Int_t           event_lumiRange;
   Int_t           event_fitStatus;
   Int_t           event_fitID;
   Int_t           event_nEvents;
   Float_t         bs_x0;
   Float_t         bs_y0;
   Float_t         bs_z;
   Float_t         bs_ax;
   Float_t         bs_ay;
   Float_t         bs_sx;
   Float_t         bs_sy;
   Float_t         bs_sz;
   Float_t         bs_rhoxy;
   Float_t         bs_k;
   Float_t         bsCov_x0x0;
   Float_t         bsCov_x0y0;
   Float_t         bsCov_x0z;
   Float_t         bsCov_x0ax;
   Float_t         bsCov_x0ay;
   Float_t         bsCov_x0sx;
   Float_t         bsCov_x0sy;
   Float_t         bsCov_x0sz;
   Float_t         bsCov_x0rhoxy;
   Float_t         bsCov_x0k;
   Float_t         bsCov_y0y0;
   Float_t         bsCov_y0z;
   Float_t         bsCov_y0ax;
   Float_t         bsCov_y0ay;
   Float_t         bsCov_y0sx;
   Float_t         bsCov_y0sy;
   Float_t         bsCov_y0sz;
   Float_t         bsCov_y0rhoxy;
   Float_t         bsCov_y0k;
   Float_t         bsCov_zz;
   Float_t         bsCov_zax;
   Float_t         bsCov_zay;
   Float_t         bsCov_zsx;
   Float_t         bsCov_zsy;
   Float_t         bsCov_zsz;
   Float_t         bsCov_zrhoxy;
   Float_t         bsCov_zk;
   Float_t         bsCov_axax;
   Float_t         bsCov_axay;
   Float_t         bsCov_axsx;
   Float_t         bsCov_axsy;
   Float_t         bsCov_axsz;
   Float_t         bsCov_axrhoxy;
   Float_t         bsCov_axk;
   Float_t         bsCov_ayay;
   Float_t         bsCov_aysx;
   Float_t         bsCov_aysy;
   Float_t         bsCov_aysz;
   Float_t         bsCov_ayrhoxy;
   Float_t         bsCov_ayk;
   Float_t         bsCov_sxsx;
   Float_t         bsCov_sxsy;
   Float_t         bsCov_sxsz;
   Float_t         bsCov_sxrhoxy;
   Float_t         bsCov_sxk;
   Float_t         bsCov_sysy;
   Float_t         bsCov_sysz;
   Float_t         bsCov_syrhoxy;
   Float_t         bsCov_syk;
   Float_t         bsCov_szsz;
   Float_t         bsCov_szrhoxy;
   Float_t         bsCov_szk;
   Float_t         bsCov_rhoxyrhoxy;
   Float_t         bsCov_rhoxyk;
   Float_t         bsCov_kk;
   Float_t         bsCentroid_xc;
   Float_t         bsCentroid_yc;
   Float_t         bsCentroid_sxc;
   Float_t         bsCentroid_syc;
   Float_t         bsCovCentroid_xcxc;
   Float_t         bsCovCentroid_ycyc;
   Float_t         bsCovCentroid_axcaxc;
   Float_t         bsCovCentroid_aycayc;
   Float_t         bsCovCentroid_sxcsxc;
   Float_t         bsCovCentroid_sycsyc;

   // List of branches
   TBranch        *b_event;   //!
   TBranch        *b_bs;   //!
   TBranch        *b_bsCov;   //!
   TBranch        *b_bsCentroid;   //!
   TBranch        *b_bsCovCentroid;   //!

   bsTree(TTree *tree=0);
   virtual ~bsTree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef bsTree_cxx
bsTree::bsTree(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("beamspot.root");
      if (!f) {
         f = new TFile("beamspot.root");
         f->cd("beamspot.root:/Beamspot");
      }
      tree = (TTree*)gDirectory->Get("Beamspots");

   }
   Init(tree);
}

bsTree::~bsTree()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t bsTree::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t bsTree::LoadTree(Long64_t entry)
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

void bsTree::Init(TTree *tree)
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
   fChain->SetBranchAddress("bs", &bs_x0, &b_bs);
   fChain->SetBranchAddress("bsCov", &bsCov_x0x0, &b_bsCov);
   fChain->SetBranchAddress("bsCentroid", &bsCentroid_xc, &b_bsCentroid);
   fChain->SetBranchAddress("bsCovCentroid", &bsCovCentroid_xcxc, &b_bsCovCentroid);
   Notify();
}

Bool_t bsTree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void bsTree::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t bsTree::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef bsTree_cxx

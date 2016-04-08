/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Feb  8 11:45:23 2008 by ROOT version 5.18/00
// from TTree PerfTreeAll/Performance Tree for All
// found on file: all5212.root
//////////////////////////////////////////////////////////

#ifndef readJFBTagAna_h
#define readJFBTagAna_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class readJFBTagAna {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           Flavour;
   Double_t        DeltaRtoBorCorTau;
   Double_t        Discriminator;
   Double_t        JetMomentum;
   Double_t        JetPt;
   Double_t        JetEta;
   Double_t        JetPhi;
   Int_t           nVTX;
   Int_t           nTracksAtVtx;
   Int_t           nSingleTracks;
   Double_t        energyFraction;
   Double_t        mass;
   Double_t        significance3d;
   Double_t        deltaphi;
   Double_t        deltaeta;

   // List of branches
   TBranch        *b_Flavour;   //!
   TBranch        *b_DeltaRtoBorCorTau;   //!
   TBranch        *b_Discriminator;   //!
   TBranch        *b_JetMomentum;   //!
   TBranch        *b_JetPt;   //!
   TBranch        *b_JetEta;   //!
   TBranch        *b_JetPhi;   //!
   TBranch        *b_nVTX;   //!
   TBranch        *b_nTracksAtVtx;   //!
   TBranch        *b_nSingleTracks;   //!
   TBranch        *b_energyFraction;   //!
   TBranch        *b_mass;   //!
   TBranch        *b_significance3d;   //!
   TBranch        *b_deltaphi;   //!
   TBranch        *b_deltaeta;   //!

   readJFBTagAna(TTree *tree=0);
   virtual ~readJFBTagAna();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef readJFBTagAna_cxx
readJFBTagAna::readJFBTagAna(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("all5212.root");
      if (!f) {
         f = new TFile("all5212.root");
      }
      tree = (TTree*)gDirectory->Get("PerfTreeAll");

   }
   Init(tree);
}

readJFBTagAna::~readJFBTagAna()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t readJFBTagAna::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t readJFBTagAna::LoadTree(Long64_t entry)
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

void readJFBTagAna::Init(TTree *tree)
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

   fChain->SetBranchAddress("Flavour", &Flavour, &b_Flavour);
   fChain->SetBranchAddress("DeltaRtoBorCorTau", &DeltaRtoBorCorTau, &b_DeltaRtoBorCorTau);
   fChain->SetBranchAddress("Discriminator", &Discriminator, &b_Discriminator);
   fChain->SetBranchAddress("JetMomentum", &JetMomentum, &b_JetMomentum);
   fChain->SetBranchAddress("JetPt", &JetPt, &b_JetPt);
   fChain->SetBranchAddress("JetEta", &JetEta, &b_JetEta);
   fChain->SetBranchAddress("JetPhi", &JetPhi, &b_JetPhi);
   fChain->SetBranchAddress("nVTX", &nVTX, &b_nVTX);
   fChain->SetBranchAddress("nTracksAtVtx", &nTracksAtVtx, &b_nTracksAtVtx);
   fChain->SetBranchAddress("nSingleTracks", &nSingleTracks, &b_nSingleTracks);
   fChain->SetBranchAddress("energyFraction", &energyFraction, &b_energyFraction);
   fChain->SetBranchAddress("mass", &mass, &b_mass);
   fChain->SetBranchAddress("significance3d", &significance3d, &b_significance3d);
   fChain->SetBranchAddress("deltaphi", &deltaphi, &b_deltaphi);
   fChain->SetBranchAddress("deltaeta", &deltaeta, &b_deltaeta);
   Notify();
}

Bool_t readJFBTagAna::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void readJFBTagAna::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t readJFBTagAna::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef readJFBTagAna_cxx

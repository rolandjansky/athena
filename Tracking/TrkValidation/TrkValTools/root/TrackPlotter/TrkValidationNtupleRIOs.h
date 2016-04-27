/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sat Mar 29 16:22:45 2008 by ROOT version 5.18/00
// from TTree TRT_RIOs/TRT_DriftCircles
// found on file: TrkValidation2.root
//////////////////////////////////////////////////////////

#ifndef TrkValidationNtupleRIOs_h
#define TrkValidationNtupleRIOs_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <vector>
using namespace std;

#ifdef __CINT__
#pragma link C++ class vector< int >;
#pragma link C++ class vector< float >;
#endif

class TrkValidationNtupleRIOs {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           TRT_EventNumber;
   Int_t           TRT_nRIOs;
   vector<float>   *TRT_DriftRadius;
   vector<float>   *TRT_SurfaceX;
   vector<float>   *TRT_SurfaceY;
   vector<float>   *TRT_SurfaceZ;

   // List of branches
   TBranch        *b_TRT_event_number;   //!
   TBranch        *b_Number_of_RIOs;   //!
   TBranch        *b_TRT_DriftRadius;   //!
   TBranch        *b_TRT_SurfaceX;   //!
   TBranch        *b_TRT_SurfaceY;   //!
   TBranch        *b_TRT_SurfaceZ;   //!

   TrkValidationNtupleRIOs(TTree *tree=0);
   virtual ~TrkValidationNtupleRIOs();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef TrkValidationNtupleRIOs_cxx
TrkValidationNtupleRIOs::TrkValidationNtupleRIOs(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("TrkValidation2.root");
      if (!f) {
         f = new TFile("TrkValidation2.root");
         f->cd("TrkValidation2.root:/Validation");
      }
      tree = (TTree*)gDirectory->Get("TRT_RIOs");

   }
   Init(tree);
}

TrkValidationNtupleRIOs::~TrkValidationNtupleRIOs()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t TrkValidationNtupleRIOs::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t TrkValidationNtupleRIOs::LoadTree(Long64_t entry)
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

void TrkValidationNtupleRIOs::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   TRT_DriftRadius = 0;
   TRT_SurfaceX = 0;
   TRT_SurfaceY = 0;
   TRT_SurfaceZ = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("EventNumber", &TRT_EventNumber, &b_TRT_event_number);
   fChain->SetBranchAddress("TRT_nRIOs", &TRT_nRIOs, &b_Number_of_RIOs);
   fChain->SetBranchAddress("TRT_DriftRadius", &TRT_DriftRadius, &b_TRT_DriftRadius);
   fChain->SetBranchAddress("TRT_SurfaceX", &TRT_SurfaceX, &b_TRT_SurfaceX);
   fChain->SetBranchAddress("TRT_SurfaceY", &TRT_SurfaceY, &b_TRT_SurfaceY);
   fChain->SetBranchAddress("TRT_SurfaceZ", &TRT_SurfaceZ, &b_TRT_SurfaceZ);
   Notify();
}

Bool_t TrkValidationNtupleRIOs::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void TrkValidationNtupleRIOs::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t TrkValidationNtupleRIOs::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef TrkValidationNtupleRIOs_cxx

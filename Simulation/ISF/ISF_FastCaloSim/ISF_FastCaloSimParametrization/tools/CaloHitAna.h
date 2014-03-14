/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Oct 29 15:33:10 2013 by ROOT version 5.34/05
// from TTree CaloHitAna/CaloHitAna
// found on file: ISF_HitAnalysispi.root
//////////////////////////////////////////////////////////

#ifndef CaloHitAna_h
#define CaloHitAna_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include <vector>
#include <vector>
#include <vector>
#include <vector>

// Fixed size dimensions of array or collections stored in the TTree if any.

class CaloHitAna {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain
   TString         ffilename;
   // Declaration of leaf types
   vector<float>   *HitX;
   vector<float>   *HitY;
   vector<float>   *HitZ;
   vector<float>   *HitE;
   vector<float>   *HitTime;
   vector<int>     *NHit;
   vector<string>  *HitIdentifier;
   vector<bool>    *HitIsLArBarrel;
   vector<bool>    *HitIsLArEndCap;
   vector<bool>    *HitIsHEC;
   vector<bool>    *HitIsFCAL;
   vector<bool>    *HitIsTile;
   vector<int>     *HitSampling;
   vector<float>   *HitSamplingFraction;
   vector<float>   *TruthE;
   vector<float>   *TruthPx;
   vector<float>   *TruthPy;
   vector<float>   *TruthPz;
   vector<int>     *TruthPDG;

   // List of branches
   TBranch        *b_HitX;   //!
   TBranch        *b_HitY;   //!
   TBranch        *b_HitZ;   //!
   TBranch        *b_HitE;   //!
   TBranch        *b_HitTime;   //!
   TBranch        *b_NHit;   //!
   TBranch        *b_HitIdentifier;   //!
   TBranch        *b_HitIsLArBarrel;   //!
   TBranch        *b_HitIsLArEndCap;   //!
   TBranch        *b_HitIsHEC;   //!
   TBranch        *b_HitIsFCAL;   //!
   TBranch        *b_HitIsTile;   //!
   TBranch        *b_HitSampling;   //!
   TBranch        *b_HitSamplingFraction;   //!
   TBranch        *b_TruthE;   //!
   TBranch        *b_TruthPx;   //!
   TBranch        *b_TruthPy;   //!
   TBranch        *b_TruthPz;   //!
   TBranch        *b_TruthPDG;   //!

   CaloHitAna(TString filename="ISF_HitAnalysis.root", TTree *tree=0);
   virtual ~CaloHitAna();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef CaloHitAna_cxx
CaloHitAna::CaloHitAna(TString filename, TTree *tree) : fChain(0) 
{
  ffilename=filename;
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(filename);
      if (!f || !f->IsOpen()) {
         f = new TFile(filename);
      }
      TString dirname=filename;
      dirname+=":/ISF_HitAnalysis";
      TDirectory * dir = (TDirectory*)f->Get(dirname);
      dir->GetObject("CaloHitAna",tree);

   }
   Init(tree);
}

CaloHitAna::~CaloHitAna()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t CaloHitAna::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t CaloHitAna::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void CaloHitAna::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   HitX = 0;
   HitY = 0;
   HitZ = 0;
   HitE = 0;
   HitTime = 0;
   NHit = 0;
   HitIdentifier = 0;
   HitIsLArBarrel = 0;
   HitIsLArEndCap = 0;
   HitIsHEC = 0;
   HitIsFCAL = 0;
   HitIsTile = 0;
   HitSampling = 0;
   HitSamplingFraction = 0;
   TruthE = 0;
   TruthPx = 0;
   TruthPy = 0;
   TruthPz = 0;
   TruthPDG = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("HitX", &HitX, &b_HitX);
   fChain->SetBranchAddress("HitY", &HitY, &b_HitY);
   fChain->SetBranchAddress("HitZ", &HitZ, &b_HitZ);
   fChain->SetBranchAddress("HitE", &HitE, &b_HitE);
   fChain->SetBranchAddress("HitTime", &HitTime, &b_HitTime);
   fChain->SetBranchAddress("NHit", &NHit, &b_NHit);
   fChain->SetBranchAddress("HitIdentifier", &HitIdentifier, &b_HitIdentifier);
   fChain->SetBranchAddress("HitIsLArBarrel", &HitIsLArBarrel, &b_HitIsLArBarrel);
   fChain->SetBranchAddress("HitIsLArEndCap", &HitIsLArEndCap, &b_HitIsLArEndCap);
   fChain->SetBranchAddress("HitIsHEC", &HitIsHEC, &b_HitIsHEC);
   fChain->SetBranchAddress("HitIsFCAL", &HitIsFCAL, &b_HitIsFCAL);
   fChain->SetBranchAddress("HitIsTile", &HitIsTile, &b_HitIsTile);
   fChain->SetBranchAddress("HitSampling", &HitSampling, &b_HitSampling);
   fChain->SetBranchAddress("HitSamplingFraction", &HitSamplingFraction, &b_HitSamplingFraction);
   fChain->SetBranchAddress("TruthE", &TruthE, &b_TruthE);
   fChain->SetBranchAddress("TruthPx", &TruthPx, &b_TruthPx);
   fChain->SetBranchAddress("TruthPy", &TruthPy, &b_TruthPy);
   fChain->SetBranchAddress("TruthPz", &TruthPz, &b_TruthPz);
   fChain->SetBranchAddress("TruthPDG", &TruthPDG, &b_TruthPDG);
   Notify();
}

Bool_t CaloHitAna::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void CaloHitAna::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t CaloHitAna::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef CaloHitAna_cxx

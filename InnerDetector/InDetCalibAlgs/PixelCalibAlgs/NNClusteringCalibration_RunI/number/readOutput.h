/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sat Feb 12 00:30:53 2011 by ROOT version 5.27/01
// from TTree clusterTree/clusterTree
// found on file: outputNN.root
//////////////////////////////////////////////////////////

#ifndef readOutput_h
#define readOutput_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class readOutput {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           sizeX;
   Int_t           sizeY;
   Float_t         meanPointX;
   Float_t         meanPointY;
   vector<vector<float> > *matrixOfToT;
   vector<float>   *vectorOfPitchesY;
   Float_t         phi;
   Float_t         theta;
   Float_t         phiBS;
   Float_t         thetaBS;
   Float_t         etaModule;
   Int_t           ClusterPixLayer;
   Int_t           ClusterPixBarrelEC;
   Int_t           nParticles;
   vector<double>  *positionsX;
   vector<double>  *positionsY;

   // List of branches
   TBranch        *b_sizeX;   //!
   TBranch        *b_sizeY;   //!
   TBranch        *b_meanPointX;   //!
   TBranch        *b_meanPointY;   //!
   TBranch        *b_matrixOfToT;   //!
   TBranch        *b_vectorOfPitchesY;   //!
   TBranch        *b_phi;   //!
   TBranch        *b_theta;   //!
   TBranch        *b_phiBS;   //!
   TBranch        *b_thetaBS;   //!
   TBranch        *b_etaModule;   //!
   TBranch        *b_ClusterPixLayer;   //!
   TBranch        *b_ClusterPixBarrelEC;   //!
   TBranch        *b_nParticles;   //!
   TBranch        *b_positionsX;   //!
   TBranch        *b_positionsY;   //!

   readOutput(TTree *tree=0);
   virtual ~readOutput();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef readOutput_cxx
readOutput::readOutput(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("outputNN.root");
      if (!f) {
         f = new TFile("outputNN.root");
      }
      tree = (TTree*)gDirectory->Get("clusterTree");

   }
   Init(tree);
}

readOutput::~readOutput()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t readOutput::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t readOutput::LoadTree(Long64_t entry)
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

void readOutput::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   matrixOfToT = 0;
   vectorOfPitchesY = 0;
   positionsX = 0;
   positionsY = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("sizeX", &sizeX, &b_sizeX);
   fChain->SetBranchAddress("sizeY", &sizeY, &b_sizeY);
   fChain->SetBranchAddress("meanPointX", &meanPointX, &b_meanPointX);
   fChain->SetBranchAddress("meanPointY", &meanPointY, &b_meanPointY);
   fChain->SetBranchAddress("matrixOfToT", &matrixOfToT, &b_matrixOfToT);
   fChain->SetBranchAddress("vectorOfPitchesY", &vectorOfPitchesY, &b_vectorOfPitchesY);
   fChain->SetBranchAddress("phi", &phi, &b_phi);
   fChain->SetBranchAddress("theta", &theta, &b_theta);
   fChain->SetBranchAddress("phiBS", &phiBS, &b_phiBS);
   fChain->SetBranchAddress("thetaBS", &thetaBS, &b_thetaBS);
   fChain->SetBranchAddress("etaModule", &etaModule, &b_etaModule);
   fChain->SetBranchAddress("ClusterPixLayer", &ClusterPixLayer, &b_ClusterPixLayer);
   fChain->SetBranchAddress("ClusterPixBarrelEC", &ClusterPixBarrelEC, &b_ClusterPixBarrelEC);
   fChain->SetBranchAddress("nParticles", &nParticles, &b_nParticles);
   fChain->SetBranchAddress("positionsX", &positionsX, &b_positionsX);
   fChain->SetBranchAddress("positionsY", &positionsY, &b_positionsY);
   Notify();
}

Bool_t readOutput::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void readOutput::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t readOutput::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef readOutput_cxx

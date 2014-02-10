/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Aug 16 16:52:51 2006 by ROOT version 5.10/00d
// from TTree SurfaceValidationTree/TrackingVolume :InDet::Detectors::Pixel::Barrel
// found on file: InDetMaterialG4-ATLAS-DC3-06.root
//////////////////////////////////////////////////////////

#ifndef SurfaceValidationTree_h
#define SurfaceValidationTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class SurfaceValidationTree {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leave types
   Float_t         Local1;
   Float_t         Local2;
   Float_t         CorrFactor;
   Float_t         PathInX0;
   Float_t         Rho;

   // List of branches
   TBranch        *b_local1;   //!
   TBranch        *b_local2;   //!
   TBranch        *b_corrF;   //!
   TBranch        *b_pathInX0;   //!
   TBranch        *b_rho;   //!

   SurfaceValidationTree(TTree *tree=0);
   virtual ~SurfaceValidationTree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop(int bins, TString& ext);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif




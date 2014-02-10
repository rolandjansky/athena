/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sun Aug 13 22:48:31 2006 by ROOT version 5.10/00
//////////////////////////////////////////////////////////

#ifndef LayerValidationTree_h
#define LayerValidationTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class LayerValidationTree {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leave types
   Float_t         DensedHitR;
   Float_t         DensedHitZ;
   Float_t         DensedHitPhi;
   Float_t         DensedHitTheta;
   Float_t         CorrFactor;
   Float_t         PathInX0;
   Float_t         Rho;
   Int_t           Hits;
   Float_t         HitPositionX[100];   //[layerhits]
   Float_t         HitPositionY[100];   //[layerhits]
   Float_t         HitPositionZ[100];   //[layerhits]
   Float_t         HitPositionR[100];   //[layerhits]

   // List of branches
   TBranch        *b_densedR;   //!
   TBranch        *b_densedZ;   //!
   TBranch        *b_densedHitPhi;   //!
   TBranch        *b_dense;   //!
   TBranch        *b_corrF;   //!
   TBranch        *b_pathInX0;   //!
   TBranch        *b_rho;   //!
   TBranch        *b_layerhits;   //!
   TBranch        *b_HitPositionX;   //!
   TBranch        *b_HitPositionY;   //!
   TBranch        *b_HitPositionZ;   //!
   TBranch        *b_HitPositionR;   //!

   LayerValidationTree(TTree *tree=0);
   virtual ~LayerValidationTree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop(bool cylinderLayer, TString& ext);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
        
   
};

#endif


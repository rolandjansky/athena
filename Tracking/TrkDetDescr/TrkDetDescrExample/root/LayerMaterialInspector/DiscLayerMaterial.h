/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed May 23 16:46:50 2007 by ROOT version 5.14/00e
// from TTree DiscLayerMaterial/InDet::Detectors::TRT::NegativeEndcap
// found on file: TrackingGeometryTest.root
//////////////////////////////////////////////////////////

#ifndef DiscLayerMaterial_h
#define DiscLayerMaterial_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class DiscLayerMaterial {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leave types
   Int_t           LayerIndex;
   Float_t         LayerInnerRadius;
   Float_t         LayerOuterRadius;
   Float_t         LayerPositionZ;
   Int_t           LayerMaterialBinsR;
   Int_t           LayerMaterialBinsPhi;
   Int_t           LayerMaterialBinsTotal;
   Float_t         LayerMaterialX0[50000];   //[discMatBinsTotal]
   Float_t         LayerMaterialZARho[50000];   //[discMatBinsTotal]
   Float_t         LayerReferenceMinHalfX;
   Float_t         LayerReferenceMaxHalfX;
   Float_t         LayerReferenceHalfY;
   Int_t           LayerReferenceBinsX;
   Int_t           LayerReferenceBinsY;
   Int_t           LayerReferenceBinsTotal;
   Float_t         LayerReferenceX0[50000];   //[discRefBinsTotal]
   Float_t         LayerReferenceZARho[50000];   //[discRefBinsTotal]

   // List of branches
   TBranch        *b_discLayerIndex;   //!
   TBranch        *b_discInnerR;   //!
   TBranch        *b_discOuterR;   //!
   TBranch        *b_discPositionZ;   //!
   TBranch        *b_discMatBinsR;   //!
   TBranch        *b_discMatBinsPhi;   //!
   TBranch        *b_discMatBinsTotal;   //!
   TBranch        *b_LayerMaterialX0;   //!
   TBranch        *b_LayerMaterialZARho;   //!
   TBranch        *b_discRefHalfMinX;   //!
   TBranch        *b_discRefHalfMaxX;   //!
   TBranch        *b_discRefHalfY;   //!
   TBranch        *b_discRefBinsX;   //!
   TBranch        *b_discRefBinsY;   //!
   TBranch        *b_discRefBinsTotal;   //!
   TBranch        *b_LayerReferenceX0;   //!
   TBranch        *b_LayerReferenceZARho;   //!

   DiscLayerMaterial(TTree *tree=0);
   virtual ~DiscLayerMaterial();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);

   private:
     TString          m_name; 
};

#endif

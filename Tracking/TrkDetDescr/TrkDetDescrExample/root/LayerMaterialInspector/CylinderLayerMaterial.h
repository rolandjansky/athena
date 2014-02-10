/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed May 23 16:45:59 2007 by ROOT version 5.14/00e
// from TTree CylinderLayerMaterial/InDet::GapVolumes::PixelSctNegativeSectorGap
// found on file: TrackingGeometryTest.root
//////////////////////////////////////////////////////////

#ifndef CylinderLayerMaterial_h
#define CylinderLayerMaterial_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TString.h>

class CylinderLayerMaterial {
  public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leave types
   Int_t           LayerIndex;
   Float_t         LayerRadius;
   Float_t         LayerHalfZ;
   Int_t           LayerMaterialBinsZ;
   Int_t           LayerMaterialBinsPhi;
   Int_t           LayerMaterialBinsTotal;
   Float_t         LayerMaterialX0[50000];   //[cylMatBinsTotal]
   Float_t         LayerMaterialZARho[50000];   //[cylMatBinsTotal]
   Float_t         LayerReferenceHalfX;
   Float_t         LayerReferenceHalfY;
   Int_t           LayerReferenceBinsX;
   Int_t           LayerReferenceBinsY;
   Int_t           LayerReferenceBinsTotal;
   Float_t         LayerReferenceX0[50000];   //[cylRefBinsTotal]
   Float_t         LayerReferenceZARho[50000];   //[cylRefBinsTotal]

   // List of branches
   TBranch        *b_cylLayerIndex;   //!
   TBranch        *b_cylRadius;   //!
   TBranch        *b_cylHalfZ;   //!
   TBranch        *b_cylMatBinsZ;   //!
   TBranch        *b_cylMatBinsPhi;   //!
   TBranch        *b_cylMatBinsTotal;   //!
   TBranch        *b_LayerMaterialX0;   //!
   TBranch        *b_LayerMaterialZARho;   //!
   TBranch        *b_cylRefHalfX;   //!
   TBranch        *b_cylRefHalfY;   //!
   TBranch        *b_cylRefBinsX;   //!
   TBranch        *b_cylRefBinsY;   //!
   TBranch        *b_cylRefBinsTotal;   //!
   TBranch        *b_LayerReferenceX0;   //!
   TBranch        *b_LayerReferenceZARho;   //!

   CylinderLayerMaterial(TTree *tree=0);
   virtual ~CylinderLayerMaterial();
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


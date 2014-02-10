/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MaterialMapperTree_h
#define MaterialMapperTree_h

#include <TProfile.h>
#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <vector>

class MaterialMapperTree {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leave types
   Float_t         Angle;
   
   Float_t         Eta;
   Float_t         Phi;
   
   Float_t         BeamPipePathInX0;
   Float_t         BeamPipePathInL0;
   Float_t         BeamPipeA;
   Float_t         BeamPipeZ;
   Float_t         BeamPipeRho;
   Float_t         BeamPipePath;

   Float_t         PixelPathInX0;
   Float_t         PixelPathInL0;
   Float_t         PixelA;
   Float_t         PixelZ;
   Float_t         PixelRho;
   Float_t         PixelPath;

   Float_t         SctPathInX0;
   Float_t         SctPathInL0;
   Float_t         SctA;
   Float_t         SctZ;
   Float_t         SctRho;
   Float_t         SctPath;

   Float_t         TrtPathInX0;
   Float_t         TrtPathInL0;
   Float_t         TrtA;
   Float_t         TrtZ;
   Float_t         TrtRho;
   Float_t         TrtPath;

   Float_t         InDetServPathInX0;
   Float_t         InDetServPathInL0;
   Float_t         InDetServA;
   Float_t         InDetServZ;
   Float_t         InDetServRho;
   Float_t         InDetServPath;

   Float_t         InDetPathInX0;
   Float_t         InDetPathInL0;
   Float_t         InDetA;
   Float_t         InDetZ;
   Float_t         InDetRho;
   Float_t         InDetPath;

   Float_t         SolenoidPathInX0;
   Float_t         SolenoidPathInL0;
   Float_t         SolenoidA;
   Float_t         SolenoidZ;
   Float_t         SolenoidRho;
   Float_t         SolenoidPath;

   Float_t         LArPathInX0;
   Float_t         LArPathInL0;
   Float_t         LArA;
   Float_t         LArZ;
   Float_t         LArRho;
   Float_t         LArPath;

   Float_t         TilePathInX0;
   Float_t         TilePathInL0;
   Float_t         TileA;
   Float_t         TileZ;
   Float_t         TileRho;
   Float_t         TilePath;

   Float_t         CaloServPathInX0;
   Float_t         CaloServPathInL0;
   Float_t         CaloServA;
   Float_t         CaloServZ;
   Float_t         CaloServRho;
   Float_t         CaloServPath;

   Float_t         CaloPathInX0;
   Float_t         CaloPathInL0;
   Float_t         CaloA;
   Float_t         CaloZ;
   Float_t         CaloRho;
   Float_t         CaloPath;

   Float_t         TotalPathInX0;
   Float_t         TotalPathInL0;
   Float_t         TotalA;
   Float_t         TotalZ;
   Float_t         TotalRho;
   Float_t         TotalPath;

   // List of branches
   TBranch        *b_Eta;   //!
   TBranch        *b_Phi;   //!

   TBranch        *b_BeamPipePathInX0;   //!
   TBranch        *b_BeamPipePathInL0;   //!
   TBranch        *b_BeamPipeA;   //!
   TBranch        *b_BeamPipeZ;   //!
   TBranch        *b_BeamPipeRho;   //!
   TBranch        *b_BeamPipePath;   //!
 
   TBranch        *b_PixelPathInX0;   //!
   TBranch        *b_PixelPathInL0;   //!
   TBranch        *b_PixelA;   //!
   TBranch        *b_PixelZ;   //!
   TBranch        *b_PixelRho;   //!
   TBranch        *b_PixelPath;   //!

   TBranch        *b_SctPathInX0;   //!
   TBranch        *b_SctPathInL0;   //!
   TBranch        *b_SctA;   //!
   TBranch        *b_SctZ;   //!
   TBranch        *b_SctRho;   //!
   TBranch        *b_SctPath;   //!

   TBranch        *b_TrtPathInX0;   //!
   TBranch        *b_TrtPathInL0;   //!
   TBranch        *b_TrtA;   //!
   TBranch        *b_TrtZ;   //!
   TBranch        *b_TrtRho;   //!
   TBranch        *b_TrtPath;   //!

   TBranch        *b_InDetServPathInX0;   //!
   TBranch        *b_InDetServPathInL0;   //!
   TBranch        *b_InDetServA;   //!
   TBranch        *b_InDetServZ;   //!
   TBranch        *b_InDetServRho;   //!
   TBranch        *b_InDetServPath;   //!

   TBranch        *b_InDetPathInX0;   //!
   TBranch        *b_InDetPathInL0;   //!
   TBranch        *b_InDetA;   //!
   TBranch        *b_InDetZ;   //!
   TBranch        *b_InDetRho;   //!
   TBranch        *b_InDetPath;   //!

   TBranch        *b_SolenoidPathInX0;   //!
   TBranch        *b_SolenoidPathInL0;   //!
   TBranch        *b_SolenoidA;   //!
   TBranch        *b_SolenoidZ;   //!
   TBranch        *b_SolenoidRho;   //!
   TBranch        *b_SolenoidPath;   //!

   TBranch        *b_LArPathInX0;   //!
   TBranch        *b_LArPathInL0;   //!
   TBranch        *b_LArA;   //!
   TBranch        *b_LArZ;   //!
   TBranch        *b_LArRho;   //!
   TBranch        *b_LArPath;   //!

   TBranch        *b_TilePathInX0;   //!
   TBranch        *b_TilePathInL0;   //!
   TBranch        *b_TileA;   //!
   TBranch        *b_TileZ;   //!
   TBranch        *b_TileRho;   //!
   TBranch        *b_TilePath;   //!

   TBranch        *b_CaloServPathInX0;   //!
   TBranch        *b_CaloServPathInL0;   //!
   TBranch        *b_CaloServA;   //!
   TBranch        *b_CaloServZ;   //!
   TBranch        *b_CaloServRho;   //!
   TBranch        *b_CaloServPath;   //!

   TBranch        *b_CaloPathInX0;   //!
   TBranch        *b_CaloPathInL0;   //!
   TBranch        *b_CaloA;   //!
   TBranch        *b_CaloZ;   //!
   TBranch        *b_CaloRho;   //!
   TBranch        *b_CaloPath;   //!

   TBranch        *b_TotalPathInX0;   //!
   TBranch        *b_TotalPathInL0;   //!
   TBranch        *b_TotalA;   //!
   TBranch        *b_TotalZ;   //!
   TBranch        *b_TotalRho;   //!
   TBranch        *b_TotalPath;   //!

   MaterialMapperTree(TTree *tree=0);
   virtual ~MaterialMapperTree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual std::vector<TProfile*>* profileHistograms(int bin, TString type, bool etaphi=true);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

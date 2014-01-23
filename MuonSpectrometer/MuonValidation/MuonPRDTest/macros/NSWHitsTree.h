/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed May  8 14:48:48 2013 by ROOT version 5.34/05
// from TTree NSWHitsTree/Ntuple of NSWHits
// found on file: /tmp/jomeyer/NSWPRDValAlg_allDet.root
//////////////////////////////////////////////////////////

#ifndef NSWHitsTree_h
#define NSWHitsTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include <vector>
#include <vector>
#include <vector>

// Fixed size dimensions of array or collections stored in the TTree if any.

class NSWHitsTree {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   UInt_t          runNumber;
   UInt_t          eventNumber;
   UInt_t          TruthVertex_n;
   vector<double>  *TruthVertex_X;
   vector<double>  *TruthVertex_Y;
   vector<double>  *TruthVertex_Z;
   vector<double>  *TruthVertex_T;
   vector<int>     *TruthVertex_Id;
   UInt_t          TruthParticle_n;
   vector<double>  *TruthParticle_Pt;
   vector<double>  *TruthParticle_Eta;
   vector<double>  *TruthParticle_Phi;
   vector<double>  *TruthParticle_E;
   vector<double>  *TruthParticle_M;
   vector<int>     *TruthParticle_Pdg;
   vector<int>     *TruthParticle_Status;
   vector<int>     *TruthParticle_Barcode;
   vector<int>     *TruthParticle_Production_vertex_id;
   vector<int>     *TruthParticle_End_vertex_id;
   UInt_t          MuEntry_Particle_n;
   vector<double>  *MuEntry_Particle_Pt;
   vector<double>  *MuEntry_Particle_Eta;
   vector<double>  *MuEntry_Particle_Phi;
   vector<int>     *MuEntry_Particle_Pdg;
   vector<int>     *MuEntry_Particle_Barcode;
   vector<double>  *MuEntry_Position_Eta;
   vector<double>  *MuEntry_Position_Phi;
   UInt_t          Hits_sTGC_n;
   vector<int>     *Hits_sTGC_trackId;
   vector<int>     *Hits_sTGC_truthEl;
   vector<double>  *Hits_sTGC_globalTime;
   vector<double>  *Hits_sTGC_hitGlobalPositionX;
   vector<double>  *Hits_sTGC_hitGlobalPositionY;
   vector<double>  *Hits_sTGC_hitGlobalPositionZ;
   vector<double>  *Hits_sTGC_hitGlobalPositionR;
   vector<double>  *Hits_sTGC_hitGlobalPositionP;
   vector<double>  *Hits_sTGC_hitGlobalDirectionX;
   vector<double>  *Hits_sTGC_hitGlobalDirectionY;
   vector<double>  *Hits_sTGC_hitGlobalDirectionZ;
   vector<double>  *Hits_sTGC_hitLocalPositionX;
   vector<double>  *Hits_sTGC_hitLocalPositionY;
   vector<double>  *Hits_sTGC_hitLocalPositionZ;
   vector<double>  *Hits_sTGC_detector_globalPositionX;
   vector<double>  *Hits_sTGC_detector_globalPositionY;
   vector<double>  *Hits_sTGC_detector_globalPositionZ;
   vector<double>  *Hits_sTGC_detector_globalPositionR;
   vector<double>  *Hits_sTGC_detector_globalPositionP;
   vector<double>  *Hits_sTGC_hitToDsurfacePositionX;
   vector<double>  *Hits_sTGC_hitToDsurfacePositionY;
   vector<double>  *Hits_sTGC_hitToDsurfacePositionZ;
   vector<double>  *Hits_sTGC_hitToRsurfacePositionX;
   vector<double>  *Hits_sTGC_hitToRsurfacePositionY;
   vector<double>  *Hits_sTGC_hitToRsurfacePositionZ;
   vector<double>  *Hits_sTGC_FastDigitRsurfacePositionX;
   vector<double>  *Hits_sTGC_FastDigitRsurfacePositionY;
   vector<int>     *Hits_sTGC_particleEncoding;
   vector<double>  *Hits_sTGC_kineticEnergy;
   vector<double>  *Hits_sTGC_depositEnergy;
   vector<double>  *Hits_sTGC_StepLength;
   vector<string>  *Hits_sTGC_sim_stationName;
   vector<int>     *Hits_sTGC_wedgeId;
   vector<int>     *Hits_sTGC_wedgeType;
   vector<int>     *Hits_sTGC_detectorNumber;
   vector<int>     *Hits_sTGC_sim_stationEta;
   vector<int>     *Hits_sTGC_sim_stationPhi;
   vector<int>     *Hits_sTGC_sim_multilayer;
   vector<int>     *Hits_sTGC_sim_layer;
   vector<int>     *Hits_sTGC_sim_side;
   vector<int>     *Hits_sTGC_stripNumber;
   vector<int>     *Hits_sTGC_wireNumber;
   vector<string>  *Hits_sTGC_off_stationName;
   vector<int>     *Hits_sTGC_off_stationEta;
   vector<int>     *Hits_sTGC_off_stationPhi;
   vector<int>     *Hits_sTGC_off_multiplet;
   vector<int>     *Hits_sTGC_off_gas_gap;
   vector<int>     *Hits_sTGC_off_channel_type;
   vector<int>     *Hits_sTGC_off_channel;
   UInt_t          FastDigit_sTGC;
   vector<string>  *FastDigit_sTGC_stationName;
   vector<int>     *FastDigit_sTGC_stationEta;
   vector<int>     *FastDigit_sTGC_stationPhi;
   vector<int>     *FastDigit_sTGC_multiplet;
   vector<int>     *FastDigit_sTGC_gas_gap;
   vector<int>     *FastDigit_sTGC_channel_type;
   vector<int>     *FastDigit_sTGC_channel;
   vector<double>  *FastDigit_sTGC_globalPosX;
   vector<double>  *FastDigit_sTGC_globalPosY;
   vector<double>  *FastDigit_sTGC_globalPosZ;
   vector<double>  *FastDigit_sTGC_localPosX;
   vector<double>  *FastDigit_sTGC_localPosY;
   vector<double>  *FastDigit_sTGC_covMatrix_1_1;
   vector<double>  *FastDigit_sTGC_covMatrix_2_2;
   vector<int>     *FastDigit_sTGC_truth_barcode;
   vector<double>  *FastDigit_sTGC_truth_hitOnSurfaceX;
   vector<double>  *FastDigit_sTGC_truth_hitOnSurfaceY;
   UInt_t          Hits_MM_n;
   vector<int>     *Hits_MM_trackId;
   vector<int>     *Hits_MM_truthEl;
   vector<double>  *Hits_MM_globalTime;
   vector<double>  *Hits_MM_hitGlobalPositionX;
   vector<double>  *Hits_MM_hitGlobalPositionY;
   vector<double>  *Hits_MM_hitGlobalPositionZ;
   vector<double>  *Hits_MM_hitGlobalPositionR;
   vector<double>  *Hits_MM_hitGlobalPositionP;
   vector<double>  *Hits_MM_hitGlobalDirectionX;
   vector<double>  *Hits_MM_hitGlobalDirectionY;
   vector<double>  *Hits_MM_hitGlobalDirectionZ;
   vector<double>  *Hits_MM_hitLocalPositionX;
   vector<double>  *Hits_MM_hitLocalPositionY;
   vector<double>  *Hits_MM_hitLocalPositionZ;
   vector<double>  *Hits_MM_detector_globalPositionX;
   vector<double>  *Hits_MM_detector_globalPositionY;
   vector<double>  *Hits_MM_detector_globalPositionZ;
   vector<double>  *Hits_MM_detector_globalPositionR;
   vector<double>  *Hits_MM_detector_globalPositionP;
   vector<double>  *Hits_MM_hitToDsurfacePositionX;
   vector<double>  *Hits_MM_hitToDsurfacePositionY;
   vector<double>  *Hits_MM_hitToDsurfacePositionZ;
   vector<double>  *Hits_MM_hitToRsurfacePositionX;
   vector<double>  *Hits_MM_hitToRsurfacePositionY;
   vector<double>  *Hits_MM_hitToRsurfacePositionZ;
   vector<double>  *Hits_MM_FastDigitRsurfacePositionX;
   vector<double>  *Hits_MM_FastDigitRsurfacePositionY;
   vector<int>     *Hits_MM_particleEncoding;
   vector<double>  *Hits_MM_kineticEnergy;
   vector<double>  *Hits_MM_depositEnergy;
   vector<double>  *Hits_MM_StepLength;
   vector<string>  *Hits_MM_sim_stationName;
   vector<int>     *Hits_MM_sim_stationEta;
   vector<int>     *Hits_MM_sim_stationPhi;
   vector<int>     *Hits_MM_sim_multilayer;
   vector<int>     *Hits_MM_sim_layer;
   vector<int>     *Hits_MM_sim_side;
   vector<string>  *Hits_MM_off_stationName;
   vector<int>     *Hits_MM_off_stationEta;
   vector<int>     *Hits_MM_off_stationPhi;
   vector<int>     *Hits_MM_off_multiplet;
   vector<int>     *Hits_MM_off_gas_gap;
   vector<int>     *Hits_MM_off_channel;
   UInt_t          FastDigit_MM;
   vector<string>  *FastDigit_MM_stationName;
   vector<int>     *FastDigit_MM_stationEta;
   vector<int>     *FastDigit_MM_stationPhi;
   vector<int>     *FastDigit_MM_multiplet;
   vector<int>     *FastDigit_MM_gas_gap;
   vector<int>     *FastDigit_MM_channel;
   vector<double>  *FastDigit_MM_globalPosX;
   vector<double>  *FastDigit_MM_globalPosY;
   vector<double>  *FastDigit_MM_globalPosZ;
   vector<double>  *FastDigit_MM_localPosX;
   vector<double>  *FastDigit_MM_localPosY;
   vector<double>  *FastDigit_MM_covMatrix_1_1;
   vector<int>     *FastDigit_MM_truth_barcode;
   vector<double>  *FastDigit_MM_truth_hitOnSurfaceX;
   vector<double>  *FastDigit_MM_truth_hitOnSurfaceY;

   // List of branches
   TBranch        *b_runNumber;   //!
   TBranch        *b_eventNumber;   //!
   TBranch        *b_TruthVertex_n;   //!
   TBranch        *b_TruthVertex_X;   //!
   TBranch        *b_TruthVertex_Y;   //!
   TBranch        *b_TruthVertex_Z;   //!
   TBranch        *b_TruthVertex_T;   //!
   TBranch        *b_TruthVertex_Id;   //!
   TBranch        *b_TruthParticle_n;   //!
   TBranch        *b_TruthParticle_Pt;   //!
   TBranch        *b_TruthParticle_Eta;   //!
   TBranch        *b_TruthParticle_Phi;   //!
   TBranch        *b_TruthParticle_E;   //!
   TBranch        *b_TruthParticle_M;   //!
   TBranch        *b_TruthParticle_Pdg;   //!
   TBranch        *b_TruthParticle_Status;   //!
   TBranch        *b_TruthParticle_Barcode;   //!
   TBranch        *b_TruthParticle_Production_vertex_id;   //!
   TBranch        *b_TruthParticle_End_vertex_id;   //!
   TBranch        *b_MuEntryParticle_n;   //!
   TBranch        *b_MuEntry_Particle_Pt;   //!
   TBranch        *b_MuEntry_Particle_Eta;   //!
   TBranch        *b_MuEntry_Particle_Phi;   //!
   TBranch        *b_MuEntry_Particle_Pdg;   //!
   TBranch        *b_MuEntry_Particle_Barcode;   //!
   TBranch        *b_MuEntry_Position_Eta;   //!
   TBranch        *b_MuEntry_Position_Phi;   //!
   TBranch        *b_Hits_sTGC_nSimHits;   //!
   TBranch        *b_Hits_sTGC_trackId;   //!
   TBranch        *b_Hits_sTGC_truthEl;   //!
   TBranch        *b_Hits_sTGC_globalTime;   //!
   TBranch        *b_Hits_sTGC_hitGlobalPositionX;   //!
   TBranch        *b_Hits_sTGC_hitGlobalPositionY;   //!
   TBranch        *b_Hits_sTGC_hitGlobalPositionZ;   //!
   TBranch        *b_Hits_sTGC_hitGlobalPositionR;   //!
   TBranch        *b_Hits_sTGC_hitGlobalPositionP;   //!
   TBranch        *b_Hits_sTGC_hitGlobalDirectionX;   //!
   TBranch        *b_Hits_sTGC_hitGlobalDirectionY;   //!
   TBranch        *b_Hits_sTGC_hitGlobalDirectionZ;   //!
   TBranch        *b_Hits_sTGC_hitLocalPositionX;   //!
   TBranch        *b_Hits_sTGC_hitLocalPositionY;   //!
   TBranch        *b_Hits_sTGC_hitLocalPositionZ;   //!
   TBranch        *b_Hits_sTGC_detector_globalPositionX;   //!
   TBranch        *b_Hits_sTGC_detector_globalPositionY;   //!
   TBranch        *b_Hits_sTGC_detector_globalPositionZ;   //!
   TBranch        *b_Hits_sTGC_detector_globalPositionR;   //!
   TBranch        *b_Hits_sTGC_detector_globalPositionP;   //!
   TBranch        *b_Hits_sTGC_hitToDsurfacePositionX;   //!
   TBranch        *b_Hits_sTGC_hitToDsurfacePositionY;   //!
   TBranch        *b_Hits_sTGC_hitToDsurfacePositionZ;   //!
   TBranch        *b_Hits_sTGC_hitToRsurfacePositionX;   //!
   TBranch        *b_Hits_sTGC_hitToRsurfacePositionY;   //!
   TBranch        *b_Hits_sTGC_hitToRsurfacePositionZ;   //!
   TBranch        *b_Hits_sTGC_FastDigitRsurfacePositionX;   //!
   TBranch        *b_Hits_sTGC_FastDigitRsurfacePositionY;   //!
   TBranch        *b_Hits_sTGC_particleEncoding;   //!
   TBranch        *b_Hits_sTGC_kineticEnergy;   //!
   TBranch        *b_Hits_sTGC_depositEnergy;   //!
   TBranch        *b_Hits_sTGC_StepLength;   //!
   TBranch        *b_Hits_sTGC_sim_stationName;   //!
   TBranch        *b_Hits_sTGC_wedgeId;   //!
   TBranch        *b_Hits_sTGC_wedgeType;   //!
   TBranch        *b_Hits_sTGC_detectorNumber;   //!
   TBranch        *b_Hits_sTGC_sim_stationEta;   //!
   TBranch        *b_Hits_sTGC_sim_stationPhi;   //!
   TBranch        *b_Hits_sTGC_sim_multilayer;   //!
   TBranch        *b_Hits_sTGC_sim_layer;   //!
   TBranch        *b_Hits_sTGC_sim_side;   //!
   TBranch        *b_Hits_sTGC_stripNumber;   //!
   TBranch        *b_Hits_sTGC_wireNumber;   //!
   TBranch        *b_Hits_sTGC_off_stationName;   //!
   TBranch        *b_Hits_sTGC_off_stationEta;   //!
   TBranch        *b_Hits_sTGC_off_stationPhi;   //!
   TBranch        *b_Hits_sTGC_off_multiplet;   //!
   TBranch        *b_Hits_sTGC_off_gas_gap;   //!
   TBranch        *b_Hits_sTGC_off_channel_type;   //!
   TBranch        *b_Hits_sTGC_off_channel;   //!
   TBranch        *b_FastDigits_sTGC_n;   //!
   TBranch        *b_FastDigit_sTGC_stationName;   //!
   TBranch        *b_FastDigit_sTGC_stationEta;   //!
   TBranch        *b_FastDigit_sTGC_stationPhi;   //!
   TBranch        *b_FastDigit_sTGC_multiplet;   //!
   TBranch        *b_FastDigit_sTGC_gas_gap;   //!
   TBranch        *b_FastDigit_sTGC_channel_type;   //!
   TBranch        *b_FastDigit_sTGC_channel;   //!
   TBranch        *b_FastDigit_sTGC_globalPosX;   //!
   TBranch        *b_FastDigit_sTGC_globalPosY;   //!
   TBranch        *b_FastDigit_sTGC_globalPosZ;   //!
   TBranch        *b_FastDigit_sTGC_localPosX;   //!
   TBranch        *b_FastDigit_sTGC_localPosY;   //!
   TBranch        *b_FastDigit_sTGC_covMatrix_1_1;   //!
   TBranch        *b_FastDigit_sTGC_covMatrix_2_2;   //!
   TBranch        *b_FastDigit_sTGC_truth_barcode;   //!
   TBranch        *b_FastDigit_sTGC_truth_hitOnSurfaceX;   //!
   TBranch        *b_FastDigit_sTGC_truth_hitOnSurfaceY;   //!
   TBranch        *b_Hits_MM_n;   //!
   TBranch        *b_Hits_MM_trackId;   //!
   TBranch        *b_Hits_MM_truthEl;   //!
   TBranch        *b_Hits_MM_globalTime;   //!
   TBranch        *b_Hits_MM_hitGlobalPositionX;   //!
   TBranch        *b_Hits_MM_hitGlobalPositionY;   //!
   TBranch        *b_Hits_MM_hitGlobalPositionZ;   //!
   TBranch        *b_Hits_MM_hitGlobalPositionR;   //!
   TBranch        *b_Hits_MM_hitGlobalPositionP;   //!
   TBranch        *b_Hits_MM_hitGlobalDirectionX;   //!
   TBranch        *b_Hits_MM_hitGlobalDirectionY;   //!
   TBranch        *b_Hits_MM_hitGlobalDirectionZ;   //!
   TBranch        *b_Hits_MM_hitLocalPositionX;   //!
   TBranch        *b_Hits_MM_hitLocalPositionY;   //!
   TBranch        *b_Hits_MM_hitLocalPositionZ;   //!
   TBranch        *b_Hits_MM_detector_globalPositionX;   //!
   TBranch        *b_Hits_MM_detector_globalPositionY;   //!
   TBranch        *b_Hits_MM_detector_globalPositionZ;   //!
   TBranch        *b_Hits_MM_detector_globalPositionR;   //!
   TBranch        *b_Hits_MM_detector_globalPositionP;   //!
   TBranch        *b_Hits_MM_hitToDsurfacePositionX;   //!
   TBranch        *b_Hits_MM_hitToDsurfacePositionY;   //!
   TBranch        *b_Hits_MM_hitToDsurfacePositionZ;   //!
   TBranch        *b_Hits_MM_hitToRsurfacePositionX;   //!
   TBranch        *b_Hits_MM_hitToRsurfacePositionY;   //!
   TBranch        *b_Hits_MM_hitToRsurfacePositionZ;   //!
   TBranch        *b_Hits_MM_FastDigitRsurfacePositionX;   //!
   TBranch        *b_Hits_MM_FastDigitRsurfacePositionY;   //!
   TBranch        *b_Hits_MM_particleEncoding;   //!
   TBranch        *b_Hits_MM_kineticEnergy;   //!
   TBranch        *b_Hits_MM_depositEnergy;   //!
   TBranch        *b_Hits_MM_StepLength;   //!
   TBranch        *b_Hits_MM_sim_stationName;   //!
   TBranch        *b_Hits_MM_sim_stationEta;   //!
   TBranch        *b_Hits_MM_sim_stationPhi;   //!
   TBranch        *b_Hits_MM_sim_multilayer;   //!
   TBranch        *b_Hits_MM_sim_layer;   //!
   TBranch        *b_Hits_MM_sim_side;   //!
   TBranch        *b_Hits_MM_off_stationName;   //!
   TBranch        *b_Hits_MM_off_stationEta;   //!
   TBranch        *b_Hits_MM_off_stationPhi;   //!
   TBranch        *b_Hits_MM_off_multiplet;   //!
   TBranch        *b_Hits_MM_off_gas_gap;   //!
   TBranch        *b_Hits_MM_off_channel;   //!
   TBranch        *b_FastDigits_MM_n;   //!
   TBranch        *b_FastDigit_MM_stationName;   //!
   TBranch        *b_FastDigit_MM_stationEta;   //!
   TBranch        *b_FastDigit_MM_stationPhi;   //!
   TBranch        *b_FastDigit_MM_multiplet;   //!
   TBranch        *b_FastDigit_MM_gas_gap;   //!
   TBranch        *b_FastDigit_MM_channel;   //!
   TBranch        *b_FastDigit_MM_globalPosX;   //!
   TBranch        *b_FastDigit_MM_globalPosY;   //!
   TBranch        *b_FastDigit_MM_globalPosZ;   //!
   TBranch        *b_FastDigit_MM_localPosX;   //!
   TBranch        *b_FastDigit_MM_localPosY;   //!
   TBranch        *b_FastDigit_MM_covMatrix_1_1;   //!
   TBranch        *b_FastDigit_MM_truth_barcode;   //!
   TBranch        *b_FastDigit_MM_truth_hitOnSurfaceX;   //!
   TBranch        *b_FastDigit_MM_truth_hitOnSurfaceY;   //!

   NSWHitsTree(TTree *tree=0);
   virtual ~NSWHitsTree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef NSWHitsTree_cxx
NSWHitsTree::NSWHitsTree(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/tmp/jomeyer/NSWPRDValAlg_allDet.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("/tmp/jomeyer/NSWPRDValAlg_allDet.root");
      }
      f->GetObject("NSWHitsTree",tree);

   }
   Init(tree);
}

NSWHitsTree::~NSWHitsTree()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t NSWHitsTree::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t NSWHitsTree::LoadTree(Long64_t entry)
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

void NSWHitsTree::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   TruthVertex_X = 0;
   TruthVertex_Y = 0;
   TruthVertex_Z = 0;
   TruthVertex_T = 0;
   TruthVertex_Id = 0;
   TruthParticle_Pt = 0;
   TruthParticle_Eta = 0;
   TruthParticle_Phi = 0;
   TruthParticle_E = 0;
   TruthParticle_M = 0;
   TruthParticle_Pdg = 0;
   TruthParticle_Status = 0;
   TruthParticle_Barcode = 0;
   TruthParticle_Production_vertex_id = 0;
   TruthParticle_End_vertex_id = 0;
   MuEntry_Particle_Pt = 0;
   MuEntry_Particle_Eta = 0;
   MuEntry_Particle_Phi = 0;
   MuEntry_Particle_Pdg = 0;
   MuEntry_Particle_Barcode = 0;
   MuEntry_Position_Eta = 0;
   MuEntry_Position_Phi = 0;
   Hits_sTGC_trackId = 0;
   Hits_sTGC_truthEl = 0;
   Hits_sTGC_globalTime = 0;
   Hits_sTGC_hitGlobalPositionX = 0;
   Hits_sTGC_hitGlobalPositionY = 0;
   Hits_sTGC_hitGlobalPositionZ = 0;
   Hits_sTGC_hitGlobalPositionR = 0;
   Hits_sTGC_hitGlobalPositionP = 0;
   Hits_sTGC_hitGlobalDirectionX = 0;
   Hits_sTGC_hitGlobalDirectionY = 0;
   Hits_sTGC_hitGlobalDirectionZ = 0;
   Hits_sTGC_hitLocalPositionX = 0;
   Hits_sTGC_hitLocalPositionY = 0;
   Hits_sTGC_hitLocalPositionZ = 0;
   Hits_sTGC_detector_globalPositionX = 0;
   Hits_sTGC_detector_globalPositionY = 0;
   Hits_sTGC_detector_globalPositionZ = 0;
   Hits_sTGC_detector_globalPositionR = 0;
   Hits_sTGC_detector_globalPositionP = 0;
   Hits_sTGC_hitToDsurfacePositionX = 0;
   Hits_sTGC_hitToDsurfacePositionY = 0;
   Hits_sTGC_hitToDsurfacePositionZ = 0;
   Hits_sTGC_hitToRsurfacePositionX = 0;
   Hits_sTGC_hitToRsurfacePositionY = 0;
   Hits_sTGC_hitToRsurfacePositionZ = 0;
   Hits_sTGC_FastDigitRsurfacePositionX = 0;
   Hits_sTGC_FastDigitRsurfacePositionY = 0;
   Hits_sTGC_particleEncoding = 0;
   Hits_sTGC_kineticEnergy = 0;
   Hits_sTGC_depositEnergy = 0;
   Hits_sTGC_StepLength = 0;
   Hits_sTGC_sim_stationName = 0;
   Hits_sTGC_wedgeId = 0;
   Hits_sTGC_wedgeType = 0;
   Hits_sTGC_detectorNumber = 0;
   Hits_sTGC_sim_stationEta = 0;
   Hits_sTGC_sim_stationPhi = 0;
   Hits_sTGC_sim_multilayer = 0;
   Hits_sTGC_sim_layer = 0;
   Hits_sTGC_sim_side = 0;
   Hits_sTGC_stripNumber = 0;
   Hits_sTGC_wireNumber = 0;
   Hits_sTGC_off_stationName = 0;
   Hits_sTGC_off_stationEta = 0;
   Hits_sTGC_off_stationPhi = 0;
   Hits_sTGC_off_multiplet = 0;
   Hits_sTGC_off_gas_gap = 0;
   Hits_sTGC_off_channel_type = 0;
   Hits_sTGC_off_channel = 0;
   FastDigit_sTGC_stationName = 0;
   FastDigit_sTGC_stationEta = 0;
   FastDigit_sTGC_stationPhi = 0;
   FastDigit_sTGC_multiplet = 0;
   FastDigit_sTGC_gas_gap = 0;
   FastDigit_sTGC_channel_type = 0;
   FastDigit_sTGC_channel = 0;
   FastDigit_sTGC_globalPosX = 0;
   FastDigit_sTGC_globalPosY = 0;
   FastDigit_sTGC_globalPosZ = 0;
   FastDigit_sTGC_localPosX = 0;
   FastDigit_sTGC_localPosY = 0;
   FastDigit_sTGC_covMatrix_1_1 = 0;
   FastDigit_sTGC_covMatrix_2_2 = 0;
   FastDigit_sTGC_truth_barcode = 0;
   FastDigit_sTGC_truth_hitOnSurfaceX = 0;
   FastDigit_sTGC_truth_hitOnSurfaceY = 0;
   Hits_MM_trackId = 0;
   Hits_MM_truthEl = 0;
   Hits_MM_globalTime = 0;
   Hits_MM_hitGlobalPositionX = 0;
   Hits_MM_hitGlobalPositionY = 0;
   Hits_MM_hitGlobalPositionZ = 0;
   Hits_MM_hitGlobalPositionR = 0;
   Hits_MM_hitGlobalPositionP = 0;
   Hits_MM_hitGlobalDirectionX = 0;
   Hits_MM_hitGlobalDirectionY = 0;
   Hits_MM_hitGlobalDirectionZ = 0;
   Hits_MM_hitLocalPositionX = 0;
   Hits_MM_hitLocalPositionY = 0;
   Hits_MM_hitLocalPositionZ = 0;
   Hits_MM_detector_globalPositionX = 0;
   Hits_MM_detector_globalPositionY = 0;
   Hits_MM_detector_globalPositionZ = 0;
   Hits_MM_detector_globalPositionR = 0;
   Hits_MM_detector_globalPositionP = 0;
   Hits_MM_hitToDsurfacePositionX = 0;
   Hits_MM_hitToDsurfacePositionY = 0;
   Hits_MM_hitToDsurfacePositionZ = 0;
   Hits_MM_hitToRsurfacePositionX = 0;
   Hits_MM_hitToRsurfacePositionY = 0;
   Hits_MM_hitToRsurfacePositionZ = 0;
   Hits_MM_FastDigitRsurfacePositionX = 0;
   Hits_MM_FastDigitRsurfacePositionY = 0;
   Hits_MM_particleEncoding = 0;
   Hits_MM_kineticEnergy = 0;
   Hits_MM_depositEnergy = 0;
   Hits_MM_StepLength = 0;
   Hits_MM_sim_stationName = 0;
   Hits_MM_sim_stationEta = 0;
   Hits_MM_sim_stationPhi = 0;
   Hits_MM_sim_multilayer = 0;
   Hits_MM_sim_layer = 0;
   Hits_MM_sim_side = 0;
   Hits_MM_off_stationName = 0;
   Hits_MM_off_stationEta = 0;
   Hits_MM_off_stationPhi = 0;
   Hits_MM_off_multiplet = 0;
   Hits_MM_off_gas_gap = 0;
   Hits_MM_off_channel = 0;
   FastDigit_MM_stationName = 0;
   FastDigit_MM_stationEta = 0;
   FastDigit_MM_stationPhi = 0;
   FastDigit_MM_multiplet = 0;
   FastDigit_MM_gas_gap = 0;
   FastDigit_MM_channel = 0;
   FastDigit_MM_globalPosX = 0;
   FastDigit_MM_globalPosY = 0;
   FastDigit_MM_globalPosZ = 0;
   FastDigit_MM_localPosX = 0;
   FastDigit_MM_localPosY = 0;
   FastDigit_MM_covMatrix_1_1 = 0;
   FastDigit_MM_truth_barcode = 0;
   FastDigit_MM_truth_hitOnSurfaceX = 0;
   FastDigit_MM_truth_hitOnSurfaceY = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("runNumber", &runNumber, &b_runNumber);
   fChain->SetBranchAddress("eventNumber", &eventNumber, &b_eventNumber);
   fChain->SetBranchAddress("TruthVertex_n", &TruthVertex_n, &b_TruthVertex_n);
   fChain->SetBranchAddress("TruthVertex_X", &TruthVertex_X, &b_TruthVertex_X);
   fChain->SetBranchAddress("TruthVertex_Y", &TruthVertex_Y, &b_TruthVertex_Y);
   fChain->SetBranchAddress("TruthVertex_Z", &TruthVertex_Z, &b_TruthVertex_Z);
   fChain->SetBranchAddress("TruthVertex_T", &TruthVertex_T, &b_TruthVertex_T);
   fChain->SetBranchAddress("TruthVertex_Id", &TruthVertex_Id, &b_TruthVertex_Id);
   fChain->SetBranchAddress("TruthParticle_n", &TruthParticle_n, &b_TruthParticle_n);
   fChain->SetBranchAddress("TruthParticle_Pt", &TruthParticle_Pt, &b_TruthParticle_Pt);
   fChain->SetBranchAddress("TruthParticle_Eta", &TruthParticle_Eta, &b_TruthParticle_Eta);
   fChain->SetBranchAddress("TruthParticle_Phi", &TruthParticle_Phi, &b_TruthParticle_Phi);
   fChain->SetBranchAddress("TruthParticle_E", &TruthParticle_E, &b_TruthParticle_E);
   fChain->SetBranchAddress("TruthParticle_M", &TruthParticle_M, &b_TruthParticle_M);
   fChain->SetBranchAddress("TruthParticle_Pdg", &TruthParticle_Pdg, &b_TruthParticle_Pdg);
   fChain->SetBranchAddress("TruthParticle_Status", &TruthParticle_Status, &b_TruthParticle_Status);
   fChain->SetBranchAddress("TruthParticle_Barcode", &TruthParticle_Barcode, &b_TruthParticle_Barcode);
   fChain->SetBranchAddress("TruthParticle_Production_vertex_id", &TruthParticle_Production_vertex_id, &b_TruthParticle_Production_vertex_id);
   fChain->SetBranchAddress("TruthParticle_End_vertex_id", &TruthParticle_End_vertex_id, &b_TruthParticle_End_vertex_id);
   fChain->SetBranchAddress("MuEntry_Particle_n", &MuEntry_Particle_n, &b_MuEntryParticle_n);
   fChain->SetBranchAddress("MuEntry_Particle_Pt", &MuEntry_Particle_Pt, &b_MuEntry_Particle_Pt);
   fChain->SetBranchAddress("MuEntry_Particle_Eta", &MuEntry_Particle_Eta, &b_MuEntry_Particle_Eta);
   fChain->SetBranchAddress("MuEntry_Particle_Phi", &MuEntry_Particle_Phi, &b_MuEntry_Particle_Phi);
   fChain->SetBranchAddress("MuEntry_Particle_Pdg", &MuEntry_Particle_Pdg, &b_MuEntry_Particle_Pdg);
   fChain->SetBranchAddress("MuEntry_Particle_Barcode", &MuEntry_Particle_Barcode, &b_MuEntry_Particle_Barcode);
   fChain->SetBranchAddress("MuEntry_Position_Eta", &MuEntry_Position_Eta, &b_MuEntry_Position_Eta);
   fChain->SetBranchAddress("MuEntry_Position_Phi", &MuEntry_Position_Phi, &b_MuEntry_Position_Phi);
   fChain->SetBranchAddress("Hits_sTGC_n", &Hits_sTGC_n, &b_Hits_sTGC_nSimHits);
   fChain->SetBranchAddress("Hits_sTGC_trackId", &Hits_sTGC_trackId, &b_Hits_sTGC_trackId);
   fChain->SetBranchAddress("Hits_sTGC_truthEl", &Hits_sTGC_truthEl, &b_Hits_sTGC_truthEl);
   fChain->SetBranchAddress("Hits_sTGC_globalTime", &Hits_sTGC_globalTime, &b_Hits_sTGC_globalTime);
   fChain->SetBranchAddress("Hits_sTGC_hitGlobalPositionX", &Hits_sTGC_hitGlobalPositionX, &b_Hits_sTGC_hitGlobalPositionX);
   fChain->SetBranchAddress("Hits_sTGC_hitGlobalPositionY", &Hits_sTGC_hitGlobalPositionY, &b_Hits_sTGC_hitGlobalPositionY);
   fChain->SetBranchAddress("Hits_sTGC_hitGlobalPositionZ", &Hits_sTGC_hitGlobalPositionZ, &b_Hits_sTGC_hitGlobalPositionZ);
   fChain->SetBranchAddress("Hits_sTGC_hitGlobalPositionR", &Hits_sTGC_hitGlobalPositionR, &b_Hits_sTGC_hitGlobalPositionR);
   fChain->SetBranchAddress("Hits_sTGC_hitGlobalPositionP", &Hits_sTGC_hitGlobalPositionP, &b_Hits_sTGC_hitGlobalPositionP);
   fChain->SetBranchAddress("Hits_sTGC_hitGlobalDirectionX", &Hits_sTGC_hitGlobalDirectionX, &b_Hits_sTGC_hitGlobalDirectionX);
   fChain->SetBranchAddress("Hits_sTGC_hitGlobalDirectionY", &Hits_sTGC_hitGlobalDirectionY, &b_Hits_sTGC_hitGlobalDirectionY);
   fChain->SetBranchAddress("Hits_sTGC_hitGlobalDirectionZ", &Hits_sTGC_hitGlobalDirectionZ, &b_Hits_sTGC_hitGlobalDirectionZ);
   fChain->SetBranchAddress("Hits_sTGC_hitLocalPositionX", &Hits_sTGC_hitLocalPositionX, &b_Hits_sTGC_hitLocalPositionX);
   fChain->SetBranchAddress("Hits_sTGC_hitLocalPositionY", &Hits_sTGC_hitLocalPositionY, &b_Hits_sTGC_hitLocalPositionY);
   fChain->SetBranchAddress("Hits_sTGC_hitLocalPositionZ", &Hits_sTGC_hitLocalPositionZ, &b_Hits_sTGC_hitLocalPositionZ);
   fChain->SetBranchAddress("Hits_sTGC_detector_globalPositionX", &Hits_sTGC_detector_globalPositionX, &b_Hits_sTGC_detector_globalPositionX);
   fChain->SetBranchAddress("Hits_sTGC_detector_globalPositionY", &Hits_sTGC_detector_globalPositionY, &b_Hits_sTGC_detector_globalPositionY);
   fChain->SetBranchAddress("Hits_sTGC_detector_globalPositionZ", &Hits_sTGC_detector_globalPositionZ, &b_Hits_sTGC_detector_globalPositionZ);
   fChain->SetBranchAddress("Hits_sTGC_detector_globalPositionR", &Hits_sTGC_detector_globalPositionR, &b_Hits_sTGC_detector_globalPositionR);
   fChain->SetBranchAddress("Hits_sTGC_detector_globalPositionP", &Hits_sTGC_detector_globalPositionP, &b_Hits_sTGC_detector_globalPositionP);
   fChain->SetBranchAddress("Hits_sTGC_hitToDsurfacePositionX", &Hits_sTGC_hitToDsurfacePositionX, &b_Hits_sTGC_hitToDsurfacePositionX);
   fChain->SetBranchAddress("Hits_sTGC_hitToDsurfacePositionY", &Hits_sTGC_hitToDsurfacePositionY, &b_Hits_sTGC_hitToDsurfacePositionY);
   fChain->SetBranchAddress("Hits_sTGC_hitToDsurfacePositionZ", &Hits_sTGC_hitToDsurfacePositionZ, &b_Hits_sTGC_hitToDsurfacePositionZ);
   fChain->SetBranchAddress("Hits_sTGC_hitToRsurfacePositionX", &Hits_sTGC_hitToRsurfacePositionX, &b_Hits_sTGC_hitToRsurfacePositionX);
   fChain->SetBranchAddress("Hits_sTGC_hitToRsurfacePositionY", &Hits_sTGC_hitToRsurfacePositionY, &b_Hits_sTGC_hitToRsurfacePositionY);
   fChain->SetBranchAddress("Hits_sTGC_hitToRsurfacePositionZ", &Hits_sTGC_hitToRsurfacePositionZ, &b_Hits_sTGC_hitToRsurfacePositionZ);
   fChain->SetBranchAddress("Hits_sTGC_FastDigitRsurfacePositionX", &Hits_sTGC_FastDigitRsurfacePositionX, &b_Hits_sTGC_FastDigitRsurfacePositionX);
   fChain->SetBranchAddress("Hits_sTGC_FastDigitRsurfacePositionY", &Hits_sTGC_FastDigitRsurfacePositionY, &b_Hits_sTGC_FastDigitRsurfacePositionY);
   fChain->SetBranchAddress("Hits_sTGC_particleEncoding", &Hits_sTGC_particleEncoding, &b_Hits_sTGC_particleEncoding);
   fChain->SetBranchAddress("Hits_sTGC_kineticEnergy", &Hits_sTGC_kineticEnergy, &b_Hits_sTGC_kineticEnergy);
   fChain->SetBranchAddress("Hits_sTGC_depositEnergy", &Hits_sTGC_depositEnergy, &b_Hits_sTGC_depositEnergy);
   fChain->SetBranchAddress("Hits_sTGC_StepLength", &Hits_sTGC_StepLength, &b_Hits_sTGC_StepLength);
   fChain->SetBranchAddress("Hits_sTGC_sim_stationName", &Hits_sTGC_sim_stationName, &b_Hits_sTGC_sim_stationName);
   fChain->SetBranchAddress("Hits_sTGC_wedgeId", &Hits_sTGC_wedgeId, &b_Hits_sTGC_wedgeId);
   fChain->SetBranchAddress("Hits_sTGC_wedgeType", &Hits_sTGC_wedgeType, &b_Hits_sTGC_wedgeType);
   fChain->SetBranchAddress("Hits_sTGC_detectorNumber", &Hits_sTGC_detectorNumber, &b_Hits_sTGC_detectorNumber);
   fChain->SetBranchAddress("Hits_sTGC_sim_stationEta", &Hits_sTGC_sim_stationEta, &b_Hits_sTGC_sim_stationEta);
   fChain->SetBranchAddress("Hits_sTGC_sim_stationPhi", &Hits_sTGC_sim_stationPhi, &b_Hits_sTGC_sim_stationPhi);
   fChain->SetBranchAddress("Hits_sTGC_sim_multilayer", &Hits_sTGC_sim_multilayer, &b_Hits_sTGC_sim_multilayer);
   fChain->SetBranchAddress("Hits_sTGC_sim_layer", &Hits_sTGC_sim_layer, &b_Hits_sTGC_sim_layer);
   fChain->SetBranchAddress("Hits_sTGC_sim_side", &Hits_sTGC_sim_side, &b_Hits_sTGC_sim_side);
   fChain->SetBranchAddress("Hits_sTGC_stripNumber", &Hits_sTGC_stripNumber, &b_Hits_sTGC_stripNumber);
   fChain->SetBranchAddress("Hits_sTGC_wireNumber", &Hits_sTGC_wireNumber, &b_Hits_sTGC_wireNumber);
   fChain->SetBranchAddress("Hits_sTGC_off_stationName", &Hits_sTGC_off_stationName, &b_Hits_sTGC_off_stationName);
   fChain->SetBranchAddress("Hits_sTGC_off_stationEta", &Hits_sTGC_off_stationEta, &b_Hits_sTGC_off_stationEta);
   fChain->SetBranchAddress("Hits_sTGC_off_stationPhi", &Hits_sTGC_off_stationPhi, &b_Hits_sTGC_off_stationPhi);
   fChain->SetBranchAddress("Hits_sTGC_off_multiplet", &Hits_sTGC_off_multiplet, &b_Hits_sTGC_off_multiplet);
   fChain->SetBranchAddress("Hits_sTGC_off_gas_gap", &Hits_sTGC_off_gas_gap, &b_Hits_sTGC_off_gas_gap);
   fChain->SetBranchAddress("Hits_sTGC_off_channel_type", &Hits_sTGC_off_channel_type, &b_Hits_sTGC_off_channel_type);
   fChain->SetBranchAddress("Hits_sTGC_off_channel", &Hits_sTGC_off_channel, &b_Hits_sTGC_off_channel);
   fChain->SetBranchAddress("FastDigit_sTGC", &FastDigit_sTGC, &b_FastDigits_sTGC_n);
   fChain->SetBranchAddress("FastDigit_sTGC_stationName", &FastDigit_sTGC_stationName, &b_FastDigit_sTGC_stationName);
   fChain->SetBranchAddress("FastDigit_sTGC_stationEta", &FastDigit_sTGC_stationEta, &b_FastDigit_sTGC_stationEta);
   fChain->SetBranchAddress("FastDigit_sTGC_stationPhi", &FastDigit_sTGC_stationPhi, &b_FastDigit_sTGC_stationPhi);
   fChain->SetBranchAddress("FastDigit_sTGC_multiplet", &FastDigit_sTGC_multiplet, &b_FastDigit_sTGC_multiplet);
   fChain->SetBranchAddress("FastDigit_sTGC_gas_gap", &FastDigit_sTGC_gas_gap, &b_FastDigit_sTGC_gas_gap);
   fChain->SetBranchAddress("FastDigit_sTGC_channel_type", &FastDigit_sTGC_channel_type, &b_FastDigit_sTGC_channel_type);
   fChain->SetBranchAddress("FastDigit_sTGC_channel", &FastDigit_sTGC_channel, &b_FastDigit_sTGC_channel);
   fChain->SetBranchAddress("FastDigit_sTGC_globalPosX", &FastDigit_sTGC_globalPosX, &b_FastDigit_sTGC_globalPosX);
   fChain->SetBranchAddress("FastDigit_sTGC_globalPosY", &FastDigit_sTGC_globalPosY, &b_FastDigit_sTGC_globalPosY);
   fChain->SetBranchAddress("FastDigit_sTGC_globalPosZ", &FastDigit_sTGC_globalPosZ, &b_FastDigit_sTGC_globalPosZ);
   fChain->SetBranchAddress("FastDigit_sTGC_localPosX", &FastDigit_sTGC_localPosX, &b_FastDigit_sTGC_localPosX);
   fChain->SetBranchAddress("FastDigit_sTGC_localPosY", &FastDigit_sTGC_localPosY, &b_FastDigit_sTGC_localPosY);
   fChain->SetBranchAddress("FastDigit_sTGC_covMatrix_1_1", &FastDigit_sTGC_covMatrix_1_1, &b_FastDigit_sTGC_covMatrix_1_1);
   fChain->SetBranchAddress("FastDigit_sTGC_covMatrix_2_2", &FastDigit_sTGC_covMatrix_2_2, &b_FastDigit_sTGC_covMatrix_2_2);
   fChain->SetBranchAddress("FastDigit_sTGC_truth_barcode", &FastDigit_sTGC_truth_barcode, &b_FastDigit_sTGC_truth_barcode);
   fChain->SetBranchAddress("FastDigit_sTGC_truth_hitOnSurfaceX", &FastDigit_sTGC_truth_hitOnSurfaceX, &b_FastDigit_sTGC_truth_hitOnSurfaceX);
   fChain->SetBranchAddress("FastDigit_sTGC_truth_hitOnSurfaceY", &FastDigit_sTGC_truth_hitOnSurfaceY, &b_FastDigit_sTGC_truth_hitOnSurfaceY);
   fChain->SetBranchAddress("Hits_MM_n", &Hits_MM_n, &b_Hits_MM_n);
   fChain->SetBranchAddress("Hits_MM_trackId", &Hits_MM_trackId, &b_Hits_MM_trackId);
   fChain->SetBranchAddress("Hits_MM_truthEl", &Hits_MM_truthEl, &b_Hits_MM_truthEl);
   fChain->SetBranchAddress("Hits_MM_globalTime", &Hits_MM_globalTime, &b_Hits_MM_globalTime);
   fChain->SetBranchAddress("Hits_MM_hitGlobalPositionX", &Hits_MM_hitGlobalPositionX, &b_Hits_MM_hitGlobalPositionX);
   fChain->SetBranchAddress("Hits_MM_hitGlobalPositionY", &Hits_MM_hitGlobalPositionY, &b_Hits_MM_hitGlobalPositionY);
   fChain->SetBranchAddress("Hits_MM_hitGlobalPositionZ", &Hits_MM_hitGlobalPositionZ, &b_Hits_MM_hitGlobalPositionZ);
   fChain->SetBranchAddress("Hits_MM_hitGlobalPositionR", &Hits_MM_hitGlobalPositionR, &b_Hits_MM_hitGlobalPositionR);
   fChain->SetBranchAddress("Hits_MM_hitGlobalPositionP", &Hits_MM_hitGlobalPositionP, &b_Hits_MM_hitGlobalPositionP);
   fChain->SetBranchAddress("Hits_MM_hitGlobalDirectionX", &Hits_MM_hitGlobalDirectionX, &b_Hits_MM_hitGlobalDirectionX);
   fChain->SetBranchAddress("Hits_MM_hitGlobalDirectionY", &Hits_MM_hitGlobalDirectionY, &b_Hits_MM_hitGlobalDirectionY);
   fChain->SetBranchAddress("Hits_MM_hitGlobalDirectionZ", &Hits_MM_hitGlobalDirectionZ, &b_Hits_MM_hitGlobalDirectionZ);
   fChain->SetBranchAddress("Hits_MM_hitLocalPositionX", &Hits_MM_hitLocalPositionX, &b_Hits_MM_hitLocalPositionX);
   fChain->SetBranchAddress("Hits_MM_hitLocalPositionY", &Hits_MM_hitLocalPositionY, &b_Hits_MM_hitLocalPositionY);
   fChain->SetBranchAddress("Hits_MM_hitLocalPositionZ", &Hits_MM_hitLocalPositionZ, &b_Hits_MM_hitLocalPositionZ);
   fChain->SetBranchAddress("Hits_MM_detector_globalPositionX", &Hits_MM_detector_globalPositionX, &b_Hits_MM_detector_globalPositionX);
   fChain->SetBranchAddress("Hits_MM_detector_globalPositionY", &Hits_MM_detector_globalPositionY, &b_Hits_MM_detector_globalPositionY);
   fChain->SetBranchAddress("Hits_MM_detector_globalPositionZ", &Hits_MM_detector_globalPositionZ, &b_Hits_MM_detector_globalPositionZ);
   fChain->SetBranchAddress("Hits_MM_detector_globalPositionR", &Hits_MM_detector_globalPositionR, &b_Hits_MM_detector_globalPositionR);
   fChain->SetBranchAddress("Hits_MM_detector_globalPositionP", &Hits_MM_detector_globalPositionP, &b_Hits_MM_detector_globalPositionP);
   fChain->SetBranchAddress("Hits_MM_hitToDsurfacePositionX", &Hits_MM_hitToDsurfacePositionX, &b_Hits_MM_hitToDsurfacePositionX);
   fChain->SetBranchAddress("Hits_MM_hitToDsurfacePositionY", &Hits_MM_hitToDsurfacePositionY, &b_Hits_MM_hitToDsurfacePositionY);
   fChain->SetBranchAddress("Hits_MM_hitToDsurfacePositionZ", &Hits_MM_hitToDsurfacePositionZ, &b_Hits_MM_hitToDsurfacePositionZ);
   fChain->SetBranchAddress("Hits_MM_hitToRsurfacePositionX", &Hits_MM_hitToRsurfacePositionX, &b_Hits_MM_hitToRsurfacePositionX);
   fChain->SetBranchAddress("Hits_MM_hitToRsurfacePositionY", &Hits_MM_hitToRsurfacePositionY, &b_Hits_MM_hitToRsurfacePositionY);
   fChain->SetBranchAddress("Hits_MM_hitToRsurfacePositionZ", &Hits_MM_hitToRsurfacePositionZ, &b_Hits_MM_hitToRsurfacePositionZ);
   fChain->SetBranchAddress("Hits_MM_FastDigitRsurfacePositionX", &Hits_MM_FastDigitRsurfacePositionX, &b_Hits_MM_FastDigitRsurfacePositionX);
   fChain->SetBranchAddress("Hits_MM_FastDigitRsurfacePositionY", &Hits_MM_FastDigitRsurfacePositionY, &b_Hits_MM_FastDigitRsurfacePositionY);
   fChain->SetBranchAddress("Hits_MM_particleEncoding", &Hits_MM_particleEncoding, &b_Hits_MM_particleEncoding);
   fChain->SetBranchAddress("Hits_MM_kineticEnergy", &Hits_MM_kineticEnergy, &b_Hits_MM_kineticEnergy);
   fChain->SetBranchAddress("Hits_MM_depositEnergy", &Hits_MM_depositEnergy, &b_Hits_MM_depositEnergy);
   fChain->SetBranchAddress("Hits_MM_StepLength", &Hits_MM_StepLength, &b_Hits_MM_StepLength);
   fChain->SetBranchAddress("Hits_MM_sim_stationName", &Hits_MM_sim_stationName, &b_Hits_MM_sim_stationName);
   fChain->SetBranchAddress("Hits_MM_sim_stationEta", &Hits_MM_sim_stationEta, &b_Hits_MM_sim_stationEta);
   fChain->SetBranchAddress("Hits_MM_sim_stationPhi", &Hits_MM_sim_stationPhi, &b_Hits_MM_sim_stationPhi);
   fChain->SetBranchAddress("Hits_MM_sim_multilayer", &Hits_MM_sim_multilayer, &b_Hits_MM_sim_multilayer);
   fChain->SetBranchAddress("Hits_MM_sim_layer", &Hits_MM_sim_layer, &b_Hits_MM_sim_layer);
   fChain->SetBranchAddress("Hits_MM_sim_side", &Hits_MM_sim_side, &b_Hits_MM_sim_side);
   fChain->SetBranchAddress("Hits_MM_off_stationName", &Hits_MM_off_stationName, &b_Hits_MM_off_stationName);
   fChain->SetBranchAddress("Hits_MM_off_stationEta", &Hits_MM_off_stationEta, &b_Hits_MM_off_stationEta);
   fChain->SetBranchAddress("Hits_MM_off_stationPhi", &Hits_MM_off_stationPhi, &b_Hits_MM_off_stationPhi);
   fChain->SetBranchAddress("Hits_MM_off_multiplet", &Hits_MM_off_multiplet, &b_Hits_MM_off_multiplet);
   fChain->SetBranchAddress("Hits_MM_off_gas_gap", &Hits_MM_off_gas_gap, &b_Hits_MM_off_gas_gap);
   fChain->SetBranchAddress("Hits_MM_off_channel", &Hits_MM_off_channel, &b_Hits_MM_off_channel);
   fChain->SetBranchAddress("FastDigit_MM", &FastDigit_MM, &b_FastDigits_MM_n);
   fChain->SetBranchAddress("FastDigit_MM_stationName", &FastDigit_MM_stationName, &b_FastDigit_MM_stationName);
   fChain->SetBranchAddress("FastDigit_MM_stationEta", &FastDigit_MM_stationEta, &b_FastDigit_MM_stationEta);
   fChain->SetBranchAddress("FastDigit_MM_stationPhi", &FastDigit_MM_stationPhi, &b_FastDigit_MM_stationPhi);
   fChain->SetBranchAddress("FastDigit_MM_multiplet", &FastDigit_MM_multiplet, &b_FastDigit_MM_multiplet);
   fChain->SetBranchAddress("FastDigit_MM_gas_gap", &FastDigit_MM_gas_gap, &b_FastDigit_MM_gas_gap);
   fChain->SetBranchAddress("FastDigit_MM_channel", &FastDigit_MM_channel, &b_FastDigit_MM_channel);
   fChain->SetBranchAddress("FastDigit_MM_globalPosX", &FastDigit_MM_globalPosX, &b_FastDigit_MM_globalPosX);
   fChain->SetBranchAddress("FastDigit_MM_globalPosY", &FastDigit_MM_globalPosY, &b_FastDigit_MM_globalPosY);
   fChain->SetBranchAddress("FastDigit_MM_globalPosZ", &FastDigit_MM_globalPosZ, &b_FastDigit_MM_globalPosZ);
   fChain->SetBranchAddress("FastDigit_MM_localPosX", &FastDigit_MM_localPosX, &b_FastDigit_MM_localPosX);
   fChain->SetBranchAddress("FastDigit_MM_localPosY", &FastDigit_MM_localPosY, &b_FastDigit_MM_localPosY);
   fChain->SetBranchAddress("FastDigit_MM_covMatrix_1_1", &FastDigit_MM_covMatrix_1_1, &b_FastDigit_MM_covMatrix_1_1);
   fChain->SetBranchAddress("FastDigit_MM_truth_barcode", &FastDigit_MM_truth_barcode, &b_FastDigit_MM_truth_barcode);
   fChain->SetBranchAddress("FastDigit_MM_truth_hitOnSurfaceX", &FastDigit_MM_truth_hitOnSurfaceX, &b_FastDigit_MM_truth_hitOnSurfaceX);
   fChain->SetBranchAddress("FastDigit_MM_truth_hitOnSurfaceY", &FastDigit_MM_truth_hitOnSurfaceY, &b_FastDigit_MM_truth_hitOnSurfaceY);
   Notify();
}

Bool_t NSWHitsTree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void NSWHitsTree::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t NSWHitsTree::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef NSWHitsTree_cxx

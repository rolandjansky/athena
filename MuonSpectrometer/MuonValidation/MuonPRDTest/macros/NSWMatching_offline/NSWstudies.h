/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Jul 26 12:33:06 2018 by ROOT version 6.12/04
// from TTree NSWHitsTree/Ntuple of NSWHits
// found on file: NSWPRDValAlg.full.ntuple.root
//////////////////////////////////////////////////////////

#ifndef NSWstudies_h
#define NSWstudies_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"
#include "vector"
#include "vector"
#include "vector"
#include "vector"
#include "vector"

class NSWstudies {
public :
   //Additions
   void match_Hits_Digits (Flocalize_collection&, Flocalize_collection&);
   void fillHists (Flocalize_collection& oData, vector< TH1I* >& hist_vec);
   void plotError (Flocalize_collection oPRD, vector<TH1F*> hists_pull);

   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

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
   vector<double>  *MuEntry_Position_X;
   vector<double>  *MuEntry_Position_Y;
   vector<double>  *MuEntry_Position_Z;
   UInt_t          Hits_sTGC_n;
   vector<int>     *Hits_sTGC_trackId;
   vector<bool>    *Hits_sTGC_isInsideBounds;
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
   UInt_t          Digits_sTGC;
   UInt_t          Digits_sTGC_Pad_Digits;
   vector<double>  *Digits_sTGC_time;
   vector<int>     *Digits_sTGC_bctag;
   vector<double>  *Digits_sTGC_charge;
   vector<bool>    *Digits_sTGC_isDead;
   vector<bool>    *Digits_sTGC_isPileup;
   vector<string>  *Digits_sTGC_stationName;
   vector<int>     *Digits_sTGC_stationEta;
   vector<int>     *Digits_sTGC_stationPhi;
   vector<int>     *Digits_sTGC_multiplet;
   vector<int>     *Digits_sTGC_gas_gap;
   vector<int>     *Digits_sTGC_channel_type;
   vector<int>     *Digits_sTGC_channel;
   vector<int>     *Digits_sTGC_stationEtaMin;
   vector<int>     *Digits_sTGC_stationEtaMax;
   vector<int>     *Digits_sTGC_stationPhiMin;
   vector<int>     *Digits_sTGC_stationPhiMax;
   vector<int>     *Digits_sTGC_gas_gapMin;
   vector<int>     *Digits_sTGC_gas_gapMax;
   vector<int>     *Digits_sTGC_padEta;
   vector<int>     *Digits_sTGC_padPhi;
   vector<int>     *Digits_sTGC_numberOfMultilayers;
   vector<int>     *Digits_sTGC_multilayerMin;
   vector<int>     *Digits_sTGC_multilayerMax;
   vector<int>     *Digits_sTGC_channelTypeMin;
   vector<int>     *Digits_sTGC_channelTypeMax;
   vector<int>     *Digits_sTGC_channelMin;
   vector<int>     *Digits_sTGC_channelMax;
   vector<int>     *Digits_sTGC_channelNumber;
   vector<double>  *Digits_sTGC_channelPosX;
   vector<double>  *Digits_sTGC_channelPosY;
   vector<double>  *Digits_sTGC_localPosX;
   vector<double>  *Digits_sTGC_localPosY;
   vector<double>  *Digits_sTGC_globalPosX;
   vector<double>  *Digits_sTGC_globalPosY;
   vector<double>  *Digits_sTGC_globalPosZ;
   vector<double>  *Digits_sTGC_PadglobalCornerPosX;
   vector<double>  *Digits_sTGC_PadglobalCornerPosY;
   vector<double>  *Digits_sTGC_PadglobalCornerPosZ;
   UInt_t          SDO_sTGC;
   vector<string>  *SDO_sTGC_stationName;
   vector<int>     *SDO_sTGC_stationEta;
   vector<int>     *SDO_sTGC_stationPhi;
   vector<int>     *SDO_sTGC_multiplet;
   vector<int>     *SDO_sTGC_gas_gap;
   vector<int>     *SDO_sTGC_channel;
   vector<int>     *SDO_sTGC_channel_type;
   vector<int>     *SDO_sTGC_word;
   vector<int>     *SDO_sTGC_barcode;
   vector<double>  *SDO_sTGC_globalPosX;
   vector<double>  *SDO_sTGC_globalPosY;
   vector<double>  *SDO_sTGC_globalPosZ;
   vector<double>  *SDO_sTGC_global_time;
   vector<double>  *SDO_sTGC_Energy;
   vector<double>  *SDO_sTGC_tof;
   vector<double>  *SDO_sTGC_localPosX;
   vector<double>  *SDO_sTGC_localPosY;
   Int_t           RDO_sTGC_n;
   vector<string>  *RDO_sTGC_stationName;
   vector<int>     *RDO_sTGC_stationEta;
   vector<int>     *RDO_sTGC_stationPhi;
   vector<int>     *RDO_sTGC_multiplet;
   vector<int>     *RDO_sTGC_gas_gap;
   vector<int>     *RDO_sTGC_channel;
   vector<int>     *RDO_sTGC_channel_type;
   vector<double>  *RDO_sTGC_time;
   vector<double>  *RDO_sTGC_charge;
   vector<unsigned short> *RDO_sTGC_bcTag;
   vector<bool>    *RDO_sTGC_isDead;
   UInt_t          PRD_sTGC;
   vector<string>  *PRD_sTGC_stationName;
   vector<int>     *PRD_sTGC_stationEta;
   vector<int>     *PRD_sTGC_stationPhi;
   vector<int>     *PRD_sTGC_multiplet;
   vector<int>     *PRD_sTGC_gas_gap;
   vector<int>     *PRD_sTGC_channel_type;
   vector<int>     *PRD_sTGC_channel;
   vector<double>  *PRD_sTGC_globalPosX;
   vector<double>  *PRD_sTGC_globalPosY;
   vector<double>  *PRD_sTGC_globalPosZ;
   vector<double>  *PRD_sTGC_localPosX;
   vector<double>  *PRD_sTGC_localPosY;
   vector<double>  *PRD_sTGC_covMatrix_1_1;
   vector<double>  *PRD_sTGC_covMatrix_2_2;
   UInt_t          Hits_MM_n;
   vector<int>     *Hits_MM_trackId;
   vector<bool>    *Hits_MM_isInsideBounds;
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
   UInt_t          Digits_MM;
   vector<string>  *Digits_MM_stationName;
   vector<int>     *Digits_MM_stationEta;
   vector<int>     *Digits_MM_stationPhi;
   vector<int>     *Digits_MM_multiplet;
   vector<int>     *Digits_MM_gas_gap;
   vector<int>     *Digits_MM_channel;
   vector<vector<float> > *Digits_MM_time;
   vector<vector<float> > *Digits_MM_charge;
   vector<vector<int> > *Digits_MM_stripPosition;
   vector<vector<double> > *Digits_MM_stripLposX;
   vector<vector<double> > *Digits_MM_stripLposY;
   vector<vector<double> > *Digits_MM_stripGposX;
   vector<vector<double> > *Digits_MM_stripGposY;
   vector<vector<double> > *Digits_MM_stripGposZ;
   vector<vector<float> > *Digits_MM_stripResponse_time;
   vector<vector<float> > *Digits_MM_stripResponse_charge;
   vector<vector<int> > *Digits_MM_stripResponse_stripPosition;
   vector<vector<double> > *Digits_MM_stripResponse_stripLposX;
   vector<vector<double> > *Digits_MM_stripResponse_stripLposY;
   vector<vector<double> > *Digits_MM_stripresponse_stripGposX;
   vector<vector<double> > *Digits_MM_stripResponse_stripGposY;
   vector<vector<double> > *Digits_MM_stripResponse_stripGposZ;
   vector<vector<float> > *Digits_MM_time_trigger;
   vector<vector<float> > *Digits_MM_charge_trigger;
   vector<vector<int> > *Digits_MM_position_trigger;
   vector<vector<int> > *Digits_MM_MMFE_VMM_id_trigger;
   vector<vector<int> > *Digits_MM_VMM_id_trigger;
   UInt_t          SDO_MM;
   vector<string>  *SDO_MM_stationName;
   vector<int>     *SDO_MM_stationEta;
   vector<int>     *SDO_MM_stationPhi;
   vector<int>     *SDO_MM_multiplet;
   vector<int>     *SDO_MM_gas_gap;
   vector<int>     *SDO_MM_channel;
   vector<int>     *SDO_MM_word;
   vector<int>     *SDO_MM_barcode;
   vector<double>  *SDO_MM_globalPosX;
   vector<double>  *SDO_MM_globalPosY;
   vector<double>  *SDO_MM_globalPosZ;
   vector<double>  *SDO_MM_global_time;
   vector<double>  *SDO_MM_localPosX;
   vector<double>  *SDO_MM_localPosY;
   Int_t           RDO_MM_n;
   vector<string>  *RDO_MM_stationName;
   vector<int>     *RDO_MM_stationEta;
   vector<int>     *RDO_MM_stationPhi;
   vector<int>     *RDO_MM_multiplet;
   vector<int>     *RDO_MM_gas_gap;
   vector<int>     *RDO_MM_channel;
   vector<int>     *RDO_MM_time;
   vector<int>     *RDO_MM_charge;
   UInt_t          PRD_MM;
   vector<string>  *PRD_MM_stationName;
   vector<int>     *PRD_MM_stationEta;
   vector<int>     *PRD_MM_stationPhi;
   vector<int>     *PRD_MM_multiplet;
   vector<int>     *PRD_MM_gas_gap;
   vector<int>     *PRD_MM_channel;
   vector<double>  *PRD_MM_globalPosX;
   vector<double>  *PRD_MM_globalPosY;
   vector<double>  *PRD_MM_globalPosZ;
   vector<double>  *PRD_MM_localPosX;
   vector<double>  *PRD_MM_localPosY;
   vector<double>  *PRD_MM_covMatrix_1_1;

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
   TBranch        *b_MuEntry_Position_X;   //!
   TBranch        *b_MuEntry_Position_Y;   //!
   TBranch        *b_MuEntry_Position_Z;   //!
   TBranch        *b_Hits_sTGC_nSimHits;   //!
   TBranch        *b_Hits_sTGC_trackId;   //!
   TBranch        *b_Hits_sTGC_isInsideBounds;   //!
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
   TBranch        *b_Digits_sTGC_n;   //!
   TBranch        *b_Digits_sTGC_Pad_Digits_n;   //!
   TBranch        *b_Digits_sTGC_time;   //!
   TBranch        *b_Digits_sTGC_bctag;   //!
   TBranch        *b_Digits_sTGC_charge;   //!
   TBranch        *b_Digits_sTGC_isDead;   //!
   TBranch        *b_Digits_sTGC_isPileup;   //!
   TBranch        *b_Digits_sTGC_stationName;   //!
   TBranch        *b_Digits_sTGC_stationEta;   //!
   TBranch        *b_Digits_sTGC_stationPhi;   //!
   TBranch        *b_Digits_sTGC_multiplet;   //!
   TBranch        *b_Digits_sTGC_gas_gap;   //!
   TBranch        *b_Digits_sTGC_channel_type;   //!
   TBranch        *b_Digits_sTGC_channel;   //!
   TBranch        *b_Digits_sTGC_stationEtaMin;   //!
   TBranch        *b_Digits_sTGC_stationEtaMax;   //!
   TBranch        *b_Digits_sTGC_stationPhiMin;   //!
   TBranch        *b_Digits_sTGC_stationPhiMax;   //!
   TBranch        *b_Digits_sTGC_gas_gapMin;   //!
   TBranch        *b_Digits_sTGC_gas_gapMax;   //!
   TBranch        *b_Digits_sTGC_padEta;   //!
   TBranch        *b_Digits_sTGC_padPhi;   //!
   TBranch        *b_Digits_sTGC_numberOfMultilayers;   //!
   TBranch        *b_Digits_sTGC_multilayerMin;   //!
   TBranch        *b_Digits_sTGC_multilayerMax;   //!
   TBranch        *b_Digits_sTGC_channelTypeMin;   //!
   TBranch        *b_Digits_sTGC_channelTypeMax;   //!
   TBranch        *b_Digits_sTGC_channelMin;   //!
   TBranch        *b_Digits_sTGC_channelMax;   //!
   TBranch        *b_Digits_sTGC_channelNumber;   //!
   TBranch        *b_Digits_sTGC_channelPosX;   //!
   TBranch        *b_Digits_sTGC_channelPosY;   //!
   TBranch        *b_Digits_sTGC_localPosX;   //!
   TBranch        *b_Digits_sTGC_localPosY;   //!
   TBranch        *b_Digits_sTGC_globalPosX;   //!
   TBranch        *b_Digits_sTGC_globalPosY;   //!
   TBranch        *b_Digits_sTGC_globalPosZ;   //!
   TBranch        *b_Digits_sTGC_PadglobalCornerPosX;   //!
   TBranch        *b_Digits_sTGC_PadglobalCornerPosY;   //!
   TBranch        *b_Digits_sTGC_PadglobalCornerPosZ;   //!
   TBranch        *b_SDOs_sTGC_n;   //!
   TBranch        *b_SDO_sTGC_stationName;   //!
   TBranch        *b_SDO_sTGC_stationEta;   //!
   TBranch        *b_SDO_sTGC_stationPhi;   //!
   TBranch        *b_SDO_sTGC_multiplet;   //!
   TBranch        *b_SDO_sTGC_gas_gap;   //!
   TBranch        *b_SDO_sTGC_channel;   //!
   TBranch        *b_SDO_sTGC_channel_type;   //!
   TBranch        *b_SDO_sTGC_word;   //!
   TBranch        *b_SDO_sTGC_barcode;   //!
   TBranch        *b_SDO_sTGC_globalPosX;   //!
   TBranch        *b_SDO_sTGC_globalPosY;   //!
   TBranch        *b_SDO_sTGC_globalPosZ;   //!
   TBranch        *b_SDO_sTGC_global_time;   //!
   TBranch        *b_SDO_sTGC_Energy;   //!
   TBranch        *b_SDO_sTGC_tof;   //!
   TBranch        *b_SDO_sTGC_localPosX;   //!
   TBranch        *b_SDO_sTGC_localPosY;   //!
   TBranch        *b_RDO_sTGC_n;   //!
   TBranch        *b_RDO_sTGC_stationName;   //!
   TBranch        *b_RDO_sTGC_stationEta;   //!
   TBranch        *b_RDO_sTGC_stationPhi;   //!
   TBranch        *b_RDO_sTGC_multiplet;   //!
   TBranch        *b_RDO_sTGC_gas_gap;   //!
   TBranch        *b_RDO_sTGC_channel;   //!
   TBranch        *b_RDO_sTGC_channel_type;   //!
   TBranch        *b_RDO_sTGC_time;   //!
   TBranch        *b_RDO_sTGC_charge;   //!
   TBranch        *b_RDO_sTGC_bcTag;   //!
   TBranch        *b_RDO_sTGC_isDead;   //!
   TBranch        *b_PRDs_sTGC_n;   //!
   TBranch        *b_PRD_sTGC_stationName;   //!
   TBranch        *b_PRD_sTGC_stationEta;   //!
   TBranch        *b_PRD_sTGC_stationPhi;   //!
   TBranch        *b_PRD_sTGC_multiplet;   //!
   TBranch        *b_PRD_sTGC_gas_gap;   //!
   TBranch        *b_PRD_sTGC_channel_type;   //!
   TBranch        *b_PRD_sTGC_channel;   //!
   TBranch        *b_PRD_sTGC_globalPosX;   //!
   TBranch        *b_PRD_sTGC_globalPosY;   //!
   TBranch        *b_PRD_sTGC_globalPosZ;   //!
   TBranch        *b_PRD_sTGC_localPosX;   //!
   TBranch        *b_PRD_sTGC_localPosY;   //!
   TBranch        *b_PRD_sTGC_covMatrix_1_1;   //!
   TBranch        *b_PRD_sTGC_covMatrix_2_2;   //!
   TBranch        *b_Hits_MM_n;   //!
   TBranch        *b_Hits_MM_trackId;   //!
   TBranch        *b_Hits_MM_isInsideBounds;   //!
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
   TBranch        *b_Digits_MM_n;   //!
   TBranch        *b_Digits_MM_stationName;   //!
   TBranch        *b_Digits_MM_stationEta;   //!
   TBranch        *b_Digits_MM_stationPhi;   //!
   TBranch        *b_Digits_MM_multiplet;   //!
   TBranch        *b_Digits_MM_gas_gap;   //!
   TBranch        *b_Digits_MM_channel;   //!
   TBranch        *b_Digits_MM_time;   //!
   TBranch        *b_Digits_MM_charge;   //!
   TBranch        *b_Digits_MM_stripPosition;   //!
   TBranch        *b_Digits_MM_stripLposX;   //!
   TBranch        *b_Digits_MM_stripLposY;   //!
   TBranch        *b_Digits_MM_stripGposX;   //!
   TBranch        *b_Digits_MM_stripGposY;   //!
   TBranch        *b_Digits_MM_stripGposZ;   //!
   TBranch        *b_Digits_MM_stripResponse_time;   //!
   TBranch        *b_Digits_MM_stripResponse_charge;   //!
   TBranch        *b_Digits_MM_stripResponse_stripPosition;   //!
   TBranch        *b_Digits_MM_stripResponse_stripLposX;   //!
   TBranch        *b_Digits_MM_stripResponse_stripLposY;   //!
   TBranch        *b_Digits_MM_stripresponse_stripGposX;   //!
   TBranch        *b_Digits_MM_stripResponse_stripGposY;   //!
   TBranch        *b_Digits_MM_stripResponse_stripGposZ;   //!
   TBranch        *b_Digits_MM_time_trigger;   //!
   TBranch        *b_Digits_MM_charge_trigger;   //!
   TBranch        *b_Digits_MM_position_trigger;   //!
   TBranch        *b_Digits_MM_MMFE_VMM_id_trigger;   //!
   TBranch        *b_Digits_MM_VMM_id_trigger;   //!
   TBranch        *b_SDOs_MM_n;   //!
   TBranch        *b_SDO_MM_stationName;   //!
   TBranch        *b_SDO_MM_stationEta;   //!
   TBranch        *b_SDO_MM_stationPhi;   //!
   TBranch        *b_SDO_MM_multiplet;   //!
   TBranch        *b_SDO_MM_gas_gap;   //!
   TBranch        *b_SDO_MM_channel;   //!
   TBranch        *b_SDO_MM_word;   //!
   TBranch        *b_SDO_MM_barcode;   //!
   TBranch        *b_SDO_MM_globalPosX;   //!
   TBranch        *b_SDO_MM_globalPosY;   //!
   TBranch        *b_SDO_MM_globalPosZ;   //!
   TBranch        *b_SDO_MM_global_time;   //!
   TBranch        *b_SDO_MM_localPosX;   //!
   TBranch        *b_SDO_MM_localPosY;   //!
   TBranch        *b_RDO_MM_n;   //!
   TBranch        *b_RDO_MM_stationName;   //!
   TBranch        *b_RDO_MM_stationEta;   //!
   TBranch        *b_RDO_MM_stationPhi;   //!
   TBranch        *b_RDO_MM_multiplet;   //!
   TBranch        *b_RDO_MM_gas_gap;   //!
   TBranch        *b_RDO_MM_channel;   //!
   TBranch        *b_RDO_MM_time;   //!
   TBranch        *b_RDO_MM_charge;   //!
   TBranch        *b_PRDs_MM_n;   //!
   TBranch        *b_PRD_MM_stationName;   //!
   TBranch        *b_PRD_MM_stationEta;   //!
   TBranch        *b_PRD_MM_stationPhi;   //!
   TBranch        *b_PRD_MM_multiplet;   //!
   TBranch        *b_PRD_MM_gas_gap;   //!
   TBranch        *b_PRD_MM_channel;   //!
   TBranch        *b_PRD_MM_globalPosX;   //!
   TBranch        *b_PRD_MM_globalPosY;   //!
   TBranch        *b_PRD_MM_globalPosZ;   //!
   TBranch        *b_PRD_MM_localPosX;   //!
   TBranch        *b_PRD_MM_localPosY;   //!
   TBranch        *b_PRD_MM_covMatrix_1_1;   //!

   NSWstudies(TTree *tree=0);
   virtual ~NSWstudies();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef NSWstudies_cxx
NSWstudies::NSWstudies(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("NSWPRDValAlg.reco.ntuple.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("NSWPRDValAlg.reco.ntuple.root");
      }
      f->GetObject("NSWValTree",tree);

   }
   Init(tree);
}

NSWstudies::~NSWstudies()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t NSWstudies::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t NSWstudies::LoadTree(Long64_t entry)
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

void NSWstudies::Init(TTree *tree)
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
   MuEntry_Position_X = 0;
   MuEntry_Position_Y = 0;
   MuEntry_Position_Z = 0;
   Hits_sTGC_trackId = 0;
   Hits_sTGC_isInsideBounds = 0;
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
   Digits_sTGC_time = 0;
   Digits_sTGC_bctag = 0;
   Digits_sTGC_charge = 0;
   Digits_sTGC_isDead = 0;
   Digits_sTGC_isPileup = 0;
   Digits_sTGC_stationName = 0;
   Digits_sTGC_stationEta = 0;
   Digits_sTGC_stationPhi = 0;
   Digits_sTGC_multiplet = 0;
   Digits_sTGC_gas_gap = 0;
   Digits_sTGC_channel_type = 0;
   Digits_sTGC_channel = 0;
   Digits_sTGC_stationEtaMin = 0;
   Digits_sTGC_stationEtaMax = 0;
   Digits_sTGC_stationPhiMin = 0;
   Digits_sTGC_stationPhiMax = 0;
   Digits_sTGC_gas_gapMin = 0;
   Digits_sTGC_gas_gapMax = 0;
   Digits_sTGC_padEta = 0;
   Digits_sTGC_padPhi = 0;
   Digits_sTGC_numberOfMultilayers = 0;
   Digits_sTGC_multilayerMin = 0;
   Digits_sTGC_multilayerMax = 0;
   Digits_sTGC_channelTypeMin = 0;
   Digits_sTGC_channelTypeMax = 0;
   Digits_sTGC_channelMin = 0;
   Digits_sTGC_channelMax = 0;
   Digits_sTGC_channelNumber = 0;
   Digits_sTGC_channelPosX = 0;
   Digits_sTGC_channelPosY = 0;
   Digits_sTGC_localPosX = 0;
   Digits_sTGC_localPosY = 0;
   Digits_sTGC_globalPosX = 0;
   Digits_sTGC_globalPosY = 0;
   Digits_sTGC_globalPosZ = 0;
   Digits_sTGC_PadglobalCornerPosX = 0;
   Digits_sTGC_PadglobalCornerPosY = 0;
   Digits_sTGC_PadglobalCornerPosZ = 0;
   SDO_sTGC_stationName = 0;
   SDO_sTGC_stationEta = 0;
   SDO_sTGC_stationPhi = 0;
   SDO_sTGC_multiplet = 0;
   SDO_sTGC_gas_gap = 0;
   SDO_sTGC_channel = 0;
   SDO_sTGC_channel_type = 0;
   SDO_sTGC_word = 0;
   SDO_sTGC_barcode = 0;
   SDO_sTGC_globalPosX = 0;
   SDO_sTGC_globalPosY = 0;
   SDO_sTGC_globalPosZ = 0;
   SDO_sTGC_global_time = 0;
   SDO_sTGC_Energy = 0;
   SDO_sTGC_tof = 0;
   SDO_sTGC_localPosX = 0;
   SDO_sTGC_localPosY = 0;
   RDO_sTGC_stationName = 0;
   RDO_sTGC_stationEta = 0;
   RDO_sTGC_stationPhi = 0;
   RDO_sTGC_multiplet = 0;
   RDO_sTGC_gas_gap = 0;
   RDO_sTGC_channel = 0;
   RDO_sTGC_channel_type = 0;
   RDO_sTGC_time = 0;
   RDO_sTGC_charge = 0;
   RDO_sTGC_bcTag = 0;
   RDO_sTGC_isDead = 0;
   PRD_sTGC_stationName = 0;
   PRD_sTGC_stationEta = 0;
   PRD_sTGC_stationPhi = 0;
   PRD_sTGC_multiplet = 0;
   PRD_sTGC_gas_gap = 0;
   PRD_sTGC_channel_type = 0;
   PRD_sTGC_channel = 0;
   PRD_sTGC_globalPosX = 0;
   PRD_sTGC_globalPosY = 0;
   PRD_sTGC_globalPosZ = 0;
   PRD_sTGC_localPosX = 0;
   PRD_sTGC_localPosY = 0;
   PRD_sTGC_covMatrix_1_1 = 0;
   PRD_sTGC_covMatrix_2_2 = 0;
   Hits_MM_trackId = 0;
   Hits_MM_isInsideBounds = 0;
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
   Digits_MM_stationName = 0;
   Digits_MM_stationEta = 0;
   Digits_MM_stationPhi = 0;
   Digits_MM_multiplet = 0;
   Digits_MM_gas_gap = 0;
   Digits_MM_channel = 0;
   Digits_MM_time = 0;
   Digits_MM_charge = 0;
   Digits_MM_stripPosition = 0;
   Digits_MM_stripLposX = 0;
   Digits_MM_stripLposY = 0;
   Digits_MM_stripGposX = 0;
   Digits_MM_stripGposY = 0;
   Digits_MM_stripGposZ = 0;
   Digits_MM_stripResponse_time = 0;
   Digits_MM_stripResponse_charge = 0;
   Digits_MM_stripResponse_stripPosition = 0;
   Digits_MM_stripResponse_stripLposX = 0;
   Digits_MM_stripResponse_stripLposY = 0;
   Digits_MM_stripresponse_stripGposX = 0;
   Digits_MM_stripResponse_stripGposY = 0;
   Digits_MM_stripResponse_stripGposZ = 0;
   Digits_MM_time_trigger = 0;
   Digits_MM_charge_trigger = 0;
   Digits_MM_position_trigger = 0;
   Digits_MM_MMFE_VMM_id_trigger = 0;
   Digits_MM_VMM_id_trigger = 0;
   SDO_MM_stationName = 0;
   SDO_MM_stationEta = 0;
   SDO_MM_stationPhi = 0;
   SDO_MM_multiplet = 0;
   SDO_MM_gas_gap = 0;
   SDO_MM_channel = 0;
   SDO_MM_word = 0;
   SDO_MM_barcode = 0;
   SDO_MM_globalPosX = 0;
   SDO_MM_globalPosY = 0;
   SDO_MM_globalPosZ = 0;
   SDO_MM_global_time = 0;
   SDO_MM_localPosX = 0;
   SDO_MM_localPosY = 0;
   RDO_MM_stationName = 0;
   RDO_MM_stationEta = 0;
   RDO_MM_stationPhi = 0;
   RDO_MM_multiplet = 0;
   RDO_MM_gas_gap = 0;
   RDO_MM_channel = 0;
   RDO_MM_time = 0;
   RDO_MM_charge = 0;
   PRD_MM_stationName = 0;
   PRD_MM_stationEta = 0;
   PRD_MM_stationPhi = 0;
   PRD_MM_multiplet = 0;
   PRD_MM_gas_gap = 0;
   PRD_MM_channel = 0;
   PRD_MM_globalPosX = 0;
   PRD_MM_globalPosY = 0;
   PRD_MM_globalPosZ = 0;
   PRD_MM_localPosX = 0;
   PRD_MM_localPosY = 0;
   PRD_MM_covMatrix_1_1 = 0;
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
   fChain->SetBranchAddress("MuEntry_Position_X", &MuEntry_Position_X, &b_MuEntry_Position_X);
   fChain->SetBranchAddress("MuEntry_Position_Y", &MuEntry_Position_Y, &b_MuEntry_Position_Y);
   fChain->SetBranchAddress("MuEntry_Position_Z", &MuEntry_Position_Z, &b_MuEntry_Position_Z);
   fChain->SetBranchAddress("Hits_sTGC_n", &Hits_sTGC_n, &b_Hits_sTGC_nSimHits);
   fChain->SetBranchAddress("Hits_sTGC_trackId", &Hits_sTGC_trackId, &b_Hits_sTGC_trackId);
   fChain->SetBranchAddress("Hits_sTGC_isInsideBounds", &Hits_sTGC_isInsideBounds, &b_Hits_sTGC_isInsideBounds);
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
   fChain->SetBranchAddress("Digits_sTGC", &Digits_sTGC, &b_Digits_sTGC_n);
   fChain->SetBranchAddress("Digits_sTGC_Pad_Digits", &Digits_sTGC_Pad_Digits, &b_Digits_sTGC_Pad_Digits_n);
   fChain->SetBranchAddress("Digits_sTGC_time", &Digits_sTGC_time, &b_Digits_sTGC_time);
   fChain->SetBranchAddress("Digits_sTGC_bctag", &Digits_sTGC_bctag, &b_Digits_sTGC_bctag);
   fChain->SetBranchAddress("Digits_sTGC_charge", &Digits_sTGC_charge, &b_Digits_sTGC_charge);
   fChain->SetBranchAddress("Digits_sTGC_isDead", &Digits_sTGC_isDead, &b_Digits_sTGC_isDead);
   fChain->SetBranchAddress("Digits_sTGC_isPileup", &Digits_sTGC_isPileup, &b_Digits_sTGC_isPileup);
   fChain->SetBranchAddress("Digits_sTGC_stationName", &Digits_sTGC_stationName, &b_Digits_sTGC_stationName);
   fChain->SetBranchAddress("Digits_sTGC_stationEta", &Digits_sTGC_stationEta, &b_Digits_sTGC_stationEta);
   fChain->SetBranchAddress("Digits_sTGC_stationPhi", &Digits_sTGC_stationPhi, &b_Digits_sTGC_stationPhi);
   fChain->SetBranchAddress("Digits_sTGC_multiplet", &Digits_sTGC_multiplet, &b_Digits_sTGC_multiplet);
   fChain->SetBranchAddress("Digits_sTGC_gas_gap", &Digits_sTGC_gas_gap, &b_Digits_sTGC_gas_gap);
   fChain->SetBranchAddress("Digits_sTGC_channel_type", &Digits_sTGC_channel_type, &b_Digits_sTGC_channel_type);
   fChain->SetBranchAddress("Digits_sTGC_channel", &Digits_sTGC_channel, &b_Digits_sTGC_channel);
   fChain->SetBranchAddress("Digits_sTGC_stationEtaMin", &Digits_sTGC_stationEtaMin, &b_Digits_sTGC_stationEtaMin);
   fChain->SetBranchAddress("Digits_sTGC_stationEtaMax", &Digits_sTGC_stationEtaMax, &b_Digits_sTGC_stationEtaMax);
   fChain->SetBranchAddress("Digits_sTGC_stationPhiMin", &Digits_sTGC_stationPhiMin, &b_Digits_sTGC_stationPhiMin);
   fChain->SetBranchAddress("Digits_sTGC_stationPhiMax", &Digits_sTGC_stationPhiMax, &b_Digits_sTGC_stationPhiMax);
   fChain->SetBranchAddress("Digits_sTGC_gas_gapMin", &Digits_sTGC_gas_gapMin, &b_Digits_sTGC_gas_gapMin);
   fChain->SetBranchAddress("Digits_sTGC_gas_gapMax", &Digits_sTGC_gas_gapMax, &b_Digits_sTGC_gas_gapMax);
   fChain->SetBranchAddress("Digits_sTGC_padEta", &Digits_sTGC_padEta, &b_Digits_sTGC_padEta);
   fChain->SetBranchAddress("Digits_sTGC_padPhi", &Digits_sTGC_padPhi, &b_Digits_sTGC_padPhi);
   fChain->SetBranchAddress("Digits_sTGC_numberOfMultilayers", &Digits_sTGC_numberOfMultilayers, &b_Digits_sTGC_numberOfMultilayers);
   fChain->SetBranchAddress("Digits_sTGC_multilayerMin", &Digits_sTGC_multilayerMin, &b_Digits_sTGC_multilayerMin);
   fChain->SetBranchAddress("Digits_sTGC_multilayerMax", &Digits_sTGC_multilayerMax, &b_Digits_sTGC_multilayerMax);
   fChain->SetBranchAddress("Digits_sTGC_channelTypeMin", &Digits_sTGC_channelTypeMin, &b_Digits_sTGC_channelTypeMin);
   fChain->SetBranchAddress("Digits_sTGC_channelTypeMax", &Digits_sTGC_channelTypeMax, &b_Digits_sTGC_channelTypeMax);
   fChain->SetBranchAddress("Digits_sTGC_channelMin", &Digits_sTGC_channelMin, &b_Digits_sTGC_channelMin);
   fChain->SetBranchAddress("Digits_sTGC_channelMax", &Digits_sTGC_channelMax, &b_Digits_sTGC_channelMax);
   fChain->SetBranchAddress("Digits_sTGC_channelNumber", &Digits_sTGC_channelNumber, &b_Digits_sTGC_channelNumber);
   fChain->SetBranchAddress("Digits_sTGC_channelPosX", &Digits_sTGC_channelPosX, &b_Digits_sTGC_channelPosX);
   fChain->SetBranchAddress("Digits_sTGC_channelPosY", &Digits_sTGC_channelPosY, &b_Digits_sTGC_channelPosY);
   fChain->SetBranchAddress("Digits_sTGC_localPosX", &Digits_sTGC_localPosX, &b_Digits_sTGC_localPosX);
   fChain->SetBranchAddress("Digits_sTGC_localPosY", &Digits_sTGC_localPosY, &b_Digits_sTGC_localPosY);
   fChain->SetBranchAddress("Digits_sTGC_globalPosX", &Digits_sTGC_globalPosX, &b_Digits_sTGC_globalPosX);
   fChain->SetBranchAddress("Digits_sTGC_globalPosY", &Digits_sTGC_globalPosY, &b_Digits_sTGC_globalPosY);
   fChain->SetBranchAddress("Digits_sTGC_globalPosZ", &Digits_sTGC_globalPosZ, &b_Digits_sTGC_globalPosZ);
   fChain->SetBranchAddress("Digits_sTGC_PadglobalCornerPosX", &Digits_sTGC_PadglobalCornerPosX, &b_Digits_sTGC_PadglobalCornerPosX);
   fChain->SetBranchAddress("Digits_sTGC_PadglobalCornerPosY", &Digits_sTGC_PadglobalCornerPosY, &b_Digits_sTGC_PadglobalCornerPosY);
   fChain->SetBranchAddress("Digits_sTGC_PadglobalCornerPosZ", &Digits_sTGC_PadglobalCornerPosZ, &b_Digits_sTGC_PadglobalCornerPosZ);
   fChain->SetBranchAddress("SDO_sTGC", &SDO_sTGC, &b_SDOs_sTGC_n);
   fChain->SetBranchAddress("SDO_sTGC_stationName", &SDO_sTGC_stationName, &b_SDO_sTGC_stationName);
   fChain->SetBranchAddress("SDO_sTGC_stationEta", &SDO_sTGC_stationEta, &b_SDO_sTGC_stationEta);
   fChain->SetBranchAddress("SDO_sTGC_stationPhi", &SDO_sTGC_stationPhi, &b_SDO_sTGC_stationPhi);
   fChain->SetBranchAddress("SDO_sTGC_multiplet", &SDO_sTGC_multiplet, &b_SDO_sTGC_multiplet);
   fChain->SetBranchAddress("SDO_sTGC_gas_gap", &SDO_sTGC_gas_gap, &b_SDO_sTGC_gas_gap);
   fChain->SetBranchAddress("SDO_sTGC_channel", &SDO_sTGC_channel, &b_SDO_sTGC_channel);
   fChain->SetBranchAddress("SDO_sTGC_channel_type", &SDO_sTGC_channel_type, &b_SDO_sTGC_channel_type);
   fChain->SetBranchAddress("SDO_sTGC_word", &SDO_sTGC_word, &b_SDO_sTGC_word);
   fChain->SetBranchAddress("SDO_sTGC_barcode", &SDO_sTGC_barcode, &b_SDO_sTGC_barcode);
   fChain->SetBranchAddress("SDO_sTGC_globalPosX", &SDO_sTGC_globalPosX, &b_SDO_sTGC_globalPosX);
   fChain->SetBranchAddress("SDO_sTGC_globalPosY", &SDO_sTGC_globalPosY, &b_SDO_sTGC_globalPosY);
   fChain->SetBranchAddress("SDO_sTGC_globalPosZ", &SDO_sTGC_globalPosZ, &b_SDO_sTGC_globalPosZ);
   fChain->SetBranchAddress("SDO_sTGC_global_time", &SDO_sTGC_global_time, &b_SDO_sTGC_global_time);
   fChain->SetBranchAddress("SDO_sTGC_Energy", &SDO_sTGC_Energy, &b_SDO_sTGC_Energy);
   fChain->SetBranchAddress("SDO_sTGC_tof", &SDO_sTGC_tof, &b_SDO_sTGC_tof);
   fChain->SetBranchAddress("SDO_sTGC_localPosX", &SDO_sTGC_localPosX, &b_SDO_sTGC_localPosX);
   fChain->SetBranchAddress("SDO_sTGC_localPosY", &SDO_sTGC_localPosY, &b_SDO_sTGC_localPosY);
   fChain->SetBranchAddress("RDO_sTGC_n", &RDO_sTGC_n, &b_RDO_sTGC_n);
   fChain->SetBranchAddress("RDO_sTGC_stationName", &RDO_sTGC_stationName, &b_RDO_sTGC_stationName);
   fChain->SetBranchAddress("RDO_sTGC_stationEta", &RDO_sTGC_stationEta, &b_RDO_sTGC_stationEta);
   fChain->SetBranchAddress("RDO_sTGC_stationPhi", &RDO_sTGC_stationPhi, &b_RDO_sTGC_stationPhi);
   fChain->SetBranchAddress("RDO_sTGC_multiplet", &RDO_sTGC_multiplet, &b_RDO_sTGC_multiplet);
   fChain->SetBranchAddress("RDO_sTGC_gas_gap", &RDO_sTGC_gas_gap, &b_RDO_sTGC_gas_gap);
   fChain->SetBranchAddress("RDO_sTGC_channel", &RDO_sTGC_channel, &b_RDO_sTGC_channel);
   fChain->SetBranchAddress("RDO_sTGC_channel_type", &RDO_sTGC_channel_type, &b_RDO_sTGC_channel_type);
   fChain->SetBranchAddress("RDO_sTGC_time", &RDO_sTGC_time, &b_RDO_sTGC_time);
   fChain->SetBranchAddress("RDO_sTGC_charge", &RDO_sTGC_charge, &b_RDO_sTGC_charge);
   fChain->SetBranchAddress("RDO_sTGC_bcTag", &RDO_sTGC_bcTag, &b_RDO_sTGC_bcTag);
   fChain->SetBranchAddress("RDO_sTGC_isDead", &RDO_sTGC_isDead, &b_RDO_sTGC_isDead);
   fChain->SetBranchAddress("PRD_sTGC", &PRD_sTGC, &b_PRDs_sTGC_n);
   fChain->SetBranchAddress("PRD_sTGC_stationName", &PRD_sTGC_stationName, &b_PRD_sTGC_stationName);
   fChain->SetBranchAddress("PRD_sTGC_stationEta", &PRD_sTGC_stationEta, &b_PRD_sTGC_stationEta);
   fChain->SetBranchAddress("PRD_sTGC_stationPhi", &PRD_sTGC_stationPhi, &b_PRD_sTGC_stationPhi);
   fChain->SetBranchAddress("PRD_sTGC_multiplet", &PRD_sTGC_multiplet, &b_PRD_sTGC_multiplet);
   fChain->SetBranchAddress("PRD_sTGC_gas_gap", &PRD_sTGC_gas_gap, &b_PRD_sTGC_gas_gap);
   fChain->SetBranchAddress("PRD_sTGC_channel_type", &PRD_sTGC_channel_type, &b_PRD_sTGC_channel_type);
   fChain->SetBranchAddress("PRD_sTGC_channel", &PRD_sTGC_channel, &b_PRD_sTGC_channel);
   fChain->SetBranchAddress("PRD_sTGC_globalPosX", &PRD_sTGC_globalPosX, &b_PRD_sTGC_globalPosX);
   fChain->SetBranchAddress("PRD_sTGC_globalPosY", &PRD_sTGC_globalPosY, &b_PRD_sTGC_globalPosY);
   fChain->SetBranchAddress("PRD_sTGC_globalPosZ", &PRD_sTGC_globalPosZ, &b_PRD_sTGC_globalPosZ);
   fChain->SetBranchAddress("PRD_sTGC_localPosX", &PRD_sTGC_localPosX, &b_PRD_sTGC_localPosX);
   fChain->SetBranchAddress("PRD_sTGC_localPosY", &PRD_sTGC_localPosY, &b_PRD_sTGC_localPosY);
   fChain->SetBranchAddress("PRD_sTGC_covMatrix_1_1", &PRD_sTGC_covMatrix_1_1, &b_PRD_sTGC_covMatrix_1_1);
   fChain->SetBranchAddress("PRD_sTGC_covMatrix_2_2", &PRD_sTGC_covMatrix_2_2, &b_PRD_sTGC_covMatrix_2_2);
   fChain->SetBranchAddress("Hits_MM_n", &Hits_MM_n, &b_Hits_MM_n);
   fChain->SetBranchAddress("Hits_MM_trackId", &Hits_MM_trackId, &b_Hits_MM_trackId);
   fChain->SetBranchAddress("Hits_MM_isInsideBounds", &Hits_MM_isInsideBounds, &b_Hits_MM_isInsideBounds);
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
   fChain->SetBranchAddress("Digits_MM", &Digits_MM, &b_Digits_MM_n);
   fChain->SetBranchAddress("Digits_MM_stationName", &Digits_MM_stationName, &b_Digits_MM_stationName);
   fChain->SetBranchAddress("Digits_MM_stationEta", &Digits_MM_stationEta, &b_Digits_MM_stationEta);
   fChain->SetBranchAddress("Digits_MM_stationPhi", &Digits_MM_stationPhi, &b_Digits_MM_stationPhi);
   fChain->SetBranchAddress("Digits_MM_multiplet", &Digits_MM_multiplet, &b_Digits_MM_multiplet);
   fChain->SetBranchAddress("Digits_MM_gas_gap", &Digits_MM_gas_gap, &b_Digits_MM_gas_gap);
   fChain->SetBranchAddress("Digits_MM_channel", &Digits_MM_channel, &b_Digits_MM_channel);
   fChain->SetBranchAddress("Digits_MM_time", &Digits_MM_time, &b_Digits_MM_time);
   fChain->SetBranchAddress("Digits_MM_charge", &Digits_MM_charge, &b_Digits_MM_charge);
   fChain->SetBranchAddress("Digits_MM_stripPosition", &Digits_MM_stripPosition, &b_Digits_MM_stripPosition);
   fChain->SetBranchAddress("Digits_MM_stripLposX", &Digits_MM_stripLposX, &b_Digits_MM_stripLposX);
   fChain->SetBranchAddress("Digits_MM_stripLposY", &Digits_MM_stripLposY, &b_Digits_MM_stripLposY);
   fChain->SetBranchAddress("Digits_MM_stripGposX", &Digits_MM_stripGposX, &b_Digits_MM_stripGposX);
   fChain->SetBranchAddress("Digits_MM_stripGposY", &Digits_MM_stripGposY, &b_Digits_MM_stripGposY);
   fChain->SetBranchAddress("Digits_MM_stripGposZ", &Digits_MM_stripGposZ, &b_Digits_MM_stripGposZ);
   fChain->SetBranchAddress("Digits_MM_stripResponse_time", &Digits_MM_stripResponse_time, &b_Digits_MM_stripResponse_time);
   fChain->SetBranchAddress("Digits_MM_stripResponse_charge", &Digits_MM_stripResponse_charge, &b_Digits_MM_stripResponse_charge);
   fChain->SetBranchAddress("Digits_MM_stripResponse_stripPosition", &Digits_MM_stripResponse_stripPosition, &b_Digits_MM_stripResponse_stripPosition);
   fChain->SetBranchAddress("Digits_MM_stripResponse_stripLposX", &Digits_MM_stripResponse_stripLposX, &b_Digits_MM_stripResponse_stripLposX);
   fChain->SetBranchAddress("Digits_MM_stripResponse_stripLposY", &Digits_MM_stripResponse_stripLposY, &b_Digits_MM_stripResponse_stripLposY);
   fChain->SetBranchAddress("Digits_MM_stripresponse_stripGposX", &Digits_MM_stripresponse_stripGposX, &b_Digits_MM_stripresponse_stripGposX);
   fChain->SetBranchAddress("Digits_MM_stripResponse_stripGposY", &Digits_MM_stripResponse_stripGposY, &b_Digits_MM_stripResponse_stripGposY);
   fChain->SetBranchAddress("Digits_MM_stripResponse_stripGposZ", &Digits_MM_stripResponse_stripGposZ, &b_Digits_MM_stripResponse_stripGposZ);
   fChain->SetBranchAddress("Digits_MM_time_trigger", &Digits_MM_time_trigger, &b_Digits_MM_time_trigger);
   fChain->SetBranchAddress("Digits_MM_charge_trigger", &Digits_MM_charge_trigger, &b_Digits_MM_charge_trigger);
   fChain->SetBranchAddress("Digits_MM_position_trigger", &Digits_MM_position_trigger, &b_Digits_MM_position_trigger);
   fChain->SetBranchAddress("Digits_MM_MMFE_VMM_id_trigger", &Digits_MM_MMFE_VMM_id_trigger, &b_Digits_MM_MMFE_VMM_id_trigger);
   fChain->SetBranchAddress("Digits_MM_VMM_id_trigger", &Digits_MM_VMM_id_trigger, &b_Digits_MM_VMM_id_trigger);
   fChain->SetBranchAddress("SDO_MM", &SDO_MM, &b_SDOs_MM_n);
   fChain->SetBranchAddress("SDO_MM_stationName", &SDO_MM_stationName, &b_SDO_MM_stationName);
   fChain->SetBranchAddress("SDO_MM_stationEta", &SDO_MM_stationEta, &b_SDO_MM_stationEta);
   fChain->SetBranchAddress("SDO_MM_stationPhi", &SDO_MM_stationPhi, &b_SDO_MM_stationPhi);
   fChain->SetBranchAddress("SDO_MM_multiplet", &SDO_MM_multiplet, &b_SDO_MM_multiplet);
   fChain->SetBranchAddress("SDO_MM_gas_gap", &SDO_MM_gas_gap, &b_SDO_MM_gas_gap);
   fChain->SetBranchAddress("SDO_MM_channel", &SDO_MM_channel, &b_SDO_MM_channel);
   fChain->SetBranchAddress("SDO_MM_word", &SDO_MM_word, &b_SDO_MM_word);
   fChain->SetBranchAddress("SDO_MM_barcode", &SDO_MM_barcode, &b_SDO_MM_barcode);
   fChain->SetBranchAddress("SDO_MM_globalPosX", &SDO_MM_globalPosX, &b_SDO_MM_globalPosX);
   fChain->SetBranchAddress("SDO_MM_globalPosY", &SDO_MM_globalPosY, &b_SDO_MM_globalPosY);
   fChain->SetBranchAddress("SDO_MM_globalPosZ", &SDO_MM_globalPosZ, &b_SDO_MM_globalPosZ);
   fChain->SetBranchAddress("SDO_MM_global_time", &SDO_MM_global_time, &b_SDO_MM_global_time);
   fChain->SetBranchAddress("SDO_MM_localPosX", &SDO_MM_localPosX, &b_SDO_MM_localPosX);
   fChain->SetBranchAddress("SDO_MM_localPosY", &SDO_MM_localPosY, &b_SDO_MM_localPosY);
   fChain->SetBranchAddress("RDO_MM_n", &RDO_MM_n, &b_RDO_MM_n);
   fChain->SetBranchAddress("RDO_MM_stationName", &RDO_MM_stationName, &b_RDO_MM_stationName);
   fChain->SetBranchAddress("RDO_MM_stationEta", &RDO_MM_stationEta, &b_RDO_MM_stationEta);
   fChain->SetBranchAddress("RDO_MM_stationPhi", &RDO_MM_stationPhi, &b_RDO_MM_stationPhi);
   fChain->SetBranchAddress("RDO_MM_multiplet", &RDO_MM_multiplet, &b_RDO_MM_multiplet);
   fChain->SetBranchAddress("RDO_MM_gas_gap", &RDO_MM_gas_gap, &b_RDO_MM_gas_gap);
   fChain->SetBranchAddress("RDO_MM_channel", &RDO_MM_channel, &b_RDO_MM_channel);
   fChain->SetBranchAddress("RDO_MM_time", &RDO_MM_time, &b_RDO_MM_time);
   fChain->SetBranchAddress("RDO_MM_charge", &RDO_MM_charge, &b_RDO_MM_charge);
   fChain->SetBranchAddress("PRD_MM", &PRD_MM, &b_PRDs_MM_n);
   fChain->SetBranchAddress("PRD_MM_stationName", &PRD_MM_stationName, &b_PRD_MM_stationName);
   fChain->SetBranchAddress("PRD_MM_stationEta", &PRD_MM_stationEta, &b_PRD_MM_stationEta);
   fChain->SetBranchAddress("PRD_MM_stationPhi", &PRD_MM_stationPhi, &b_PRD_MM_stationPhi);
   fChain->SetBranchAddress("PRD_MM_multiplet", &PRD_MM_multiplet, &b_PRD_MM_multiplet);
   fChain->SetBranchAddress("PRD_MM_gas_gap", &PRD_MM_gas_gap, &b_PRD_MM_gas_gap);
   fChain->SetBranchAddress("PRD_MM_channel", &PRD_MM_channel, &b_PRD_MM_channel);
   fChain->SetBranchAddress("PRD_MM_globalPosX", &PRD_MM_globalPosX, &b_PRD_MM_globalPosX);
   fChain->SetBranchAddress("PRD_MM_globalPosY", &PRD_MM_globalPosY, &b_PRD_MM_globalPosY);
   fChain->SetBranchAddress("PRD_MM_globalPosZ", &PRD_MM_globalPosZ, &b_PRD_MM_globalPosZ);
   fChain->SetBranchAddress("PRD_MM_localPosX", &PRD_MM_localPosX, &b_PRD_MM_localPosX);
   fChain->SetBranchAddress("PRD_MM_localPosY", &PRD_MM_localPosY, &b_PRD_MM_localPosY);
   fChain->SetBranchAddress("PRD_MM_covMatrix_1_1", &PRD_MM_covMatrix_1_1, &b_PRD_MM_covMatrix_1_1);
   Notify();
}

Bool_t NSWstudies::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void NSWstudies::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t NSWstudies::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef NSWstudies_cxx

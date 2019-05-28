/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// *****************************************************************************
//  Filename : TrigTileMuFex.h
//  Author   : Heuijin Lim 
//  Created  : Sep. 2007
//
//  DESCRIPTION:
//    Muon candidates from TileCal are stored in the Feature, "TileMuFeature" 
//    and produced 2nd-ROIs. From these 2nd-ROIs, the tracks from 
//    Inner-Detector are extracted and compared with the muons from TileCal.
//    The combined muons with track are stored in the Feature, 
//    "TileTrkMuFeature". They will be used in TileMuHypo.cxx  of
//    Trigger/TrigHypothesis/TrigMuonHypo 
//  
//  HISTORY:
//
//  BUGS:
//
//*****************************************************************************

#ifndef  TRIGTILEMUID_TRIGTILEMUFEX_H
#define  TRIGTILEMUID_TRIGTILEMUFEX_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/IToolSvc.h"

#include "TrigInterfaces/FexAlgo.h"
#include "TrigMuonEvent/TileTrackMuFeature.h"
#include "TrigInDetEvent/TrigInDetTrack.h"

#include "MagFieldInterfaces/IMagFieldSvc.h"



namespace HLT {
  class TriggerElement;
}
class ITrigInDetTrackExtrapolator;
class TileMuFeature;



class TrigTileMuFex : public HLT::FexAlgo
{
 public:

  /** Constructor */
  TrigTileMuFex(const std::string& name, ISvcLocator* pSvcLocator);

  /** Destructor */
  ~TrigTileMuFex();

  /** hltInitialize */
  HLT::ErrorCode hltInitialize();

  /** hltExecute */
  HLT::ErrorCode hltExecute(const HLT::TriggerElement*, HLT::TriggerElement*);

  /** hltFinalize */
  HLT::ErrorCode hltFinalize();

  /** Magnetic Field Map callback */ 
  StatusCode magneticFieldInit(IOVSVC_CALLBACK_ARGS);

 private:

  TrigInDetTrack::AlgoId m_algoId;

  std::string m_ID_algo_to_use;
  float m_DelPhi_Cut;
  float m_DelEta_Cut;
  float m_Pt_Cut;
  std::string  m_Meth_ExtrapTileR;
  int m_Typ_IDTrk;

  /** Radius of calorimeter face */
  float m_RCAL;

  /** Z of calorimeter face */
  float m_ZCAL;

  /** Pointer for extrapolating TrigInDetTracks to calorimeter surface */
  ITrigInDetTrackExtrapolator* m_trackExtrapolator;
  std::string m_trackExtrapolatorName;

  bool m_GetTruthMuon;
  std::string m_key_for_truth;

  /** Magnetic field service */ 
  MagField::IMagFieldSvc* m_MagFieldSvc; 

  /** flag to switch on automatic configuration of algo. with magnetic field configuration */ 
  BooleanProperty m_useAthenaFieldService;

  /** Monitoring */
  int m_NTrack;
  int m_NTruth;
  std::vector<float> m_DelEtaTruth;
  std::vector<float> m_DelPhiTruth; 
  std::vector<float> m_DelPtTruth;
  std::vector<float> m_PtTruth;
  std::vector<float> m_PtTrack;
  std::vector<float> m_PtTrackAll;
  std::vector<float> m_EtaTrackAll;
  std::vector<float> m_PhiTrackAll; 
};

#endif 

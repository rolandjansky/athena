/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     TrigL2JpsieeHypo.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBphysHypo
// 
// AUTHOR:   Julie Kirk, Ricardo Neves
// 
// ********************************************************************

#ifndef TRIG_TrigL2JpsieeFex_H 
#define TRIG_TrigL2JpsieeFex_H

#include <string>
#include "GaudiKernel/ToolHandle.h"
#include "TrigInterfaces/FexAlgo.h"

#include "TrigInDetEvent/TrigInDetTrackFitPar.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
#include "TrigInDetToolInterfaces/ITrigVertexFitter.h"
#include "TrigInDetToolInterfaces/ITrigL2VertexFitter.h"
#include "TrigParticle/TrigL2Bphys.h"
#include "TrigParticle/TrigL2BphysContainer.h"

#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigCaloEvent/TrigEMCluster.h"
#include "TrigParticle/TrigElectron.h"
#include "TrigParticle/TrigElectronContainer.h"

#include "TrigBphysHypo/Constants.h"

class StoreGateSvc;
class TriggerElement;

class ITrigVertexFitter;
class ITrigL2VertexFitter;
class ITrigVertexingTool;

class ITrigInDetTrackExtrapolator;

class TrigL2JpsieeFex: public HLT::FexAlgo {
  
  public:
    TrigL2JpsieeFex(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigL2JpsieeFex();
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltFinalize();     
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE );
    HLT::ErrorCode acceptInput(const HLT::TriggerElement* inputTE, bool& pass );
    bool PassTrackPair(const TrigInDetTrackFitPar* param_1, const TrigInDetTrackFitPar* param_2);

  private:

    // Properties:
    
    StoreGateSvc* m_storeGate;

    ToolHandle<ITrigVertexFitter>  m_vertFitter;
    ToolHandle<ITrigL2VertexFitter> m_L2vertFitter;
    ToolHandle<ITrigVertexingTool> m_vertexingTool;
      
    unsigned int m_TrackAlgoId;
    float        m_TrackPTthr;
    float        m_dEtaTrackPair;
    float        m_dPhiTrackPair;
    float        m_dz0TrackPair;
    float        m_pTsumTrackPair;
    float        m_JpsiMasslow;
    float        m_JpsiMasshigh;
    bool         m_doVertexFit;
    bool         m_doT2Calo;
    bool         m_AcceptAll;

  unsigned int m_lastEvent;
  unsigned int m_lastEventPassed;
  unsigned int m_countTotalEvents;
  unsigned int m_countTotalRoI;
  unsigned int m_countPassedEvents;
  unsigned int m_countPassedRoIs;
  unsigned int m_countPassedTrackPair;
  unsigned int m_countPassedJpsiMass;

  // Cluster-RoI match cuts
  float  m_dEtaClusRoI;
  float  m_dPhiClusRoI;

  // Calo-ID matching
  float m_calotrackdeta;
  float m_calotrackdphi;
  float m_calotrackdeoverp_low;
  float m_calotrackdeoverp_high;
    
  // TRT track cut;
  float m_TRTCut;
  //radius and Z of calorimeter face 
  float m_RCAL;
  float m_ZCAL;
  
  TrigL2BphysContainer* m_trigBphysColl;
  TrigVertexCollection* m_VertexColl;

  /* monitored variables*/
  int mon_Ntrack;
  std::vector<float> mon_trackPt;
  std::vector<float> mon_dEta;
  std::vector<float> mon_dPhi;
  std::vector<float> mon_Eta;
  std::vector<float> mon_Phi;
  std::vector<float> mon_dz0;
  std::vector<float> mon_Ptsum;
  std::vector<float> mon_JpsiMass;
  std::vector<float> mon_JpsiFitMass;

  std::vector<float> mon_vRoIEta;
  std::vector<float> mon_vRoIPhi;
  std::vector<float> mon_vcalo_dEta;
  std::vector<float> mon_vcalo_dPhi; 
  std::vector<float> mon_vTRTRatio;
  std::vector<float> mon_vetoverpt;
  std::vector<float> mon_vdEtaCalo;
  std::vector<float> mon_vdPhiCalo;
  
  ITrigInDetTrackExtrapolator* m_trackExtrapolator;
  std::string m_trackExtrapolatorName;
};

#endif

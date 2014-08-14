/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigL2DsPhiPiHypo.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBphysHypo
//
// AUTHOR:   Julie Kirk
//
// ********************************************************************

#ifndef TRIG_TrigL2DsPhiPiFex_H
#define TRIG_TrigL2DsPhiPiFex_H

#include <string>
#include "GaudiKernel/ToolHandle.h"
#include "TrigInterfaces/FexAlgo.h"

#include "TrigInDetEvent/TrigInDetTrackFitPar.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
#include "TrigInDetToolInterfaces/ITrigVertexFitter.h"
#include "TrigInDetToolInterfaces/ITrigL2VertexFitter.h"
#include "TrigParticle/TrigL2Bphys.h"
#include "TrigParticle/TrigL2BphysContainer.h"

#include "TrigBphysHypo/Constants.h"

class StoreGateSvc;
class TriggerElement;

class ITrigVertexFitter;
class ITrigL2VertexFitter;
class ITrigVertexingTool;

class TrigL2DsPhiPiFex: public HLT::FexAlgo {

  public:
    TrigL2DsPhiPiFex(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigL2DsPhiPiFex();
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltFinalize();
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE );
    //    HLT::ErrorCode acceptInput(const HLT::TriggerElement* inputTE, bool& pass );
    bool PassTrackPair(const TrigInDetTrackFitPar* param_1, const TrigInDetTrackFitPar* param_2);
    double closeTrack(TrigVertex* p_Vtx, TrigInDetTrack* track);

  private:

    // Properties:

    StoreGateSvc* m_storeGate;

    //    ITrigVertexFitter* m_vertFitter;
    //ITrigL2VertexFitter* m_L2vertFitter;
    //TrigVertexingTool* m_vertexingTool;
    ToolHandle<ITrigVertexFitter>  m_vertFitter;
    ToolHandle<ITrigL2VertexFitter> m_L2vertFitter;
    ToolHandle<ITrigVertexingTool> m_vertexingTool;

    unsigned int m_trackalgoID;
    float        m_trackPtthr;
    float        m_dEta_cut;
    float        m_dPhi_cut;
    float        m_dz0_cut;
    float        m_pTsum_cut;
    std::vector<float>        m_Etabin;
    std::vector<float>        m_PhiMasslow_cut;
    std::vector<float>        m_PhiMasshigh_cut;
    std::vector<float>        m_DsMasslow_cut;
    std::vector<float>        m_DsMasshigh_cut;
    /// TS begin
    float        m_PhiChi2_cut;
    float        m_DsChi2_cut;
    /// TS end
    bool         m_doVertexFit;
    bool         m_acceptAll;
    unsigned int          m_NumberDsToKeep;

  TrigTimer* m_TotTimer;
  TrigTimer* m_VtxFitTimer;


  unsigned int m_lastEvent;
  unsigned int m_lastEventPassed;
  unsigned int m_countTotalEvents;
  unsigned int m_countTotalRoI;
  unsigned int m_countPassedEvents;
  unsigned int m_countPassedRoIs;
  unsigned int m_countPassedTrackPair;
  unsigned int m_countPassedPhiMass;
  unsigned int m_countPassedDsMass;
  /// TS begin
  unsigned int m_countPassedPhiChi2;
  unsigned int m_countPassedDsChi2;
  unsigned int m_countPassedPhiVertex;
  unsigned int m_countPassedDsVertex;
  /// TS end

  TrigL2BphysContainer* m_trigBphysColl;
  TrigL2BphysContainer* m_trigPhiColl;
  TrigVertexCollection* m_VertexColl;

  /* monitored variables*/
  int mon_Ntrack;
  std::vector<float> mon_trackPt;
  std::vector<float> mon_dEta;
  std::vector<float> mon_dPhi;
  std::vector<float> mon_dz0;
  std::vector<float> mon_Ptsum;
  std::vector<float> mon_PhiMass;
  std::vector<float> mon_PhiFitMass;
  std::vector<float> mon_DsMass;
  std::vector<float> mon_DsFitMass;
  /// EK, TS begin
  std::vector<float> mon_PhiChi2prob;
  std::vector<float> mon_PhiChi2prob_max;
  std::vector<float> mon_DsChi2prob;
  std::vector<float> mon_DsChi2prob_max;
  std::vector<float> mon_PhiChi2prob_DsPass;
  std::vector<float> mon_PhiChi2prob_max_DsPass;
  /// EK end

};

#endif

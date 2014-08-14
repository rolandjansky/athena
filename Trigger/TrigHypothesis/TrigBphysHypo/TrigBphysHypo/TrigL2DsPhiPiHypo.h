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

#ifndef TRIG_TrigL2DsPhiPiHypo_H
#define TRIG_TrigL2DsPhiPiHypo_H

#include <string>
#include "TrigInterfaces/HypoAlgo.h"
#include "TrigInDetEvent/TrigInDetTrackFitPar.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
#include "TrigInDetToolInterfaces/ITrigVertexFitter.h"
#include "TrigInDetToolInterfaces/ITrigL2VertexFitter.h"
#include "TrigVertexFitter/TrigVertexingTool.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigBphysHypo/Constants.h"

class StoreGateSvc;
class TriggerElement;

class TrigL2DsPhiPiHypo: public HLT::HypoAlgo {

  public:
    TrigL2DsPhiPiHypo(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigL2DsPhiPiHypo();
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltFinalize();
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);
    bool PassTrackPair(const TrigInDetTrackFitPar* param_1, const TrigInDetTrackFitPar* param_2);
    double Mass(std::vector<const TrigInDetTrackFitPar*> tracks, std::vector<double> massHypo);
    double closeTrack(TrigVertex* p_Vtx, TrigInDetTrack* track);
  private:

    // Properties:

    StoreGateSvc* m_storeGate;

    std::vector<float>        m_Etabin;
    std::vector<float>        m_PhiMasslow_cut;
    std::vector<float>        m_PhiMasshigh_cut;
    std::vector<float>        m_DsMasslow_cut;
    std::vector<float>        m_DsMasshigh_cut;
    bool         m_useVertexFit;
    bool         m_acceptAll;
    /// TS begin
    float        m_PhiChi2_cut;
    float        m_DsChi2_cut;
    /// TS end

  unsigned int m_lastEvent;
  unsigned int m_lastEventPassed;
  unsigned int m_countTotalEvents;
  unsigned int m_countTotalRoI;
  unsigned int m_countPassedEvents;
  unsigned int m_countPassedRoIs;
  unsigned int m_countPassedTrackPair;
  unsigned int m_countPassedPhiMass;
  unsigned int m_countPassedDsMass;
  /* monitored variables */
           int mon_cutCounter;
};

#endif

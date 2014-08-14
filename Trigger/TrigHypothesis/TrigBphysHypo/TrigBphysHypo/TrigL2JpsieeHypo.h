/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     TrigL2JpsieeHypo.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBphysHypo
// 
// AUTHOR:   Julie Kirk
// 
// ********************************************************************

#ifndef TRIG_TrigL2JpsieeHypo_H 
#define TRIG_TrigL2JpsieeHypo_H

#include <string>
#include "TrigInterfaces/HypoAlgo.h"
#include "TrigInDetEvent/TrigInDetTrackFitPar.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
#include "TrigInDetToolInterfaces/ITrigVertexFitter.h"
#include "TrigInDetToolInterfaces/ITrigL2VertexFitter.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigBphysHypo/Constants.h"

class StoreGateSvc;
class TriggerElement;

class TrigL2JpsieeHypo: public HLT::HypoAlgo {
  
  public:
    TrigL2JpsieeHypo(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigL2JpsieeHypo();
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltFinalize();     
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);
    bool PassTrackPair(const TrigInDetTrackFitPar* param_1, const TrigInDetTrackFitPar* param_2);
    double Mass(std::vector<const TrigInDetTrackFitPar*> tracks, std::vector<double> massHypo);
    double closeTrack(TrigVertex* p_Vtx, TrigInDetTrack* track);
  private:

    // Properties:
    
    StoreGateSvc* m_storeGate;

    float        m_JpsiMasslow_cut;
    float        m_JpsiMasshigh_cut;
    bool         m_useVertexFit;
    bool         m_acceptAll;

  unsigned int m_lastEvent;
  unsigned int m_lastEventPassed;
  unsigned int m_countTotalEvents;
  unsigned int m_countTotalRoI;
  unsigned int m_countPassedEvents;
  unsigned int m_countPassedRoIs;
  unsigned int m_countPassedJpsiMass;

  /* monitored variables */
  int mon_cutCounter;
  std::vector<float> mon_Mass; 
};

#endif

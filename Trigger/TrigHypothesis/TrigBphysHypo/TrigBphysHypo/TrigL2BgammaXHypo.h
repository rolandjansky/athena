/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     TrigL2BgammaXHypo.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBphysHypo
// 
// AUTHOR:   Julie Kirk
// copied from Jpsiee, currently dummy algo
// ********************************************************************

#ifndef TRIG_TrigL2BgammaXHypo_H 
#define TRIG_TrigL2BgammaXHypo_H

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

class TrigL2BgammaXHypo: public HLT::HypoAlgo {
  
  public:
    TrigL2BgammaXHypo(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigL2BgammaXHypo();
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltFinalize();     
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);
    bool PassTrackPair(const TrigInDetTrackFitPar* param_1, const TrigInDetTrackFitPar* param_2);
    double Mass(std::vector<const TrigInDetTrackFitPar*> tracks, std::vector<double> massHypo);
  private:

    // Properties:
    
    StoreGateSvc* m_storeGate;

    bool         m_acceptAll;

  unsigned int m_lastEvent;
  unsigned int m_lastEventPassed;
  unsigned int m_countTotalEvents;
  unsigned int m_countTotalRoI;
  unsigned int m_countPassedEvents;
  unsigned int m_countPassedRoIs;
  unsigned int m_countPassedJpsiMass;
           int m_cutCounter;
};

#endif

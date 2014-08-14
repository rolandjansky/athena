/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     TrigEFDsPhiPiHypo.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBphysHypo
// 
// AUTHOR:   Julie Kirk
// 
// ********************************************************************

#ifndef TRIG_TrigEFDsPhiPiHypo_H 
#define TRIG_TrigEFDsPhiPiHypo_H

#include <string>
#include "TrigInterfaces/HypoAlgo.h"
#include "Particle/TrackParticleContainer.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigBphysHypo/Constants.h"

class StoreGateSvc;
class TriggerElement;

class TrigEFDsPhiPiHypo: public HLT::HypoAlgo {
  
  public:
    TrigEFDsPhiPiHypo(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigEFDsPhiPiHypo();
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, bool& pass);
    HLT::ErrorCode hltFinalize();     
    bool execHLTAlgorithm(TriggerElement* outputTE);
    bool PassTrackPair(const Trk::Perigee* trckPerigee1, const Trk::Perigee* trckPerigee2);
    double Mass(std::vector<const  Trk::Perigee*> tracks, std::vector<double> massHypo);
    //    double closeTrack(TrigVertex* p_Vtx, TrigInDetTrack* track);
  private:

    // Properties:
    std::vector<float>        m_Etabin;
    std::vector<float>        m_PhiMasslow_cut;
    std::vector<float>        m_PhiMasshigh_cut;
    std::vector<float>        m_DsMasslow_cut;
    std::vector<float>        m_DsMasshigh_cut;


    bool m_acceptAll;
    bool m_useVertexFit;

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

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     TrigEFDsPhiPiFex.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBphysHypo
// 
// AUTHOR:   Julie Kirk
// 
// ********************************************************************

#ifndef TRIG_TrigEFDsPhiPiFex_H 
#define TRIG_TrigEFDsPhiPiFex_H

#include <string>
#include "TrigInterfaces/FexAlgo.h"
#include "Particle/TrackParticleContainer.h"
#include "TrigParticle/TrigEFBphys.h"
#include "TrigParticle/TrigEFBphysContainer.h"

#include "TrigBphysHypo/Constants.h"

class StoreGateSvc;
class TriggerElement;

class TrigEFDsPhiPiFex: public HLT::FexAlgo {
  
  public:
    TrigEFDsPhiPiFex(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigEFDsPhiPiFex();
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltFinalize();     
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE);
    HLT::ErrorCode acceptInput(const HLT::TriggerElement* inputTE, bool& pass );
    bool PassTrackPair(const Trk::Perigee* trckPerigee1, const Trk::Perigee* trckPerigee2);
    //    double closeTrack(TrigVertex* p_Vtx, TrigInDetTrack* track);
  private:

    // Properties:
    
    
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


  unsigned int m_lastEvent;
  unsigned int m_lastEventPassed;
  unsigned int m_countTotalEvents;
  unsigned int m_countTotalRoI;
  unsigned int m_countPassedEvents;
  unsigned int m_countPassedRoIs;
  unsigned int m_countPassedTrackPair;
  unsigned int m_countPassedPhiMass;
  unsigned int m_countPassedDsMass;

  bool m_acceptAll;

  TrigEFBphysContainer* m_trigBphysColl;

  /* monitored variables*/
  int mon_Ntrack;
  std::vector<float> mon_trackPt;
  std::vector<float> mon_dEta;
  std::vector<float> mon_dPhi;
  std::vector<float> mon_dz0;
  std::vector<float> mon_Ptsum;
  std::vector<float> mon_PhiMass;
  //std::vector<float> mon_PhiFitMass;
  std::vector<float> mon_DsMass;
  //std::vector<float> mon_DsFitMass;
};

#endif

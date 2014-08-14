/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     TrigEFJpsieeFex.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBphysHypo
// 
// AUTHOR:   Julie Kirk
// 
// ********************************************************************

#ifndef TRIG_TrigEFJpsieeFex_H 
#define TRIG_TrigEFJpsieeFex_H

#include <string>
#include "TrigInterfaces/FexAlgo.h"
#include "Particle/TrackParticleContainer.h"
#include "TrigParticle/TrigEFBphys.h"
#include "TrigParticle/TrigEFBphysContainer.h"
#include "TrkParameters/TrackParameters.h"

#include "TrigBphysHypo/Constants.h"

class StoreGateSvc;
class TriggerElement;

class TrigEFJpsieeFex: public HLT::FexAlgo {
  
  public:
    TrigEFJpsieeFex(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigEFJpsieeFex();
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltFinalize();     
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE);
    HLT::ErrorCode acceptInput(const HLT::TriggerElement* inputTE, bool& pass );
    bool PassTrackPair(const Trk::Perigee* trckPerigee1, const Trk::Perigee* trckPerigee2);

  private:

    // Properties:
    
    float        m_trackPtthr;
    float        m_dEta_cut;
    float        m_dPhi_cut;
    float        m_dz0_cut;
    float        m_pTsum_cut;


  bool m_acceptAll;

};

#endif

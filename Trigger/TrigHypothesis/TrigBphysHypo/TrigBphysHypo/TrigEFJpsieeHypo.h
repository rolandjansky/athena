/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     TrigEFJpsieeHypo.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBphysHypo
// 
// AUTHOR:   Julie Kirk
// 
// ********************************************************************

#ifndef TRIG_TrigEFJpsieeHypo_H 
#define TRIG_TrigEFJpsieeHypo_H

#include <string>
#include "TrigInterfaces/HypoAlgo.h"
#include "Particle/TrackParticleContainer.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigBphysHypo/Constants.h"

class StoreGateSvc;
class TriggerElement;

class TrigEFJpsieeHypo: public HLT::HypoAlgo {
  
  public:
    TrigEFJpsieeHypo(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigEFJpsieeHypo();
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, bool& pass);
    HLT::ErrorCode hltFinalize();     
    bool execHLTAlgorithm(TriggerElement* outputTE);
    bool PassTrackPair(const Trk::Perigee* trckPerigee1, const Trk::Perigee* trckPerigee2);
    double Mass(std::vector<const  Trk::Perigee*> tracks, std::vector<double> massHypo);
    //    double closeTrack(TrigVertex* p_Vtx, TrigInDetTrack* track);
  private:


    bool m_acceptAll;

};

#endif

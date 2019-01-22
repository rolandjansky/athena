/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBphysHypo/TrigEFBMuMuFex.h
 **
 **   Description: EF hypothesis algorithms for B0_s -> mu+ mu-  
 **                currently a dummy algorithm
 **
 **   Author: J.Kirk
 **
 **   Created:   12.09.07
 **   Modified:     
 **
 **************************************************************************/ 

#ifndef TRIG_TrigEFBMuMuHypo_H 
#define TRIG_TrigEFBMuMuHypo_H

// standard stuff
#include <string>
// general athena stuff
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "TrigInterfaces/HypoAlgo.h"

#include "TrigParticle/TrigEFBphys.h"
#include "TrigParticle/TrigEFBphysContainer.h"
#include "BeamSpotConditionsData/BeamSpotData.h"
#include "TrigBphysHypo/Constants.h"

class TrigEFBMuMuHypo: public HLT::HypoAlgo  {
  
  public:
    TrigEFBMuMuHypo(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigEFBMuMuHypo();
    HLT::ErrorCode hltInitialize();
    //StatusCode execute();
    HLT::ErrorCode hltFinalize();     
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);
    
  private:


    // Mass window cuts
    bool m_oppositeCharge;
    float m_lowerMassCut;
    float m_upperMassCut;
    float m_LxyCut;
    bool m_ApplyupperMassCut;
    bool m_ApplyChi2Cut;
    bool m_ApplyLxyCut;
    float m_Chi2VtxCut;

    // to set Accept-All mode: should be done with force-accept when possible
    bool m_acceptAll;


  //Counters
  int m_lastEvent;
  int m_lastEventPassed;
  unsigned int m_countTotalEvents;
  unsigned int m_countTotalRoI;
  unsigned int m_countPassedEvents;
  unsigned int m_countPassedRoIs;
  unsigned int m_countPassedBsMass;
  unsigned int m_countPassedChi2Cut;
  unsigned int m_countPassedLxyCut;


  int m_mon_cutCounter;
  double m_mon_MuMumass;

  SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };

};

#endif

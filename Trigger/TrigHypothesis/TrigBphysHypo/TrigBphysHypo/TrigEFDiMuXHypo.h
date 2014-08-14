/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBphysHypo/TrigL2DiMuXHypo.h
 **
 **   Description: L2 hypothesis algorithms for B+ -> K+ mu+ mu-  
 **                Create TrigL2Bphys
 **
 **   Author: C. Adorisio
 **
 **   Created:   10.03.2009
 **   Modified:  
 **
 **************************************************************************/ 
#ifndef TRIG_TrigEFDiMuXHypo_H 
#define TRIG_TrigEFDiMuXHypo_H

#include "TrigInterfaces/HypoAlgo.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"

#include "Particle/TrackParticleContainer.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigParticle/TrigEFBphys.h"
#include "TrigParticle/TrigEFBphysContainer.h"

#include "TrigBphysHypo/Constants.h"

class TrigEFDiMuXHypo: public HLT::HypoAlgo {
  
 public:
  TrigEFDiMuXHypo(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigEFDiMuXHypo();
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, bool& pass);
  HLT::ErrorCode hltFinalize();     
  
 private:
  bool m_acceptAll;
  float m_lowerBplusMassCut;
  float m_upperBplusMassCut;
  
  int m_lastEvent;
  int m_lastEventPassed;
  
  unsigned int m_countTotalEvents;
  unsigned int m_countTotalRoI;
  unsigned int m_countPassedEvents;
  unsigned int m_countPassedRoIs;
  unsigned int m_countPassedBplusMass;
  
  /* monitored variables*/
  std::vector<float> mon_bmass;
};

#endif

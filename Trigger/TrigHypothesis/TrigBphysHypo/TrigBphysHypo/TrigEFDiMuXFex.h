/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBphysHypo/TrigEFDiMuXFex.h
 **
 **   Description: EF hypothesis algorithms for B+ -> K+ mu+ mu-  
 **
 **   Author: C. Adorisio (Cristina.Adorisio@cern.ch)
 **
 **   Created:   27.10.2008
 **   Modified:  
 **              
 **
 **************************************************************************/ 

#ifndef TRIG_TrigEFDiMuXFex_H 
#define TRIG_TrigEFDiMuXFex_H

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigInterfaces/FexAlgo.h"

#include "Particle/TrackParticleContainer.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigParticle/TrigEFBphys.h"
#include "TrigParticle/TrigEFBphysContainer.h"

#include "TrigBphysHypo/Constants.h"
#include "TrkParameters/TrackParameters.h"

class TrigEFDiMuXFex: public HLT::FexAlgo {
  
 public:
  TrigEFDiMuXFex(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigEFDiMuXFex();
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();     
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE);

 private:
  
  //Counters
  int m_lastEvent;
  int m_lastEvent_DiMu;
  int m_lastEventPassed;
  int m_lastEventPassedMuMu;
  int m_lastEventPassedKMuMu;
  
  unsigned int m_countTotalEvents;
  unsigned int m_countTotalRoI;
  unsigned int m_countTotalEvents_DiMu;
  unsigned int m_countTotalRoI_DiMu;

  unsigned int m_countPassedEvents;
  unsigned int m_countPassedRoIs;
  unsigned int m_countPassedEventsMuMu;
  unsigned int m_countPassedEventsKMuMu;
  
  unsigned int m_countPassedMuMuMassCut;
  unsigned int m_countPassedKMuMuMassCut;


  /* Properties */
  BooleanProperty m_acceptAll;

  // Mass window cuts
  float m_lowerMuMuMassCut;
  float m_upperMuMuMassCut;
  float m_lowerKplusMuMuMassCut;
  float m_upperKplusMuMuMassCut;
  
  /* monitored variables*/
  std::vector<float> mon_dimumass; 
  std::vector<float> mon_kdimumass;

  TrigEFBphysContainer* m_trigBphysColl;
  
};

#endif

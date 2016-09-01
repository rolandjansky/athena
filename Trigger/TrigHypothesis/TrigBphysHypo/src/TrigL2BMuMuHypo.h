/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBphysHypo/TrigL2BMuMuFex.h
 **
 **   Description: L2 hypothesis algorithms for B0_s -> mu+ mu-  
 **                Create TrigL2Bphys
 **
 **   Author: S.Sivoklokov (on the base of Carlo Schiavi example)
 **
 **   Created:   05.092006
 **   Modified:     
 **
 **************************************************************************/ 

#ifndef TRIG_TrigL2BMuMuHypo_H 
#define TRIG_TrigL2BMuMuHypo_H

// standard stuff
#include <string>
// general athena stuff
//#include "GaudiKernel/MsgStream.h"
//#include "GaudiKernel/IToolSvc.h"
//#include "GaudiKernel/StatusCode.h"
#include "TrigInterfaces/HypoAlgo.h"

//#include "TrigTimeAlgs/TrigTimerSvc.h"
//#include "TrigInDetEvent/TrigVertexCollection.h"
//#include "TrigParticle/TrigL2Bphys.h"
//#include "TrigParticle/TrigL2BphysContainer.h"

#include "TrigBphysHypo/Constants.h"

class TrigBphysHelperUtilsTool;


class TrigL2BMuMuHypo: public HLT::HypoAlgo  {
  
  public:
    TrigL2BMuMuHypo(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigL2BMuMuHypo();
    HLT::ErrorCode hltInitialize();
    //StatusCode execute();
    HLT::ErrorCode hltFinalize();     
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);
    
  private:
    ToolHandle <TrigBphysHelperUtilsTool> m_bphysHelperTool;


    // Mass window cuts
    bool m_oppositeCharge;
    float m_lowerMassCut;
    float m_upperMassCut;
    bool m_ApplyupperMassCut;
    bool m_ApplyChi2Cut;
    float m_Chi2VtxCut;

    // to set Accept-All mode: should be done with force-accept when possible
    bool m_acceptAll;
    
    bool m_acceptSameMuon;


  //Counters
  int m_lastEvent;
  int m_lastEventPassed;
  unsigned int m_countTotalEvents;
  unsigned int m_countTotalRoI;
  unsigned int m_countPassedEvents;
  unsigned int m_countPassedRoIs;
  unsigned int m_countPassedBsMass;
  unsigned int m_countPassedChi2Cut;

    //  TrigL2BphysContainer* m_trigBphysColl;
    //  TrigVertexCollection* m_VertexColl;

  /* monitored variables */
  int m_mon_cutCounter;
  std::vector<float> m_mon_MuMumass;
  
};

#endif

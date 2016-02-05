/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ***************************************************
//
// NAME:     HLTBjetMonTool.h
// PACKAGE:  Trigger/TrigMonitoring/TrigHLTMonitoring
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro@cern.ch
// 
// ***************************************************

#ifndef HLTBJETMONTOOL_H
#define HLTBJETMONTOOL_H

#include "TrigHLTMonitoring/IHLTMonTool.h"
#include <string>
#include <vector>



#include "TrigTrackJetFinderTool/ITrigTrackJetFinderTool.h"


#include "TrigDecisionTool/TrigDecisionTool.h"


class HLTBjetMonTool : public IHLTMonTool {
  
 public:

  HLTBjetMonTool(const std::string & type, const std::string & name, const IInterface* parent); 
  ~HLTBjetMonTool();
  
  StatusCode init();


#ifdef ManagedMonitorToolBase_Uses_API_201401
  StatusCode book();
#else
  StatusCode book(bool newEventsBlock, bool newLumiBlock, bool newRun);  
#endif


  StatusCode fill();


#ifdef ManagedMonitorToolBase_Uses_API_201401
  StatusCode proc();
#else
  StatusCode proc(bool endOfEventsBlock, bool endOfLumiBlock, bool endOfRun);  
#endif



 private:

  ToolHandle<ITrigTrackJetFinderTool> m_trackJetFinderTool;

  std::vector<std::string> m_TriggerChainBjet, m_TriggerChainMujet;
 
  ToolHandle<Trig::TrigDecisionTool> m_trigDec; //!

  float phiCorr(float);
  std::string m_priVtxKey;
  float m_etCut;

};

#endif

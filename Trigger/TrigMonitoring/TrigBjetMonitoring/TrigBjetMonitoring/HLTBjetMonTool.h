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
#include <map>



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
  std::vector<std::string> m_TriggerChainBjet_x, m_TriggerChainMujet_x;

  std::map<std::string,std::string> m_Chain2Dir;
  std::vector<std::string> m_Shifter_jSplit, m_Expert_jSplit, m_Shifter_jUnSplit,  m_Expert_jUnSplit, m_Shifter_mujet, m_Expert_mujet;
  ToolHandle<Trig::TrigDecisionTool> m_trigDec; //!

  float phiCorr(float);
  float m_etCut;
  std::string m_sv1_infosource;

};

#endif

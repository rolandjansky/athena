/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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



#include "TrigInDetToolInterfaces/ITrigTrackJetFinderTool.h"


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

  Gaudi::Property<std::string> m_onlineBjetContainerKey {this,"OnlineBJetContainerKey","HLT_bJets",
      "The SG key of the online BJet container from the TriggerEDMRun3"};

  SG::ReadHandleKey<xAOD::VertexContainer> m_offlineVertexContainerKey {this,"OfflineVertexContainerName","PrimaryVertices","Key of offline primary vertexes"};
  SG::ReadHandleKey<xAOD::VertexContainer> m_onlineVertexContainerKey {this,"OnlineVertexContainerName","HLT_IDVertex_FS","Key of online bjet primary vertexes"}; // MS 290620
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_onlineTrackContainerKey {this,"OnlineTrackContainerName","HLT_IDTrack_Bjet_IDTrig","Key of online tracks of bjets"};
  SG::ReadHandleKey<xAOD::BTaggingContainer> m_onlineBTaggingContainerKey {this,"OnlineBTaggingContainerName","HLT_BTagging","Key of online b-tagging object"};

  ToolHandle<Trig::TrigDecisionTool> m_trigDec; //!

  float phiCorr(float);
  float m_etCut;
  std::string m_sv1_infosource;

};

#endif

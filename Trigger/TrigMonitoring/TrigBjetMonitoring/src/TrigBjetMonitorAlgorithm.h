/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGBJETMONITORING_TRIGBJETMONITORALGORITHM_H
#define TRIGBJETMONITORING_TRIGBJETMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "TRandom3.h"

#include "StoreGate/ReadHandleKey.h"

#include "TrigDecisionTool/TrigDecisionTool.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTracking/VertexContainer.h"

class TrigBjetMonitorAlgorithm : public AthMonitorAlgorithm {
 public:
  TrigBjetMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TrigBjetMonitorAlgorithm();
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

 private:
  Gaudi::Property<bool> m_doRandom {this,"RandomHist",true};
  Gaudi::Property<std::string> m_btaggingLinkName{this, "BtaggingLinkName", "btag"}; // TM 2021-10-30
  //  const std::vector<std::string> m_allChains;
  std::vector<std::string> m_allChains;
  SG::ReadHandleKey<xAOD::MuonContainer> m_muonContainerKey;
  SG::ReadHandleKey<xAOD::VertexContainer> m_offlineVertexContainerKey {this,"OfflineVertexContainerName","PrimaryVertices","Key of offline primary vertexes"};
  SG::ReadHandleKey<xAOD::VertexContainer> m_onlineVertexContainerKey {this,"OnlineVertexContainerName","HLT_IDVertex_FS","Key of online bjet primary vertexes"}; // MS 290620
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_onlineTrackContainerKey {this,"OnlineTrackContainerName","HLT_IDTrack_Bjet_IDTrig","Key of online tracks of bjets"};

  ToolHandle<Trig::TrigDecisionTool> m_trigDec; //!


};
#endif

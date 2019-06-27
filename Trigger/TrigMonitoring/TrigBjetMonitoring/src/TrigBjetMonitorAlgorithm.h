/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGBJETMONITORING_TRIGBJETMONITORALGORITHM_H
#define TRIGBJETMONITORING_TRIGBJETMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoring/Monitored.h"

#include "TRandom3.h"

#include "StoreGate/ReadHandleKey.h"

#include "xAODMuon/MuonContainer.h"
#include "xAODTracking/VertexContainer.h"

class TrigBjetMonitorAlgorithm : public AthMonitorAlgorithm {
 public:
  TrigBjetMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TrigBjetMonitorAlgorithm();
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
 private:
  //  Gaudi::Property<bool> m_doRandom {this,"RandomHist",false}; 
  Gaudi::Property<bool> m_doRandom {this,"RandomHist",true};
  std::vector<std::string> m_expert;
  std::vector<std::string> m_expertI;
  std::vector<std::string> m_shifter;
  std::vector<std::string> m_shifterI;
  SG::ReadHandleKey<xAOD::MuonContainer> m_MuonContainerKey;
  SG::ReadHandleKey<xAOD::VertexContainer> m_VertexContainerKey;
};
#endif

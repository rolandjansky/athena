/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     DataFlowMonAlg.h
// PACKAGE:  DataQualityTools
//
// AUTHOR:   Peter Onyisi <ponyisi@cern.ch>
//
//
// ********************************************************************
#ifndef DQTDataFlowMonAlg_H
#define DQTDataFlowMonAlg_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"

class DQTDataFlowMonAlg: public AthMonitorAlgorithm
{

 public:

  DQTDataFlowMonAlg(const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~DQTDataFlowMonAlg() {};

  virtual StatusCode initialize() override;

  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

  ToolHandle<IDQFilterTool> m_atlasReadyFilter{this,"ReadyFilterTool","DQAtlasReadyFilterTool/DQAtlasReadyFilterTool"};

 private:
    SG::ReadDecorHandleKey<xAOD::EventInfo> m_TileStatusKey{this, "TileStatusKey", "EventInfo.TileStatus"};

};

#endif

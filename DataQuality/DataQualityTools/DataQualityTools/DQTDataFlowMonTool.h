/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     DataFlowMonTool.h
// PACKAGE:  DataQualityTools
//
// AUTHOR:   Peter Onyisi <ponyisi@cern.ch>
//	     
//
// ********************************************************************
#ifndef DQTDataFlowMonTool_H
#define DQTDataFlowMonTool_H

#include <set>
#include <string>
#include "GaudiKernel/ToolHandle.h"
#include <stdint.h>

#include "StoreGate/ReadHandleKey.h"
#include "xAODEventInfo/EventInfo.h"

#include "DataQualityTools/DataQualityFatherMonTool.h"

class TGraph;

class DQTDataFlowMonTool: public DataQualityFatherMonTool
{

 public:
  
  DQTDataFlowMonTool(const std::string & type, const std::string & name, const IInterface* parent);

  virtual ~DQTDataFlowMonTool();

  virtual StatusCode initialize() override;
    
  virtual StatusCode bookHistogramsRecurrent( ) override;
  virtual StatusCode bookHistograms( ) override;
  virtual StatusCode fillHistograms( ) override;
  virtual StatusCode procHistograms( ) override;
  virtual StatusCode checkHists(bool fromFinalize) override;

private:

  std::string m_releaseString;
  std::string m_releaseStageString;

  // TH1I_LW* m_events_lb;
  // TH1I_LW* m_release_stage_lowStat;
  TH1I* m_events_lb;
  TH1I* m_release_stage_lowStat;

  //TH2I_LW* m_eventflag_summary_lowStat;
  //TH2I_LW* m_eventflag_summary_lowStat;
  TH2I* m_eventflag_summary_lowStat;
  //TH2I* m_eventflag_summary_lb;
  //  TGraph* m_eventflag_run[xAOD::EventInfo::nDets];
  TH1D* m_sumweights;
  typedef std::pair<unsigned int, unsigned int> EvFlagPt_t;
  //std::vector<EvFlagPt_t>* m_eventflag_vec[xAOD::EventInfo::nDets];

  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey
  { this, "EventInfoKey", "EventInfo", "" };
};

#endif

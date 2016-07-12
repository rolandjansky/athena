/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//this
#include "CalibNtupleAnalysisAlg/GoodRunListFilter.h"

//GoodRunsLists
#include "GoodRunsLists/IGoodRunsListSelectorTool.h"

//MuonCalibEventBase
#include "MuonCalibEventBase/MuonCalibEvent.h"
#include "MuonCalibEventBase/MuonCalibEventInfo.h"
namespace MuonCalib {

GoodRunListFilter::GoodRunListFilter(const std::string &t, const std::string &n, const IInterface *p): AthAlgTool(t, n, p) {
  declareProperty("GoodRunListTool", m_good_runlist_tool);
  declareInterface< CalibSegmentPreparationTool >(this);
}

StatusCode GoodRunListFilter::initialize(void) {
  ATH_CHECK( m_good_runlist_tool.retrieve() );
  return StatusCode::SUCCESS;
}

void GoodRunListFilter::prepareSegments(const MuonCalibEvent *&event, std::map<NtupleStationId, MuonCalibSegment *> &segments) {
  const MuonCalibEventInfo &event_info=event->eventInfo();
  if(!m_good_runlist_tool->passRunLB(event_info.runNumber(), event_info.lumiBlock())) {
    segments.clear();
  }
}

} //namespace MuonCalib

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     DQTDataFlowMonAlg.cxx
// PACKAGE:  DataQualityTools  
//
// AUTHORS:  Peter Onyisi (ponyisi@cern.ch)
//
//
// ********************************************************************

#include "DataQualityTools/DQTDataFlowMonAlg.h"
#include "AthenaMonitoring/Monitored.h"

using xAOD::EventInfo;

DQTDataFlowMonAlg::DQTDataFlowMonAlg( const std::string& name,
				      ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name, pSvcLocator)
{
}


StatusCode DQTDataFlowMonAlg::initialize()
{
  return AthMonitorAlgorithm::initialize();
}


StatusCode 
DQTDataFlowMonAlg::fillHistograms( const EventContext& ctx ) const
{
  ATH_MSG_DEBUG("in fillHistograms()");

  using namespace Monitored;

  auto environment = Scalar("environment", (int) m_environment);
  auto group = getGroup("default");
  fill(group, environment);
  
  if( m_environment != Environment_t::tier0Raw ) {
    SG::ReadHandle<xAOD::EventInfo> evtinfo { GetEventInfo(ctx) };
    if (! evtinfo.isValid()) {
      ATH_MSG_ERROR("Could not retrieve EventInfo");
      return StatusCode::FAILURE;
    } else {
      if (evtinfo->eventType(xAOD::EventInfo::IS_SIMULATION)) {
	auto weight = Scalar<double>("mcweight", evtinfo->eventType(xAOD::EventInfo::IS_SIMULATION) ? evtinfo->mcEventWeight() : 1);
	auto lb = Scalar("LB", evtinfo->lumiBlock());
	fill(group, weight, lb);
      }

      std::vector<int> detstatevec(EventInfo::nDets+1);
      std::vector<int> detstatevec_idx(EventInfo::nDets+1);
      std::iota(detstatevec_idx.begin(), detstatevec_idx.end(), 0);
	
      auto detstates = Collection("detstates", detstatevec);
      auto detstates_idx = Collection("detstates_idx", detstatevec_idx);
      EventInfo::EventFlagErrorState worststate = EventInfo::NotSet;
      for (int i = 0; i < EventInfo::nDets; i++) {
	EventInfo::EventFlagErrorState detstate = evtinfo->errorState((EventInfo::EventFlagSubDet) i);
	detstatevec[i] = detstate;
	if (detstate > worststate) worststate = detstate;
      }
      detstatevec[EventInfo::nDets] = worststate;
      fill(group, detstates, detstates_idx);
    }
  }

  return StatusCode::SUCCESS;
}



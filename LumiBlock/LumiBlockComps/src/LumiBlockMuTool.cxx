/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LumiBlockMuTool.h"
#include "StoreGate/ReadDecorHandle.h"

//--------------------------------------------------

LumiBlockMuTool::LumiBlockMuTool(const std::string& type,
				 const std::string& name,
				 const IInterface* parent)
  : base_class(type, name, parent)
{
  declareInterface<ILumiBlockMuTool>(this);
}

StatusCode
LumiBlockMuTool::initialize()
{
  ATH_MSG_DEBUG("LumiBlockMuTool::initialize() begin");
  ATH_CHECK(m_eventInfoKey.initialize());
  ATH_CHECK(m_rdhkActMu.initialize());
  ATH_CHECK(m_rdhkAveMu.initialize());
  return StatusCode::SUCCESS;
}

float
LumiBlockMuTool::actualInteractionsPerCrossing() const {

  SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfoKey);
  SG::ReadDecorHandle<xAOD::EventInfo,float> actMu(m_rdhkActMu);
  float mu = actMu.isPresent() ? actMu(0) : 0.;

  if (eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION) && mu < 0) return eventInfo->lumiBlock() % 100;
  return mu;
}

float
LumiBlockMuTool::averageInteractionsPerCrossing() const{

  SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfoKey);
  SG::ReadDecorHandle<xAOD::EventInfo,float> aveMu(m_rdhkAveMu);
  float mu = aveMu.isPresent() ? aveMu(0) : 0.;

  if (eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION) && mu <= 0) return eventInfo->lumiBlock() % 100;
  return mu;
}

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LumiBlockMuTool.h"

//--------------------------------------------------

LumiBlockMuTool::LumiBlockMuTool(const std::string& type,
				 const std::string& name,
				 const IInterface* parent)
  : base_class(type, name, parent),
    m_MCLumiBlockHack(false)
{
  declareInterface<ILumiBlockMuTool>(this);
  declareProperty("MCLumiBlockHack", m_MCLumiBlockHack);
}

StatusCode
LumiBlockMuTool::initialize()
{
  ATH_MSG_DEBUG("LumiBlockMuTool::initialize() begin");
  ATH_CHECK(m_eventInfoKey.initialize());
  return StatusCode::SUCCESS;
}

float
LumiBlockMuTool::actualInteractionsPerCrossing() const {

  SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfoKey);  

  // Read MC data from LB number?

  if (eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION)) {

    if (m_MCLumiBlockHack) {
      ATH_MSG_DEBUG("Returning lumi block as mu for MC event ");
      return eventInfo->lumiBlock() % 100; // Never greater than 100 according to Ayana
    }
    
    // Try reading from eventInfo, but fall back if less than zero
    // Changed 9/10/13, as actualInteractionsPerCrossing should return -999
    // if value is not filled, but can return 0 as a valid result
    if ( eventInfo->actualInteractionsPerCrossing() >= 0) {
      return eventInfo->actualInteractionsPerCrossing();
    } else {
      return eventInfo->lumiBlock() % 100;
    }

  }

  // Read mu from eventInfo
  return eventInfo->actualInteractionsPerCrossing();
}

float
LumiBlockMuTool::averageInteractionsPerCrossing() const{

  SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfoKey);

  // Read MC data from LB number?
  if (eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION)) {

    if (m_MCLumiBlockHack) {
      ATH_MSG_DEBUG("Returning lumi block as mu for MC event ");
      return eventInfo->lumiBlock() % 100;
    }

    // Try reading from eventInfo, but fall back if zero
    if ( eventInfo->averageInteractionsPerCrossing() > 0) {
      return eventInfo->averageInteractionsPerCrossing();
    } else {
      return eventInfo->lumiBlock() % 100;
    }
    
  } 

  // Read mu from eventInfo
  return eventInfo->averageInteractionsPerCrossing();
}

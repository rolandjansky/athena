/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LumiBlockComps/LumiBlockMuTool.h"

//--------------------------------------------------

LumiBlockMuTool::LumiBlockMuTool(const std::string& type,
				 const std::string& name,
				 const IInterface* parent)
  : AthAlgTool(type, name, parent),
    m_MCLumiBlockHack(false),
    m_useDB(false)
{
  declareInterface<ILumiBlockMuTool>(this);
  declareProperty("MCLumiBlockHack", m_MCLumiBlockHack);
  declareProperty("UseDB", m_useDB);
}

StatusCode
LumiBlockMuTool::initialize()
{
  ATH_MSG_DEBUG("LumiBlockMuTool::initialize() begin");
  ATH_CHECK(m_eventInfoKey.initialize());

  // Nothing else to do if not reading from DB
  if (!m_useDB) 
    return StatusCode::SUCCESS;

  // Get the Luminosity tool
  CHECK(m_lumiTool.retrieve());

  ATH_MSG_DEBUG("LumiBlockMuTool::initialize() end");
  return StatusCode::SUCCESS;
}

StatusCode
LumiBlockMuTool::finalize()
{
  ATH_MSG_DEBUG("LumiBlockMuTool::finalize()");
  return StatusCode::SUCCESS;
}

float
LumiBlockMuTool::actualInteractionsPerCrossing() const {

  SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfoKey);  

  // Take value from DB?     
  if (m_useDB) {
    float mu = 0.;
    if (m_lumiTool->muToLumi() > 0.)
      mu = m_lumiTool->lbLuminosityPerBCID()/m_lumiTool->muToLumi();

    ATH_MSG_DEBUG("From DB, LB " << eventInfo->lumiBlock() << " bcid " << eventInfo->bcid() << " -> " << mu);
    
    return mu;
  }

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

  // Already set by callback if using DB
  if (m_useDB) return m_lumiTool->lbAverageInteractionsPerCrossing();

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

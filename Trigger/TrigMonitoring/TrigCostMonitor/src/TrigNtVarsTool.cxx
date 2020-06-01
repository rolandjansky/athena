/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <string>

// Framework
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

// Reconstruction and MC
#include "GeneratorObjects/McEventCollection.h"
#include "TruthHelper/IsGenStable.h"
#include "TruthHelper/PileUpType.h"

// Local
#include "TrigMonitoringEvent/TrigMonConfig.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"
#include "TrigCostMonitor/TrigNtVarsTool.h"

//---------------------------------------------------------------------------------------
Trig::TrigNtVarsTool::TrigNtVarsTool(const std::string &name,
             const std::string &type,
             const IInterface  *parent)
  :AthAlgTool(name, type, parent),
   m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool"),
   m_config(0)
{
  declareInterface<Trig::ITrigNtTool>(this);

  declareProperty("collectMC",     m_collectMC  = true);
  declareProperty("collectPU",     m_collectPU  = true);
  declareProperty("collectTD",     m_collectTD  = false);
  declareProperty("keyMCEvent",    m_keyMCEvent = "TruthEvent");
  declareProperty("keyPileUp",     m_keyPileUp  = "OverlayEvent");
  declareProperty("trigDec",       m_trigDec);
}

//---------------------------------------------------------------------------------------
StatusCode Trig::TrigNtVarsTool::initialize()
{    
  //
  // Get services
  //

  // Get TrigDecisionTool
  if(m_collectTD) CHECK(m_trigDec.retrieve());

  return StatusCode::SUCCESS;
}
//---------------------------------------------------------------------------------------
StatusCode Trig::TrigNtVarsTool::finalize()
{
  //
  // Clean up internal state
  //
  ATH_MSG_DEBUG("finalize()" );

  m_config = 0;

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtVarsTool::Fill(TrigMonConfig *confg)
{
  //
  // Copy configuration
  //
  m_config = 0;

  if(confg) {
    m_config = confg;
  }
  else {
    ATH_MSG_WARNING("Null TrigMonConfig pointer" );
    return false;
  }

  return true;
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtVarsTool::Fill(TrigMonEvent &event)
{
  //
  // Process current event
  //
  if(m_collectMC) CollectMC(event);
  if(m_collectPU) CollectPU(event);
  if(m_collectTD) CollectTD(event);

  return true;
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtVarsTool::CollectTD(TrigMonEvent& /*event*/)
{

  ATH_MSG_ERROR("This tool is depricated. It needs migrating if it is to function again." );


  return true;
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtVarsTool::CollectMC(TrigMonEvent& /*event*/)
{  

  ATH_MSG_ERROR("This tool is depricated. It needs migrating if it is to function again." );


  return true;
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtVarsTool::CollectPU(TrigMonEvent& /*event*/)
{

  ATH_MSG_ERROR("This tool is depricated. It needs migrating if it is to function again." );


  return true;
}

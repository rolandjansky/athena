/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaMonitoring/DQFilledBunchFilterTool.h"
#include "AthenaKernel/errorcheck.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

DQFilledBunchFilterTool::DQFilledBunchFilterTool(const std::string& type,const std::string& name,const IInterface* parent)
: AthAlgTool( type, name, parent )
, m_alwaysReturnTrue(false)
, m_invert(false)
, m_bunchtool("Trig::TrigConfBunchCrossingTool/BunchCrossingTool")
{
  declareInterface<IDQFilterTool>(this);
 declareProperty("alwaysReturnTrue", m_alwaysReturnTrue);
 declareProperty("invert", m_invert);
 declareProperty("bunchCrossingTool", m_bunchtool);
}
        
DQFilledBunchFilterTool::~DQFilledBunchFilterTool () {}

StatusCode DQFilledBunchFilterTool::initialize()
{
  ATH_MSG_VERBOSE("ATLAS Ready initialize");
  // don't register callback if we always return true anyway
  if (m_alwaysReturnTrue) return StatusCode::SUCCESS;

  CHECK( m_bunchtool.retrieve() );
  return StatusCode::SUCCESS;
}

bool DQFilledBunchFilterTool::accept() const {
  if (m_alwaysReturnTrue) {
    return true;
  } else {
    const EventInfo* eventInfo(0);
    CHECK( evtStore()->retrieve( eventInfo ), false );
    
    EventID::number_type bcid = eventInfo->event_ID()->bunch_crossing_id();  
    bool value = m_bunchtool->isFilled(bcid) ^ m_invert;
    ATH_MSG_VERBOSE("Filled bunch DQ tool accept called, value " << value);
    return value;
  }
}

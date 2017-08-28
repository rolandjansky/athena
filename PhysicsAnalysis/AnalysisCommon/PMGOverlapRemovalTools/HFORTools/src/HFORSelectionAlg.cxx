/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// HFORTools includes
#include "HFORSelectionAlg.h"

// EDM includes
#include "xAODEventInfo/EventInfo.h"
 

// Constructors
////////////////
HFORSelectionAlg::HFORSelectionAlg( const std::string& name,
                                    ISvcLocator* pSvcLocator ) :
 ::AthFilterAlgorithm( name, pSvcLocator ),
 m_hforSelectionTool("HFORSelectionTool/ToolInstanceName")
{
  //
  // Property declaration
  //
  declareProperty( "HFORSelectionTool", m_hforSelectionTool,
                   "The HFORSelectionTool, in case you need to give a non-standard configuration instance to this algorithm" );
}



// Destructor
///////////////
HFORSelectionAlg::~HFORSelectionAlg()
{}



// Athena Algorithm's Hooks
////////////////////////////
StatusCode HFORSelectionAlg::initialize() {
  ATH_MSG_DEBUG( "Initializing " << name() << "..." );
  ATH_MSG_DEBUG( "Using: " << m_hforSelectionTool );
  ATH_CHECK( m_hforSelectionTool.retrieve() );
  return StatusCode::SUCCESS;
}



StatusCode HFORSelectionAlg::finalize() {
  ATH_MSG_DEBUG("Finalizing " << name() << "...");
  ATH_CHECK( m_hforSelectionTool.release() );
  return StatusCode::SUCCESS;
}



StatusCode HFORSelectionAlg::execute() {
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  // Only run this algorithm on Monte Carlo
  const xAOD::EventInfo* eventInfo = nullptr;
  CHECK( evtStore()->retrieve(eventInfo) );
  const bool isSim = eventInfo->eventType(xAOD::EventInfo::EventType::IS_SIMULATION);
  if ( !isSim ) {
    ATH_MSG_DEBUG ("It is a data event... nothing to be done...");
    this->setFilterPassed( true );
    return StatusCode::SUCCESS;
  }

  // Default pass/fail event decission
  bool passEvent = true;
  // Check if this event overlaps
  passEvent = m_hforSelectionTool->isSelected();

  // Set the event pass/fail decission
  this->setFilterPassed( passEvent );
  ATH_MSG_DEBUG("Accepting this event due to HFOR (yes/no): " << passEvent);

  return StatusCode::SUCCESS;
}

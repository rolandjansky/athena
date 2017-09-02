
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//  includes
#include "TriggerSelectionAlg.h"

// Tool includes
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "xAODEventInfo/EventInfo.h"


TriggerSelectionAlg::TriggerSelectionAlg( const std::string& name, ISvcLocator* pSvcLocator ):
  AthFilterAlgorithm( name, pSvcLocator ),
  m_trigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool"),
  m_triggerList(),
  m_decoEvtInfo(true),
  m_evtInfoName("EventInfo"),
  m_varPrefix("pass_"),
  m_storePrescaleInfo(false),
  m_varNameList()
{
  declareProperty("TrigDecisionTool",  m_trigDecisionTool, "The TrigDecisionTool" );
  declareProperty("TriggerList",       m_triggerList, "The list of triggers to cut on" );
  declareProperty("DecorateEventInfo", m_decoEvtInfo,
                  "Decide if we also want to decorate the xAOD::EventInfo object with the pass/fail information" );
  declareProperty("EventInfoName",     m_evtInfoName, "Name of the xAOD::EventInfo object that we want to decorate" );
  declareProperty("VarNamePrefix",     m_varPrefix, "Prefix used for the decoration variables" );
  declareProperty("StorePrescaleInfo", m_storePrescaleInfo,
                  "Decide if we also want to decorate the xAOD::EventInfo object with the full-chain prescale information" );
}


TriggerSelectionAlg::~TriggerSelectionAlg() {}


StatusCode TriggerSelectionAlg::initialize()
{
  ATH_MSG_DEBUG ("Initializing " << name() << "...");

  // Print the configuration to the log file
  ATH_MSG_DEBUG( "Using: " << m_trigDecisionTool );
  ATH_MSG_DEBUG( "Using: " << m_triggerList );
  ATH_MSG_DEBUG( "Using: " << m_decoEvtInfo );
  ATH_MSG_DEBUG( "Using: " << m_evtInfoName );
  ATH_MSG_DEBUG( "Using: " << m_varPrefix );
  ATH_MSG_DEBUG( "Using: " << m_storePrescaleInfo );

  // Retrieve the TrigDecisionTool
  ATH_CHECK(m_trigDecisionTool.retrieve());

  // Create the list of decoration variables
  for ( const std::string& trigName : m_triggerList.value() ){
    m_varNameList.push_back( m_varPrefix.value() + trigName );
  }

  return StatusCode::SUCCESS;
}


StatusCode TriggerSelectionAlg::finalize()
{
  ATH_MSG_DEBUG("Finalizing " << name() << "...");

  // Release all tools
  ATH_CHECK( m_trigDecisionTool.release() );

  return StatusCode::SUCCESS;
}


StatusCode TriggerSelectionAlg::execute()
{
  ATH_MSG_DEBUG("Executing " << name() << "...");

  // Get the xAOD::EventInfo object, if requested
  const xAOD::EventInfo* evtInfo = nullptr;
  if ( m_decoEvtInfo.value() ){
    ATH_CHECK( evtStore()->retrieve( evtInfo, m_evtInfoName.value() ) );
  }

  // Create a results Vector
  std::vector<bool> trigResultsVec( m_triggerList.value().size(), false );
  std::vector<float> trigPrescalesVec( m_triggerList.value().size(), 0. );

  //Check if event passes trigger selection
  bool eventPasses = false;
  if( !(m_triggerList.value().empty()) ) {
    for( std::size_t i=0; i<m_triggerList.value().size(); ++i ) {
      if( m_trigDecisionTool->isPassed(m_triggerList.value().at(i)) ) {
        trigResultsVec[i] = true;
        eventPasses = true;
        ATH_MSG_VERBOSE("Name of passed trigger: " << m_triggerList.value().at(i));
        if ( !(m_decoEvtInfo.value()) ){
          break; // Found a trigger which we passed, nothing more to do here
        }
      }
      //We sometimes want to keep track of the chain prescale
      if (m_storePrescaleInfo) {
	trigPrescalesVec[i] = m_trigDecisionTool->getPrescale(m_triggerList.value().at(i));
        ATH_MSG_VERBOSE("Retrieved prescale of " << trigPrescalesVec[i] << " for trigger: " << m_triggerList.value().at(i));
      }
    }
  }

  // Decorate the EventInfo, if requested
  if ( m_decoEvtInfo.value() ){
    const size_t decoSize = trigResultsVec.size();
    if ( decoSize != m_varNameList.size() ){
      ATH_MSG_FATAL("Different number of trigger results and variable names");
      return StatusCode::FAILURE;
    }
    // We also want to decorate the xAOD::EventInfo object with the results
    for( std::size_t i=0; i<decoSize; ++i ) {
      SG::AuxElement::Decorator<char> decoPassTrig(m_varNameList[i]);
      decoPassTrig(*evtInfo) = static_cast<char>(trigResultsVec[i]);
      //We sometimes want to keep track of the chain prescale
      if (m_storePrescaleInfo) {
	SG::AuxElement::Decorator<float> decoPrescaleTrig("prescale_"+m_triggerList.value().at(i));
	decoPrescaleTrig(*evtInfo) = static_cast<float>(trigPrescalesVec[i]);
      }
    }
  }

  // Say if this event should be accepted or not
  this->setFilterPassed( eventPasses );
  ATH_MSG_DEBUG("Event passes trigger selection: " << eventPasses );

  return StatusCode::SUCCESS;
}

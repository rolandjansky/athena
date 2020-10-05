/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// EDM includes

// Local includes
#include "EventCleaningTestAlg.h"


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
EventCleaningTestAlg::EventCleaningTestAlg(const std::string& name,
                                             ISvcLocator* svcLoc)
    : AthAlgorithm(name, svcLoc),
      m_ecTool("ECUtils::EventCleaningTool/EventCleaningTool", this)
{
  m_ecTool.declarePropertyFor( this, "EventCleaningTool" );
  declareProperty("EventCleanPrefix", m_prefix = "",
                  "Input name of event cleaning decorator prefix");
  declareProperty("CleaningLevel", m_cleaningLevel = "LooseBad",
                  "Input cleaning level");
  declareProperty("JetCollectionName", m_collection = "AntiKt4EMTopoJets",
                  "Jet collection name");
  declareProperty("doEvent",m_doEvent = true,
                  "Decorate the EventInfo");
}

//-----------------------------------------------------------------------------
// Initialize
//-----------------------------------------------------------------------------
StatusCode EventCleaningTestAlg::initialize()
{
  ATH_MSG_INFO("Initialize");

  // Try to retrieve the tool
  ATH_CHECK( m_ecTool.retrieve() );

  // Create the decorator
  if(m_doEvent) m_dec_eventClean = std::make_unique<SG::AuxElement::Decorator<char>>(m_prefix + "eventClean_" + m_cleaningLevel);

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Execute
//-----------------------------------------------------------------------------
StatusCode EventCleaningTestAlg::execute()
{
  // Jets
  const xAOD::JetContainer* jets = nullptr;
  ATH_CHECK( evtStore()->retrieve(jets, m_collection) );

  // Apply the event cleaning
  bool result = 0;
  result = m_ecTool->acceptEvent(jets) ;

  //Decorate event
  if(m_doEvent){
    const xAOD::EventInfo* eventInfo = nullptr;
    ATH_CHECK( evtStore()->retrieve(eventInfo, "EventInfo") );
    (*m_dec_eventClean)(*eventInfo) = result;
  }

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Finalize
//-----------------------------------------------------------------------------
StatusCode EventCleaningTestAlg::finalize()
{
  ATH_MSG_INFO("Finalize");

  return StatusCode::SUCCESS;
}


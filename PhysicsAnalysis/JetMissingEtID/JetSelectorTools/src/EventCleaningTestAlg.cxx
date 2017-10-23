/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EDM includes

// Local includes
#include "EventCleaningTestAlg.h"

static const float GeV = 1000.;
static const float invGeV = 1./GeV;

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
EventCleaningTestAlg::EventCleaningTestAlg(const std::string& name,
                                             ISvcLocator* svcLoc)
    : AthAlgorithm(name, svcLoc),
      m_ecTool("EventCleaningTool", this),
      m_dec_eventClean(0)
{
  declareProperty("EventCleaningTool", m_ecTool);
  declareProperty("PtCut", m_pt = 20000.0,
                  "Minimum pt for jets");
  declareProperty("EtaCut", m_eta = 2.8,
                  "Max eta for jets");
  declareProperty("JvtDecorator", m_jvt = "passJvt",
                  "Input name of Jvt decorator");
  declareProperty("OrDecorator", m_or = "passOR",
                  "Input name of OR decorator");
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
  m_dec_eventClean = new SG::AuxElement::Decorator<char>(m_prefix + "eventClean_" + m_cleaningLevel);

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Execute
//-----------------------------------------------------------------------------
StatusCode EventCleaningTestAlg::execute()
{
  // Jets
  const xAOD::JetContainer* jets = 0;
  ATH_CHECK( evtStore()->retrieve(jets, m_collection) );

  // Apply the event cleaning
  bool result = 0;
  result = m_ecTool->acceptEvent(jets) ;
 
  //Decorate event
  if(m_doEvent){
  const xAOD::EventInfo* eventInfo = 0;
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

  delete m_dec_eventClean; 

  return StatusCode::SUCCESS;
}


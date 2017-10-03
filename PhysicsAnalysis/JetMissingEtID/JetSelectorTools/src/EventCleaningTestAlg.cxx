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
      m_ecTool("EventCleaningTool", this)
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
  declareProperty("CleaningLevel", m_cleaningLevel = "LooseBad",
                  "Input cleaning level");
  declareProperty("JetCollectionName", m_collection = "AntiKt4EMTopoJets",
                  "Jet collection name");
}

//-----------------------------------------------------------------------------
// Initialize
//-----------------------------------------------------------------------------
StatusCode EventCleaningTestAlg::initialize()
{
  ATH_MSG_INFO("Initialize");

  // Try to retrieve the tool
  ATH_CHECK( m_ecTool.retrieve() );

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
  const static SG::AuxElement::Decorator<char> dec_eventClean("DFCommonJets_eventClean_" + m_cleaningLevel);
  const xAOD::EventInfo* eventInfo = 0;
  ATH_CHECK( evtStore()->retrieve(eventInfo, "EventInfo") );
  dec_eventClean(*eventInfo) = result; 

  return StatusCode::SUCCESS;
}


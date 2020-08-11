
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// JetRecAlg.cxx 

#include <memory>
#include "JetRecAlg.h"
#include "JetInterface/IJetExecuteTool.h"
#include "xAODJet/JetAuxContainer.h"


using std::string;

//**********************************************************************

StatusCode JetRecAlg::initialize() {

  ATH_CHECK(m_jetprovider.retrieve());
  ATH_MSG_INFO(" Initialized  IJetProvider : "<< m_jetprovider->name());
  
  ATH_MSG_INFO(" Initialize .... List of modifiers: ");
  ATH_CHECK(m_modifiers.retrieve());
  for(ToolHandle<IJetModifier> t : m_modifiers){
    ATH_MSG_INFO("    --> : "<< t->name());
  }

  ATH_CHECK(m_output.initialize());
  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode JetRecAlg::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode JetRecAlg::execute(const EventContext& ctx) const {
  // Get JetContainer by running fastjet, grooming or copying existing jets...
  // Pass in the WriteHandle so that the provider can do the record, and we
  // needn't know the type of the jet aux container
  // We can subsequently access the jets from the handle and don't have to
  // worry about memory management.
  SG::WriteHandle<xAOD::JetContainer> jetContHandle(m_output,ctx);
  ATH_CHECK( m_jetprovider->getAndRecordJets(jetContHandle) );

  ATH_MSG_DEBUG("Created jet container of size "<< jetContHandle->size() << "  | writing to "<< m_output.key() );

  ATH_MSG_DEBUG("Applying jet modifiers to " << m_output.key());

  // Calculate moments, calibrate, sort, filter...  -----------
  for(const ToolHandle<IJetModifier>& t : m_modifiers){
    ATH_MSG_DEBUG("Running " << t.name());
    ATH_CHECK(t->modify(*jetContHandle));
  }

  return StatusCode::SUCCESS;

}

//**********************************************************************


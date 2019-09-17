
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetAlgorithm.cxx 

#include <memory>
#include "JetAlgorithm.h"
#include "JetInterface/IJetExecuteTool.h"

using std::string;

//**********************************************************************

JetAlgorithm::JetAlgorithm(const std::string& name, 
                           ISvcLocator* pSvcLocator )
  : ::AthAlgorithm( name, pSvcLocator ) { 
  //declareProperty( "Tools", m_exetools);
}

//**********************************************************************

JetAlgorithm::~JetAlgorithm() { }

//**********************************************************************

StatusCode JetAlgorithm::initialize() {

  ATH_MSG_INFO(" Initialize .... List of modifiers: ");
  ATH_CHECK(m_modifiers.retrieve());
  for(ToolHandle<IJetModifier> t : m_modifiers){
    ATH_MSG_INFO("    --> : "<< t->name());
  }
  ATH_CHECK(m_jetprovider.retrieve());
  ATH_MSG_INFO(" Initialize .... IJetProvider : "<< m_jetprovider->name());
  ATH_CHECK(m_output.initialize());
  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode JetAlgorithm::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode JetAlgorithm::execute() {  

  // Build JetContainer by running fastjet, grooming or copying existing jets...
  std::unique_ptr<xAOD::JetContainer>  jets( m_jetprovider->build() );
  if(jets==nullptr){
    ATH_MSG_ERROR("Builder tool "<< m_jetprovider->name() << "  returned a null pointer");
    return StatusCode::FAILURE;
  }
    
  // Calculate moments, calibrate, sort, filter...  -----------
  for(ToolHandle<IJetModifier> t : m_modifiers){
    ATH_CHECK(t->modify(*jets));
  }

  ATH_MSG_DEBUG("Done jet finding "<< jets->size());
  
  SG::WriteHandle<xAOD::JetContainer> jetContHandle(m_output);
  ATH_CHECK( jetContHandle.record(std::move(jets)) );
  
  return StatusCode::SUCCESS;

}

//**********************************************************************

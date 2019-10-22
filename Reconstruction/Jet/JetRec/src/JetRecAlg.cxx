
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// JetRecAlg.cxx 

#include <memory>
#include "JetRecAlg.h"
#include "JetInterface/IJetExecuteTool.h"

using std::string;

//**********************************************************************

JetRecAlg::JetRecAlg(const std::string& name, 
                           ISvcLocator* pSvcLocator )
  : ::AthAlgorithm( name, pSvcLocator ) { 
  //declareProperty( "Tools", m_exetools);
}

//**********************************************************************

JetRecAlg::~JetRecAlg() { }

//**********************************************************************

StatusCode JetRecAlg::initialize() {

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

StatusCode JetRecAlg::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode JetRecAlg::execute() {  

  // Build JetContainer by running fastjet, grooming or copying existing jets...
  std::unique_ptr<xAOD::JetContainer>  jets( m_jetprovider->build() );
  if(jets==nullptr){
    ATH_MSG_ERROR("Builder tool "<< m_jetprovider->name() << "  returned a null pointer");
    return StatusCode::FAILURE;
  }
    
  // Calculate moments, calibrate, sort, filter...  -----------
  for(const ToolHandle<IJetModifier> t : m_modifiers){
    ATH_CHECK(t->modify(*jets));
  }

  ATH_MSG_DEBUG("Done jet finding "<< jets->size() << "  | writing to "<< m_output.key() );
  
  SG::WriteHandle<xAOD::JetContainer> jetContHandle(m_output);
  ATH_CHECK( jetContHandle.record(std::move(jets)) );
  
  return StatusCode::SUCCESS;

}

//**********************************************************************



/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// JetRecAlg.cxx 

#include <memory>
#include "JetRecAlg.h"
#include "JetInterface/IJetExecuteTool.h"
#include "xAODJet/JetAuxContainer.h"


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

StatusCode JetRecAlg::execute() {  

  // Get JetContainer by running fastjet, grooming or copying existing jets...
  std::unique_ptr<xAOD::JetContainer> jets( m_jetprovider->getJets() );
  if(jets==nullptr){
    ATH_MSG_ERROR("Builder tool "<< m_jetprovider->name() << "  returned a null pointer");
    return StatusCode::FAILURE;
  }
  std::unique_ptr<xAOD::JetAuxContainer>  auxCont( dynamic_cast<xAOD::JetAuxContainer *>(jets->getStore() ) );  

  ATH_MSG_DEBUG("Created jet container of size "<< jets->size() << "  | writing to "<< m_output.key() );

  // Write out JetContainer and JetAuxContainer
  SG::WriteHandle<xAOD::JetContainer> jetContHandle(m_output);
  ATH_CHECK( jetContHandle.record(std::move(jets), std::move(auxCont) ) );

  ATH_MSG_DEBUG("Applying jet modifiers to " << m_output.key());

  // Calculate moments, calibrate, sort, filter...  -----------
  for(const ToolHandle<IJetModifier> t : m_modifiers){
    ATH_CHECK(t->modify(*jetContHandle));
  }

  return StatusCode::SUCCESS;

}

//**********************************************************************


/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "DecisionHandling/HypoBase.h"

HypoBase::HypoBase( const std::string& name, ISvcLocator* pSvcLocator )
  : ::AthReentrantAlgorithm( name, pSvcLocator ) {}

HypoBase::~HypoBase() {} 

const SG::ReadHandleKey<TrigCompositeUtils::DecisionContainer>& HypoBase::decisionInput() const{
  return m_input;
}

const SG::WriteHandleKey<TrigCompositeUtils::DecisionContainer>& HypoBase::decisionOutput() const{
  return m_output;
}

StatusCode HypoBase::sysInitialize() {
  CHECK( AthReentrantAlgorithm::sysInitialize() ); // initialise base class
  CHECK( m_input.initialize() );
  renounce(m_input); // make inputs implicit, i.e. not required by scheduler
  ATH_MSG_DEBUG("Will consume implicit decision: " << m_input.key() );
  CHECK( m_output.initialize() );
  ATH_MSG_DEBUG(" and produce decision: " << m_output.key() );
  return StatusCode::SUCCESS;
}

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// PseudoJetAlgorithm.cxx 

#include "PseudoJetAlgorithm.h"
#include "JetInterface/IPseudoJetGetter.h"

using std::string;

//**********************************************************************

PseudoJetAlgorithm::PseudoJetAlgorithm(const std::string& name, 
                           ISvcLocator* pSvcLocator )
  : ::AthReentrantAlgorithm( name, pSvcLocator ),m_pjg(this) {
  declareProperty( "PJGetter", m_pjg);
}

//**********************************************************************

PseudoJetAlgorithm::~PseudoJetAlgorithm() { }

//**********************************************************************

StatusCode PseudoJetAlgorithm::initialize() {
  ATH_MSG_INFO("Initializing " << name() << "...");
  ATH_MSG_INFO("Retrieving pseudojet getter " << m_pjg->name());
  ATH_CHECK( m_pjg.retrieve() );
  m_pjg->print();
  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode PseudoJetAlgorithm::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode PseudoJetAlgorithm::execute(const EventContext& /*ctx*/) const {
  ATH_MSG_VERBOSE("Executing " << name() << "...");

  ATH_CHECK( m_pjg->createAndRecord() );

  return StatusCode::SUCCESS;
}

//**********************************************************************

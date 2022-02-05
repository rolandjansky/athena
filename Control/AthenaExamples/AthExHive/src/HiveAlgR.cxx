/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "HiveAlgR.h"
#include "GaudiKernel/ServiceHandle.h"

HiveAlgR::HiveAlgR( const std::string& name, 
		    ISvcLocator* pSvcLocator ) : 
  ::AthReentrantAlgorithm( name, pSvcLocator )
{}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
HiveAlgR::~HiveAlgR() {}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
StatusCode HiveAlgR::initialize() {

  info() << "initialize: " << index() << endmsg;

  ATH_CHECK( m_wrh1.initialize() );
  ATH_CHECK( m_evt.initialize() );

  return StatusCode::SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
StatusCode HiveAlgR::finalize() {
  info() << "finalize: " << index() << endmsg;
  return StatusCode::SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
StatusCode HiveAlgR::execute(const EventContext& ctx) const {

  info() << "execute: " << index() << " on " << ctx << endmsg;

  SG::ReadHandle<xAOD::EventInfo> evt(m_evt,ctx);
  ATH_MSG_INFO("   EventInfo:  r: " << evt->runNumber()
               << " e: " << evt->eventNumber() );

  SG::WriteHandle<HiveDataObj> wh1(m_wrh1,ctx);
  ATH_CHECK(wh1.record(std::make_unique<HiveDataObj>(10000 +evt->eventNumber()*100)));

  ATH_MSG_INFO("  write: " << wh1.key() << " = " << wh1->val() );

  return StatusCode::SUCCESS;

}


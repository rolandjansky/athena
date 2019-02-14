/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifdef REENTRANT_GAUDI

#include "HiveAlgR.h"
#include "GaudiKernel/ServiceHandle.h"

#include <thread>
#include <chrono>
#include <ctime>
#include <memory>

DECLARE_COMPONENT(HiveAlgR)

HiveAlgR::HiveAlgR( const std::string& name, 
		    ISvcLocator* pSvcLocator ) : 
  ::AthReentrantAlgorithm( name, pSvcLocator )
{}

HiveAlgR::~HiveAlgR() {}

StatusCode HiveAlgR::initialize() {

  info() << "initialize: " << index() << endmsg;

  ATH_CHECK( m_wrh1.initialize() );
  ATH_CHECK( m_evt.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode HiveAlgR::finalize() {
  info() << "finalize: " << index() << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode HiveAlgR::execute(const EventContext& ctx) const {

  info() << "execute_R: " << index() << " on " << ctx << endmsg;


  SG::ReadHandle<xAOD::EventInfo> evt(m_evt);
  ATH_MSG_INFO("   EventInfo:  r: " << evt->runNumber()
               << " e: " << evt->eventNumber() );


  SG::WriteHandle<HiveDataObj> wh1(m_wrh1);
  ATH_CHECK( wh1.record( std::make_unique<HiveDataObj> 
                         ( HiveDataObj(10000 + 
                                       evt->event_ID()->event_number()*100 )))
             );
  ATH_MSG_INFO("  write: " << wh1.key() << " = " << wh1->val() );

  return StatusCode::SUCCESS;

}

#endif // REENTRANT_GAUDI

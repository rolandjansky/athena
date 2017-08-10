/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HiveAlgA.h"
#include "GaudiKernel/ServiceHandle.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include <thread>
#include <chrono>
#include <ctime>
#include <memory>

HiveAlgA::HiveAlgA( const std::string& name, 
		    ISvcLocator* pSvcLocator ) : 
  ::HiveAlgBase( name, pSvcLocator ), m_i(1)
{
}

HiveAlgA::~HiveAlgA() {}

StatusCode HiveAlgA::initialize() {
  ATH_MSG_DEBUG("initialize " << name());
  
  ATH_CHECK( m_evt.initialize() );
  ATH_CHECK( m_wrh1.initialize() );
  ATH_CHECK( m_wrh2.initialize() );

  return HiveAlgBase::initialize();
}

StatusCode HiveAlgA::finalize() {
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

StatusCode HiveAlgA::execute() {

  ATH_MSG_DEBUG("execute " << name());

  SG::ReadHandle<EventInfo> evt( m_evt );
  if (!evt.isValid()) {
    ATH_MSG_ERROR ("Could not retrieve EventInfo");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("   EventInfo:  r: " << evt->event_ID()->run_number()
		 << " e: " << evt->event_ID()->event_number() );
  }

  sleep();

  SG::WriteHandle<HiveDataObj> wrh1( m_wrh1 );
  wrh1 = std::make_unique< HiveDataObj >
    ( HiveDataObj(10000 + evt->event_ID()->event_number()*100 + m_i) );

  SG::WriteHandle<HiveDataObj> wrh2( m_wrh2 );
  wrh2 = std::make_unique< HiveDataObj >( HiveDataObj(10050+m_i) );
  
  ATH_MSG_INFO("  write: " << wrh1.key() << " = " << wrh1->val() );
  ATH_MSG_INFO("  write: " << wrh2.key() << " = " << wrh2->val() );
  
  ATH_CHECK(wrh1.isValid());
  ATH_CHECK(wrh2.isValid());
    
  m_i += 1;

  return StatusCode::SUCCESS;

}


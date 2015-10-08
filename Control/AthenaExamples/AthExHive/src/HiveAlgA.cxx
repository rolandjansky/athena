/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HiveAlgA.h"
#include "CxxUtils/make_unique.h"
#include "GaudiKernel/ServiceHandle.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include <thread>
#include <chrono>
#include <ctime>

HiveAlgA::HiveAlgA( const std::string& name, 
		    ISvcLocator* pSvcLocator ) : 
  ::HiveAlgBase( name, pSvcLocator ),
  m_wrh1("a1"),
  m_wrh2("a2"),
  m_evt("McEventInfo")
{
  
  declareProperty("Key_W1",m_wrh1);
  declareProperty("Key_W2",m_wrh2);  
  declareProperty("EvtInfo", m_evt);
  
  m_i = 1;

}

HiveAlgA::~HiveAlgA() {}

StatusCode HiveAlgA::initialize() {
  ATH_MSG_DEBUG("initialize " << name());
  return HiveAlgBase::initialize();
}

StatusCode HiveAlgA::finalize() {
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

StatusCode HiveAlgA::execute() {

  ATH_MSG_DEBUG("execute " << name());

  if (!m_evt.isValid()) {
    ATH_MSG_ERROR ("Could not retrieve EventInfo");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("   EventInfo:  r: " << m_evt->event_ID()->run_number()
		 << " e: " << m_evt->event_ID()->event_number() );
  }

  sleep();

  m_wrh1 = CxxUtils::make_unique< HiveDataObj >
    ( HiveDataObj(10000 + m_evt->event_ID()->event_number()*100 + m_i) );
  m_wrh2 = CxxUtils::make_unique< HiveDataObj >( HiveDataObj(10050+m_i) );
  
  ATH_MSG_INFO("  write: " << m_wrh1.key() << " = " << m_wrh1->val() );
  ATH_MSG_INFO("  write: " << m_wrh2.key() << " = " << m_wrh2->val() );
  
  ATH_CHECK(m_wrh1.isValid());
  ATH_CHECK(m_wrh2.isValid());
    
  m_i += 1;

  return StatusCode::SUCCESS;

}


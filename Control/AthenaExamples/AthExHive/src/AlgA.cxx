/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "AlgA.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

#include "GaudiKernel/ServiceHandle.h"

#include <thread>
#include <chrono>
#include <ctime>
#include <memory>

AlgA::AlgA( const std::string& name, 
		    ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator )
{
}

//---------------------------------------------------------------------------

AlgA::~AlgA() {}

StatusCode AlgA::initialize() {
  ATH_MSG_DEBUG("initialize " << name());

  ATH_CHECK( m_wrh1.initialize() );
  ATH_CHECK( m_wrh2.initialize() );
  ATH_CHECK( m_evt.initialize() );

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------

StatusCode AlgA::finalize() {
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------

StatusCode AlgA::execute() {

  ATH_MSG_DEBUG("execute " << name());

  SG::ReadHandle<EventInfo> evt(m_evt);
  ATH_MSG_INFO("   EventInfo:  r: " << evt->event_ID()->run_number()
               << " e: " << evt->event_ID()->event_number()
               << " evt: " << Gaudi::Hive::currentContextEvt() );


  unsigned int i = Gaudi::Hive::currentContextEvt() + 1;

  SG::WriteHandle<HiveDataObj> wh1(m_wrh1);
  ATH_CHECK( wh1.record( std::make_unique<HiveDataObj> 
                         ( HiveDataObj(10000 + 
                                       evt->event_ID()->event_number()*100 + 
                                       i) ) )
             );
  ATH_MSG_INFO("  write: " << wh1.key() << " = " << wh1->val() );


  SG::WriteHandle<HiveDataObj> wh2(m_wrh2);
  ATH_CHECK( wh2.record( std::make_unique< HiveDataObj >( HiveDataObj(10050+i) ) ) );
  ATH_MSG_INFO("  write: " << wh2.key() << " = " << wh2->val() );
    
  return StatusCode::SUCCESS;

}


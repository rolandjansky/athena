/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "AlgT.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

#include "GaudiKernel/ServiceHandle.h"

#include <thread>
#include <chrono>
#include <ctime>
#include <memory>

AlgT::AlgT( const std::string& name, 
		    ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator )
{}

//---------------------------------------------------------------------------

AlgT::~AlgT() {}

StatusCode AlgT::initialize() {
  ATH_MSG_DEBUG("initialize " << name());

  if (m_rdh1.key() != "") ATH_CHECK( m_rdh1.initialize() );
  ATH_CHECK( m_wrh1.initialize() );
  ATH_CHECK( m_evt.initialize() );

  if ( m_tool1.retrieve().isFailure() ) {
    ATH_MSG_FATAL 
      (m_tool1.propertyName() << ": Failed to retrieve tool "
       << m_tool1.type());
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("retreived " << m_tool1.name());
  }

  if ( m_tool2.retrieve().isFailure() ) {
    ATH_MSG_FATAL 
      (m_tool2.propertyName() << ": Failed to retrieve tool "
       << m_tool2.type());
    return StatusCode::FAILURE;
  }else {
    ATH_MSG_INFO("retreived " << m_tool2.name());
  }

  
  if ( m_tool3.retrieve().isFailure() ) {
    ATH_MSG_FATAL 
      (m_tool3.propertyName() << ": Failed to retrieve tool "
       << m_tool3.type());
    return StatusCode::FAILURE;
  }else {
    ATH_MSG_INFO("retreived " << m_tool3.name());
  }


  
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------

StatusCode AlgT::finalize() {
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------

StatusCode AlgT::execute() {

  ATH_MSG_DEBUG("execute " << name());

  SG::ReadHandle<xAOD::EventInfo> evt(m_evt);
  ATH_MSG_INFO("   EventInfo:  r: " << evt->runNumber()
               << " e: " << evt->eventNumber() );


  SG::WriteHandle<HiveDataObj> wh1(m_wrh1);
  ATH_CHECK( wh1.record( std::make_unique<HiveDataObj> 
                         ( HiveDataObj(10000 + 
                                       evt->eventNumber()) ) )
             );
  ATH_MSG_INFO("  write: " << wh1.key() << " = " << wh1->val() );


  m_tool1->doSomething();
  m_tool2->doSomething();
  m_tool3->doSomething();
  

  return StatusCode::SUCCESS;

}


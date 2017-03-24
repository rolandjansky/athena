/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AlgB.h"
#include "CxxUtils/make_unique.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include <thread>
#include <chrono>

AlgB::AlgB( const std::string& name, 
            ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_rdh1("a1"),
  m_wrh1("a3")
{
  
  declareProperty("Key_R1",m_rdh1);
  declareProperty("Key_W1",m_wrh1);

}

AlgB::~AlgB() {}

StatusCode AlgB::initialize() {
  ATH_MSG_DEBUG("initialize " << name());

  ATH_CHECK( m_rdh1.initialize() );
  ATH_CHECK( m_wrh1.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode AlgB::finalize() {
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

StatusCode AlgB::execute() {
  ATH_MSG_DEBUG("execute " << name());

  SG::ReadHandle<HiveDataObj> rh1(m_rdh1);
  if (!rh1.isValid()) {
    ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << m_rdh1.key());
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("  read: " << rh1.key() << " = " << rh1->val() );

  SG::WriteHandle<HiveDataObj> wh1(m_wrh1);
  ATH_CHECK( wh1.record( CxxUtils::make_unique< HiveDataObj >
                         ( HiveDataObj(3300 + rh1->val()) ) ) );

  ATH_MSG_INFO("  write: " << wh1.key() << " = " << wh1->val() );
  ATH_CHECK(wh1.isValid());

  return StatusCode::SUCCESS;

}


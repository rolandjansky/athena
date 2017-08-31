/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AlgB.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include <thread>
#include <chrono>
#include <memory>

AlgB::AlgB( const std::string& name, 
            ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator )
{
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
  ATH_CHECK( wh1.record( std::make_unique< HiveDataObj >
                         ( HiveDataObj(3300 + rh1->val()) ) ) );

  ATH_MSG_INFO("  write: " << wh1.key() << " = " << wh1->val() );
  ATH_CHECK(wh1.isValid());

  return StatusCode::SUCCESS;

}


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HiveAlgD.h"
#include <thread>
#include <chrono>
#include <memory>

HiveAlgD::HiveAlgD( const std::string& name, 
                      ISvcLocator* pSvcLocator ) : 
  ::HiveAlgBase( name, pSvcLocator )
{
}

HiveAlgD::~HiveAlgD() {}

StatusCode HiveAlgD::initialize() {
  ATH_MSG_DEBUG("initialize " << name());

  ATH_CHECK( m_rdh1.initialize() );
  ATH_CHECK( m_wrh1.initialize() );

  return HiveAlgBase::initialize();
}

StatusCode HiveAlgD::finalize() {
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

StatusCode HiveAlgD::execute() {

  ATH_MSG_DEBUG("execute " << name());

  sleep();

  SG::ReadHandle<HiveDataObj> rdh1( m_rdh1 );
  if (!rdh1.isValid()) {
    ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << rdh1.key());
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("  read: " << rdh1.key() << " = " << rdh1->val() );
  
  SG::WriteHandle<HiveDataObj> wrh1( m_wrh1 );
  wrh1 = std::make_unique< HiveDataObj >( HiveDataObj(40000) );
  ATH_MSG_INFO("  write: " << wrh1.key() << " = " << wrh1->val() );

  return StatusCode::SUCCESS;

}


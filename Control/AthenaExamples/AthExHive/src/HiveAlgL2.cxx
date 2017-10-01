/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HiveAlgL2.h"
#include "GaudiKernel/ServiceHandle.h"
#include <thread>
#include <chrono>
#include <memory>

HiveAlgL2::HiveAlgL2( const std::string& name, 
                      ISvcLocator* pSvcLocator ) : 
  ::HiveAlgBase( name, pSvcLocator )
  ,m_udh1("l1")
{
  
  declareProperty("Key_U1",m_udh1);

}

HiveAlgL2::~HiveAlgL2() {}

StatusCode HiveAlgL2::initialize() {
  ATH_MSG_DEBUG("initialize " << name());

  ATH_CHECK( m_rdh1.initialize() );
  ATH_CHECK( m_udh1.initialize() );

  return HiveAlgBase::initialize();
}

StatusCode HiveAlgL2::finalize() {
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

StatusCode HiveAlgL2::execute() {

  ATH_MSG_DEBUG("execute " << name());

  sleep();

  SG::ReadHandle<HiveDataObj> rdh1( m_rdh1 );
  if (!rdh1.isValid()) {
    ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << rdh1.key());
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("  read: " << rdh1.key() << " = " << rdh1->val() );
  
  SG::UpdateHandle<HiveDataObj> udh1( m_udh1 );

  udh1->val( udh1->val() + 1);

  //  wrh1 = std::make_unique< HiveDataObj >( HiveDataObj(rdh1->val()+1) );
  
  ATH_MSG_INFO("  update: " << udh1.key() << " = " << udh1->val() );

  return StatusCode::SUCCESS;

}


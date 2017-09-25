/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HiveAlgL3.h"
#include "GaudiKernel/ServiceHandle.h"
#include <thread>
#include <chrono>
#include <memory>

HiveAlgL3::HiveAlgL3( const std::string& name, 
                      ISvcLocator* pSvcLocator ) : 
  ::HiveAlgBase( name, pSvcLocator )
  ,m_udh1("l1")
{
  
  declareProperty("Key_U1",m_udh1);

}

HiveAlgL3::~HiveAlgL3() {}

StatusCode HiveAlgL3::initialize() {
  ATH_MSG_DEBUG("initialize " << name());

  ATH_CHECK( m_udh1.initialize() );

  return HiveAlgBase::initialize();
}

StatusCode HiveAlgL3::finalize() {
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

StatusCode HiveAlgL3::execute() {

  ATH_MSG_DEBUG("execute " << name());

  sleep();

  SG::UpdateHandle<HiveDataObj> udh1( m_udh1 );

  udh1->val( udh1->val() + 1);

  ATH_MSG_INFO("  update: " << udh1.key() << " = " << udh1->val() );

  return StatusCode::SUCCESS;

}


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HiveAlgC.h"
#include "CxxUtils/make_unique.h"
#include "GaudiKernel/ServiceHandle.h"
#include <thread>
#include <chrono>

HiveAlgC::HiveAlgC( const std::string& name, 
                      ISvcLocator* pSvcLocator ) : 
  ::HiveAlgBase( name, pSvcLocator ),
  m_rdh1("a1"),
  m_wrh1("c1"),
  m_wrh2("c2")
{
  
  declareProperty("Key_R1",m_rdh1);
  declareProperty("Key_W1",m_wrh1);
  declareProperty("Key_W2",m_wrh2);

}

HiveAlgC::~HiveAlgC() {}

StatusCode HiveAlgC::initialize() {
  ATH_MSG_DEBUG("initialize " << name());
  return HiveAlgBase::initialize();
}

StatusCode HiveAlgC::finalize() {
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

StatusCode HiveAlgC::execute() {

  ATH_MSG_DEBUG("execute " << name());

  sleep();

  if (!m_rdh1.isValid()) {
    ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << m_rdh1.key());
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("  read: " << m_rdh1.key() << " = " << m_rdh1->val() );
  
  m_wrh1 = CxxUtils::make_unique< HiveDataObj >( HiveDataObj(30000) );
  m_wrh2 = CxxUtils::make_unique< HiveDataObj >( HiveDataObj(30001) );
  
  ATH_MSG_INFO("  write: " << m_wrh1.key() << " = " << m_wrh1->val() );
  ATH_MSG_INFO("  write: " << m_wrh2.key() << " = " << m_wrh2->val() );

  return StatusCode::SUCCESS;

}


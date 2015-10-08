/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HiveAlgD.h"
#include "CxxUtils/make_unique.h"
#include <thread>
#include <chrono>

HiveAlgD::HiveAlgD( const std::string& name, 
                      ISvcLocator* pSvcLocator ) : 
  ::HiveAlgBase( name, pSvcLocator ),
  m_rdh1("a2"),
  m_wrh1("d1")
{
  
  declareProperty("Key_R1",m_rdh1);
  declareProperty("Key_W1",m_wrh1);

}

HiveAlgD::~HiveAlgD() {}

StatusCode HiveAlgD::initialize() {
  ATH_MSG_DEBUG("initialize " << name());
  return HiveAlgBase::initialize();
}

StatusCode HiveAlgD::finalize() {
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

StatusCode HiveAlgD::execute() {

  ATH_MSG_DEBUG("execute " << name());

  sleep();

  if (!m_rdh1.isValid()) {
    ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << m_rdh1.key());
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("  read: " << m_rdh1.key() << " = " << m_rdh1->val() );
  
  m_wrh1 = CxxUtils::make_unique< HiveDataObj >( HiveDataObj(40000) );
  ATH_MSG_INFO("  write: " << m_wrh1.key() << " = " << m_wrh1->val() );

  return StatusCode::SUCCESS;

}


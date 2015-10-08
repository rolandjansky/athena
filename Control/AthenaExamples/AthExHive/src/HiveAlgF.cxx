/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HiveAlgF.h"
#include "CxxUtils/make_unique.h"
#include <thread>
#include <chrono>

HiveAlgF::HiveAlgF( const std::string& name, 
                      ISvcLocator* pSvcLocator ) : 
  ::HiveAlgBase( name, pSvcLocator ),
  m_rdh1("c2"),
  m_rdh2("e1"),
  m_rdh3("d1")
{
  
  declareProperty("Key_R1",m_rdh1);
  declareProperty("Key_R2",m_rdh2);
  declareProperty("Key_R3",m_rdh3);

}

HiveAlgF::~HiveAlgF() {}

StatusCode HiveAlgF::initialize() {
  ATH_MSG_DEBUG("initialize " << name());
  return HiveAlgBase::initialize ();
}

StatusCode HiveAlgF::finalize() {
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

StatusCode HiveAlgF::execute() {

  ATH_MSG_DEBUG("execute " << name());
 
  sleep();

  if (!m_rdh1.isValid()) {
    ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << m_rdh1.key());
    return StatusCode::FAILURE;
  }
  if (!m_rdh2.isValid()) {
    ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << m_rdh2.key());
    return StatusCode::FAILURE;
  }
  if (!m_rdh3.isValid()) {
    ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << m_rdh3.key());
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("  read: " << m_rdh1.key() << " = " << m_rdh1->val() );
  ATH_MSG_INFO("  read: " << m_rdh2.key() << " = " << m_rdh2->val() );
  ATH_MSG_INFO("  read: " << m_rdh3.key() << " = " << m_rdh3->val() );

  return StatusCode::SUCCESS;

}


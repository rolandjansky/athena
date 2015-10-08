/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HiveAlgE.h"
#include "CxxUtils/make_unique.h"
#include <thread>
#include <chrono>

HiveAlgE::HiveAlgE( const std::string& name, 
                      ISvcLocator* pSvcLocator ) : 
  ::HiveAlgBase( name, pSvcLocator ),
  m_rdh1("c1"),
  m_rdh2("b1"),
  m_wrh1("e1")
{

  declareProperty("Key_R1",m_rdh1);
  declareProperty("Key_R2",m_rdh2);
  declareProperty("Key_W1",m_wrh1);

}

HiveAlgE::~HiveAlgE() {}

StatusCode HiveAlgE::initialize() {
  ATH_MSG_DEBUG("initialize " << name());
  return HiveAlgBase::initialize();
}

StatusCode HiveAlgE::finalize() {
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

StatusCode HiveAlgE::execute() {

  ATH_MSG_DEBUG("execute " << name());
  
  sleep();

  //
  // This is how we used to do it without VarHandles
  //
  // HiveDataObj *or1, *or2;
  // ATH_CHECK(evtStore()->retrieve(or1,"c1"));
  // ATH_MSG_INFO("  read: c1 = " << or1->val() );
  
  // ATH_CHECK(evtStore()->retrieve(or2,"b1"));
  // ATH_MSG_INFO("  read: b1 = " << or2->val() );
  
  // HiveDataObj* ow1 = new HiveDataObj(50000);
  
  // ATH_CHECK(evtStore()->record( ow1 , "e1"));
  // ATH_MSG_INFO("  write: e1 = " << ow1->val());

  if (!m_rdh1.isValid()) {
    ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << m_rdh1.key());
    return StatusCode::FAILURE;
  }
  if (!m_rdh2.isValid()) {
    ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << m_rdh2.key());
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("  read: " << m_rdh1.key() << " = " << m_rdh1->val() );
  ATH_MSG_INFO("  read: " << m_rdh2.key() << " = " << m_rdh2->val() );
  
  m_wrh1 = CxxUtils::make_unique< HiveDataObj >( HiveDataObj(50000) );
  
  ATH_MSG_INFO("  write: " << m_wrh1.key() << " = " << m_wrh1->val() );


  return StatusCode::SUCCESS;

}


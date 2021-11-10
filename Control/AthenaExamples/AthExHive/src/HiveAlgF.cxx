/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "HiveAlgF.h"

HiveAlgF::HiveAlgF( const std::string& name, 
                      ISvcLocator* pSvcLocator ) : 
  ::HiveAlgBase( name, pSvcLocator )
{
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
HiveAlgF::~HiveAlgF() {}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
StatusCode HiveAlgF::initialize() {
  ATH_MSG_DEBUG("initialize " << name());

  ATH_CHECK( m_rdh1.initialize() );
  ATH_CHECK( m_rdh2.initialize() );
  ATH_CHECK( m_rdh3.initialize() );
  ATH_CHECK( m_rdh4.initialize() );
  ATH_CHECK( m_rdh5.initialize() );
  ATH_CHECK( m_rdh6.initialize() );

  // initialize base class
  return HiveAlgBase::initialize ();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
StatusCode HiveAlgF::finalize() {
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
StatusCode HiveAlgF::execute() {

  ATH_MSG_DEBUG("execute " << name());
 
  sleep();

  SG::ReadHandle<HiveDataObj> rdh1( m_rdh1 );
  if (!rdh1.isValid()) {
    ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << rdh1.key());
    return StatusCode::FAILURE;
  }

  SG::ReadHandle<HiveDataObj> rdh2( m_rdh2 );
  if (!rdh2.isValid()) {
    ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << rdh2.key());
    return StatusCode::FAILURE;
  }

  SG::ReadHandle<HiveDataObj> rdh3( m_rdh3 );
  if (!rdh3.isValid()) {
    ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << rdh3.key());
    return StatusCode::FAILURE;
  }

  SG::ReadHandle<HiveDataObj> rdh4( m_rdh4 );
  if (!rdh4.isValid()) {
    ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << rdh4.key());
    return StatusCode::FAILURE;
  }

  SG::ReadHandle<HiveDataObj> rdh5( m_rdh5 );
  if (!rdh5.isValid()) {
    ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << rdh5.key());
    return StatusCode::FAILURE;
  }

  SG::ReadHandle<HiveDataObj> rdh6( m_rdh6 );
  if (!rdh6.isValid()) {
    ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << rdh6.key());
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("  read: " << rdh1.key() << " = " << rdh1->val() );
  ATH_MSG_INFO("  read: " << rdh2.key() << " = " << rdh2->val() );
  ATH_MSG_INFO("  read: " << rdh3.key() << " = " << rdh3->val() );

  ATH_MSG_INFO("test: " << getContext().eventID().event_number() << " "
               << rdh1->val() + rdh2->val() + rdh3->val() 
               << " " << rdh1->val()
               << " " << rdh2->val()
               << " " << rdh3->val()
               << " " << rdh4->val()
               << " " << rdh5->val()
               << " " << rdh6->val()
               );

  return StatusCode::SUCCESS;

}


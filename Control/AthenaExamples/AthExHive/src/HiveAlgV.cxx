/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HiveAlgV.h"
#include "CxxUtils/make_unique.h"
#include <thread>
#include <chrono>
#include <vector>

HiveAlgV::HiveAlgV( const std::string& name, 
                      ISvcLocator* pSvcLocator ) : 
  ::HiveAlgBase( name, pSvcLocator )
{
  
  declareProperty("Key_RV",m_rhv);

}

HiveAlgV::~HiveAlgV() {}

StatusCode HiveAlgV::initialize() {
  ATH_MSG_DEBUG("initialize " << name());

  ATH_CHECK( m_rhv.initialize() );

  ATH_MSG_INFO("ReadHandleKeyArray of size " << m_rhv.size());

  return HiveAlgBase::initialize ();
}

StatusCode HiveAlgV::finalize() {
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

StatusCode HiveAlgV::execute() {

  ATH_MSG_DEBUG("execute " << name());
 
  sleep();

  std::vector< SG::ReadHandle<HiveDataObj> > rhv = m_rhv.makeHandles();

  StatusCode sc { StatusCode::SUCCESS };

  for (auto &hnd : rhv) {
    if (!hnd.isValid()) {
      ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << hnd.key());
      sc = StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO("  read: " << hnd.key() << " = " << hnd->val() );
    }
  }

  return sc;

}


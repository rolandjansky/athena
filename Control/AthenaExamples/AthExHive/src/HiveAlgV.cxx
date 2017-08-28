/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HiveAlgV.h"
#include <thread>
#include <chrono>
#include <vector>
#include <memory>

HiveAlgV::HiveAlgV( const std::string& name, 
                      ISvcLocator* pSvcLocator ) : 
  ::HiveAlgBase( name, pSvcLocator )
{
  
  declareProperty("WriteBeforeRead",m_writeFirst=true);

}

HiveAlgV::~HiveAlgV() {}

StatusCode HiveAlgV::initialize() {
  ATH_MSG_DEBUG("initialize " << name());

  ATH_CHECK( m_rhv.initialize() );
  ATH_CHECK( m_whv.initialize() );

  ATH_MSG_INFO(m_rhv.keys() << " : " << m_rhv.size());
  ATH_MSG_INFO(m_whv.keys() << " : " << m_whv.size());

  return HiveAlgBase::initialize ();
}

StatusCode HiveAlgV::finalize() {
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

StatusCode HiveAlgV::execute() {

  ATH_MSG_DEBUG("execute " << name());
 
  sleep();

  StatusCode sc { StatusCode::SUCCESS };

  if (m_writeFirst) {
    write();
    sc = read();
  } else {
    sc = read();
    write();
  }

  return sc;
}

StatusCode
HiveAlgV::read() const {
  StatusCode sc { StatusCode::SUCCESS };
  std::vector< SG::ReadHandle<HiveDataObj> > rhv = m_rhv.makeHandles();
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

void
HiveAlgV::write() {
  std::vector< SG::WriteHandle<HiveDataObj> > whv = m_whv.makeHandles();
  for (auto &hnd : whv) {
    hnd = std::make_unique<HiveDataObj> ( HiveDataObj( 10101 ) );
    ATH_MSG_INFO("  write: " << hnd.key() << " = " << hnd->val() );
  }
}


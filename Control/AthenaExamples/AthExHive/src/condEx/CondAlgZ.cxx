/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "CondAlgZ.h"
#include "AthExHive/IASCIICondDbSvc.h"
#include "StoreGate/WriteCondHandle.h"

#include "GaudiKernel/ServiceHandle.h"

#include <thread>
#include <chrono>
#include <memory>

CondAlgZ::CondAlgZ( const std::string& name, 
            ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_cs("CondSvc",name)
{
}

CondAlgZ::~CondAlgZ() {}

StatusCode CondAlgZ::initialize() {
  ATH_MSG_DEBUG("initialize " << name());

  if (m_cs.retrieve().isFailure()) {
    ATH_MSG_ERROR("unable to retrieve CondSvc");
  }

  if (m_rchk1.initialize().isFailure()) {
    ATH_MSG_ERROR("unable to initialize ReadCondHandleKey with key" << m_rchk1.key() );
    return StatusCode::FAILURE;
  }

  if (m_rchk2.initialize().isFailure()) {
    ATH_MSG_ERROR("unable to initialize ReadCondHandleKey with key" << m_rchk2.key() );
    return StatusCode::FAILURE;
  }

  if (m_rchk3.initialize().isFailure()) {
    ATH_MSG_ERROR("unable to initialize ReadCondHandleKey with key" << m_rchk3.key() );
    return StatusCode::FAILURE;
  }


  if (m_wch.initialize().isFailure()) {
    ATH_MSG_ERROR("unable to initialize WriteCondHandleKey with key" << m_wch.key() );
    return StatusCode::FAILURE;
  }


  if (m_cs->regHandle(this, m_wch).isFailure()) {
    ATH_MSG_ERROR("unable to register WriteCondHandle " << m_wch.fullKey() 
                  << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode CondAlgZ::finalize() {
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

StatusCode CondAlgZ::execute() {
  ATH_MSG_DEBUG("execute " << name());

  EventIDBase now(getContext().eventID());

  SG::ReadCondHandle<CondDataObj>  rch1(m_rchk1);
  SG::ReadCondHandle<CondDataObjY> rch2(m_rchk2);
  SG::ReadCondHandle<CondDataObjY> rch3(m_rchk3);

  const CondDataObj  *cdo1 = *rch1;
  const CondDataObjY *cdo2 = *rch2;
  const CondDataObjY *cdo3 = *rch3;

  ATH_MSG_DEBUG(" rh1: " << cdo1->val() << " rh2: " << cdo2->val()
                << " rh3: " << cdo3->val());
  
  SG::WriteCondHandle<CondDataObjZ> wch(m_wch);

  // do we have a valid m_wch for current time?
  if ( !wch.isValid(now) ) {

    ATH_MSG_DEBUG("  CondHandle " << wch.key() 
                  << " not valid.");

    wch.addDependency(rch1);
    wch.addDependency(rch2,rch3);

    float val = rch1->val() + rch2->val() + rch3->val();

    
    if (wch.record( std::make_unique<CondDataObjZ>
                    ( CondDataObjZ(val) ) ).isFailure()) {
      ATH_MSG_ERROR("could not record CondDataObjZ " << wch.key()
                    << " val: " << val
                    << " with EventRange " << wch.getRange() );
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("recorded new CDO " << wch.key() << " = " << val
		 << " with range " << wch.getRange() );
  }

  return StatusCode::SUCCESS;

}


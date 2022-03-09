/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "CondAlgZ.h"
#include "AthExHive/IASCIICondDbSvc.h"
#include "StoreGate/WriteCondHandle.h"

#include <thread>
#include <chrono>
#include <memory>

CondAlgZ::CondAlgZ( const std::string& name, 
            ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator )
{
}

CondAlgZ::~CondAlgZ() {}

StatusCode CondAlgZ::initialize() {
  ATH_MSG_DEBUG("initialize " << name());

  ATH_CHECK( m_rchk1.initialize() );
  ATH_CHECK( m_rchk2.initialize() );
  ATH_CHECK( m_rchk3.initialize() );

  ATH_CHECK( m_wch.initialize() );

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


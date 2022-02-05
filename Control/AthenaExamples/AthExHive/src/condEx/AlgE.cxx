/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AlgE.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadCondHandle.h"

#include "GaudiKernel/ServiceHandle.h"
#include <thread>
#include <chrono>
#include <memory>


AlgE::AlgE( const std::string& name, 
            ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator )
{
}

//---------------------------------------------------------------------------

AlgE::~AlgE() {}

//---------------------------------------------------------------------------

StatusCode AlgE::initialize() {
  ATH_MSG_DEBUG("initialize " << name());

  ATH_CHECK( m_rdh1.initialize() );
  ATH_CHECK( m_rch1.initialize() );
  ATH_CHECK( m_rch2.initialize() );
  ATH_CHECK( m_rch3.initialize() );
  ATH_CHECK( m_rch4.initialize() );

  ATH_MSG_INFO( "m_rdh1 id: " << m_rdh1.fullKey() );
  ATH_MSG_INFO( "m_rch1 id: " << m_rch1.fullKey() );
  ATH_MSG_INFO( "m_rch2 id: " << m_rch2.fullKey() );
  ATH_MSG_INFO( "m_rch3 id: " << m_rch3.fullKey() );
  ATH_MSG_INFO( "m_rch4 id: " << m_rch4.fullKey() );

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------

StatusCode AlgE::finalize() {
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------

StatusCode AlgE::execute() {
  ATH_MSG_DEBUG("execute " << name());

  SG::ReadHandle<HiveDataObj> rh1(m_rdh1);
  if (!rh1.isValid()) {
    ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << m_rdh1.key());
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("  read: " << rh1.key() << " = " << rh1->val() );

  SG::ReadCondHandle<CondDataObj>  ch1( m_rch1 );
  SG::ReadCondHandle<CondDataObjY> ch2( m_rch2 );
  SG::ReadCondHandle<CondDataObjY> ch3( m_rch3 );
  SG::ReadCondHandle<CondDataObjZ> ch4( m_rch4 );

  EventIDBase t( getContext().eventID() );
  

  float val {0.0};
  
  const CondDataObj* cdo = *ch1;
  if (cdo != 0) {
    ATH_MSG_INFO("  CDO1: " << *cdo );
    val += cdo->val();
  } else {
    ATH_MSG_ERROR("  CDO1 ptr == zero");
  }

  const CondDataObjY* cdoy = *ch2;
  if (cdoy != 0) {
    ATH_MSG_INFO("  CDO2: " << *cdoy );
    val += cdoy->val();
  } else {
    ATH_MSG_ERROR("  CDO2 ptr == zero");
  }

  cdoy = *ch3;
  if (cdoy != 0) {
    ATH_MSG_INFO("  CDO3: " << *cdoy );
    val += cdoy->val();
  } else {
    ATH_MSG_ERROR("  CDO3 ptr == zero");
  }

  const CondDataObjZ *cdoz = *ch4;
  if (cdoz != 0) {
    ATH_MSG_INFO("  CDO4: " << *cdoz );

    if (cdoz->val() != val) {
      ATH_MSG_ERROR(" value of CDO4 is incorrect ");
    }
    
  } else {
    ATH_MSG_ERROR("  CDO# ptr == zero");
  }

  
  ATH_MSG_INFO("TEST: " << getContext().eventID().event_number() << " "
               << rh1->val() << " " << **ch1 << " " << **ch2 << " " << **ch3
               << " " << **ch4);
  
  return StatusCode::SUCCESS;

}


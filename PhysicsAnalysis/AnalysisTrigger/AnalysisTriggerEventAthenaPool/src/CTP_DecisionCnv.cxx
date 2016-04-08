/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
#include "CTP_DecisionCnv.h"

#include "AnalysisTriggerEventTPCnv/CTP_DecisionCnv_p1.h"
#include "AnalysisTriggerEventTPCnv/CTP_DecisionCnv_p2.h"

// AnalysisTrigger include(s):
#include "AnalysisTriggerEventTPCnv/CTP_Decision_p1.h"

static CTP_DecisionCnv_p2 TPConverter;
static CTP_DecisionCnv_p1 TPConverter_p1;

CTP_Decision_PERS* CTP_DecisionCnv::createPersistent( CTP_Decision* transObj ) {

  MsgStream log( this->messageService(), "CTP_DecisionCnv" );
  CTP_Decision_PERS* persObj = TPConverter.createPersistent( transObj, log );
  return persObj;

}

CTP_Decision* CTP_DecisionCnv::createTransient() {
  
  static pool::Guid p2_guid( "08922DA9-6E47-4D3A-9FEC-0B4440644B50" );
  static pool::Guid p1_guid( "1F421F32-8FBD-47B7-82ED-5354B9BE96D8" );
  static pool::Guid p0_guid( "2A450F66-8A30-42E1-91CB-BE1CEE3C772F" );

  if( this->compareClassGuid( p2_guid ) ) {

    std::auto_ptr< CTP_Decision_p2 > pers_ref( this->poolReadObject< CTP_Decision_p2 >() );
    MsgStream log( this->messageService(), "CTP_DecisionCnv" );
    return TPConverter.createTransient( pers_ref.get(), log );

  } else if( this->compareClassGuid( p1_guid ) ) {

    std::auto_ptr< CTP_Decision_p1 > pers_ref( this->poolReadObject< CTP_Decision_p1 >() );
    MsgStream log( this->messageService(), "CTP_DecisionCnv" );
    return TPConverter_p1.createTransient( pers_ref.get(), log );

  } else if( this->compareClassGuid( p0_guid ) ) {

    return this->poolReadObject< CTP_Decision >();

  }

  throw std::runtime_error( "Unsupported persistent version of CTP_Decision" );
  return 0;

}

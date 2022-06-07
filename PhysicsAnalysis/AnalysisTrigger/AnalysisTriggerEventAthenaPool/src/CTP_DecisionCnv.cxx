/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
#include "CTP_DecisionCnv.h"

// AnalysisTrigger include(s):
#include "AnalysisTriggerEventTPCnv/CTP_Decision_p1.h"

CTP_Decision_PERS* CTP_DecisionCnv::createPersistent( CTP_Decision* transObj ) {

  MsgStream log( this->msgSvc(), "CTP_DecisionCnv" );
  CTP_Decision_PERS* persObj = m_converter.createPersistent( transObj, log );
  return persObj;

}

CTP_Decision* CTP_DecisionCnv::createTransient() {
  
  static const pool::Guid p2_guid( "08922DA9-6E47-4D3A-9FEC-0B4440644B50" );
  static const pool::Guid p1_guid( "1F421F32-8FBD-47B7-82ED-5354B9BE96D8" );
  static const pool::Guid p0_guid( "2A450F66-8A30-42E1-91CB-BE1CEE3C772F" );

  if( this->compareClassGuid( p2_guid ) ) {

    std::unique_ptr< CTP_Decision_p2 > pers_ref( this->poolReadObject< CTP_Decision_p2 >() );
    MsgStream log( this->msgSvc(), "CTP_DecisionCnv" );
    return m_converter.createTransient( pers_ref.get(), log );

  } else if( this->compareClassGuid( p1_guid ) ) {

    std::unique_ptr< CTP_Decision_p1 > pers_ref( this->poolReadObject< CTP_Decision_p1 >() );
    MsgStream log( this->msgSvc(), "CTP_DecisionCnv" );
    return m_converter_p1.createTransient( pers_ref.get(), log );

  } else if( this->compareClassGuid( p0_guid ) ) {

    return this->poolReadObject< CTP_Decision >();

  }

  throw std::runtime_error( "Unsupported persistent version of CTP_Decision" );
  return 0;

}

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
#include "RoIBResultCnv.h"

/**
 * Function creating a persistent RoIBResult_PERS object from a transient
 * ROIB::RoIBResult one.
 */
RoIBResult_PERS* RoIBResultCnv::createPersistent( ROIB::RoIBResult* transObj ) {

   MsgStream log( this->msgSvc(), "RoIBResultCnv" );
   return m_converter.createPersistent( transObj, log );

}

/**
 * Function reading a version of RoIBResult from POOL and converting it to
 * a transient ROIB::RoIBResult object.
 */
ROIB::RoIBResult* RoIBResultCnv::createTransient() {

   static const pool::Guid p1_guid( "A9FF18A0-E5A2-4F24-82C7-605CAA9EA1F8" );
   static const pool::Guid p0_guid( "E9F89B95-329A-4FF1-9110-4CE48D1D7176" );

   if( this->compareClassGuid( p1_guid ) ) {

      std::unique_ptr< RoIBResult_p1 > pers_ref( this->poolReadObject< RoIBResult_p1 >() );
      MsgStream log( this->msgSvc(), "RoIBResultCnv" );
      return m_converter.createTransient( pers_ref.get(), log );

   } else if( this->compareClassGuid( p0_guid ) ) {

      return this->poolReadObject< ROIB::RoIBResult >();

   }

   throw std::runtime_error( "Unsupported persistent version of ROIB::RoIBResult" );
   return 0;

}

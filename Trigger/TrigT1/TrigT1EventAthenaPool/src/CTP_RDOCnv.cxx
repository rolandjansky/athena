/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
#include "CTP_RDOCnv.h"

/**
 * Function creating a persistent CTP_RDO_PERS object from a transient
 * CTP_RDO one.
 */
CTP_RDO_PERS* CTP_RDOCnv::createPersistent( CTP_RDO* transObj ) {

   MsgStream log( this->msgSvc(), "CTP_RDOCnv" );
   return m_converter.createPersistent( transObj, log );

}

/**
 * Function reading a version of CTP_RDO from POOL and converting it to
 * a transient CTP_RDO object.
 */
CTP_RDO* CTP_RDOCnv::createTransient() {

   static const pool::Guid p2_guid( "12717F15-E516-4ECD-BC07-82C72B524AD5" );
   static const pool::Guid p1_guid( "5E6D3E52-952F-4144-BC68-83ACE605AA45" );
   static const pool::Guid p0_guid( "56C714CC-DC17-4927-B413-9151C82792BB" );

   if( this->compareClassGuid( p2_guid ) ) {

      std::unique_ptr< CTP_RDO_p2 > pers_ref( this->poolReadObject< CTP_RDO_p2 >() );
      MsgStream log( this->msgSvc(), "CTP_RDOCnv" );
      return m_converter.createTransient( pers_ref.get(), log );

   } else if( this->compareClassGuid( p1_guid ) ) {

      std::unique_ptr< CTP_RDO_p1 > pers_ref( this->poolReadObject< CTP_RDO_p1 >() );
      MsgStream log( this->msgSvc(), "CTP_RDOCnv" );
      return m_converter_p1.createTransient( pers_ref.get(), log );

   } else if( this->compareClassGuid( p0_guid ) ) {

      return this->poolReadObject< CTP_RDO >();

   }

   throw std::runtime_error( "Unsupported persistent version of CTP_RDO" );
   return 0;

}

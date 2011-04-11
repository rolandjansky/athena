/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// TrigT1 include(s):
#include "TrigT1EventTPCnv/CTP_RDOCnv_p1.h"
#include "TrigT1EventTPCnv/CTP_RDOCnv_p2.h"

// Local include(s):
#include "CTP_RDOCnv.h"

/// The converter for translating the information between
/// transient and persistent objects
static CTP_RDOCnv_p2 TPConverter;
static CTP_RDOCnv_p1 TPConverter_p1;

/**
 * Function creating a persistent CTP_RDO_PERS object from a transient
 * CTP_RDO one.
 */
CTP_RDO_PERS* CTP_RDOCnv::createPersistent( CTP_RDO* transObj ) {

   MsgStream log( this->messageService(), "CTP_RDOCnv" );
   return TPConverter.createPersistent( transObj, log );

}

/**
 * Function reading a version of CTP_RDO from POOL and converting it to
 * a transient CTP_RDO object.
 */
CTP_RDO* CTP_RDOCnv::createTransient() {

   static pool::Guid p2_guid( "12717F15-E516-4ECD-BC07-82C72B524AD5" );
   static pool::Guid p1_guid( "5E6D3E52-952F-4144-BC68-83ACE605AA45" );
   static pool::Guid p0_guid( "56C714CC-DC17-4927-B413-9151C82792BB" );

   if( this->compareClassGuid( p2_guid ) ) {

      std::auto_ptr< CTP_RDO_p2 > pers_ref( this->poolReadObject< CTP_RDO_p2 >() );
      MsgStream log( this->messageService(), "CTP_RDOCnv" );
      return TPConverter.createTransient( pers_ref.get(), log );

   } else if( this->compareClassGuid( p1_guid ) ) {

      std::auto_ptr< CTP_RDO_p1 > pers_ref( this->poolReadObject< CTP_RDO_p1 >() );
      MsgStream log( this->messageService(), "CTP_RDOCnv" );
      return TPConverter_p1.createTransient( pers_ref.get(), log );

   } else if( this->compareClassGuid( p0_guid ) ) {

      return this->poolReadObject< CTP_RDO >();

   }

   throw std::runtime_error( "Unsupported persistent version of CTP_RDO" );
   return 0;

}

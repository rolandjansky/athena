/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// TrigT1 include(s):
#include "TrigT1EventTPCnv/MuCTPI_RDOCnv_p1.h"

// Local include(s):
#include "MuCTPI_RDOCnv.h"

/// The converter for translating the information between
/// transient and persistent objects
static MuCTPI_RDOCnv_p1 TPConverter;

/**
 * Function creating a persistent MuCTPI_RDO_PERS object from a transient
 * MuCTPI_RDO one.
 */
MuCTPI_RDO_PERS* MuCTPI_RDOCnv::createPersistent( MuCTPI_RDO* transObj ) {

   MsgStream log( this->messageService(), "MuCTPI_RDOCnv" );
   return TPConverter.createPersistent( transObj, log );

}

/**
 * Function reading a version of MuCTPI_RDO from POOL and converting it to
 * a transient MuCTPI_RDO object.
 */
MuCTPI_RDO* MuCTPI_RDOCnv::createTransient() {

   static pool::Guid p1_guid( "406BC4C7-56B6-4956-A66A-B749BCD35009" );
   static pool::Guid p0_guid( "5BE3FA7E-CC70-4842-A095-CA046164764D" );

   if( this->compareClassGuid( p1_guid ) ) {

      std::auto_ptr< MuCTPI_RDO_p1 > pers_ref( this->poolReadObject< MuCTPI_RDO_p1 >() );
      MsgStream log( this->messageService(), "MuCTPI_RDOCnv" );
      return TPConverter.createTransient( pers_ref.get(), log );

   } else if( this->compareClassGuid( p0_guid ) ) {

      return this->poolReadObject< MuCTPI_RDO >();

   }

   throw std::runtime_error( "Unsupported persistent version of MuCTPI_RDO" );
   return 0;

}

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
#include "MuCTPI_RDOCnv.h"

/**
 * Function creating a persistent MuCTPI_RDO_PERS object from a transient
 * MuCTPI_RDO one.
 */
MuCTPI_RDO_PERS* MuCTPI_RDOCnv::createPersistent( MuCTPI_RDO* transObj ) {

   MsgStream log( this->msgSvc(), "MuCTPI_RDOCnv" );
   return m_converter.createPersistent( transObj, log );

}

/**
 * Function reading a version of MuCTPI_RDO from POOL and converting it to
 * a transient MuCTPI_RDO object.
 */
MuCTPI_RDO* MuCTPI_RDOCnv::createTransient() {

   static const pool::Guid p1_guid( "406BC4C7-56B6-4956-A66A-B749BCD35009" );
   static const pool::Guid p0_guid( "5BE3FA7E-CC70-4842-A095-CA046164764D" );

   if( this->compareClassGuid( p1_guid ) ) {

      std::unique_ptr< MuCTPI_RDO_p1 > pers_ref( this->poolReadObject< MuCTPI_RDO_p1 >() );
      MsgStream log( this->msgSvc(), "MuCTPI_RDOCnv" );
      return m_converter.createTransient( pers_ref.get(), log );

   } else if( this->compareClassGuid( p0_guid ) ) {

      return this->poolReadObject< MuCTPI_RDO >();

   }

   throw std::runtime_error( "Unsupported persistent version of MuCTPI_RDO" );
   return 0;

}

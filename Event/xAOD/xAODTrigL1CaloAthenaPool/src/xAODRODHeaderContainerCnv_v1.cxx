/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODRODHeaderContainerCnv_v1.cxx 694352 2015-09-11 12:05:29Z krasznaa $

// System include(s):
#include <stdexcept>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
#include "xAODRODHeaderContainerCnv_v1.h"

/// Convenience macro defining the converter's output message level
#define MSGLVL MSG::DEBUG

/// Convenience macro for printing messages in the converter
#define ATH_MSG( MSG )                          \
   do {                                         \
      if( log.level() <= MSGLVL ) {             \
         log << MSGLVL << MSG << endmsg;        \
      }                                         \
   } while( 0 )

xAODRODHeaderContainerCnv_v1::xAODRODHeaderContainerCnv_v1()
   : T_AthenaPoolTPCnvBase< xAOD::RODHeaderContainer,
                            xAOD::RODHeaderContainer_v1 >() {

}

void xAODRODHeaderContainerCnv_v1::
persToTrans( const xAOD::RODHeaderContainer_v1* oldObj,
             xAOD::RODHeaderContainer* newObj,
             MsgStream& log ) {

   // Greet the user:
   ATH_MSG( "Converting xAOD::RODHeaderContainer_v1 to current version..." );

   // Clear the transient object:
   newObj->clear();

   // Simply fill the transient object with as many default xAOD::RODHeader
   // objects, as many there are in the old object:
   for( size_t i = 0; i < oldObj->size(); ++i ) {
      newObj->push_back( new xAOD::RODHeader() );
   }

   // Tell the user what happened:
   ATH_MSG( "Converting xAOD::RODHeaderContainer_v1 to current version "
            "[OK]" );

   // Return gracefully:
   return;
}

/// This function is never supposed to be called...
void xAODRODHeaderContainerCnv_v1::
transToPers( const xAOD::RODHeaderContainer*,
             xAOD::RODHeaderContainer_v1*,
             MsgStream& log ) {

   log << MSG::ERROR << "xAODRODHeaderContainerCnv_v1::transToPers got "
       << "called" << endmsg;
   throw std::runtime_error( "xAODRODHeaderContainerCnv_v1::transToPers got "
                             "called" );

   return;
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODHIEventShapeContainerCnv_v1.cxx 694377 2015-09-11 13:50:57Z krasznaa $

// System include(s):
#include <stdexcept>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
#include "xAODHIEventShapeContainerCnv_v1.h"

/// Convenience macro defining the converter's output message level
#define MSGLVL MSG::DEBUG

/// Convenience macro for printing messages in the converter
#define ATH_MSG( MSG )                          \
   do {                                         \
      if( log.level() <= MSGLVL ) {             \
         log << MSGLVL << MSG << endmsg;        \
      }                                         \
   } while( 0 )

xAODHIEventShapeContainerCnv_v1::xAODHIEventShapeContainerCnv_v1()
   : T_AthenaPoolTPCnvBase< xAOD::HIEventShapeContainer,
                            xAOD::HIEventShapeContainer_v1 >() {

}

void xAODHIEventShapeContainerCnv_v1::
persToTrans( const xAOD::HIEventShapeContainer_v1* oldObj,
             xAOD::HIEventShapeContainer* newObj,
             MsgStream& log ) {

   // Greet the user:
   ATH_MSG( "Converting xAOD::HIEventShapeContainer_v1 to current version..." );

   // Clear the transient object:
   newObj->clear();

   // Simply fill the transient object with as many default xAOD::HIEventShape
   // objects, as many there are in the old object:
   for( size_t i = 0; i < oldObj->size(); ++i ) {
      newObj->push_back( new xAOD::HIEventShape() );
   }

   // Tell the user what happened:
   ATH_MSG( "Converting xAOD::HIEventShapeContainer_v1 to current version "
            "[OK]" );

   // Return gracefully:
   return;
}

/// This function is never supposed to be called...
void xAODHIEventShapeContainerCnv_v1::
transToPers( const xAOD::HIEventShapeContainer*,
             xAOD::HIEventShapeContainer_v1*,
             MsgStream& log ) {

   log << MSG::ERROR << "xAODHIEventShapeContainerCnv_v1::transToPers got "
       << "called" << endmsg;
   throw std::runtime_error( "xAODHIEventShapeContainerCnv_v1::transToPers got "
                             "called" );

   return;
}

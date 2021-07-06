/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigRingerRingsAuxContainerCnv_v1.cxx 713588 2015-12-09 13:59:15Z jodafons $

// System include(s):
#include <stdexcept>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// EDM include(s):
#include "xAODTrigRinger/versions/TrigRingerRingsContainer_v1.h"
#include "xAODTrigRinger/TrigRingerRingsContainer.h"
#include "xAODTrigRinger/TrigRingerRings.h"
#include "AthContainers/tools/copyAuxStoreThinned.h"

// Local include(s):
#include "xAODTrigRingerRingsAuxContainerCnv_v1.h"

/// Convenience macro for setting the level of output messages
#define MSGLVL MSG::DEBUG

/// Another convenience macro for printing messages in the converter
#define ATH_MSG( MSG )                          \
   do {                                         \
      if( log.level() <= MSGLVL ) {             \
         log << MSGLVL << MSG << endmsg;        \
      }                                         \
   } while( 0 )

xAODTrigRingerRingsAuxContainerCnv_v1::xAODTrigRingerRingsAuxContainerCnv_v1()
{
}

void xAODTrigRingerRingsAuxContainerCnv_v1::
persToTrans( const xAOD::TrigRingerRingsAuxContainer_v1* oldObj,
             xAOD::TrigRingerRingsAuxContainer* newObj,
             MsgStream& log ) const {

   // Greet the user:
   ATH_MSG( "Converting xAOD::TrigRingerRingsAuxContainer_v1 to current "
            "version..." );

   // Clear the transient object:
   newObj->resize( 0 );

   SG::copyAuxStoreThinned( *oldObj, *newObj, nullptr );

   // Set up an interface container for the old object:
   xAOD::TrigRingerRingsContainer_v1 oldInt;
   for( size_t i = 0; i < oldObj->size(); ++i ) {
      oldInt.push_back( new xAOD::TrigRingerRings_v1() );
   }
   oldInt.setStore( oldObj );

   // Set up an interface container for the new object:
   xAOD::TrigRingerRingsContainer newInt;
   for( size_t i = 0; i < newObj->size(); ++i ) {
      newInt.push_back( new xAOD::TrigRingerRings() );
   }
   newInt.setStore( newObj );

   // Translate the objects one by one:
   for( size_t i = 0; i < oldInt.size(); ++i ) {
      newInt[ i ]->setRings( oldInt[ i ]->rings() );
   }

   return;
}

/// This function should never be called, as we are not supposed to convert
/// object before writing.
///
void xAODTrigRingerRingsAuxContainerCnv_v1::
transToPers( const xAOD::TrigRingerRingsAuxContainer*,
             xAOD::TrigRingerRingsAuxContainer_v1*,
             MsgStream& log ) const {

   log << MSG::ERROR
       << "Somebody called xAODTrigRingerRingsAuxContainerCnv_v1::transToPers"
       << endmsg;
   throw std::runtime_error( "Somebody called "
                             "xAODTrigRingerRingsAuxContainerCnv_v1::"
                             "transToPers" );

   return;
}

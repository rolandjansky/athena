/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Core EDM include(s):
#include "AthContainers/tools/copyAuxStoreThinned.h"

// Local include(s):
#include "xAODBTaggingAuxContainerCnv_v1.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/versions/BTaggingContainer_v1.h"

/// Convenience macro for setting the level of output messages
#define MSGLVL MSG::DEBUG

/// Another convenience macro for printing messages in the converter
#define ATH_MSG( MSG )                          \
   do {                                         \
      if( log.level() <= MSGLVL ) {             \
         log << MSGLVL << MSG << endmsg;        \
      }                                         \
   } while( 0 )

xAODBTaggingAuxContainerCnv_v1::xAODBTaggingAuxContainerCnv_v1()
{
}

void xAODBTaggingAuxContainerCnv_v1::
persToTrans( const xAOD::BTaggingAuxContainer_v1* oldObj,
             xAOD::BTaggingAuxContainer* newObj,
             MsgStream& log ) const {

   // Greet the user:
   ATH_MSG( "Converting xAOD::BTaggingAuxContainer_v1 to current version..." );

   // Clear the transient object:
   newObj->resize( 0 );

   // Copy the payload of the v1 object into the latest one by misusing
   // the thinning code a bit...
   SG::copyAuxStoreThinned( *oldObj, *newObj, nullptr );

   // Print what happened:
   ATH_MSG( "Converting xAOD::BTaggingAuxContainer_v1 to current version "
            "[OK]" );

   return;
}

/// This function should never be called, as we are not supposed to convert
/// object before writing.
///
void xAODBTaggingAuxContainerCnv_v1::
transToPers( const xAOD::BTaggingAuxContainer*,
             xAOD::BTaggingAuxContainer_v1*,
             MsgStream& log ) const {

   log << MSG::ERROR
       << "Somebody called xAODBTaggingAuxContainerCnv_v1::transToPers"
       << endmsg;
   throw std::runtime_error( "Somebody called xAODBTaggingAuxContainerCnv_v1::"
                             "transToPers" );

   return;
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODPhotonTrigAuxContainerOfflineCnv_v1.cxx 705790 2015-11-04 13:09:23Z krasznaa $

// System include(s):
#include <stdexcept>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Core EDM include(s):
#include "AthContainers/tools/copyAuxStoreThinned.h"

// Local include(s):
#include "xAODPhotonTrigAuxContainerOfflineCnv_v1.h"

/// Convenience macro for setting the level of output messages
#define MSGLVL MSG::DEBUG

/// Another convenience macro for printing messages in the converter
#define ATH_MSG( MSG )                          \
   do {                                         \
      if( log.level() <= MSGLVL ) {             \
         log << MSGLVL << MSG << endmsg;        \
      }                                         \
   } while( 0 )

xAODPhotonTrigAuxContainerOfflineCnv_v1::
xAODPhotonTrigAuxContainerOfflineCnv_v1()
   : T_AthenaPoolTPCnvBase< xAOD::PhotonTrigAuxContainer,
                            xAOD::PhotonAuxContainer_v1 >() {

}

void xAODPhotonTrigAuxContainerOfflineCnv_v1::
persToTrans( const xAOD::PhotonAuxContainer_v1* oldObj,
             xAOD::PhotonTrigAuxContainer* newObj,
             MsgStream& log ) {

   // Greet the user:
   ATH_MSG( "Converting xAOD::PhotonAuxContainer_v1 to "
            "xAOD::PhotonTrigAuxContainer..." );

   // Clear the transient object:
   newObj->resize( 0 );

   // Copy the payload of the v1 object into the latest one by misusing
   // the thinning code a bit...
   SG::copyAuxStoreThinned( *oldObj, *newObj, 0 );

   // Print what happened:
   ATH_MSG( "Converting xAOD::PhotonAuxContainer_v1 to "
            "xAOD::PhotonTrigAuxContainer [OK]" );

   return;
}

/// This function should never be called, as we are not supposed to convert
/// object before writing.
///
void xAODPhotonTrigAuxContainerOfflineCnv_v1::
transToPers( const xAOD::PhotonTrigAuxContainer*,
             xAOD::PhotonAuxContainer_v1*,
             MsgStream& log ) {

   log << MSG::ERROR
       << "Somebody called xAODPhotonTrigAuxContainerOfflineCnv_v1::"
       << "transToPers" << endmsg;
   throw std::runtime_error( "Somebody called "
                             "xAODPhotonTrigAuxContainerOfflineCnv_v1::"
                             "transToPers" );

   return;
}

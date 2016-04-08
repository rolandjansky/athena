/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigEMClusterAuxContainerCnv_v1.cxx 621103 2014-10-10 12:30:01Z $

// System include(s):
#include <stdexcept>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Core EDM include(s):
#include "AthContainers/tools/copyAuxStoreThinned.h"

// Local include(s):
#include "xAODTrigEMClusterAuxContainerCnv_v1.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigCalo/versions/TrigEMClusterContainer_v1.h"

/// Convenience macro for setting the level of output messages
#define MSGLVL MSG::DEBUG

/// Another convenience macro for printing messages in the converter
#define ATH_MSG( MSG )                          \
   do {                                         \
      if( log.level() <= MSGLVL ) {             \
         log << MSGLVL << MSG << endmsg;        \
      }                                         \
   } while( 0 )

xAODTrigEMClusterAuxContainerCnv_v1::xAODTrigEMClusterAuxContainerCnv_v1()
  : T_AthenaPoolTPCnvBase< xAOD::TrigEMClusterAuxContainer,
                           xAOD::TrigEMClusterAuxContainer_v1 >() {

}

void xAODTrigEMClusterAuxContainerCnv_v1::
persToTrans( const xAOD::TrigEMClusterAuxContainer_v1* oldObj,
             xAOD::TrigEMClusterAuxContainer* newObj,
             MsgStream& log ) {

   // Greet the user:
   ATH_MSG( "Converting xAOD::TrigEMClusterAuxContainer_v1 to current version..." );

   // Clear the transient object:
   newObj->resize( 0 );

   // Copy the payload of the v1 object into the latest one by misusing
   // the thinning code a bit...
   SG::copyAuxStoreThinned( *oldObj, *newObj, 0 );
   
   // Print what happened:
   ATH_MSG( "Converting xAOD::TrigEMClusterAuxContainer_v1 to current version "
            "[OK]" );

   return;
}

/// This function should never be called, as we are not supposed to convert
/// object before writing.
///
void xAODTrigEMClusterAuxContainerCnv_v1::
transToPers( const xAOD::TrigEMClusterAuxContainer*,
             xAOD::TrigEMClusterAuxContainer_v1*,
             MsgStream& log ) {

   log << MSG::ERROR
       << "Somebody called xAODTrigEMClusterAuxContainerCnv_v1::transToPers"
       << endmsg;
   throw std::runtime_error( "Somebody called xAODTrigEMClusterAuxContainerCnv_v1::"
                             "transToPers" );

   return;
}

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODCaloClusterAuxContainerCnv_v1.cxx 628099 2014-11-13 09:30:05Z krasznaa $

// System include(s):
#include <stdexcept>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Core EDM include(s):
#include "AthContainers/tools/copyAuxStoreThinned.h"

// Local include(s):
#include "xAODCaloClusterAuxContainerCnv_v1.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/versions/CaloClusterContainer_v1.h"

/// Convenience macro for setting the level of output messages
#define MSGLVL MSG::DEBUG

/// Another convenience macro for printing messages in the converter
#define ATH_MSG( MSG )                          \
   do {                                         \
      if( log.level() <= MSGLVL ) {             \
         log << MSGLVL << MSG << endmsg;        \
      }                                         \
   } while( 0 )

xAODCaloClusterAuxContainerCnv_v1::xAODCaloClusterAuxContainerCnv_v1()
{
}

void xAODCaloClusterAuxContainerCnv_v1::
persToTrans( const xAOD::CaloClusterAuxContainer_v1* oldObj,
             xAOD::CaloClusterAuxContainer* newObj,
             MsgStream& log ) const {

   // Greet the user:
   ATH_MSG( "Converting xAOD::CaloClusterAuxContainer_v1 to current version..." );

   // Clear the transient object:
   newObj->resize( 0 );

   // Copy the payload of the v1 object into the latest one by misusing
   // the thinning code a bit...
   SG::copyAuxStoreThinned( *oldObj, *newObj, nullptr );

   // Print what happened:
   ATH_MSG( "Converting xAOD::CaloClusterAuxContainer_v1 to current version "
            "[OK]" );

   return;
}

/// This function should never be called, as we are not supposed to convert
/// object before writing.
///
void xAODCaloClusterAuxContainerCnv_v1::
transToPers( const xAOD::CaloClusterAuxContainer*,
             xAOD::CaloClusterAuxContainer_v1*,
             MsgStream& log ) const {

   log << MSG::ERROR
       << "Somebody called xAODCaloClusterAuxContainerCnv_v1::transToPers"
       << endmsg;
   throw std::runtime_error( "Somebody called xAODCaloClusterAuxContainerCnv_v1::"
                             "transToPers" );

   return;
}

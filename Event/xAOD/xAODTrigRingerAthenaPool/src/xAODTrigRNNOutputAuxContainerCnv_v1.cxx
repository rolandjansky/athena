/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <stdexcept>
#include <vector>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// EDM include(s):
#include "AthContainers/tools/copyAuxStoreThinned.h"
#include "xAODTrigRinger/versions/TrigRNNOutputContainer_v1.h"
#include "xAODTrigRinger/TrigRNNOutputContainer.h"
#include "xAODTrigRinger/TrigRNNOutput.h"

// Local include(s):
#include "xAODTrigRNNOutputAuxContainerCnv_v1.h"

/// Convenience macro for setting the level of output messages
#define MSGLVL MSG::DEBUG

/// Another convenience macro for printing messages in the converter
#define ATH_MSG( MSG )                          \
   do {                                         \
      if( log.level() <= MSGLVL ) {             \
         log << MSGLVL << MSG << endmsg;        \
      }                                         \
   } while( 0 )

xAODTrigRNNOutputAuxContainerCnv_v1::xAODTrigRNNOutputAuxContainerCnv_v1()
{
}

void xAODTrigRNNOutputAuxContainerCnv_v1::
persToTrans( const xAOD::TrigRNNOutputAuxContainer_v1* oldObj,
             xAOD::TrigRNNOutputAuxContainer* newObj,
             MsgStream& log ) const {

   // Greet the user:
   ATH_MSG( "Converting xAOD::TrigRNNOutputAuxContainer_v1 to current version..." );

   // Clear the transient object:
   newObj->resize(0);
   SG::copyAuxStoreThinned( *oldObj, *newObj, nullptr );

   // Set up an interface container on top of the old object:
   xAOD::TrigRNNOutputContainer_v1 oldInt;
   for( size_t i = 0; i < oldObj->size(); ++i ) {
     oldInt.push_back( new xAOD::TrigRNNOutput_v1() );
   }
   oldInt.setStore( oldObj );

   // Set up an interface container on top of the new object:
   xAOD::TrigRNNOutputContainer newInt;
   for (size_t i = 0; i < newObj->size(); ++i) {
      newInt.push_back( new xAOD::TrigRNNOutput() );
   }
   newInt.setStore( newObj );

   // Convert the objects one by one:
   for( size_t i = 0; i < oldInt.size(); ++i ) {
      newInt[ i ]->setRnnDecision( oldInt[ i ]->decision() );
   }

   return;
}

/// This function should never be called, as we are not supposed to convert
/// object before writing.
///
void xAODTrigRNNOutputAuxContainerCnv_v1::
transToPers( const xAOD::TrigRNNOutputAuxContainer*,
             xAOD::TrigRNNOutputAuxContainer_v1*,
             MsgStream& log ) const {

   log << MSG::ERROR
       << "Somebody called xAODTrigRNNOutputAuxContainerCnv_v1::transToPers"
       << endmsg;
   throw std::runtime_error( "Somebody called "
                             "xAODTrigRNNOutputAuxContainerCnv_v1::"
                             "transToPers" );

   return;
}

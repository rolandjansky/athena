/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <stdexcept>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// EDM include(s):
#include "xAODTrigger/versions/TrigCompositeAuxContainer_v1.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"
#include "xAODTrigger/versions/TrigCompositeContainer_v1.h"
#include "xAODTrigger/TrigCompositeContainer.h"

// Core EDM include(s):
#include "AthContainers/tools/copyAuxStoreThinned.h"

// Local include(s):
#include "xAODTrigCompositeAuxContainerCnv_v1.h"

/// Convenience macro for setting the level of output messages
#define MSGLVL MSG::DEBUG

/// Another convenience macro for printing messages in the converter
#define ATH_MSG( MSG )                          \
   do {                                         \
      if( log.level() <= MSGLVL ) {             \
         log << MSGLVL << MSG << endmsg;        \
      }                                         \
   } while( 0 )

xAODTrigCompositeAuxContainerCnv_v1::xAODTrigCompositeAuxContainerCnv_v1()
  : T_AthenaPoolTPCnvBase< xAOD::TrigCompositeAuxContainer,
                           xAOD::TrigCompositeAuxContainer_v1 >() {

}

void xAODTrigCompositeAuxContainerCnv_v1::
persToTrans( const xAOD::TrigCompositeAuxContainer_v1* oldObj,
             xAOD::TrigCompositeAuxContainer* newObj,
             MsgStream& log ) {

   // Greet the user:
   ATH_MSG( "Converting xAOD::TrigCompositeAuxContainer_v1 to current version..." );

   // ROOT work-around https://sft.its.cern.ch/jira/browse/ROOT-9762
   // This resets the newly made underlying ByteStreamAuxContainer_1 such that it knows about its dynamic content
   // This can be removed once xAODTrigger's selection.xml is correctly calling this.
   xAOD::TrigCompositeAuxContainer_v1* mutableOld = const_cast<xAOD::TrigCompositeAuxContainer_v1*>(oldObj);
   if (mutableOld) {
      mutableOld->reset();
   } else {
      throw std::runtime_error("Unable to const_cast xAOD::TrigCompositeAuxContainer_v1. Needed due to ROOT-9762.");
   }

   // Clear the transient object:
   newObj->resize( 0 );

   // This line does the conversion.
   SG::copyAuxStoreThinned( *oldObj, *newObj, 0 ); 

   // Print what happened:
   ATH_MSG( "Converting xAOD::TrigCompositeAuxContainer_v1 to current version "
      "[OK]" );
   
   return;
}

/// This function should never be called, as we are not supposed to convert
/// object before writing.
///
void xAODTrigCompositeAuxContainerCnv_v1::transToPers( const xAOD::TrigCompositeAuxContainer*,
                                                       xAOD::TrigCompositeAuxContainer_v1*,
                                                       MsgStream& log ) {

   log << MSG::ERROR
       << "Somebody called xAODTrigCompositeAuxContainerCnv_v1::transToPers"
       << endmsg;
   throw std::runtime_error( "Somebody called xAODTrigCompositeAuxContainerCnv_v1::"
                             "transToPers" );

   return;
}

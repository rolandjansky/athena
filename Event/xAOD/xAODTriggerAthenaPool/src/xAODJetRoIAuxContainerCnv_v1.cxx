/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// System include(s):
#include <stdexcept>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// EDM include(s):
#include "AthContainers/tools/copyAuxStoreThinned.h"
#include "xAODTrigger/versions/JetRoIContainer_v1.h"
#include "xAODTrigger/JetRoIContainer.h"

// Local include(s):
#include "xAODJetRoIAuxContainerCnv_v1.h"

/// Convenience macro for setting the level of output messages
#define MSGLVL MSG::INFO

/// Another convenience macro for printing messages in the converter
#define ATH_MSG( MSG )                          \
   do {                                         \
      if( log.level() <= MSGLVL ) {             \
         log << MSGLVL << MSG << endmsg;        \
      }                                         \
   } while( 0 )

xAODJetRoIAuxContainerCnv_v1::xAODJetRoIAuxContainerCnv_v1()
{
}

void xAODJetRoIAuxContainerCnv_v1::
persToTrans( const xAOD::JetRoIAuxContainer_v1* oldObj,
             xAOD::JetRoIAuxContainer* newObj,
             MsgStream& /*log*/ ) const {

   // Greet the user:
   //ATH_MSG( "Converting xAOD::JetRoIAuxContainer_v1 to current version..." );

   // Remove this line once the converter is "ready":
   //ATH_MSG( "WARNING Converter is not complete yet!" );

   // Clear the transient object:
   newObj->resize( 0 );

   SG::copyAuxStoreThinned( *oldObj, *newObj, nullptr );

   // Now a bit of jiggery-pokery to set threshold pattern
   xAOD::JetRoIContainer newInt;
   for (size_t i = 0; i < newObj->size(); ++i) {
      newInt.push_back( new xAOD::JetRoI() );
   }
   newInt.setStore( newObj);

   for (size_t i = 0; i < newObj->size(); ++i) {
     if ((newInt[i]->roiWord()&0xe0000000)==0x80000000) {
       uint32_t pattern = newInt[i]->roiWord()&0xfff;
       newInt[i]->setThrPattern(pattern);
     }
   }


   return;
}

/// This function should never be called, as we are not supposed to convert
/// object before writing.
///
void xAODJetRoIAuxContainerCnv_v1::transToPers( const xAOD::JetRoIAuxContainer*,
                                                xAOD::JetRoIAuxContainer_v1*,
                                                MsgStream& log ) const {

   log << MSG::ERROR
       << "Somebody called xAODJetRoIAuxContainerCnv_v1::transToPers"
       << endmsg;
   throw std::runtime_error( "Somebody called xAODJetRoIAuxContainerCnv_v1::"
                             "transToPers" );

   return;
}


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
#include "xAODTrigger/versions/EmTauRoIContainer_v1.h"
#include "xAODTrigger/EmTauRoIContainer.h"

// Local include(s):
#include "xAODEmTauRoIAuxContainerCnv_v1.h"

/// Convenience macro for setting the level of output messages
#define MSGLVL MSG::INFO

/// Another convenience macro for printing messages in the converter
#define ATH_MSG( MSG )                          \
   do {                                         \
      if( log.level() <= MSGLVL ) {             \
         log << MSGLVL << MSG << endmsg;        \
      }                                         \
   } while( 0 )

xAODEmTauRoIAuxContainerCnv_v1::xAODEmTauRoIAuxContainerCnv_v1()
{
}

void xAODEmTauRoIAuxContainerCnv_v1::
persToTrans( const xAOD::EmTauRoIAuxContainer_v1* oldObj,
             xAOD::EmTauRoIAuxContainer* newObj,
             MsgStream& /*log*/ ) const {

   // Greet the user:
   //ATH_MSG( "Converting xAOD::EmTauRoIAuxContainer_v1 to current version..." );

   // Remove this line once the converter is "ready":
   //ATH_MSG( "WARNING Converter is not complete yet!" );

   // Clear the transient object:
   newObj->resize( 0 );

   SG::copyAuxStoreThinned( *oldObj, *newObj, nullptr );

   // Now a bit of jiggery-pokery to set threshold pattern
   xAOD::EmTauRoIContainer newInt;
   for (size_t i = 0; i < newObj->size(); ++i) {
      newInt.push_back( new xAOD::EmTauRoI() );
   }
   newInt.setStore( newObj);

   for (size_t i = 0; i < newObj->size(); ++i) {
     if (newInt[i]->roiType() == xAOD::EmTauRoI::CPRoIWord) {
       uint32_t pattern = newInt[i]->roiWord()&0xffff;
       newInt[i]->setThrPattern(pattern);
     }
   }

   return;
}

/// This function should never be called, as we are not supposed to convert
/// object before writing.
///
void xAODEmTauRoIAuxContainerCnv_v1::transToPers( const xAOD::EmTauRoIAuxContainer*,
                                                xAOD::EmTauRoIAuxContainer_v1*,
                                                MsgStream& log ) const {

   log << MSG::ERROR
       << "Somebody called xAODEmTauRoIAuxContainerCnv_v1::transToPers"
       << endmsg;
   throw std::runtime_error( "Somebody called xAODEmTauRoIAuxContainerCnv_v1::"
                             "transToPers" );

   return;
}

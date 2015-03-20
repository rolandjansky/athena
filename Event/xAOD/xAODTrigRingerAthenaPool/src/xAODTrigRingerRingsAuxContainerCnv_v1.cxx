/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <stdexcept>
#include <vector>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

//EDM include(s):
#include "AthContainers/tools/copyAuxStoreThinned.h"
#include "xAODTrigRinger/versions/TrigRingerRingsContainer_v1.h"
#include "xAODTrigRinger/TrigRingerRingsContainer.h"

#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigCalo/TrigEMCluster.h"

#include "AthLinks/ElementLink.h"

// Local include(s):
#include "xAODTrigRingerRingsAuxContainerCnv_v1.h"

/// Convenience macro for setting the level of output messages
#define MSGLVL MSG::INFO

/// Another convenience macro for printing messages in the converter
#define ATH_MSG( MSG )                          \
   do {                                         \
      if( log.level() <= MSGLVL ) {             \
         log << MSGLVL << MSG << endmsg;        \
      }                                         \
   } while( 0 )

xAODTrigRingerRingsAuxContainerCnv_v1::xAODTrigRingerRingsAuxContainerCnv_v1(): T_AthenaPoolTPCnvBase< xAOD::TrigRingerRingsAuxContainer,
                                                                                                xAOD::TrigRingerRingsAuxContainer_v1 >() {

}

void xAODTrigRingerRingsAuxContainerCnv_v1::persToTrans( const xAOD::TrigRingerRingsAuxContainer_v1* oldObj,
                                                               xAOD::TrigRingerRingsAuxContainer* newObj,
                                                               MsgStream& /*log*/ ) {

   // Greet the user:
   //ATH_MSG( "Converting xAOD::TrigRingerRingsAuxContainer_v1 to current version..." );

   // Remove this line once the converter is "ready":
   //ATH_MSG( "WARNING Converter is not complete yet!" );

   // Clear the transient object:
   newObj->resize( 0 );

   SG::copyAuxStoreThinned( *oldObj, *newObj, 0 );

   //The old  uses v1
   xAOD::TrigRingerRingsContainer_v1 oldInt;
   for( size_t i = 0; i < oldObj->size(); ++i ) {
     oldInt.push_back( new xAOD::TrigRingerRings_v1() );
   }
   oldInt.setStore( oldObj );

   // The new uses v2
   xAOD::TrigRingerRingsContainer newInt;
   for (size_t i = 0; i < newObj->size(); ++i) {
      newInt.push_back( new xAOD::TrigRingerRings() );
   }

   newInt.setStore( newObj);

   for (size_t i = 0; i < newObj->size(); ++i) {

     std::vector<float> rings = oldInt[i]->rings();
     newInt[i]->setRings( rings );
     newInt[i]->auxdata<uint32_t>("RoIword") = oldInt[i]->RoIword();

   }

   return;
}

/// This function should never be called, as we are not supposed to convert
/// object before writing.
///
void xAODTrigRingerRingsAuxContainerCnv_v1::transToPers( const xAOD::TrigRingerRingsAuxContainer*,
                                                               xAOD::TrigRingerRingsAuxContainer_v1*,
                                                               MsgStream& log ) 
{

   log << MSG::ERROR << "Somebody called xAODTrigRingerRingsAuxContainerCnv_v1::transToPers" << endmsg;
   throw std::runtime_error( "Somebody called xAODTrigRingerRingsAuxContainerCnv_v1::"
                             "transToPers" );

   return;
}

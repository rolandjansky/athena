/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODJetTrigAuxContainerCnv_v1.cxx 800408 2017-03-13 09:28:39Z sschramm $

// System include(s):
#include <stdexcept>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Core EDM include(s):
#include "AthContainers/tools/copyAuxStoreThinned.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetTrigAuxContainer.h"
#include "xAODJet/versions/JetTrigAuxContainer_v1.h"
#include "AthLinks/ElementLink.h"
#include "xAODBase/IParticleContainer.h"

// Local include(s):
#include "xAODJetTrigAuxContainerCnv_v1.h"

#include <utility>
#include <algorithm>

/// Convenience macro for setting the level of output messages
#define MSGLVL MSG::VERBOSE

/// Another convenience macro for printing messages in the converter
#define ATH_MSG( MSG )                          \
   do {                                         \
      if( log.level() <= MSGLVL ) {             \
         log << MSGLVL << MSG << endmsg;        \
      }                                         \
   } while( 0 )

xAODJetTrigAuxContainerCnv_v1::xAODJetTrigAuxContainerCnv_v1()
  : T_AthenaPoolTPCnvBase< xAOD::JetTrigAuxContainer,
                           xAOD::JetTrigAuxContainer_v1 >() {

}

void xAODJetTrigAuxContainerCnv_v1::
persToTrans( const xAOD::JetTrigAuxContainer_v1* oldObj,
             xAOD::JetTrigAuxContainer* newObj,
             MsgStream& log ) {

   // Greet the user:
   ATH_MSG( "Converting xAOD::JetTrigAuxContainer_v1 to current version..." );

   // Clear the transient object:
   newObj->resize( 0 );

   // Copy the payload of the v1 object into the latest one by misusing
   // the thinning code a bit...
   SG::copyAuxStoreThinned( *oldObj, *newObj, 0 );

   //The old  uses v_
   xAOD::JetContainer oldInt;
   for( size_t i = 0; i < oldObj->size(); ++i ) {
     oldInt.push_back( new xAOD::Jet() );
   }
   oldInt.setStore( oldObj );
   
   xAOD::JetContainer newInt;
   for( size_t i = 0; i < newObj->size(); ++i ) {
     newInt.push_back( new xAOD::Jet() );
   }
   newInt.setStore( newObj );

   std::vector< std::vector< ElementLink< xAOD::IParticleContainer > > > GhostTrack;

   // Print what happened:
   ATH_MSG( "Converting xAOD::JetTrigAuxContainer_v1 to current version "
            "[OK]" );

   return;
}

/// This function should never be called, as we are not supposed to convert
/// object before writing.
///
void xAODJetTrigAuxContainerCnv_v1::
transToPers( const xAOD::JetTrigAuxContainer*,
             xAOD::JetTrigAuxContainer_v1*,
             MsgStream& log ) {

   log << MSG::ERROR
       << "Somebody called xAODJetTrigAuxContainerCnv_v1::transToPers"
       << endmsg;
   throw std::runtime_error( "Somebody called xAODJetTrigAuxContainerCnv_v1::"
                             "transToPers" );

   return;
}

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


// System include(s):
#include <stdexcept>
#include <array>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// EDM include(s):
#include "AthContainers/AuxTypeRegistry.h"
#include "xAODTrigL1Calo/RODHeaderContainer.h"
#include "xAODTrigL1Calo/versions/RODHeaderContainer_v1.h"

// Local include(s):
#include "xAODRODHeaderAuxContainerCnv_v1.h"

/// Convenience macro defining the converter's output message level
#define MSGLVL MSG::DEBUG

/// Convenience macro for printing messages in the converter
#define ATH_MSG( MSG )                          \
   do {                                         \
      if( log.level() <= MSGLVL ) {             \
         log << MSGLVL << MSG << endmsg;        \
      }                                         \
   } while( 0 )

xAODRODHeaderAuxContainerCnv_v1::xAODRODHeaderAuxContainerCnv_v1()
{
}

void xAODRODHeaderAuxContainerCnv_v1::
persToTrans( const xAOD::RODHeaderAuxContainer_v1* oldObj,
             xAOD::RODHeaderAuxContainer* newObj,
             MsgStream& log ) const {

   // Greet the user:
   ATH_MSG( "Converting xAOD::RODHeaderAuxContainer_v1 to current "
            "version..." );

   // Clear the transient object:
   newObj->resize( 0 );

   // Set up interface containers on top of the auxiliary ones:
   xAOD::RODHeaderContainer_v1 oldInt;
   for( size_t i = 0; i < oldObj->size(); ++i ) {
      oldInt.push_back( new xAOD::RODHeader_v1() );
   }
   oldInt.setStore( oldObj );
   xAOD::RODHeaderContainer newInt;
   newInt.setStore( newObj );

   // Loop over the interface objects, and do the conversion with their help:
   for( const xAOD::RODHeader_v1* oldRH : oldInt ) {

      // Create an object for the new container:
      xAOD::RODHeader* newRH = new xAOD::RODHeader();
      newInt.push_back( newRH );

      // Copy all the variables that the interface knows about:
      newRH->initialize( oldRH->version(), oldRH->sourceId(), oldRH->run(),
                         oldRH->lvl1Id(), oldRH->bcid(), oldRH->trigType(),
                         oldRH->detType(), oldRH->statusWords(),
                         oldRH->payloadSize() );

      // Helper objects to access the auxiliary IDs of the base variables:
      static const SG::AuxElement::TypelessConstAccessor accVersion( "version" );
      static const SG::AuxElement::TypelessConstAccessor accSourceId( "sourceId" );
      static const SG::AuxElement::TypelessConstAccessor accRun( "run" );
      static const SG::AuxElement::TypelessConstAccessor accLvl1Id( "lvl1Id" );
      static const SG::AuxElement::TypelessConstAccessor accBcid( "bcId" );
      static const SG::AuxElement::TypelessConstAccessor accTrigType( "trigType" );
      static const SG::AuxElement::TypelessConstAccessor accDetType( "detType" );
      static const SG::AuxElement::TypelessConstAccessor
         accStatusWords( "statusWords" );
      static const SG::AuxElement::TypelessConstAccessor
         accPayloadSize( "payloadSize" );
      static const std::array< const SG::AuxElement::TypelessConstAccessor*, 9 >
         knownVars{ { &accVersion, &accSourceId, &accRun, &accLvl1Id, &accBcid,
               &accTrigType, &accDetType, &accStatusWords, &accPayloadSize } };

      // The index of the objects in their respective containers:
      const size_t oindex = oldRH->index();
      const size_t nindex = newRH->index();

      // The auxiliary registry, used in the copying:
      SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();

      // Now copy over all the dynamic variables, which were not covered
      // by the previous lines:
      for( SG::auxid_t auxid : oldObj->getAuxIDs() ) {

         // Ignore the ones that we already copied:
         bool ignore = false;
         for( const auto * acc : knownVars ) {
            if( auxid == acc->auxid() ) {
               ignore = true;
               break;
            }
         }
         if( ignore ) {
            continue;
         }

         // Copy the variable:
         void* dst = newInt.getDataArray( auxid );
         // With newer versions of AthContainers we'll be able to replace
         // this with:
         //
         //    const void* src = oldInt.getDataArrayAllowMissing( auxid );
         //
         // , and we won't even need to use a private function for it. But
         // in order to make the code work in 20.1.X.Y, this ugly hack needs
         // to be done to make the code safe.
         const void* src = nullptr;
         try {
            const xAOD::RODHeaderContainer_v1& helper = oldInt;
            src = helper.getDataArray( auxid );
         } catch( const SG::ExcBadAuxVar& ) {
            // This can happen with corrupt input files. In this case just
            // fill dummy values into the new object:
            r.clear( auxid, dst, nindex );
            continue;
         }
         r.copy( auxid, dst, nindex, src, oindex );
      }
   }

   // Tell the user what happened:
   ATH_MSG( "Converting xAOD::RODHeaderAuxContainer_v1 to current version "
            "[OK]" );

   // Return gracefully:
   return;
}

/// This function is never supposed to be called...
void xAODRODHeaderAuxContainerCnv_v1::
transToPers( const xAOD::RODHeaderAuxContainer*,
             xAOD::RODHeaderAuxContainer_v1*,
             MsgStream& log ) const {

   log << MSG::ERROR << "xAODRODHeaderAuxContainerCnv_v1::transToPers got "
       << "called" << endmsg;
   throw std::runtime_error( "xAODRODHeaderAuxContainerCnv_v1::transToPers "
                             "got called" );

   return;
}

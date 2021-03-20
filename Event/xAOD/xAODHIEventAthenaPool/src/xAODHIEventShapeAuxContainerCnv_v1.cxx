/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODHIEventShapeAuxContainerCnv_v1.cxx 694377 2015-09-11 13:50:57Z krasznaa $

// System include(s):
#include <stdexcept>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// EDM include(s):
#include "AthContainers/AuxTypeRegistry.h"
#include "xAODHIEvent/HIEventShapeContainer.h"
#include "xAODHIEvent/versions/HIEventShapeContainer_v1.h"

// Local include(s):
#include "xAODHIEventShapeAuxContainerCnv_v1.h"

/// Convenience macro defining the converter's output message level
#define MSGLVL MSG::DEBUG

/// Convenience macro for printing messages in the converter
#define ATH_MSG( MSG )                          \
   do {                                         \
      if( log.level() <= MSGLVL ) {             \
         log << MSGLVL << MSG << endmsg;        \
      }                                         \
   } while( 0 )

xAODHIEventShapeAuxContainerCnv_v1::xAODHIEventShapeAuxContainerCnv_v1()
   : T_AthenaPoolTPCnvBase< xAOD::HIEventShapeAuxContainer,
                            xAOD::HIEventShapeAuxContainer_v1 >() {

}

void xAODHIEventShapeAuxContainerCnv_v1::
persToTrans( const xAOD::HIEventShapeAuxContainer_v1* oldObj,
             xAOD::HIEventShapeAuxContainer* newObj,
             MsgStream& log ) {

   // Greet the user:
   ATH_MSG( "Converting xAOD::HIEventShapeAuxContainer_v1 to current "
            "version..." );

   // Clear the transient object:
   newObj->resize( 0 );

   // Set up interface containers on top of the auxiliary ones:
   xAOD::HIEventShapeContainer_v1 oldInt;
   for( size_t i = 0; i < oldObj->size(); ++i ) {
      oldInt.push_back( new xAOD::HIEventShape_v1() );
   }
   oldInt.setStore( oldObj );
   xAOD::HIEventShapeContainer newInt;
   newInt.setStore( newObj );

   // Loop over the interface objects, and do the conversion with their help:
   for( const xAOD::HIEventShape_v1* oldES : oldInt ) {

      // Create an object for the new container:
      xAOD::HIEventShape* newES = new xAOD::HIEventShape();
      newInt.push_back( newES );

      // Copy all the variables that the interface knows about:
      newES->setEt( oldES->Et() );
      newES->setArea( oldES->area() );
      newES->setRho( oldES->rho() );
      newES->setNCells( oldES->nCells() );

      newES->setEtCos( oldES->Et_cos() );
      newES->setEtSin( oldES->Et_sin() );

      newES->setEtaMin( oldES->etaMin() );
      newES->setEtaMax( oldES->etaMax() );
      newES->setLayer( oldES->layer() );

      // Helper objects to access the auxiliary IDs of the base variables:
      static const SG::AuxElement::TypelessConstAccessor accEt( "Et" );
      static const SG::AuxElement::TypelessConstAccessor accArea( "area" );
      static const SG::AuxElement::TypelessConstAccessor accRho( "rho" );
      static const SG::AuxElement::TypelessConstAccessor accEt_cos( "Et_cos" );
      static const SG::AuxElement::TypelessConstAccessor accEt_sin( "Et_sin" );
      static const SG::AuxElement::TypelessConstAccessor accEtaMin( "etaMin" );
      static const SG::AuxElement::TypelessConstAccessor accEtaMax( "etaMax" );
      static const SG::AuxElement::TypelessConstAccessor accLayer( "layer" );
      static const SG::AuxElement::TypelessConstAccessor accNCells( "nCells" );
      static const std::array< const SG::AuxElement::TypelessConstAccessor*, 9 >
         knownVars{ { &accEt, &accArea, &accRho, &accEt_cos, &accEt_sin,
               &accEtaMin, &accEtaMax, &accLayer, &accNCells } };

      // The index of the objects in their respective containers:
      const size_t oindex = oldES->index();
      const size_t nindex = newES->index();

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
         const void* src = oldInt.getDataArrayAllowMissing( auxid );
         if (!src) {
            // This can happen with corrupt input files. In this case just
            // fill dummy values into the new object:
            r.clear( auxid, dst, nindex );
            continue;
         }
         r.copy( auxid, dst, nindex, src, oindex );
      }
   }

   // Tell the user what happened:
   ATH_MSG( "Converting xAOD::HIEventShapeAuxContainer_v1 to current version "
            "[OK]" );

   // Return gracefully:
   return;
}

/// This function is never supposed to be called...
void xAODHIEventShapeAuxContainerCnv_v1::
transToPers( const xAOD::HIEventShapeAuxContainer*,
             xAOD::HIEventShapeAuxContainer_v1*,
             MsgStream& log ) {

   log << MSG::ERROR << "xAODHIEventShapeAuxContainerCnv_v1::transToPers got "
       << "called" << endmsg;
   throw std::runtime_error( "xAODHIEventShapeAuxContainerCnv_v1::transToPers "
                             "got called" );

   return;
}

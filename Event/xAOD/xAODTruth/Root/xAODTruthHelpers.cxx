/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTruthHelpers.cxx 668406 2015-05-19 15:32:15Z krasznaa $

// Core EDM include(s):
#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"

// xAOD include(s):
#include "xAODBase/IParticle.h"

// Local include(s):
#include "xAODTruth/xAODTruthHelpers.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"

namespace xAOD {

   namespace TruthHelpers {

      /// @param p The particle that we find the associated truth particle for
      /// @returns A pointer to the associated truth particle if available,
      ///          or a null pointer if not
      ///
      const xAOD::TruthParticle* getTruthParticle( const xAOD::IParticle& p ) {

         /// A convenience type declaration
         typedef ElementLink< xAOD::TruthParticleContainer > Link_t;

         /// A static accessor for the information
         static const SG::AuxElement::ConstAccessor< Link_t > acc( "truthParticleLink" );

         // Check if such a link exists on the object:
         if( ! acc.isAvailable( p ) ) {
            return 0;
         }

         // Get the link:
         const Link_t& link = acc( p );

         // Check if the link is valid:
         if( ! link.isValid() ) {
            return 0;
         }

         // Everything has passed, let's return the pointer:
         return *link;
      }

      /// @param p The particle that we want to find the truth type of
      /// @returns 0 if the truth type is not available, or the truth type
      ///          determined by MCTruthClassifier, if it is
      ///
      int getParticleTruthType( const xAOD::IParticle& p ) {

         /// A static accessor for the information
         static const SG::AuxElement::ConstAccessor< int > acc( "truthType" );

         // Check if such a variable exists on the object:
         if( ! acc.isAvailable( p ) ) {
            return 0;
         }

         // Let's return the value:
         return acc( p );
      }

      /// @param p The particle that we want to find the truth origin of
      /// @returns 0 if the truth origin is not available, or the truth origin
      ///          determined by MCTruthClassifier, if it is
      ///
      int getParticleTruthOrigin( const xAOD::IParticle& p ) {

         /// A static accessor for the information
         static const SG::AuxElement::ConstAccessor< int > acc( "truthOrigin" );

         // Check if such a variable exists on the object:
         if( ! acc.isAvailable( p ) ) {
            return 0;
         }

         // Let's return the value:
         return acc( p );
      }

   } // namespace TruthHelpers

} // namespace xAOD

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

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
         static SG::AuxElement::ConstAccessor< Link_t > acc( "truthParticleLink" );

         // Check if such a link exists on the object:
         if( acc.isAvailable( p ) ) {

            // Get the link:
            const Link_t& link = acc( p );

            // Check if the link is valid:
            if( link.isValid() ) {
               // Everything has passed, let's return the pointer:
               return *link;
            }

         }

         /// Alternative name - this may work in case of TRUTH3 / small truth collections
         static SG::AuxElement::ConstAccessor< Link_t > acc_alt( "TruthLink" );

         // Check if such a link exists on the object:
         if( acc_alt.isAvailable( p ) ) {

            // Get the link:
            const Link_t& link_alt = acc_alt( p );

            // Check if the link is valid:
            if( link_alt.isValid() ) {
               // Everything has passed, let's return the pointer:
               return *link_alt;
            }

         }

         // Something has failed. Return a nullptr
         return nullptr;
      }


      /// @param particle The truth particle we want to get the final copy of
      /// @returns A pointer to the final truth particle copy if available,
      ///          or pointer to the same particle if no copies done.
      ///
      const xAOD::TruthParticle* getFinalCopy( const xAOD::TruthParticle& particle ) {

         // Loop over the children
         for( size_t i{}; i < particle.nChildren(); ++i ) {

            // Check if particle pointer exists and it is the same particle (same PDG ID)
            if( particle.child( i ) != nullptr && particle.child( i )->pdgId() == particle.pdgId() ) {
               // Recursively check again
               // It is fine to return when we find first copy candidate as there can only be one
               return getFinalCopy( *particle.child( i ) );
            }
         }

         // Return the same particle if no copies found
         return &particle;
      }


      /// @param p The particle that we want to find the truth type of
      /// @returns 0 if the truth type is not available, or the truth type
      ///          determined by MCTruthClassifier, if it is
      ///
      int getParticleTruthType( const xAOD::IParticle& p ) {

         /// A static accessor for the information
         static SG::AuxElement::ConstAccessor< int > acc( "truthType" );

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
         static SG::AuxElement::ConstAccessor< int > acc( "truthOrigin" );

         // Check if such a variable exists on the object:
         if( ! acc.isAvailable( p ) ) {
            return 0;
         }

         // Let's return the value:
         return acc( p );
      }

   } // namespace TruthHelpers

} // namespace xAOD

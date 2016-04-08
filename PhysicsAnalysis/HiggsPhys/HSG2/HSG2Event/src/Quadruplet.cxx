/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Quadruplet.cxx 485033 2012-02-24 16:54:50Z krasznaa $

// EDM include(s):
#include "VxVertex/VxCandidate.h"
#include "ParticleEvent/CompositeParticle.h"

// Local include(s):
#include "../HSG2Event/Quadruplet.h"

namespace HSG2 {

   Quadruplet::Quadruplet()
      : m_particle(), m_vertex(), m_finalState( fs_undef ) {

   }

   /**
    * @param particle The CombinedParticle holding the 4 leptons
    * @param vertex The vertex reconstructed from the lepton tracks
    * @param fs The final state type
    */
   Quadruplet::Quadruplet( const ElementLink< CompositeParticleContainer >& particle,
                           const ElementLink< VxContainer >& vertex,
                           FinalState fs )
      : m_particle( particle ), m_vertex( vertex ), m_finalState( fs ) {

   }

   /**
    * @returns The CombinedParticle holding the 4 leptons
    */
   const ElementLink< CompositeParticleContainer >& Quadruplet::particle() const {

      return m_particle;
   }

   /**
    * @returns The vertex reconstructed from the lepton tracks
    */
   const ElementLink< VxContainer >& Quadruplet::vertex() const {

      return m_vertex;
   }

   /**
    * @returns The final state type
    */
   FinalState Quadruplet::finalState() const {

      return m_finalState;
   }

   /**
    * The function only does anything if the ElementLink given to it is valid. If not,
    * it just ignores the passed argument.
    *
    * @param particle The CombinedParticle holding the 4 leptons
    */
   void Quadruplet::setParticle( const ElementLink< CompositeParticleContainer >& particle ) {

      if( particle.isValid() ) {
         m_particle = particle;
      }

      return;
   }

   /**
    * The function only does anything if the ElementLink given to it is valid. If not,
    * it just ignores the passed argument.
    *
    * @param vertex The vertex reconstructed from the lepton tracks
    */
   void Quadruplet::setVertex( const ElementLink< VxContainer>& vertex ) {

      if( vertex.isValid() ) {
         m_vertex = vertex;
      }

      return;
   }

   /**
    * @param fs The final state type
    */
   void Quadruplet::setFinalState( FinalState fs ) {

      m_finalState = fs;
      return;
   }

} // namespace HSG2

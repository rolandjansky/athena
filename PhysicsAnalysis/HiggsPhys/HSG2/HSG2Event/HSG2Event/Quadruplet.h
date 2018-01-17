// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Quadruplet.h 485033 2012-02-24 16:54:50Z krasznaa $
#ifndef HSG2Event_Quadruplet_H
#define HSG2Event_Quadruplet_H

// EDM include(s):
#include "AthLinks/ElementLink.h"
#include "VxVertex/VxContainer.h"
#include "ParticleEvent/CompositeParticleContainer.h"

/**
 *  @short Namespace for HSG2 related classes
 *
 *         To play nice, we put (almost) all the classes used in the
 *         Athena codes of the HSG2 (H->ZZ) group into this namespace.
 *
 * @author Kirill Prokofiev <Kirill.Prokofiev@cern.ch>
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 485033 $
 * $Date: 2012-02-24 17:54:50 +0100 (Fri, 24 Feb 2012) $
 */
namespace HSG2 {

   /**
    * @short Type definition used for 4-lepton quadruplets
    *
    *        It might look better inside the Quadruplet class, but
    *        oh well... It's just a simple listing of 4-lepton
    *        types that the analysis uses.
    */
   enum FinalState {
      fs_4mu   = 0, ///< 4 muon final state
      fs_4e    = 1, ///< 4 electron final state
      fs_2e2mu = 2, ///< 2 electron - 2 muon final state
      fs_undef = 100 ///< Undefined final state, needed for technical reasons
   }; // enum FinalState
    
   /**
    *  @class HSG2::Quadruplet.h
    *  @short Top level object defining lepton quadruplets
    *
    *         Multi-lepton objects can in principle simply be created using
    *         the CompositeParticle class on its own. The reason why we need
    *         this class is that we want to associate the 4-lepton composite
    *         objects with the vertex objects that were reconstructed using
    *         their tracks.
    *                         
    * @author Kirill Prokofiev <Kirill.Prokofiev@cern.ch>
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 485033 $
    * $Date: 2012-02-24 17:54:50 +0100 (Fri, 24 Feb 2012) $ 
    */
   class Quadruplet {

   public:
      /// Default constructor needed for persistency 
      Quadruplet();
      /// Constructor specifying all properties of the object
      Quadruplet( const ElementLink< CompositeParticleContainer >& particle,
                  const ElementLink< VxContainer >& vertex,
                  FinalState fs );

      /// Get the composite particle describing this quadruplet
      const ElementLink< CompositeParticleContainer >& particle() const;
      /// Get the vertex created from the lepton tracks
      const ElementLink< VxContainer >& vertex() const;
      /// Get the type of the quadruplet
      FinalState finalState() const;

      /// Set the composite particle describing this quadruplet
      void setParticle( const ElementLink< CompositeParticleContainer >& comb );
      /// Set the vertex created from the lepton tracks
      void setVertex( const ElementLink< VxContainer >& vertex );
      /// Set the type of the quadruplet
      void setFinalState( FinalState fs );

   protected:
      /// Link to the composite particle holding the 4 leptons
      ElementLink< CompositeParticleContainer > m_particle;
      /// Link to the vertex reconstructed from the leptons
      ElementLink< VxContainer > m_vertex;
      /// Decoded final state enumeration to be used during event processing
      FinalState m_finalState;

   }; // class Quadruplet

}//end of namespace definitions

#endif /// HSG2Event_Quadruplet_H

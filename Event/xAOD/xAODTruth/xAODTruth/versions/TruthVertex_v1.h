// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TruthVertex_v1.h 624338 2014-10-27 15:08:55Z krasznaa $
#ifndef XAODTRUTH_VERSIONS_TRUTHVERTEX_V1_H
#define XAODTRUTH_VERSIONS_TRUTHVERTEX_V1_H

// ROOT include(s):
#include <TLorentzVector.h>

// EDM include(s):
#include "AthContainers/AuxElement.h"
#include "AthLinks/ElementLink.h"

// xAOD include(s):
#include "xAODBase/ObjectType.h"

// Local include(s):
#include "xAODTruth/TruthParticleContainerFwd.h"

namespace xAOD {

   /// Class describing a truth vertex in the MC record
   ///
   /// The xAOD truth record mimicks the structure of HepMC. Truth particles
   /// and truth vertices form a decay tree. Where each truth particle is
   /// connected to a single production and decay vertex. And each vertex is
   /// connected to one or more incoming, and one or more outgoing particles.
   ///
   /// @author Andy Buckley <Andy.Buckley@cern.ch>
   ///
   /// $Revision: 624338 $
   /// $Date: 2014-10-27 16:08:55 +0100 (Mon, 27 Oct 2014) $
   ///
   class TruthVertex_v1 : public SG::AuxElement {

   public:
      /// Default constructor
      TruthVertex_v1();

      /// @name Functions identifying the vertex in the MC record
      /// @{

      /// Set vertex ID code
      void setId( int value );
      /// Vertex ID code
      int id() const;

      /// Set barcode
      void setBarcode( int value );
      /// Barcode
      int barcode() const;

      /// @}

      /// @name Links to the particles associated with this vertex
      /// @{

      /// Type of one truth particle link
      typedef ElementLink< TruthParticleContainer > TPLink_t;
      /// Type used to save the links to incoming and outgoing particles
      typedef std::vector< TPLink_t > TPLinks_t;

      /// Get all the incoming particles
      const TPLinks_t& incomingParticleLinks() const;
      /// Set all the incoming particles
      void setIncomingParticleLinks( const TPLinks_t& links );
      /// Get the number of incoming particles
      size_t nIncomingParticles() const;
      /// Get one of the incoming particles
      const TruthParticle_v1* incomingParticle( size_t index ) const;
      /// Add one incoming particle
      void addIncomingParticleLink( const TPLink_t& link );
      /// Remove all incoming particles
      void clearIncomingParticleLinks();

      /// Get all the outgoing particles
      const TPLinks_t& outgoingParticleLinks() const;
      /// Set all the outgoing particles
      void setOutgoingParticleLinks( const TPLinks_t& links );
      /// Get the number of outgoing particles
      size_t nOutgoingParticles() const;
      /// Get one of the outgoing particles
      const TruthParticle_v1* outgoingParticle( size_t index ) const;
      /// Add one outgoing particle
      void addOutgoingParticleLink( const TPLink_t& link );
      /// Remove all outgoing particles
      void clearOutgoingParticleLinks();

      /// @}

      /// @name Vertex position functions
      /// @{

      /// Vertex x displacement
      float x() const;
      /// Set the x displacement of the vertex
      void setX( float value );

      /// Vertex y displacement
      float y() const;
      /// Set the y displacement of the vertex
      void setY( float value );

      /// Vertex longitudinal distance along the beam line form the origin
      float z() const;
      /// Set the vertex's longitudinal distance from the origin
      void setZ( float value );

      /// Vertex transverse distance from the beam line
      float perp() const;
      /// Vertex pseudorapidity
      float eta() const;
      /// Vertex azimuthal angle
      float phi() const;

      /// Vertex time
      float t() const;
      /// Set the vertex time
      void setT( float value );

      /// The 4-vector type
      typedef TLorentzVector FourVec_t;

      /// The full 4-vector of the vertex
      const FourVec_t& v4() const;

      /// @}

      /// The type of the object as a simple enumeration
      Type::ObjectType type() const;

      /// Function making sure that the object is ready for persistification
      void toPersistent();

   private:
      /// Cached four vector
      mutable FourVec_t m_v4;
      /// Flag showing whether the four-vector is cached
      mutable bool m_v4Cached;

   }; // class TruthVertex_v1

} // namespace xAOD

// Declare the inheritance of the type to StoreGate:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::TruthVertex_v1, SG::AuxElement );

#endif // XAODTRUTH_VERSIONS_TRUTHVERTEX_V1_H

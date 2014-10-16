// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TruthEventBase_v1.h 622193 2014-10-16 16:08:34Z krasznaa $
#ifndef XAODTRUTH_VERSIONS_TRUTHEVENTBASE_V1_H
#define XAODTRUTH_VERSIONS_TRUTHEVENTBASE_V1_H

// System include(s):
#include <vector>

// EDM include(s):
#include "AthContainers/AuxElement.h"
#include "AthLinks/ElementLink.h"
#include "xAODBase/ObjectType.h"

// Local include(s):
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertexContainer.h"

namespace xAOD {

   /// Base class describing a pile-up or signal truth event in the MC record
   ///
   /// Provides access to particles and vertices, but no extra event info since
   /// this is different for signal and pile-up events.
   ///
   /// @author Andy Buckley <Andy.Buckley@cern.ch>
   /// @author Jovan Mitrevski <Jovan.Mitrevski@cern.ch>
   ///
   /// $Revision: 622193 $
   /// $Date: 2014-10-16 18:08:34 +0200 (Thu, 16 Oct 2014) $
   ///
   class TruthEventBase_v1 : public SG::AuxElement {

   public:
      /// Default constructor
      TruthEventBase_v1();
      /// Virtual destructor, to make vtable happy
      virtual ~TruthEventBase_v1() {}

      /// @name Access to all the particles associated with the event
      /// @{

      /// Type of a single truth particle link
      typedef ElementLink< TruthParticleContainer > TruthParticleLink_t;
      /// Type of the truth particle links vector
      typedef std::vector< TruthParticleLink_t > TruthParticleLinks_t;

      /// Get all the truth particles
      const TruthParticleLinks_t& truthParticleLinks() const;
      /// Set all the truth particles
      void setTruthParticleLinks( const TruthParticleLinks_t& plinks );
      /// Get the number of truth particles
      size_t nTruthParticles() const;
      /// Get the link to one of the truth particles
      const TruthParticleLink_t& truthParticleLink( size_t index ) const;
      /// Get a pointer to one of the truth particles
      const TruthParticle* truthParticle( size_t index ) const;
      /// Add one truth particle
      void addTruthParticleLink( const TruthParticleLink_t& plink );
      /// Remove all truth particles
      void clearTruthParticleLinks();

      /// @}

      /// @name Access to all the vertices associated with the event
      /// @{

      /// Type of a single truth vertex link
      typedef ElementLink< TruthVertexContainer > TruthVertexLink_t;
      /// Type of the truth particle links vector
      typedef std::vector< TruthVertexLink_t > TruthVertexLinks_t;

      /// Get all the truth vertices
      const TruthVertexLinks_t& truthVertexLinks() const;
      /// Set all the truth vertices
      void setTruthVertexLinks( const TruthVertexLinks_t& links );
      /// Get the number of truth vertices
      size_t nTruthVertices() const;
      /// Get the link to one of the truth vertices
      const TruthVertexLink_t& truthVertexLink( size_t index ) const;
      /// Get a pointer to one of the truth vertices
      const TruthVertex* truthVertex( size_t index ) const;
      /// Add one truth vertex
      void addTruthVertexLink( const TruthVertexLink_t& vlink );
      /// Remove all truth vertices
      void clearTruthVertexLinks();

      /// @}

      /// The type of the object as a simple enumeration
      virtual Type::ObjectType type() const = 0;

      /// Function making sure that the object is ready for persistification
      void toPersistent();

   }; // class TruthEventBase_v1

} // namespace xAOD

// Declare the base class of TruthEventBase_v1 to StoreGate:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::TruthEventBase_v1, SG::AuxElement );

#endif // XAODTRUTH_VERSIONS_TRUTHEVENTBASE_V1_H

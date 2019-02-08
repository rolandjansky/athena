// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKING_VERSIONS_VERTEX_V1_H
#define XAODTRACKING_VERSIONS_VERTEX_V1_H

// System include(s):
#include <vector>

// Core include(s):
#include "AthContainers/AuxElement.h"
#include "AthLinks/ElementLink.h"

// EDM include(s):
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"
#ifndef XAOD_STANDALONE
#ifndef XAOD_MANACORE
#  include "VxVertex/VxTrackAtVertex.h"
#endif // not XAOD_MANACORE
#endif // not XAOD_STANDALONE

// xAOD include(s):
#include "xAODTracking/TrackingPrimitives.h"
#include "xAODTracking/TrackParticleContainerFwd.h"
#include "xAODTracking/NeutralParticleContainer.h"
#include "xAODBase/ObjectType.h"

// Local include(s):
#include "xAODTracking/TrackingPrimitives.h"

//MT CachedValue
#include "CxxUtils/CachedValue.h"

namespace xAOD {

   /// Class describing a Vertex.
   ///
   /// @author Kirill Prokofiev <Kirill.Prokofev@cern.ch>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   /// @author Ruslan Mashinistov <Ruslan.Mashinistov@cern.ch>
   /// @nosubgrouping
   ///
   class Vertex_v1 : public SG::AuxElement {

   public:
      /// Default constructor
      Vertex_v1();

      /// Copy constructor
      Vertex_v1(const Vertex_v1& other);

      /// Assignment operator. This can involve creating and copying an Auxilary store, and so should be used sparingly.
      Vertex_v1& operator=(const Vertex_v1& tp );

      /// A little helper function for identifying the type in template code
      Type::ObjectType type() const;

      /// Returns the x position
      float x() const;
      /// Sets the x position
      void setX( float value );
      /// Returns the y position
      float y() const;
      /// Sets the y position
      void setY( float value );
      /// Returns the z position
      float z() const;
      /// Sets the z position
      void setZ( float value );

      /// Returns the covariance matrix as a simple vector of values
      const std::vector< float >& covariance() const;
      /// Sets the covariance matrix as a simple vector of values
      void setCovariance( const std::vector< float >& value );

      /// Returns the 3-pos
      const Amg::Vector3D& position() const;
      /// Sets the 3-position
      void setPosition( const Amg::Vector3D& position );

      /// Returns the vertex covariance matrix
      const AmgSymMatrix(3)& covariancePosition() const;
      /// Sets the vertex covariance matrix
      void setCovariancePosition( const AmgSymMatrix(3)& covariancePosition );

      /// @name Fit quality functions
      /// @{

      /// Returns the @f$ \chi^2 @f$ of the vertex fit as float.
      float chiSquared() const;
      /// Returns the number of degrees of freedom of the vertex fit as float.
      float numberDoF() const;   
      /// Set the 'Fit Quality' information.
      void setFitQuality( float chiSquared, float numberDoF );

      /// @}

      /// The type of the vertex
      VxType::VertexType vertexType() const;
      /// Set the type of the vertex
      void setVertexType( VxType::VertexType vType );

#if ( ! defined(XAOD_STANDALONE) ) && ( ! defined(XAOD_MANACORE) )
      /// Non-const access to the VxTrackAtVertex vector
      std::vector< Trk::VxTrackAtVertex >& vxTrackAtVertex();
      /// Const access to the vector of tracks fitted to the vertex (may not exist!)
      const std::vector< Trk::VxTrackAtVertex >& vxTrackAtVertex() const;
      /// Check if VxTrackAtVertices are attached to the object
      bool vxTrackAtVertexAvailable() const;
#endif // not XAOD_STANDALONE and not XAOD_MANACORE

      /// @name Track particle contents operations
      /// @{

      /// Type for the associated track particles
      typedef std::vector< ElementLink< xAOD::TrackParticleContainer > >
      TrackParticleLinks_t;
      /// Get all the particles associated with the vertex
      const TrackParticleLinks_t& trackParticleLinks() const;
      /// Set all track particle links at once
      void setTrackParticleLinks( const TrackParticleLinks_t& trackParticles );

      /// Get all the track weights
      const std::vector< float >& trackWeights() const;
      /// Set all the track weights
      void setTrackWeights( const std::vector< float >& weights );


      /// Type for the associated neutral particles
      typedef std::vector< ElementLink< xAOD::NeutralParticleContainer > > NeutralParticleLinks_t;
      /// Get all the particles associated with the vertex
      const NeutralParticleLinks_t& neutralParticleLinks() const;
      /// Set all neutral particle links at once
      void setNeutralParticleLinks( const NeutralParticleLinks_t& neutralParticles );

      /// Get all the neutral weights
      const std::vector< float >& neutralWeights() const;
      /// Set all the neutral weights
      void setNeutralWeights( const std::vector< float >& weights );


      /// Get the pointer to a given track that was used in vertex reco.
      const TrackParticle* trackParticle( size_t i ) const;
      /// Get the weight of a given track in the vertex reconstruction
      float trackWeight( size_t i ) const;
      /// Get the number of tracks associated with this vertex
      size_t nTrackParticles() const;


      /// Get the pointer to a given neutral that was used in vertex reco.
      const NeutralParticle* neutralParticle( size_t i ) const;
      /// Get the weight of a given neutral in the vertex reconstruction
      float neutralWeight( size_t i ) const;
      /// Get the number of neutrals associated with this vertex
      size_t nNeutralParticles() const;


      /// Add a new track to the vertex
      void addTrackAtVertex( const ElementLink< TrackParticleContainer >& tr,
                             float weight = 1.0 );

      /// Add a new neutral to the vertex
      void addNeutralAtVertex( const ElementLink< NeutralParticleContainer >& tr,
                             float weight = 1.0 );

      /// Remove all tracks from the vertex
      void clearTracks();

      /// Remove all neutrals from the vertex
      void clearNeutrals();


      /// @}

      /// Reset the internal cache of the object
      void resetCache();

   private:
      /// Cached position of the vertex
      CxxUtils::CachedValue<Amg::Vector3D> m_position;
      /// Cached covariance of the vertex
      CxxUtils::CachedValue<AmgSymMatrix(3)> m_covariance;

   }; // end of the Vertex_v1 class definitions

} // end of the xAOD namespace

#include "xAODCore/BaseInfo.h"
SG_BASE (xAOD::Vertex_v1, SG::AuxElement);

#endif // XAODTRACKING_VERSIONS_VERTEX_V1_H

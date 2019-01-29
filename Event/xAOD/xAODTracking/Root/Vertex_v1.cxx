/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


// System include(s):
#include <cmath>

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

// Local include(s):
#include "xAODTracking/versions/Vertex_v1.h"
#include "xAODTracking/TrackParticleContainer.h"

namespace xAOD {

   Vertex_v1::Vertex_v1()
      : SG::AuxElement(),
        m_position(), 
        m_covariance() {

   }

   Vertex_v1::Vertex_v1( const Vertex_v1& other ) 
      : SG::AuxElement(),
        m_position( other.m_position ),
        m_covariance( other.m_covariance ){
      //copy aux store content (only the stuffs already loaded in memory!)
      this->makePrivateStore( other );
   }

   Vertex_v1& Vertex_v1::operator=( const Vertex_v1& tp ) {

      // Check if anything needs to be done:
      if( this == &tp ) {
         return *this;
      }

      // If the object doesn't have an auxiliary store yet, make a private
      // one for it:
      if( ( ! hasStore() ) && ( ! container() ) ) {
         makePrivateStore();
      }

      // Invalidate the cache of the object:
      resetCache();

      // Now let the base class copy the auxiliary contents:
      SG::AuxElement::operator=( tp );

      // Return the object:
      return *this;
   }

   Type::ObjectType Vertex_v1::type() const {

      return Type::Vertex;
   }

   /////////////////////////////////////////////////////////////////////////////
   //
   //                Implementation of the primitive accessors

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( Vertex_v1, float, x, setX )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( Vertex_v1, float, y, setY )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( Vertex_v1, float, z, setZ )

   AUXSTORE_OBJECT_SETTER_AND_GETTER( Vertex_v1, std::vector< float >,
                                      covariance, setCovariance )

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //                  Implementation of the Eigen functions
   //

   const Amg::Vector3D& Vertex_v1::position() const {

      // Cache the position if necessary:
      if( ! m_position.isValid() ) {
         Amg::Vector3D tmpPosition;
         tmpPosition( 0 ) = x();
         tmpPosition( 1 ) = y();
         tmpPosition( 2 ) = z();
         m_position.set(tmpPosition);
      }

      // Return the object:
      return *(m_position.ptr());
   }

   void Vertex_v1::setPosition( const Amg::Vector3D& position ) {

      // Set the primitive variables using the simple functions:
      setX( position( 0 ) );
      setY( position( 1 ) );
      setZ( position( 2 ) );
      // Reset the cache
      m_position.reset();
      return;
   }

   const AmgSymMatrix(3)& Vertex_v1::covariancePosition() const {

      // Cache the covariance matrix if necessary:
      if( ! m_covariance.isValid() ) {
         // The matrix is now cached:
        AmgSymMatrix(3) tmpCovariance;
	      Amg::expand(covariance().begin(),covariance().end(),tmpCovariance);
	      m_covariance.set(tmpCovariance);
      }
      // Return the cached object:
      return *(m_covariance.ptr());
   }

   void Vertex_v1::setCovariancePosition( const AmgSymMatrix(3)& cov ) {

     // The to-be persistent variable:
     std::vector< float > vec;
     Amg::compress(cov,vec);

     // Set the persistent variable:
     setCovariance( vec );
     m_covariance.reset();
     return;
   }

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //            Implementation of the fit quality functions
   //

   AUXSTORE_PRIMITIVE_GETTER( Vertex_v1, float, chiSquared )
   AUXSTORE_PRIMITIVE_GETTER( Vertex_v1, float, numberDoF )

   void Vertex_v1::setFitQuality( float chiSquared, float numberDoF ) {

      static const Accessor< float > acc1( "chiSquared" );
      static const Accessor< float > acc2( "numberDoF" );
      acc1( *this ) = chiSquared;
      acc2( *this ) = numberDoF;

      return;
   }

   //
   /////////////////////////////////////////////////////////////////////////////

   AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( Vertex_v1, short, VxType::VertexType,
                                        vertexType )
   AUXSTORE_PRIMITIVE_SETTER_WITH_CAST( Vertex_v1, short, VxType::VertexType,
                                        vertexType, setVertexType )

#if ( ! defined(XAOD_STANDALONE) ) && ( ! defined(XAOD_MANACORE) )
   /// Helper object for implementing the vxTrackAtVertex functions
   static const SG::AuxElement::Accessor< std::vector< Trk::VxTrackAtVertex > >
   vxVertAcc( "vxTrackAtVertex" );

   /// This function can be used to attach an Athena-only, reconstruction
   /// specific information to the object. The data is attached as a
   /// dynamic auxiliary property. And it needs to be explicitly disabled
   /// for writing out, as we don't have a valid dictionary for it. Not to
   /// mention that these objects are very large.
   ///
   /// @returns A modifiable object, creating it if necessary
   ///
   std::vector< Trk::VxTrackAtVertex >& Vertex_v1::vxTrackAtVertex() {

      return vxVertAcc( *this );
   }

   /// This function can be used to access the Athena-only, reconstruction
   /// specific decoration on the object.
   ///
   /// Keep in mind that this decoration is not available for most of the
   /// vertices. When calling this function on an object that doesn't have this
   /// decoration, an exception will be thrown. Use the
   /// vxTrackAtVertexAvailable() function to check if this function will
   /// succeed or not.
   ///
   /// @returns A constant reference to the decoration
   ///
   const std::vector< Trk::VxTrackAtVertex >&
   Vertex_v1::vxTrackAtVertex() const {

      return vxVertAcc( *this );
   }

   /// This function can be used to check whether this Athena-only,
   /// reconstruction-specific decoration is available on the object. In
   /// most cases it isn't.
   ///
   /// @returns The availablility of the decoration
   ///
   bool Vertex_v1::vxTrackAtVertexAvailable() const {

      return vxVertAcc.isAvailable( *this );
   }
#endif // not XAOD_STANDALONE and not XAOD_MANACORE

   /////////////////////////////////////////////////////////////////////////////
   //
   //         Implementation of the track particle handling functions
   //

   /// Accessor for the track links
   static const SG::AuxElement::Accessor< Vertex_v1::TrackParticleLinks_t >
      trackAcc( "trackParticleLinks" );
   /// Accessor for the track weights
   static const SG::AuxElement::Accessor< std::vector< float > >
      weightTrackAcc( "trackWeights" );

   /// Accessor for the neutral links
   static const SG::AuxElement::Accessor< Vertex_v1::NeutralParticleLinks_t >
      neutralAcc( "neutralParticleLinks" );
   /// Accessor for the neutral weights
   static const SG::AuxElement::Accessor< std::vector< float > >
      weightNeutralAcc( "neutralWeights" );

   AUXSTORE_OBJECT_SETTER_AND_GETTER( Vertex_v1,
                                      Vertex_v1::TrackParticleLinks_t,
                                      trackParticleLinks,
                                      setTrackParticleLinks )
   AUXSTORE_OBJECT_SETTER_AND_GETTER( Vertex_v1, std::vector< float >,
                                      trackWeights, setTrackWeights )

   AUXSTORE_OBJECT_SETTER_AND_GETTER( Vertex_v1,
                                      Vertex_v1::NeutralParticleLinks_t,
                                      neutralParticleLinks,
                                      setNeutralParticleLinks )
   AUXSTORE_OBJECT_SETTER_AND_GETTER( Vertex_v1, std::vector< float >,
                                      neutralWeights, setNeutralWeights )


   const TrackParticle* Vertex_v1::trackParticle( size_t i ) const {

      if( ! trackAcc.isAvailable( *this ) ) {
         return 0;
      }
      if( ( trackAcc( *this ).size() <= i ) ||
          ( ! trackAcc( *this )[ i ].isValid() ) ) {
         return 0;
      }

      return ( *( trackAcc( *this )[ i ] ) );
   }

   float Vertex_v1::trackWeight( size_t i ) const {

      if( weightTrackAcc( *this ).size() <= i ) {
         return -1.0;
      }
      return weightTrackAcc( *this )[ i ];
   }

   size_t Vertex_v1::nTrackParticles() const {

      if( ! trackAcc.isAvailable( *this ) ) {
         return 0;
      }

      return trackAcc( *this ).size();
   }


   const NeutralParticle* Vertex_v1::neutralParticle( size_t i ) const {

      if( ! neutralAcc.isAvailable( *this ) ) {
         return 0;
      }
      if( ( neutralAcc( *this ).size() <= i ) ||
          ( ! neutralAcc( *this )[ i ].isValid() ) ) {
         return 0;
      }

      return ( *( neutralAcc( *this )[ i ] ) );
   }

   float Vertex_v1::neutralWeight( size_t i ) const {

      if( weightNeutralAcc( *this ).size() <= i ) {
         return -1.0;
      }
      return weightNeutralAcc( *this )[ i ];
   }

   size_t Vertex_v1::nNeutralParticles() const {

      if( ! neutralAcc.isAvailable( *this ) ) {
         return 0;
      }

      return neutralAcc( *this ).size();
   }




   void
   Vertex_v1::addTrackAtVertex( const ElementLink< TrackParticleContainer >& tr,
                                float weight ) {

      trackAcc( *this ).push_back( tr );
      weightTrackAcc( *this ).push_back( weight );
      return;
   }

   void
   Vertex_v1::addNeutralAtVertex( const ElementLink< NeutralParticleContainer >& tr,
                                float weight ) {

      neutralAcc( *this ).push_back( tr );
      weightNeutralAcc( *this ).push_back( weight );
      return;
   }

   void Vertex_v1::clearTracks() {

      trackAcc( *this ).clear();
      weightTrackAcc( *this ).clear();
      return;
   }

   void Vertex_v1::clearNeutrals() {

      neutralAcc( *this ).clear();
      weightNeutralAcc( *this ).clear();
      return;
   }

   /*
    * Reset the cache
    */
   void Vertex_v1::resetCache() {
      m_position.reset();
      m_covariance.reset();
      return;
   }

} // namespace xAOD

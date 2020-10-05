/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "BTagging/BTagTrackAugmenterAlg.h"
#include "GaudiKernel/ServiceHandle.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

// slightly ugly that we include both of these: ideally we'd just
// add the methods we need to the extrapolator
#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"
#include "TrkExInterfaces/IExtrapolator.h"

#include "TrkSurfaces/PerigeeSurface.h"


namespace Analysis {

  BTagTrackAugmenterAlg::BTagTrackAugmenterAlg( const std::string& name, ISvcLocator* loc )
    : AthAlgorithm(name, loc) {}

  StatusCode BTagTrackAugmenterAlg::initialize() {
    ATH_MSG_INFO( "Inizializing " << name() << "... " );

    if ( m_track_to_vx.retrieve().isFailure() ) {
      ATH_MSG_FATAL("Failed to retrieve tool " << m_track_to_vx);
      return StatusCode::FAILURE;
    }

    if ( m_extrapolator.retrieve().isFailure() ) {
      ATH_MSG_FATAL("Failed to retrieve tool " << m_extrapolator);
      return StatusCode::FAILURE;
    }
    
    // Initialize Container keys
    ATH_MSG_DEBUG( "Inizializing containers:"        );
    ATH_MSG_DEBUG( "    ** " << m_TrackContainerKey  );
    ATH_MSG_DEBUG( "    ** " << m_VertexContainerKey );

    ATH_CHECK( m_TrackContainerKey.initialize() );
    ATH_CHECK( m_VertexContainerKey.initialize() );

    // Prepare decorators
    m_dec_d0       = m_TrackContainerKey.key() + "." + m_prefix.value() + m_dec_d0.key();
    m_dec_z0       = m_TrackContainerKey.key() + "." + m_prefix.value() + m_dec_z0.key();
    m_dec_d0_sigma = m_TrackContainerKey.key() + "." + m_prefix.value() + m_dec_d0_sigma.key();
    m_dec_z0_sigma = m_TrackContainerKey.key() + "." + m_prefix.value() + m_dec_z0_sigma.key();

    m_dec_track_pos = m_TrackContainerKey.key() + "." + m_prefix.value() + m_dec_track_pos.key();
    m_dec_track_mom = m_TrackContainerKey.key() + "." + m_prefix.value() + m_dec_track_mom.key();

    // Initialize decorators
    ATH_MSG_DEBUG( "Inizializing decorators:"  );
    ATH_MSG_DEBUG( "    ** " << m_dec_d0       );
    ATH_MSG_DEBUG( "    ** " << m_dec_z0       );
    ATH_MSG_DEBUG( "    ** " << m_dec_d0_sigma );
    ATH_MSG_DEBUG( "    ** " << m_dec_z0_sigma );
    ATH_MSG_DEBUG( "    ** " << m_dec_track_pos );
    ATH_MSG_DEBUG( "    ** " << m_dec_track_mom );

    CHECK( m_dec_d0.initialize() );
    CHECK( m_dec_z0.initialize() );
    CHECK( m_dec_d0_sigma.initialize() );
    CHECK( m_dec_z0_sigma.initialize() );
    CHECK( m_dec_track_pos.initialize() );
    CHECK( m_dec_track_mom.initialize() );

    return StatusCode::SUCCESS;
  }

  StatusCode BTagTrackAugmenterAlg::execute() {
    ATH_MSG_DEBUG( "Executing " << name() << "... " );
  
    const EventContext& ctx = getContext();

    // ========================================================================================================================== 
    //    ** Retrieve Ingredients
    // ========================================================================================================================== 

    SG::ReadHandle< xAOD::VertexContainer > vertexContainerHandle = SG::makeHandle< xAOD::VertexContainer >( m_VertexContainerKey,ctx );
    CHECK( vertexContainerHandle.isValid() );
    const xAOD::VertexContainer *verteces = vertexContainerHandle.get();

    const xAOD::Vertex* primary = getPrimaryVertex( *verteces );
    if ( primary == nullptr ) {
      ATH_MSG_FATAL("No primary vertex found");
      return StatusCode::FAILURE;
    }

    SG::ReadHandle< xAOD::TrackParticleContainer > trackContainerHandle = SG::makeHandle< xAOD::TrackParticleContainer >( m_TrackContainerKey,ctx);
    CHECK( trackContainerHandle.isValid() );
    const xAOD::TrackParticleContainer* tracks = trackContainerHandle.get();
    ATH_MSG_DEBUG( "Retrieved " << tracks->size() << " input tracks..." );


    // ========================================================================================================================== 
    //    ** Make Decorators (these are outputs)
    // ========================================================================================================================== 

    SG::WriteDecorHandle< xAOD::TrackParticleContainer, float > decor_d0( m_dec_d0 );
    SG::WriteDecorHandle< xAOD::TrackParticleContainer, float > decor_z0( m_dec_z0 );
    SG::WriteDecorHandle< xAOD::TrackParticleContainer, float > decor_d0_sigma( m_dec_d0_sigma );
    SG::WriteDecorHandle< xAOD::TrackParticleContainer, float > decor_z0_sigma( m_dec_z0_sigma );

    SG::WriteDecorHandle< xAOD::TrackParticleContainer, std::vector< float > > decor_track_pos( m_dec_track_pos );
    SG::WriteDecorHandle< xAOD::TrackParticleContainer, std::vector< float > > decor_track_mom( m_dec_track_mom );

    // ========================================================================================================================== 
    //    ** Computation
    // ========================================================================================================================== 

    Trk::PerigeeSurface primary_surface( primary->position() );

    // now decorate the tracks
    for (const xAOD::TrackParticle *track: *tracks) {
      std::unique_ptr< const Trk::ImpactParametersAndSigma > ip( m_track_to_vx->estimate( track, primary ) );
      if ( ip ) {
        decor_d0(*track) = ip->IPd0;
	decor_z0(*track) = ip->IPz0SinTheta;
        decor_d0_sigma(*track) = ip->sigmad0;
        decor_z0_sigma(*track) = ip->sigmaz0SinTheta;
        ATH_MSG_DEBUG( " d0= " << ip->IPd0 <<
		       " z0SinTheta= " << ip->IPz0SinTheta <<
		       " sigmad0= " << ip->sigmad0 <<
		       " sigmaz0SinTheta= " << ip->sigmaz0SinTheta );
      } else {
        ATH_MSG_WARNING( "failed to estimate track impact parameter, using dummy values" );
        decor_d0(*track) = NAN;
        decor_z0(*track) = NAN;
        decor_d0_sigma(*track) = NAN;
        decor_z0_sigma(*track) = NAN;
      }

      // some other parameters we have go get directly from the
      // extrapolator. This is more or less copied from:
      // https://goo.gl/iWLv5T
      std::unique_ptr< const Trk::TrackParameters > extrap_pars( m_extrapolator->extrapolate( *track, primary_surface ) );
      if ( extrap_pars ) {
        const Amg::Vector3D& track_pos = extrap_pars->position();
        const Amg::Vector3D& vertex_pos = primary->position();

	const Amg::Vector3D position = track_pos - vertex_pos;
	const Amg::Vector3D momentum = extrap_pars->momentum();

	//Test output for cross checking output with stored values
	ATH_MSG_DEBUG( "vertex_pos (x,y,z)= (" << vertex_pos.x() << ", " << vertex_pos.y() << ", " << vertex_pos.z() << ")");
	ATH_MSG_DEBUG( "track_pos (x,y,z)= (" << track_pos.x() << ", " << track_pos.y() << ", " << track_pos.z() << ")");
	ATH_MSG_DEBUG( "track_displacement (x,y,z)= (" << position.x() << ", " << position.y() << ", " << position.z() << ")");
	ATH_MSG_DEBUG( "track_momentum (x,y,z)= (" << momentum.x() << ", " << momentum.y() << ", " << momentum.z() << ")");

	std::vector< float > out_vec_pos( position.data(), position.data() + position.size() );
	std::vector< float > out_vec_mom( momentum.data(), momentum.data() + momentum.size() );

	decor_track_pos (*track) = out_vec_pos;
	decor_track_mom (*track) = out_vec_mom;
      } else {
        ATH_MSG_WARNING( "failed to extrapolate track coordinates at primary vertex, using dummy values");

	std::vector< float > out_vec_pos = {NAN,NAN,NAN};
	std::vector< float > out_vec_mom = {NAN,NAN,NAN};

	decor_track_pos (*track) = out_vec_pos;
        decor_track_mom (*track) = out_vec_mom;
      }
    }

    return StatusCode::SUCCESS;
  }

  const xAOD::Vertex* BTagTrackAugmenterAlg::getPrimaryVertex( const xAOD::VertexContainer& vertexCollection ) const {
    if ( vertexCollection.size() == 0 ) {
      ATH_MSG_DEBUG( "Input vertex collection has size 0!" );
      return nullptr;
    }

    for ( const xAOD::Vertex *vertex : vertexCollection ) { 
      if ( vertex->vertexType() == xAOD::VxType::PriVtx ) 
        return vertex;
    }

    // this is taken from BTagTool, should be the beam spot if nothing
    // else exists.
    return vertexCollection.front();
  }

}



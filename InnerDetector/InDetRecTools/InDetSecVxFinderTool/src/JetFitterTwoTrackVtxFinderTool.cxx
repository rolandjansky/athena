/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetSecVxFinderTool/JetFitterTwoTrackVtxFinderTool.h"

#include "TrkVertexSeedFinderTools/CrossDistancesSeedFinder.h"
#include "TrkVertexFitters/SequentialVertexFitter.h" 

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkLinks/LinkToXAODTrackParticle.h"

using namespace InDet;

JetFitterTwoTrackVtxFinderTool::JetFitterTwoTrackVtxFinderTool(const std::string &t, const std::string &n, const IInterface *p)
  : AthAlgTool(t, n, p),
    m_tracksDecorator( "VxTrackAtVertex" )
{
  declareInterface< JetFitterTwoTrackVtxFinderTool >(this);
}

JetFitterTwoTrackVtxFinderTool::~JetFitterTwoTrackVtxFinderTool() {}


StatusCode JetFitterTwoTrackVtxFinderTool::initialize() {

  if ( m_CrossDistancesSeedFinder.retrieve().isFailure() ) {
    ATH_MSG_ERROR( "Cannot retrieve Trk::CrossDistancesSeedFinder/CrossDistancesSeedFinder" );
    return StatusCode::FAILURE;
  }

  if ( m_SequentialVertexFitter.retrieve().isFailure() ) {
    ATH_MSG_ERROR( "Cannot retrieve Trk::SequentialVertexFitter/SequentialVertexFitter" );
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode JetFitterTwoTrackVtxFinderTool::finalize() {
  return StatusCode::SUCCESS;
}


const Trk::TwoTrackVerticesInJet* JetFitterTwoTrackVtxFinderTool::doVertexFinding( const xAOD::Vertex& primaryVertex,
										   const TLorentzVector& jetMomentum,
										   std::vector< const Trk::ITrackLink* >& inputTracks) const {

  std::vector< const xAOD::Vertex* > VtxCandidates;

  ATH_MSG_DEBUG( "Looping over " << inputTracks.size() << " input tracks... " );

  // Loop over all the combinations
  for ( unsigned int indexA(0); indexA<inputTracks.size(); indexA++ ) {
    const Trk::ITrackLink* trackA = inputTracks.at( indexA );

    for ( unsigned int indexB(0); indexB<indexA; indexB++ ) {
      const Trk::ITrackLink* trackB = inputTracks.at( indexB );
      
      // Computing the Vertex candidate
      xAOD::Vertex *myCandidate = computeVtxcandidate( primaryVertex,jetMomentum,trackA,trackB );
      if ( myCandidate == nullptr ) continue;

      // Attaching tracks to vertex candidate
      std::vector< const Trk::ITrackLink* > associatedTracksAtVertex;
      associatedTracksAtVertex.push_back( trackA );
      associatedTracksAtVertex.push_back( trackB );
      m_tracksDecorator ( *myCandidate ) = associatedTracksAtVertex;

      VtxCandidates.push_back( myCandidate );
    }
  }


  ATH_MSG_DEBUG( "Found " << VtxCandidates.size() <<" 2-trk vertex candidates!" );

  const Trk::TwoTrackVerticesInJet *twoTrackVerticesInJet = new Trk::TwoTrackVerticesInJet ( VtxCandidates,
                                                                                             std::vector< const Trk::TrackParticleBase* >() );
  return twoTrackVerticesInJet;
}

xAOD::Vertex* JetFitterTwoTrackVtxFinderTool::computeVtxcandidate( const xAOD::Vertex& primaryVertex,
								   const TLorentzVector& jetMomentum,
								   const Trk::ITrackLink* trackA,
								   const Trk::ITrackLink* trackB ) const {
  
  const Trk::TrackParameters* perigeeTrackA = trackA->parameters();
  const Trk::TrackParameters* perigeeTrackB = trackB->parameters();

  std::vector< const Trk::TrackParameters* > perigeeToFit;
  perigeeToFit.push_back( perigeeTrackA );
  perigeeToFit.push_back( perigeeTrackB );

  Amg::Vector3D seedVertex;
  try {
    seedVertex = m_CrossDistancesSeedFinder->findSeed( perigeeToFit );

    if ( seedVertex.perp() > m_maxR ||
         fabs( seedVertex.z() ) > m_maxZ ) {
      ATH_MSG_DEBUG( "Vertex seed outside ID. R=" << seedVertex.perp() << " Z=" << seedVertex.z() );
      seedVertex = primaryVertex.position();
    }

  } catch (...) {
    ATH_MSG_DEBUG( "Seed finding failed. Using primary vertex as seed... (clearly not optimal)" );
    seedVertex = Amg::Vector3D( primaryVertex.position() );
  }

  ATH_MSG_DEBUG( "Seed: x=" << seedVertex.x() <<
		 " y=" << seedVertex.y() <<
		 " z=" << seedVertex.z() );
  

  // Compute V0 candidate
  std::unique_ptr< xAOD::Vertex > myCandidate( m_SequentialVertexFitter->fit(perigeeToFit,seedVertex) );

  // Check fit completed with success
  if ( myCandidate == nullptr ) {
    ATH_MSG_DEBUG( " Sequential fit failed. shouldn't happen... Skipping V0 candidate... " );
    return nullptr;
  }

  // Check ChiSquared and nDof
  if ( myCandidate->chiSquared() < 0 ||
       myCandidate->numberDoF() < 0 ) {
    ATH_MSG_DEBUG( " Fit for V0 candidate failed: chi2 or ndf negative. Deleting candidate..." );
    return nullptr;
  }

  // Check two-vertex probability
  if ( TMath::Prob( myCandidate->chiSquared(),myCandidate->numberDoF() ) <= m_twoVertexProbabilityCut ) {
    ATH_MSG_DEBUG( " Candidate not satisfying  two-vertex probability " );
    return nullptr;
  }

  // Check sign and revertFromPositiveToNegativeTags
  Amg::Vector3D jetMomSpatial( jetMomentum.X(),jetMomentum.Y(),jetMomentum.Z() );
  Amg::Vector3D twoTrkVtxPos( myCandidate->position() );
  double sign = ( twoTrkVtxPos-primaryVertex.position() ).dot( jetMomSpatial );

  if ( sign >= 0 && m_revertFromPositiveToNegativeTags ) {
    ATH_MSG_DEBUG( "Not satisfying sign and revertFromPositiveToNegativeTags requirements" );
    return nullptr;
  }

  ATH_MSG_DEBUG( "  Candidate: x=" << myCandidate->x() <<
		 " y=" << myCandidate->y() <<
		 " z=" << myCandidate->z() );

  return myCandidate.release();
}



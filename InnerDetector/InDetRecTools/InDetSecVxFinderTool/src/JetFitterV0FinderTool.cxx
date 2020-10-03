/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
 
#include "InDetSecVxFinderTool/JetFitterV0FinderTool.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkLinks/LinkToXAODTrackParticle.h"

using namespace InDet;

JetFitterV0FinderTool::JetFitterV0FinderTool(const std::string &t, const std::string &n, const IInterface *p)
  : AthAlgTool(t, n, p),
    m_compatibilityAccessor( "TrackCompatibility" ),
    m_tracksAccessor( "VxTrackAtVertex" )
{
  declareInterface< JetFitterV0FinderTool >(this);
}

JetFitterV0FinderTool::~JetFitterV0FinderTool() {}


StatusCode JetFitterV0FinderTool::initialize() {

  if ( m_jetFitterUtils.retrieve().isFailure() ) {
    ATH_MSG_ERROR( "Cannot retrieve InDet::InDetJetFitterUtils/InDetJetFitterUtils" );
    return StatusCode::FAILURE;
  }

  if ( m_mode3dfinder.retrieve().isFailure() ) {
    ATH_MSG_ERROR( "Cannot retrieve Trk::Mode3dTo1dFinder/Mode3dTo1dFinder" );
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode JetFitterV0FinderTool::finalize() {
  return StatusCode::SUCCESS;
}



const Trk::TwoTrackVerticesInJet* JetFitterV0FinderTool::doV0Finding( const xAOD::Vertex& primaryVertex,
								      const TLorentzVector& jetMomentum,
								      std::vector< const Trk::ITrackLink* >& inputTracks,
								      const std::vector< const xAOD::Vertex* >& vertexCandidates,
								      std::vector< const Trk::ITrackLink* >& tracksToUseInFirstFit,
								      std::vector< const Trk::ITrackLink* >& tracksToUseInSecondFit,
								      Amg::Vector3D& JFseedDirection ) const {

  // Vector of seeding vertices. 
  std::vector< Trk::PositionAndWeight > positionsOfSeedingVertices;

  // Using a function here, may become tool in the future!
  // Phase 3, 4 and  5: find and select v0 candidates
  std::vector< const xAOD::Vertex* > v0candidates = findV0candidates( primaryVertex,jetMomentum,inputTracks, vertexCandidates );  
  ATH_MSG_DEBUG( "Found " << v0candidates.size() <<" V0 candidates!" );
      
  // Phase 6
  // Select tracks for first fit using only the "best tracks" from the two-track vertices
  ATH_MSG_DEBUG( "Analyzing two track vertices to select the best tracks" );
  for ( const xAOD::Vertex* v0candidate : v0candidates ) {

    // Check quality for first fit
    bool satisfyCriteriaFirstFitQuality = checkCriteriaFirstFit( primaryVertex,jetMomentum,*v0candidate );
    if ( not satisfyCriteriaFirstFitQuality ) { 
      ATH_MSG_DEBUG( "Quality criteria for first fit not satisfied! skipping ... " );
      continue; 
    } 

    // The v0 candidate satisfy the selection criteria
    // Filling the vector of tracks to be used in the first fit
    std::vector< const Trk::ITrackLink* > vxTrackAtVertex = m_tracksAccessor( *v0candidate );
    const Trk::ITrackLink *firstTrack  = vxTrackAtVertex.at(0);
    const Trk::ITrackLink *secondTrack = vxTrackAtVertex.at(1);

    // Second track added first, legacy from old jetfitter
    bool secondTrackAlreadyStored = m_jetFitterUtils->checkIfTrackIsInVector( secondTrack,tracksToUseInFirstFit );
    if ( not secondTrackAlreadyStored )
      tracksToUseInFirstFit.push_back( secondTrack );

    // First track
    bool firstTrackAlreadyStored = m_jetFitterUtils->checkIfTrackIsInVector( firstTrack,tracksToUseInFirstFit );
    if ( not firstTrackAlreadyStored )
      tracksToUseInFirstFit.push_back( firstTrack );

    positionsOfSeedingVertices.push_back( Trk::PositionAndWeight( v0candidate->position(),1 ) );
  }


  // Phase 7
  ATH_MSG_DEBUG( "Determine single good tracks to add in the fit in a second step" );
  for ( const Trk::ITrackLink *trackLink : inputTracks ) {
    
    bool satisfyCriteriaSecondFitQuality = checkCriteriaSecondFit( primaryVertex,trackLink );
    if ( not satisfyCriteriaSecondFitQuality ) {
      ATH_MSG_DEBUG( "Quality criteria for second fit not satisfied! skipping ... " );
      continue;
    } 

    bool alreadyUsed = m_jetFitterUtils->checkIfTrackIsInVector( trackLink,tracksToUseInFirstFit );
    if ( alreadyUsed ) {
      ATH_MSG_VERBOSE( "Track was already used" );
      continue;
    }

    bool trackAlreadyStored = m_jetFitterUtils->checkIfTrackIsInVector( trackLink,tracksToUseInSecondFit );
    if ( not trackAlreadyStored )
      tracksToUseInSecondFit.push_back( trackLink );
  }


  //  Compute JFseedDirection
  JFseedDirection = computeSeedDirection( primaryVertex,jetMomentum,positionsOfSeedingVertices );

  // First argument is the list of vertex candidates
  // Second argument is the list of Selected Neutral Tracks To Store
  // The Trk::TwoTrackVerticesInJet takes over the ownership

  // We need to use normal pointers instead of smart pointers since the code breaks.
  // We have to fix this issue in the future
  const Trk::TwoTrackVerticesInJet *twoTrackVerticesInJet = new Trk::TwoTrackVerticesInJet( v0candidates,
                                                                                            std::vector< const Trk::TrackParticleBase* >() );
  return twoTrackVerticesInJet;
}

std::vector< const xAOD::Vertex* > JetFitterV0FinderTool::findV0candidates( const xAOD::Vertex& /*primaryVertex*/,
									    const TLorentzVector& /*jetMomentum*/,
									    std::vector< const Trk::ITrackLink* >&  /*inputTracks*/,
									    const std::vector< const xAOD::Vertex* >& vertexCandidates ) const {
  
  std::vector< const xAOD::Vertex* > v0candidates;
  ATH_MSG_DEBUG( "Looping over " << vertexCandidates.size() <<" input candidates" );

  for ( unsigned int indexA(0); indexA<vertexCandidates.size(); indexA++ ) {
    const xAOD::Vertex* myCandidate = vertexCandidates.at( indexA );

    // Here a shortlist of candidates will be applied    
    
    // Phase 4 and 5 (not called in old code)!
    // Check for photon convertion, Ks and Lambda
    // Veto candidates and tracks

    // Make a copy of the input candidate
    // This way of creating a copy creates a discrepancy w.r.t. the code in rel21, which is propagated to Jet Fitter variables.
    // However, this assures that the vertex positions retrieved via position().x() and x() are consistent
    xAOD::Vertex* toAdd = new xAOD::Vertex();
    v0candidates.push_back( toAdd );
    *toAdd = *myCandidate;
  }

  return v0candidates;
}

bool JetFitterV0FinderTool::checkCriteriaFirstFit( const xAOD::Vertex& primaryVertex,
						   const TLorentzVector& jetMomentum,
						   const xAOD::Vertex& v0candidate ) const {


  // Check vertex probability
  double vertexProb = TMath::Prob( v0candidate.chiSquared(),
				   v0candidate.numberDoF() );

  if ( ( vertexProb < m_cutTwoTrkVtxVertexProbForBFirstSelectionFirstCriterium ) &&
       ( vertexProb < m_cutTwoTrkVtxVertexProbForBFirstSelectionSecondCriterium ) ) {
    ATH_MSG_DEBUG( "V0 candidate does not satisfy the vertex prob criteria!"  );
    return false; 
  }
  


  // Retrieve and prepare objects
  const std::vector< const Trk::ITrackLink* > vxTrackAtVertex = m_tracksAccessor( v0candidate );
  const Trk::ITrackLink* trackLink1 = vxTrackAtVertex.at( 0 ); 
  const Trk::ITrackLink* trackLink2 = vxTrackAtVertex.at( 1 );
  
  if ( trackLink1 == nullptr || trackLink2 == nullptr ) {
    ATH_MSG_DEBUG( "Zero pointer (ITrackLink): skipping 2-track candidate" );
    return false;
  }

  const Trk::LinkToXAODTrackParticle* linkTrackA = dynamic_cast< const Trk::LinkToXAODTrackParticle* >( trackLink1 );
  const Trk::LinkToXAODTrackParticle* linkTrackB = dynamic_cast< const Trk::LinkToXAODTrackParticle* >( trackLink2 );

  if ( !linkTrackA || !linkTrackB ) {
    ATH_MSG_DEBUG( "Zero pointer (LinkToXAODTrackParticle): skipping 2-track candidate" );
    return false;
  }


  // TO-DO
  // remove track candidates that are supposed to be voted !




  // Take track compatibility and compute flight significance
  // compatibility is attached to tracks, so we need to retrieve xAOD::TrackParticles objects
  float compatibiltyTrackA = m_compatibilityAccessor( ***linkTrackA );
  float compatibiltyTrackB = m_compatibilityAccessor( ***linkTrackB );
  
  if ( TMath::Prob( fabs(compatibiltyTrackA),2 ) >= m_cutCompatibilityPrimaryVertexSingleTrackForBFirstSelection ) return false;
  if ( TMath::Prob( fabs(compatibiltyTrackB),2 ) >= m_cutCompatibilityPrimaryVertexSingleTrackForBFirstSelection ) return false;
  if ( TMath::Prob( fabs(compatibiltyTrackA) + fabs(compatibiltyTrackB),4 ) >= m_cutCompatibilityPrimaryVertexBothTracksForBFirstSelection ) return false;


  // Criteria on D0 and Z0 of the tracks w.r.t. primary vertex
  const Trk::TrackParameters* initialPerigee1 = linkTrackA->parameters();
  const Trk::TrackParameters* initialPerigee2 = linkTrackB->parameters();

  if ( !initialPerigee1 || !initialPerigee2 ) {
    ATH_MSG_DEBUG( "No refitted parameters available for 2-track vertex. Candidate not accepted..." );
    return false;
  }

  if ( initialPerigee1->momentum().perp() <= m_cutPtBothTracksForBFirstSelection ) return false;
  if ( initialPerigee2->momentum().perp() <= m_cutPtBothTracksForBFirstSelection ) return false;




  // Need to create Trk::RecVertex object. Is this really necessary?
  Trk::RecVertex primaryVertexRecVertex( primaryVertex.position(),
					 primaryVertex.covariancePosition(),
					 primaryVertex.numberDoF(),
					 primaryVertex.chiSquared());

  std::pair<double,double> track1_IPd0z0 = m_jetFitterUtils->getD0andZ0IP( *initialPerigee1,
									   primaryVertexRecVertex );
  
  std::pair<double,double> track2_IPd0z0 = m_jetFitterUtils->getD0andZ0IP(*initialPerigee2,
									  primaryVertexRecVertex);



  ATH_MSG_DEBUG( "Checking good quality for track A..." );
  if ( fabs( track1_IPd0z0.first ) >= m_cutIPD0BothTracksForBFirstSelection ) return false;
  if ( fabs( track1_IPd0z0.second ) >= m_cutIPZ0BothTracksForBFirstSelection ) return false;

  ATH_MSG_DEBUG( "Checking good quality for track B..." );
  if ( fabs( track2_IPd0z0.first ) >= m_cutIPD0BothTracksForBFirstSelection ) return false;
  if ( fabs( track2_IPd0z0.second ) >= m_cutIPZ0BothTracksForBFirstSelection ) return false;



  
  
  ATH_MSG_DEBUG( "Checking distance and error between two vertices..." );
  std::pair<double,double> distanceAndError = m_jetFitterUtils->getDistanceAndErrorBetweenTwoVertices( v0candidate,primaryVertexRecVertex );

  Amg::Vector3D jetMomSpatial( jetMomentum.X(), jetMomentum.Y(), jetMomentum.Z() );
  double sign = ( v0candidate.position() - primaryVertexRecVertex.position() ).dot( jetMomSpatial ); 

  double signedDistance = distanceAndError.first;
  if ( sign < 0 ) signedDistance = -signedDistance;
  double significance = signedDistance/distanceAndError.second;

  double FirstSelectionFirstCriterium = m_cutTwoTrkVtxLifetimeSignificanceForBFirstSelectionFirstCriterium;
  double FirstSelectionSecondCriterium = m_cutTwoTrkVtxLifetimeSignificanceForBFirstSelectionSecondCriterium;

  if ( m_revertFromPositiveToNegativeTags ) {
    FirstSelectionFirstCriterium = -FirstSelectionFirstCriterium;
    FirstSelectionSecondCriterium = -FirstSelectionSecondCriterium;
  }
  
  bool firstCriterium = ( vertexProb > m_cutTwoTrkVtxVertexProbForBFirstSelectionFirstCriterium  ) &&
    ( significance > FirstSelectionFirstCriterium );
  bool secondCriterium = ( vertexProb > m_cutTwoTrkVtxVertexProbForBFirstSelectionSecondCriterium ) &&
    ( significance > FirstSelectionSecondCriterium );

  if ( not ( firstCriterium || secondCriterium ) ) 
    return false;




  ATH_MSG_DEBUG( "Checking material interaction in layer..." );
  double radius = v0candidate.position().perp();
  int interactiontype=-1;

  if ( (radius > m_firstBeam_min ) && 
       (radius < m_firstBeam_max) ) {
    interactiontype=0;
  } else if ( (radius > m_secondBeam_min ) && 
	      ( radius < m_secondBeam_max ) ) {
    interactiontype=1;
  } else if ( ( radius > m_firstLayer_min ) && 
	      ( radius < m_firstLayer_max ) ) {
    interactiontype=2;
  } else if ( (radius > m_secondLayer_min ) && 
	      ( radius < m_secondLayer_max ) ) {
    interactiontype=3;
  }



  if ( interactiontype == 2 || interactiontype == 3 ) {
    bool signifCutTight = ( TMath::Prob( fabs( compatibiltyTrackA ),2 ) < m_cutCompatibilityToPrimarySingleTrackForMatInteractions ) &&
      ( TMath::Prob( fabs( compatibiltyTrackB ),2 ) < m_cutCompatibilityToPrimarySingleTrackForMatInteractions ) &&
      ( TMath::Prob( fabs( compatibiltyTrackA ) + fabs( compatibiltyTrackB ),4 ) < m_cutCompatibilityToPrimaryBothTracksForMatInteractions );
    
    if ( signifCutTight ) return false;
  }  



  // TO-DO
  // Remove candidates with track overlaps



  return true;
} 


bool JetFitterV0FinderTool::checkCriteriaSecondFit( const xAOD::Vertex& primaryVertex,
						    const Trk::ITrackLink *trackLink ) const {

  const Trk::LinkToXAODTrackParticle* linkTrack = dynamic_cast< const Trk::LinkToXAODTrackParticle* >( trackLink );
  if ( linkTrack == nullptr ) {
    ATH_MSG_DEBUG( "Zero pointer (LinkToXAODTrackParticle): skipping 2-track candidate" );
    return false;
  }
  
  float compatibilityTrack = m_compatibilityAccessor( ***linkTrack );
  
  
  const Trk::TrackParameters* perigee = trackLink->parameters();
  const AmgSymMatrix(5) *measPerigee = perigee->covariance();
  
  if ( measPerigee == nullptr ) {
    ATH_MSG_DEBUG( "Track parameters have no covariance. skipping single track candidate..." );
    return false;
  }
  
  // Need to create Trk::RecVertex object. Is this really necessary?
  Trk::RecVertex primaryVertexRecVertex( primaryVertex.position(),
					 primaryVertex.covariancePosition(),
					 primaryVertex.numberDoF(),
					 primaryVertex.chiSquared());
  
  std::pair<double,double> track_IPd0z0 = m_jetFitterUtils->getD0andZ0IP( *perigee,
									  primaryVertexRecVertex );
  std::pair<double,double> track_IPd0z0Sig = m_jetFitterUtils->getD0andZ0IPSig( *perigee,
										primaryVertexRecVertex );
  
  const double IPd0    = track_IPd0z0.first;                                                                                                       
  const double IPz0    = track_IPd0z0.second;                                                                                                      
  const double IPd0Sig = track_IPd0z0Sig.first;                                                                                                 
  const double IPz0Sig = track_IPd0z0Sig.second;                                                                                                
  const double pT      = perigee->momentum().perp();                                                                                                   
  
  double cutCompatibilityPrimaryVertexSinglePositiveLifetimeTrackForBSecondSelection = m_cutCompatibilityPrimaryVertexSinglePositiveLifetimeTrackForBSecondSelection;
  double cutCompatibilityPrimaryVertexSingleNegativeLifetimeTrackForBSecondSelection = m_cutCompatibilityPrimaryVertexSingleNegativeLifetimeTrackForBSecondSelection;
  
  if ( m_revertFromPositiveToNegativeTags ) {
    cutCompatibilityPrimaryVertexSinglePositiveLifetimeTrackForBSecondSelection = m_cutCompatibilityPrimaryVertexSingleNegativeLifetimeTrackForBSecondSelection;
    cutCompatibilityPrimaryVertexSingleNegativeLifetimeTrackForBSecondSelection = m_cutCompatibilityPrimaryVertexSinglePositiveLifetimeTrackForBSecondSelection;
  }                                                                                                                                             
  
  bool passBoxCut = ( fabs(IPd0Sig) < m_cutIPD0SigBoxSingleTrackForBSecondSelection ) && 
    ( fabs(IPz0Sig) > m_cutIPZ0SigBoxSingleTrackForBSecondSelection );
  
  
  if ( fabs(IPd0) > m_cutIPD0SingleTrackForBSecondSelection ||
       fabs(IPz0) > m_cutIPZ0SingleTrackForBSecondSelection ||
       ( compatibilityTrack >= 0 && TMath::Prob( fabs(compatibilityTrack),2) > cutCompatibilityPrimaryVertexSinglePositiveLifetimeTrackForBSecondSelection ) ||
       ( compatibilityTrack < 0 && TMath::Prob( fabs(compatibilityTrack),2) > cutCompatibilityPrimaryVertexSingleNegativeLifetimeTrackForBSecondSelection ) ||
       pT < m_cutPtSingleTrackForBSecondSelection || passBoxCut )
    {
      ATH_MSG_DEBUG( "Candidate didn't pass one of the selection cuts" );
      return false;
    }
  
  return true;
}

Amg::Vector3D JetFitterV0FinderTool::computeSeedDirection( const xAOD::Vertex& primaryVertex,
							   const TLorentzVector& jetMomentum, 
							   const std::vector< Trk::PositionAndWeight >& positionsOfSeedingVertices ) const {

  Amg::Vector3D JFseedDirection( jetMomentum.X(),jetMomentum.Y(),jetMomentum.Z() );
  JFseedDirection.normalize();
  ATH_MSG_VERBOSE( "Jet Direction would be: " << JFseedDirection );

  if ( positionsOfSeedingVertices.size() == 0 )
    return JFseedDirection;

  Amg::Vector3D theSeedVertex = m_mode3dfinder->getMode( primaryVertex.x(),primaryVertex.y(),positionsOfSeedingVertices );
    double sign = ( theSeedVertex - primaryVertex.position() ).dot( JFseedDirection );

  if ( m_revertFromPositiveToNegativeTags == false ) {

    if ( sign > 0 ) {
      JFseedDirection = ( theSeedVertex - primaryVertex.position() ).unit();
      ATH_MSG_DEBUG( "Using twotrkvtx direction for start: " << JFseedDirection );
    } else ATH_MSG_DEBUG( "NORMAL SEEDING: Seed vertex is on negative side... Using Jet Direction!" );

  } else {

    if ( sign < 0 ) {
      JFseedDirection = -( theSeedVertex - primaryVertex.position() ).unit();
      ATH_MSG_DEBUG( "Using twotrkvtx direction for start: " << JFseedDirection );
    } else ATH_MSG_DEBUG( "REVERSE SEEDING: Seed vertex is on positive side... Using Jet Direction!" );

  }

  return JFseedDirection;
}

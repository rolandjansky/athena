/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          InDetImprovedJetFitterVxFinder.cxx  -  Description
                             -------------------

    begin : January 2008
    authors: Giacinto Piacquadio (University of Freiburg)
    email : nicola.giacinto.piacquadio@cern.ch
    changes: new version in January 2008!
    
 
  2008 (c) Atlas Detector Software

  Look at the header file for more information.
     
 ***************************************************************************/


#include "InDetSecVxFinderTool/InDetImprovedJetFitterVxFinder.h"
#include "VxJetVertex/VxJetCandidate.h"
#include "VxJetVertex/PairOfVxVertexOnJetAxis.h"
#include "VxJetVertex/VxVertexOnJetAxis.h"
#include "VxJetVertex/VxClusteringTable.h"
#include "VxSecVertex/VxJetFitterVertexInfo.h"
#include <TMath.h>
#include "TrkEventPrimitives/FitQuality.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "VxVertex/RecVertex.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/LinkToTrack.h"
#include "TrkEventPrimitives/VertexType.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkNeutralParameters/NeutralParameters.h"
#include "TrkVertexSeedFinderUtils/SeedFinderParamDefs.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "VxJetVertex/TwoTrackVerticesInJet.h"
#include "VxJetVertex/SelectedTracksInJet.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "TrkVertexFitterInterfaces/INeutralParticleParameterCalculator.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/VertexContainer.h"

//17.5.2015 This is for the EDM migration
#include "TrkLinks/LinkToXAODTrackParticle.h"

// Hackathon includes
#include "InDetSecVxFinderTool/JetFitterTrackSelectorTool.h"
#include "InDetSecVxFinderTool/JetFitterTwoTrackVtxFinderTool.h"
#include "InDetSecVxFinderTool/JetFitterV0FinderTool.h"
#include "InDetSecVxFinderTool/JetFitterMultiStageFit.h"


#define InDetImprovedJetFitterVxFinder_DEBUGAddOns

namespace InDet
{

  struct TrackParticle_pair
  {
    double first;
    const Trk::TrackParticleBase* second;
    TrackParticle_pair(double p1, const Trk::TrackParticleBase* p2)
      : first (p1), second (p2) {}
    bool operator< (const TrackParticle_pair& other) const
    { return first > other.first; }
  };

  struct Track_pair
  {
    double first;
    const Trk::Track* second;
    Track_pair(double p1, const Trk::Track* p2)
      : first (p1), second (p2) {}
    bool operator< (const Track_pair& other) const
    { return first > other.first; }
  };  

  InDetImprovedJetFitterVxFinder::InDetImprovedJetFitterVxFinder(const std::string& t, const std::string& n, const IInterface*  p) :
    AthAlgTool(t,n,p)
  { 
    // new package for hackathon
    declareInterface< ISecVertexInJetFinder >(this) ;
  }
  

  InDetImprovedJetFitterVxFinder::~InDetImprovedJetFitterVxFinder() {}
  

  StatusCode InDetImprovedJetFitterVxFinder::initialize() {
    
    StatusCode sc = AlgTool::initialize();
    if(sc.isFailure()) {
      msg(MSG::ERROR)<<" Unable to initialize the AlgTool"<<endmsg;
      return sc;
    }
    
    if (m_theTrackSelector.retrieve().isFailure()) {
      msg(MSG::ERROR) << "Could not find TrackSelector tool." << endmsg;
      return StatusCode::FAILURE;
    } else msg(MSG::INFO) << " JetFitterTrackSelector retrieved" << endmsg;
    
    if (m_theTwoTrackVtxFinderTool.retrieve().isFailure()) {
      msg(MSG::ERROR) << "Could not find TwoTrackVtxFinder tool." << endmsg;
      return StatusCode::FAILURE;
    } else msg(MSG::INFO) << " JetFitterTwoTrackVtxFinder retrieved" << endmsg;

    if (m_theV0FinderTool.retrieve().isFailure()) {
      msg(MSG::ERROR) << "Could not find V0Finder tool." << endmsg;
      return StatusCode::FAILURE;
    } else msg(MSG::INFO) << " JetFitterV0Finder retrieved" << endmsg;

    if (m_multiStageFitter.retrieve().isFailure()) {
      msg(MSG::ERROR) << "Could not find JetFitterMultiStageFit tool." << endmsg;
      return StatusCode::FAILURE;
    } else msg(MSG::INFO) << " JetFitterMultiStageFit retrieved" << endmsg;

    msg(MSG::INFO) << "Initialize successful" << endmsg;
    return StatusCode::SUCCESS;
  }
  

   StatusCode InDetImprovedJetFitterVxFinder::finalize() {

     msg(MSG::INFO)  << "Finalize successful" << endmsg;
    return StatusCode::SUCCESS;

  } 

  Trk::VxSecVertexInfo* InDetImprovedJetFitterVxFinder::findSecVertex(const xAOD::Vertex & primaryVertex,
								      const TLorentzVector & jetMomentum,
								      const std::vector<const xAOD::IParticle*> & inputTracks) const
  {
    // This function when called performs JetFitter secondary vertex finding
    // It returns the JetFitter Vertex Info, VxJetFitterVertexInfo:
    // The VxJetCandidate, the TwoTrackVerticesInJet (V0s and neutrals), the selected tracks (primary and secondary)
    // So we need to do track selection, v0 finding, and then the fit with JetFitter.

//
//    // Initial track selection, track filter applied, separate primary and secondary tracks
    const Trk::SelectedTracksInJet* mySelectedTracksInJet = m_theTrackSelector->doTrackSelection( primaryVertex, jetMomentum, inputTracks );

    ATH_MSG_DEBUG( "the selected tracks now are " << mySelectedTracksInJet );
    std::vector< const Trk::ITrackLink* > primaryTracks = mySelectedTracksInJet->getPrimaryTrackLinks();
    std::vector< const Trk::ITrackLink* > secondaryTracks  = mySelectedTracksInJet->getSecondaryTrackLinks();

    // Two track vertex candidates
    const Trk::TwoTrackVerticesInJet* twoTrackVertexCandidates = m_theTwoTrackVtxFinderTool->doVertexFinding( primaryVertex, jetMomentum, secondaryTracks );
    const std::vector< const xAOD::Vertex* > vertexCandidates = twoTrackVertexCandidates->getTwoTrackVertice();

//    // V0 finding AND further track selection, these could even be further separated
//    // neutral identification, material interactions, ..., a lot is done here
    std::vector< const Trk::ITrackLink* > tracksToUseInFirstFit;
    std::vector< const Trk::ITrackLink* > tracksToUseInSecondFit;
    Amg::Vector3D JFseedDirection;

    const Trk::TwoTrackVerticesInJet* myTwoTrackVerticesInJet = m_theV0FinderTool->doV0Finding( primaryVertex, jetMomentum, secondaryTracks, vertexCandidates,
												tracksToUseInFirstFit, tracksToUseInSecondFit, JFseedDirection );

    // Sort track collection by pT
    ATH_MSG_DEBUG( "Sorting track collection (tracksToUseInFirstFit) by pT..." );
    sortTrackCollectionByPt( tracksToUseInFirstFit );
    ATH_MSG_DEBUG( "Sorting track collection (tracksToUseInSecondFit) by pT..." );
    sortTrackCollectionByPt( tracksToUseInSecondFit );

    delete twoTrackVertexCandidates;    
//

//    // One last complication, primaryVertex needs to be a different object (old edm)
//    // see lines 524-537 in old code and create primaryVertexRecVertex using the vertex edm factory

    Trk::RecVertex primaryVertexRecVertex( primaryVertex.position(),
					   primaryVertex.covariancePosition(),
					   primaryVertex.numberDoF(),
					   primaryVertex.chiSquared());


    // Refactoring: Instead at this point use JetFitterMultiStageFit:
    Trk::VxJetCandidate* myJetCandidate= m_multiStageFitter->doTwoStageFit(primaryVertexRecVertex,
                                                                           jetMomentum,
                                                                           tracksToUseInFirstFit,
                                                                           tracksToUseInSecondFit,
                                                                           JFseedDirection);


    if (myJetCandidate == 0) {
      ATH_MSG_DEBUG( "JetFitter multi stage fit returned 0" );
      return 0;
    }
    else {
      ATH_MSG_DEBUG( "USING JetFitterMultiStageFit return" );
    }

//    // Lastly create the jetfitter info, ('save' our results)

    std::vector<Trk::VxJetCandidate*> myCandidates;
    // This push_back is problematic for the migration to xAOD::Vertex, it works simply because VxJetCandidate inherits from VxCandidate
    myCandidates.push_back(myJetCandidate);

    Trk::VxJetFitterVertexInfo* myOutputInfo=new Trk::VxJetFitterVertexInfo(myCandidates,
                                                                            myTwoTrackVerticesInJet,
                                                                            mySelectedTracksInJet);

    myOutputInfo->setSVOwnership(true); 
    return myOutputInfo;
  }
  
    

  Trk::VxJetCandidate* InDetImprovedJetFitterVxFinder::findSecVertex(const Trk::RecVertex & /*primaryVertex*/,
								     const TLorentzVector & /*jetMomentum*/,
                                                                     const std::vector<const Trk::ITrackLink*> & /*firstInputTracks*/,
                                                                     const std::vector<const Trk::ITrackLink*> & /*secondInputTracks*/,
                                                                     const Amg::Vector3D & /*vtxSeedDirection*/ ) const 
  {

    // The overload is legacy. This is where we interface to JetFitter proper
    // m_jetFitterMultiStageFit->performTwoStageFit(const Trk::RecVertex & primaryVertex,
      //                                                  const TLorentzVector & jetMomentum,
      //                                                  const std::vector<const Trk::ITrackLink*> & firstInputTracks,
      //                                                  const std::vector<const Trk::ITrackLink*> & secondInputTracks,
      //                                                  const Amg::Vector3D & vtxSeedDirection

    // return myOutputInfo
    return 0;
    
  }

  void InDetImprovedJetFitterVxFinder::doTheFit(Trk::VxJetCandidate* /*myJetCandidate*/,
                                                bool /*performClustering*/ ) const {

  }

  void InDetImprovedJetFitterVxFinder::sortTrackCollectionByPt( std::vector< const Trk::ITrackLink* >& trackCollection ) const {

    if ( trackCollection.empty() ) return;

    ATH_MSG_DEBUG("re-order the tracks by pT: " << 
		  ( trackCollection.front()->parameters() )->pT() <<
		  " -- " << 
		  ( trackCollection.back()->parameters() )->pT()  );

    std::sort( trackCollection.begin(),
	       trackCollection.end(),
	       //Lambda func to sort by pt                                                                                                                                                                               
	       [](const Trk::ITrackLink* TL1, const Trk::ITrackLink* TL2) {
		 return ( (TL1->parameters())->pT() < (TL2->parameters())->pT() );
	       } );

    ATH_MSG_DEBUG("after re-order the tracks by pT: " <<
		  ( trackCollection.front()->parameters() )->pT() <<
		  " -- " <<
		  ( trackCollection.back()->parameters() )->pT()  );
    
  }


}//end namespace Rec

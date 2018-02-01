/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                         InDetPriVxFinder.cxx  -  Description
                             -------------------
    begin   : 28-01-2004
    authors : Andreas Wildauer (CERN PH-ATC), Fredrik Akesson (CERN PH-ATC)
    email   : andreas.wildauer@cern.ch, fredrik.akesson@cern.ch
    changes :
 ***************************************************************************/
#include "InDetPriVxFinder/InDetPriVxFinder.h"
// forward declares
#include "InDetRecToolInterfaces/IVertexFinder.h"
#include "TrkVertexFitterInterfaces/IVertexMergingTool.h"
#include "TrkVertexFitterInterfaces/IVertexCollectionSortingTool.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleAuxContainer.h"

// normal includes
#include "TrkParticleBase/TrackParticleBaseCollection.h"
#include "TrkVxEdmCnv/IVxCandidateXAODVertex.h"

namespace InDet
{

  InDetPriVxFinder::InDetPriVxFinder ( const std::string &n, ISvcLocator *pSvcLoc )
    : AthAlgorithm ( n, pSvcLoc ),
      m_VertexFinderTool ( "InDet::InDetPriVxFinderTool" ),
      m_VertexMergingTool( "Trk::VertexMergingTool" ),
      m_VertexCollectionSortingTool ("Trk::VertexCollectionSortingTool"),
      m_VertexEdmFactory("Trk::VertexInternalEdmFactory"),
      m_doVertexMerging(false),
      m_doVertexSorting(false),
      m_useTrackParticles(true),
      // for summary output at the end
      m_numEventsProcessed(0),
      m_totalNumVerticesWithoutDummy(0)

  {
    declareProperty ( "VertexFinderTool",m_VertexFinderTool );
    declareProperty ( "VertexMergingTool",m_VertexMergingTool );
    declareProperty ( "VertexCollectionSortingTool",m_VertexCollectionSortingTool );
    declareProperty ( "InternalEdmFactory", m_VertexEdmFactory);
    declareProperty ( "doVertexMerging",m_doVertexMerging );
    declareProperty ( "doVertexSorting",m_doVertexSorting );
    declareProperty ( "useTrackParticles", m_useTrackParticles);
  }

  InDetPriVxFinder::~InDetPriVxFinder()
  {}

  StatusCode InDetPriVxFinder::initialize()
  {
    /* Get the VertexFinderTool */
    if ( m_VertexFinderTool.retrieve().isFailure() )
    {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_VertexFinderTool << endmsg;
      return StatusCode::FAILURE;
    }
    else
    {
      msg(MSG::INFO) << "Retrieved tool " << m_VertexFinderTool << endmsg;
    }

    /*Get the Vertex Mergin Tool*/
    if (m_doVertexMerging)
    {
      if ( m_VertexMergingTool.retrieve().isFailure() )
      {
        msg(MSG::FATAL) << "Failed to retrieve tool " << m_VertexMergingTool << endmsg;
        return StatusCode::FAILURE;
      }
      else
      {
        msg(MSG::INFO) << "Retrieved tool " << m_VertexMergingTool << endmsg;
      }
    }

    /*Get the Vertex Collection Sorting Tool*/
    if (m_doVertexSorting)
    {
      if ( m_VertexCollectionSortingTool.retrieve().isFailure() )
      {
        msg(MSG::FATAL) << "Failed to retrieve tool " << m_VertexCollectionSortingTool << endmsg;
        return StatusCode::FAILURE;
      }
      else
      {
        msg(MSG::INFO) << "Retrieved tool " << m_VertexCollectionSortingTool << endmsg;
      }
    }
    if ( m_VertexEdmFactory.retrieve().isFailure() ) {
      ATH_MSG_ERROR("Failed to retrievel tool " << m_VertexEdmFactory);
      return StatusCode::FAILURE;
    }
   
    ATH_CHECK(m_trkTracksName.initialize(!m_useTrackParticles));
    ATH_CHECK(m_tracksName.initialize(m_useTrackParticles));
    ATH_CHECK(m_vxCandidatesOutputName.initialize());


    msg(MSG::INFO) << "Initialization successful" << endmsg;
    return StatusCode::SUCCESS;
  }


  StatusCode InDetPriVxFinder::execute()
  {
    m_numEventsProcessed++;

    SG::WriteHandle<xAOD::VertexContainer> outputVertices (m_vxCandidatesOutputName);

    xAOD::VertexContainer* theXAODContainer = 0;
    xAOD::VertexAuxContainer* theXAODAuxContainer = 0;
    std::pair<xAOD::VertexContainer*,xAOD::VertexAuxContainer*> theXAODContainers
	= std::make_pair( theXAODContainer, theXAODAuxContainer );

    if(m_useTrackParticles){
      SG::ReadHandle<xAOD::TrackParticleContainer> trackParticleCollection(m_tracksName);
      if(trackParticleCollection.isValid()){
	theXAODContainers = m_VertexFinderTool->findVertex ( trackParticleCollection.cptr() );
      }
      else{
	ATH_MSG_DEBUG("No TrackParticle Collection with key "<<m_tracksName.key()<<" exists in StoreGate. No Vertexing Possible");
	return StatusCode::SUCCESS;
      }
    }
    else{
      SG::ReadHandle<TrackCollection> trackCollection(m_trkTracksName);
      if(trackCollection.isValid()){
	theXAODContainers = m_VertexFinderTool->findVertex ( trackCollection.cptr() );
      }
      else{
	ATH_MSG_DEBUG("No Trk::Track Collection with key "<<m_trkTracksName.key()<<" exists in StoreGate. No Vertexing Possible");
	return StatusCode::SUCCESS;
      }

    }

    // now  re-merge and resort the vertex container and store to SG
    xAOD::VertexContainer* myVertexContainer = 0;
    xAOD::VertexAuxContainer* myVertexAuxContainer = 0;
    std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> myVxContainers
      = std::make_pair( myVertexContainer, myVertexAuxContainer );
    
    
    if (theXAODContainers.first) {
      //sort xAOD::Vertex container
      
      if(m_doVertexMerging && theXAODContainers.first->size() > 1) {
        myVxContainers = m_VertexMergingTool->mergeVertexContainer( *theXAODContainers.first );
        delete theXAODContainers.first; //also cleans up the aux store
        delete theXAODContainers.second; 
       	theXAODContainers = myVxContainers;
      }
      
      if (m_doVertexSorting && theXAODContainers.first->size() > 1) {	
	myVxContainers = m_VertexCollectionSortingTool->sortVertexContainer(*theXAODContainers.first);
	delete theXAODContainers.first; //also cleans up the aux store
        delete theXAODContainers.second; 
      } else {
	myVxContainers.first = theXAODContainers.first;
	myVxContainers.second = theXAODContainers.second;
      }
      
      if (myVxContainers.first == 0) {
	ATH_MSG_WARNING("Vertex container has no associated store.");
	return StatusCode::SUCCESS;
      }
      
      if (not myVxContainers.first->hasStore()) {
	ATH_MSG_WARNING("Vertex container has no associated store.");
	return StatusCode::SUCCESS;
      }
      
      ATH_MSG_DEBUG("Successfully reconstructed " << myVxContainers.first->size()-1 << " vertices (excluding dummy)");
      m_totalNumVerticesWithoutDummy += (myVxContainers.first->size()-1); 
      
    }
    
    ATH_CHECK(outputVertices.record(std::unique_ptr<xAOD::VertexContainer>(myVxContainers.first),std::unique_ptr<xAOD::VertexAuxContainer>(myVxContainers.second)));
    
    
    ATH_MSG_DEBUG( "Recorded Vertices with key: " << m_vxCandidatesOutputName.key() );
    
    return StatusCode::SUCCESS;
  }
  
  StatusCode InDetPriVxFinder::finalize()
  {
    if (msgLvl(MSG::INFO))
      {
	msg() << "Summary from Primary Vertex Finder (InnerDetector/InDetRecAlgs/InDetPriVxFinder)" << endmsg;
	msg() << "=== " << m_totalNumVerticesWithoutDummy << " vertices recoed in " << m_numEventsProcessed << " events (excluding dummy)." << endmsg;
	if (m_numEventsProcessed!=0) msg() << "=== " << double(m_totalNumVerticesWithoutDummy)/double(m_numEventsProcessed) << " vertices per event (excluding dummy)." << endmsg;
      } 
    return StatusCode::SUCCESS;
  }
  
} // end namespace InDet

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
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"

// normal includes
#include "TrkTrack/TrackCollection.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h"
#include "TrkVxEdmCnv/IVxCandidateXAODVertex.h"

namespace InDet
{

  InDetPriVxFinder::InDetPriVxFinder ( const std::string &n, ISvcLocator *pSvcLoc )
    : AthAlgorithm ( n, pSvcLoc ),
      m_tracksName ( "Tracks" ),
      m_vxCandidatesOutputName ( "PrimaryVertices" ),
      m_vxCandidatesOutputNameAuxPostfix ( "Aux." ),
      m_VertexFinderTool ( "InDet::InDetPriVxFinderTool" ),
      m_VertexMergingTool( "Trk::VertexMergingTool" ),
      m_VertexCollectionSortingTool ("Trk::VertexCollectionSortingTool"),
      m_VertexEdmFactory("Trk::VertexInternalEdmFactory"),
      m_doVertexMerging(false),
      m_doVertexSorting(false),
      // for summary output at the end
      m_numEventsProcessed(0),
      m_totalNumVerticesWithoutDummy(0)

  {
    declareProperty ( "TracksName",m_tracksName );
    declareProperty ( "VxCandidatesOutputName",m_vxCandidatesOutputName );
    declareProperty ( "VxCandidatesOutputNameAuxPostfix",m_vxCandidatesOutputNameAuxPostfix );
    declareProperty ( "VertexFinderTool",m_VertexFinderTool );
    declareProperty ( "VertexMergingTool",m_VertexMergingTool );
    declareProperty ( "VertexCollectionSortingTool",m_VertexCollectionSortingTool );
    declareProperty ( "InternalEdmFactory", m_VertexEdmFactory);
    declareProperty ( "doVertexMerging",m_doVertexMerging );
    declareProperty ( "doVertexSorting",m_doVertexSorting );
  }

  InDetPriVxFinder::~InDetPriVxFinder()
  {}

  StatusCode InDetPriVxFinder::initialize()
  {
    /* Get the VertexFinderTool */
    if ( m_VertexFinderTool.retrieve().isFailure() )
    {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_VertexFinderTool << endreq;
      return StatusCode::FAILURE;
    }
    else
    {
      msg(MSG::INFO) << "Retrieved tool " << m_VertexFinderTool << endreq;
    }

    /*Get the Vertex Mergin Tool*/
    if (m_doVertexMerging)
    {
      if ( m_VertexMergingTool.retrieve().isFailure() )
      {
        msg(MSG::FATAL) << "Failed to retrieve tool " << m_VertexMergingTool << endreq;
        return StatusCode::FAILURE;
      }
      else
      {
        msg(MSG::INFO) << "Retrieved tool " << m_VertexMergingTool << endreq;
      }
    }

    /*Get the Vertex Collection Sorting Tool*/
    if (m_doVertexSorting)
    {
      if ( m_VertexCollectionSortingTool.retrieve().isFailure() )
      {
        msg(MSG::FATAL) << "Failed to retrieve tool " << m_VertexCollectionSortingTool << endreq;
        return StatusCode::FAILURE;
      }
      else
      {
        msg(MSG::INFO) << "Retrieved tool " << m_VertexCollectionSortingTool << endreq;
      }
    }
    if ( m_VertexEdmFactory.retrieve().isFailure() ) {
      ATH_MSG_ERROR("Failed to retrievel tool " << m_VertexEdmFactory);
      return StatusCode::FAILURE;
    }
   

    msg(MSG::INFO) << "Initialization successful" << endreq;
    return StatusCode::SUCCESS;
  }


  StatusCode InDetPriVxFinder::execute()
  {
    m_numEventsProcessed++;

    //VxContainer* theVxContainer ( 0 );

    xAOD::VertexContainer* theXAODContainer = 0;
    xAOD::VertexAuxContainer* theXAODAuxContainer = 0;
    std::pair<xAOD::VertexContainer*,xAOD::VertexAuxContainer*> theXAODContainers
	= std::make_pair( theXAODContainer, theXAODAuxContainer );

    //---- First try if m_tracksName is a TrackCollection -----------------//
    if ( evtStore()->contains<TrackCollection> ( m_tracksName ) )
    {
      const TrackCollection *trackTES ( 0 );
      if ( evtStore()->retrieve ( trackTES, m_tracksName ).isFailure() )
      {
        if (msgLvl(MSG::DEBUG)) msg() << "Could not find TrackCollection " << m_tracksName << " in StoreGate." << endreq;
        return StatusCode::SUCCESS;
      }
      theXAODContainers = m_VertexFinderTool->findVertex ( trackTES );
    }
    //---- Second try if m_tracksName is a xAOD::TrackParticleContainer ----//
    else if ( evtStore()->contains<xAOD::TrackParticleContainer> ( m_tracksName ) )
    {
      const xAOD::TrackParticleContainer *trackParticleCollection(0);
      if ( evtStore()->retrieve ( trackParticleCollection, m_tracksName ).isFailure() )
      {
        if (msgLvl(MSG::DEBUG)) msg() << "Could not find xAOD::TrackParticleContainer " << m_tracksName << " in StoreGate." << endreq;
        return StatusCode::SUCCESS;
      }
      theXAODContainers = m_VertexFinderTool->findVertex ( trackParticleCollection );
    }
    //---- Third try if m_tracksName is a TrackParticleBaseCollection ----//
    else if ( evtStore()->contains<Trk::TrackParticleBaseCollection> ( m_tracksName ) )
    {
      const Trk::TrackParticleBaseCollection *trackParticleBaseCollection(0);
      if ( evtStore()->retrieve ( trackParticleBaseCollection, m_tracksName ).isFailure() )
      {
        if (msgLvl(MSG::DEBUG)) msg() << "Could not find Trk::TrackParticleBaseCollection " << m_tracksName << " in StoreGate." << endreq;
        return StatusCode::SUCCESS;
      }

      theXAODContainers = m_VertexFinderTool->findVertex ( trackParticleBaseCollection );
    }
    else
    {
      ATH_MSG_WARNING("Neither a TrackCollection nor a xAOD::TrackParticleContainer nor a TrackParticleBaseCollection with key " << m_tracksName << " exists in StoreGate. No vertexing possible.");
      return StatusCode::SUCCESS;
    }

    // now  re-merge and resort the vertex container and store to SG
    xAOD::VertexContainer* myVertexContainer = 0;
    xAOD::VertexAuxContainer* myVertexAuxContainer = 0;
    std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> myVxContainers
	= std::make_pair( myVertexContainer, myVertexAuxContainer );

    //VxContainer* MyTrackVxContainer = 0;

    if (theXAODContainers.first) {
      //sort xAOD::Vertex container

      if(m_doVertexMerging && theXAODContainers.first->size() > 1) {
        myVxContainers = m_VertexMergingTool->mergeVertexContainer( *theXAODContainers.first );
        //now delete and copy over theXAODContainer so sorting will still work
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

      m_totalNumVerticesWithoutDummy += (myVxContainers.first->size()-1); 
      
      std::string vxContainerAuxName = m_vxCandidatesOutputName + m_vxCandidatesOutputNameAuxPostfix;
      //---- Recording section: write the results to StoreGate ---//
     
      if (evtStore()->contains<xAOD::VertexContainer>(m_vxCandidatesOutputName)) {   
	CHECK( evtStore()->overwrite( myVxContainers.first, m_vxCandidatesOutputName,true,false) );
	CHECK( evtStore()->overwrite( myVxContainers.second, vxContainerAuxName,true,false) );	
	ATH_MSG_DEBUG( "Overwrote Vertices with key: " << m_vxCandidatesOutputName);
      }
      else{
	CHECK(evtStore()->record ( myVxContainers.first, m_vxCandidatesOutputName,false ));
	CHECK(evtStore()->record ( myVxContainers.second, vxContainerAuxName ));
	ATH_MSG_DEBUG( "Recorded Vertices with key: " << m_vxCandidatesOutputName );
      }
    }
    else { //theXAODContainer is valid
      ATH_MSG_ERROR("Unexpected error. Invalid output containers.");
      return StatusCode::FAILURE;
    }

    if( myVxContainers.first != 0 )
    {
      ATH_MSG_DEBUG("Successfully reconstructed " << myVxContainers.first->size()-1 << " vertices (excluding dummy)");
    }

    return StatusCode::SUCCESS;
  } 
  
  StatusCode InDetPriVxFinder::finalize()
  {
    if (msgLvl(MSG::INFO))
    {
      msg() << "Summary from Primary Vertex Finder (InnerDetector/InDetRecAlgs/InDetPriVxFinder)" << endreq;
      msg() << "=== " << m_totalNumVerticesWithoutDummy << " vertices recoed in " << m_numEventsProcessed << " events (excluding dummy)." << endreq;
      if (m_numEventsProcessed!=0) msg() << "=== " << double(m_totalNumVerticesWithoutDummy)/double(m_numEventsProcessed) << " vertices per event (excluding dummy)." << endreq;
    } 
    return StatusCode::SUCCESS;
  }
  
} // end namespace InDet

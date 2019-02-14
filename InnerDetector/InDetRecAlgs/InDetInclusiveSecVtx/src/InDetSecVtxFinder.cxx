/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                         InDetSecVtxFinder.cxx  -  Description
                             -------------------
    begin   : Nov 10, 2016
    authors : Lianyou SHAN ( IHEP-Beijing )
    email   : shanly@mail.ihep.ac.cn
    changes : Almost nothing changed from InDetPriVxFinder : primary vertex is interfaced
 ***************************************************************************/
#include "InDetInclusiveSecVtx/InDetSecVtxFinder.h"

// forward declares
#include "InDetRecToolInterfaces/IInDetIterativeSecVtxFinderTool.h"
#include "TrkVertexFitterInterfaces/IVertexMergingTool.h"
//#include "TrkVertexFitterInterfaces/IVertexCollectionSortingTool.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleAuxContainer.h"

// normal includes
#include "TrkParticleBase/TrackParticleBaseCollection.h"

namespace InDet
{

  InDetSecVtxFinder::InDetSecVtxFinder ( const std::string &n, ISvcLocator *pSvcLoc )
    : AthAlgorithm ( n, pSvcLoc ),
      m_VertexFinderTool ( "InDet::InDetSecVtxFinderTool", this ),
      m_VertexMergingTool( "Trk::SecVertexMergingTool", this ),
      m_doVertexMerging(false),
      m_useTrackParticles(true),
      // for summary output at the end
      m_numEventsProcessed(0),
      m_totalNumVerticesWithoutDummy(0)

  {
    declareProperty ( "VertexFinderTool",m_VertexFinderTool );
    declareProperty ( "VertexMergingTool",m_VertexMergingTool );
    declareProperty ( "doVertexMerging",m_doVertexMerging );
    declareProperty ( "useTrackParticles", m_useTrackParticles);
  }

  InDetSecVtxFinder::~InDetSecVtxFinder()
  {}

  StatusCode InDetSecVtxFinder::initialize()
  {
    /* Get the VertexFinderTool */
    if ( m_VertexFinderTool.retrieve().isFailure() )
    {
      ATH_MSG_ERROR("Failed to retrieve tool " << m_VertexFinderTool);

      return StatusCode::FAILURE;
    }
    else
    {
      ATH_MSG_INFO("Retrieved tool " << m_VertexFinderTool);
    }

    /*Get the Vertex Mergin Tool*/
    if (m_doVertexMerging)
    {
      if ( m_VertexMergingTool.retrieve().isFailure() )
      {
	ATH_MSG_ERROR("Failed to retrieve tool " << m_VertexMergingTool);
   
        return StatusCode::FAILURE;
      }
      else
      {
	ATH_MSG_INFO("Retrieved tool " << m_VertexMergingTool);
      }
    }

    /**  There is no good motivation to do Vertex Collection Sorting yet **/

    ATH_CHECK(m_trkTracksName.initialize(!m_useTrackParticles));
    ATH_CHECK(m_tracksName.initialize(m_useTrackParticles));
    ATH_CHECK(m_vxCandidatesOutputName.initialize());
    ATH_CHECK(m_PrimaryVxInputName.initialize());
   
    ATH_MSG_INFO("Initialization successful");
  
    return StatusCode::SUCCESS;
  }


  StatusCode InDetSecVtxFinder::execute()
  {
    m_numEventsProcessed++;

    SG::WriteHandle<xAOD::VertexContainer> outputVertices (m_vxCandidatesOutputName);

    xAOD::VertexContainer* theXAODContainer = 0;
    xAOD::VertexAuxContainer* theXAODAuxContainer = 0;
    std::pair<xAOD::VertexContainer*,xAOD::VertexAuxContainer*> theXAODContainers
	= std::make_pair( theXAODContainer, theXAODAuxContainer );

    // retrieve the PRIMARY Vertex
    SG::ReadHandle<xAOD::VertexContainer> vtxCont(m_PrimaryVxInputName);
    if(vtxCont.isValid()){
      const xAOD::Vertex *privtx = static_cast< const xAOD::Vertex * >( *(vtxCont->begin()) );
      if (  ! (    privtx->vertexType() == xAOD::VxType::PriVtx
                && privtx->nTrackParticles() >= 2
                && privtx->isAvailable<float> ("z")
              )
         ) ATH_MSG_WARNING(" Illed Primary vertex, keeping privtx_z0 = 0  ");
      else
        m_VertexFinderTool->setPriVtxPosition( privtx->position().x(), 
                                          privtx->position().y(), 
                                          privtx->position().z() 
                                        ) ;
    }

    else{
      ATH_MSG_WARNING("couldn't retrieve Primary vertex, keeping privtx_z0 = 0 ");
    }

 
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

      if( m_doVertexMerging && theXAODContainers.first->size() > 1) {
        myVxContainers = m_VertexMergingTool->mergeVertexContainer( *theXAODContainers.first );
        delete theXAODContainers.first; //also cleans up the aux store
        delete theXAODContainers.second; 
        theXAODContainers = myVxContainers;
      }
      
      myVxContainers.first = theXAODContainers.first;
      myVxContainers.second = theXAODContainers.second;

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
  
  StatusCode InDetSecVtxFinder::finalize()
  {
    ATH_MSG_INFO("Summary from Inclusive Secondary Vertex Finder (InnerDetector/InDetRecAlgs/InclusiveSecVtx)");
    ATH_MSG_INFO("=== " << m_totalNumVerticesWithoutDummy << " vertices recoed in " << m_numEventsProcessed << " events (excluding dummy).");
 
      if (m_numEventsProcessed!=0) 
	{
	  ATH_MSG_INFO( "=== " << double(m_totalNumVerticesWithoutDummy)/double(m_numEventsProcessed) << " vertices per event (excluding dummy).");
	}
    
    return StatusCode::SUCCESS;
  }
  
} // end namespace InDet

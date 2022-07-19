/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                         InDetSecVtxFinder.cxx  -  Description
                             -------------------
    begin   : Nov 10, 2016
    authors : Lianyou SHAN ( IHEP-Beijing ) Neža Ribarič (Lancaster university, UK)
    email   : shanly@mail.ihep.ac.cn, neza.ribaric@cern.ch
    changes : Changed name from InDetInclusiveSecVtx, added the option to chose the vertexing tool (ISV,AMVF) 
 ***************************************************************************/
#include "InDetSecVtxFinder/InDetSecVtxFinder.h"


// forward declares

#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "VxSecVertex/VxSecVertexInfo.h"
// normal includes
#include "TrkParticleBase/TrackParticleBaseCollection.h"

namespace InDet
{

  InDetSecVtxFinder::InDetSecVtxFinder ( const std::string &n, ISvcLocator *pSvcLoc )
    : AthAlgorithm ( n, pSvcLoc ),
      
      // for summary output at the end
      m_numEventsProcessed(0),
      m_totalNumVerticesWithoutDummy(0)

  {}

  

  StatusCode InDetSecVtxFinder::initialize()
  {
    /* Get the VertexFinderTool */
    
    ATH_CHECK(m_InclusiveVertexFinderTool.retrieve());
    
    ATH_CHECK( m_AdaptiveMultiVertexFinderTool.retrieve());
    
    
    /*Get the Vertex Merging Tool*/
    if (m_doVertexMerging)
    {
      if(m_FinderTool == "AMVF"){ 
        ATH_MSG_ERROR("AMVF finding and vertex merging is not possible");
        return StatusCode::FAILURE;
      }  
      ATH_CHECK( m_VertexMergingTool.retrieve());
      
    }

    /**  There is no good motivation to do Vertex Collection Sorting yet **/

    ATH_CHECK(m_inputTrackCollection.initialize(!m_useTrackParticles));
    ATH_CHECK(m_inputTrackParticles.initialize(m_useTrackParticles));
    ATH_CHECK(m_outputSecondaryVertices.initialize());
    ATH_CHECK(m_inputPrimaryVertices.initialize());
   
    ATH_MSG_DEBUG("Initialization successful");
  
    return StatusCode::SUCCESS;
  }


  StatusCode InDetSecVtxFinder::execute()
  {
    m_numEventsProcessed++;

    const EventContext& ctx = Gaudi::Hive::currentContext();

    SG::WriteHandle<xAOD::VertexContainer> outputVertices (m_outputSecondaryVertices, ctx);

    xAOD::VertexContainer* theXAODContainer = nullptr;
    xAOD::VertexAuxContainer* theXAODAuxContainer = nullptr;
    std::pair<xAOD::VertexContainer*,xAOD::VertexAuxContainer*> theXAODContainers
    = std::make_pair( theXAODContainer, theXAODAuxContainer );

    // retrieve the PRIMARY Vertex

    SG::ReadHandle<xAOD::VertexContainer> vtxCont(m_inputPrimaryVertices, ctx);
    
    if(vtxCont.isValid()){
      const xAOD::Vertex *privtx = static_cast< const xAOD::Vertex * >( *(vtxCont->begin()) );
      if(!( privtx->vertexType() == xAOD::VxType::PriVtx && privtx->nTrackParticles() >= 2 && privtx->isAvailable<float> ("z"))){
        ATH_MSG_WARNING(" Illed Primary vertex, keeping privtx_z0 = 0  ");
      }
      else{
        if(m_FinderTool == "ISV"){ 
          m_InclusiveVertexFinderTool->setPriVtxPosition( privtx->position().x(), privtx->position().y(), privtx->position().z());
        }
        else if(m_FinderTool == "AMVF"){
          m_AdaptiveMultiVertexFinderTool->setPrimaryVertexPosition( privtx->position().x(), privtx->position().y(), privtx->position().z());
        } 
        else{
          ATH_MSG_WARNING("Please specify a valid FinderTool");
        }
      }  
    }

    else{
      ATH_MSG_WARNING("couldn't retrieve Primary vertex, keeping privtx_z0 = 0 ");
    }

    std::unique_ptr<Trk::VxSecVertexInfo> foundVrts;
    if(m_useTrackParticles){
      SG::ReadHandle<xAOD::TrackParticleContainer> trackParticleCollection(m_inputTrackParticles, ctx);
      if(trackParticleCollection.isValid()){
        
        if(m_FinderTool == "ISV"){
          theXAODContainers = m_InclusiveVertexFinderTool->findVertex ( trackParticleCollection.cptr());
        }
        else if(m_FinderTool == "AMVF"){
          theXAODContainers = m_AdaptiveMultiVertexFinderTool->findVertex ( trackParticleCollection.cptr());
        }
        else{
          ATH_MSG_WARNING("Please specify a Finder Tool");
        }
        
      }
      else{
        ATH_MSG_DEBUG("No TrackParticle Collection with key "<<m_inputTrackParticles.key()<<" exists in StoreGate. No Vertexing Possible");
        return StatusCode::SUCCESS;
      }
    }
    else{
      SG::ReadHandle<TrackCollection> trackCollection(m_inputTrackCollection, ctx);
      if(trackCollection.isValid()){
        
        if(m_FinderTool == "ISV"){
          theXAODContainers = m_InclusiveVertexFinderTool->findVertex ( trackCollection.cptr() );
        }
        else{
          ATH_MSG_WARNING("Please use ISV for vertex finding with trackCollection ");
        }
        
        
      }
      else{
        ATH_MSG_DEBUG("No Trk::Track Collection with key "<< m_inputTrackCollection.key()<<" exists in StoreGate. No Vertexing Possible");
        return StatusCode::SUCCESS;
      }

    }
  
    
    // now  re-merge and resort the vertex container and store to SG
    xAOD::VertexContainer* myVertexContainer = nullptr;
    xAOD::VertexAuxContainer* myVertexAuxContainer = nullptr;
    std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> myVxContainers = std::make_pair( myVertexContainer, myVertexAuxContainer );
    ATH_MSG_DEBUG("Vertexing done, sorting the vertex container");
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
  
      
    ATH_MSG_DEBUG( "Recorded Vertices with key: " << m_outputSecondaryVertices.key() );

    return StatusCode::SUCCESS;
  } 
  
  StatusCode InDetSecVtxFinder::finalize()
  {
    ATH_MSG_DEBUG("Summary from Secondary Vertex Finder (InnerDetector/InDetRecAlgs/InDetSecVtxFinder)");
    ATH_MSG_DEBUG("=== " << m_totalNumVerticesWithoutDummy << " vertices recoed in " << m_numEventsProcessed << " events (excluding dummy).");
 
    if (m_numEventsProcessed!=0) {
      ATH_MSG_DEBUG( "=== " << double(m_totalNumVerticesWithoutDummy)/double(m_numEventsProcessed) << " vertices per event (excluding dummy).");
    }
    
    return StatusCode::SUCCESS;
  }
  
} // end namespace InDet

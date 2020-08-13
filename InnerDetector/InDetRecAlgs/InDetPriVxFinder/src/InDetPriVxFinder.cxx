/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "AthenaMonitoringKernel/Monitored.h"

// normal includes
#include "TrkParticleBase/TrackParticleBaseCollection.h"

namespace InDet
{

InDetPriVxFinder::InDetPriVxFinder
 (const std::string& name,ISvcLocator* pSvcLocator) : AthReentrantAlgorithm(name, pSvcLocator)
 { }


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

    /*Get the Vertex Merging Tool*/
    if (m_doVertexMerging) {
      if ( m_VertexMergingTool.retrieve().isFailure() )
      {
        msg(MSG::FATAL) << "Failed to retrieve tool " << m_VertexMergingTool << endmsg;
        return StatusCode::FAILURE;
      }
      else
      {
        msg(MSG::INFO) << "Retrieved tool " << m_VertexMergingTool << endmsg;
      }
    } else {
      m_VertexMergingTool.disable();
    }

    /*Get the Vertex Collection Sorting Tool*/
    if (m_doVertexSorting) {
      if ( m_VertexCollectionSortingTool.retrieve().isFailure() )
      {
        msg(MSG::FATAL) << "Failed to retrieve tool " << m_VertexCollectionSortingTool << endmsg;
        return StatusCode::FAILURE;
      }
      else
      {
        msg(MSG::INFO) << "Retrieved tool " << m_VertexCollectionSortingTool << endmsg;
      }
    } else {
      m_VertexCollectionSortingTool.disable();
    }
    if (!m_monTool.empty()) CHECK(m_monTool.retrieve());
   
    ATH_CHECK(m_trkTracksName.initialize(!m_useTrackParticles));
    ATH_CHECK(m_tracksName.initialize(m_useTrackParticles));
    ATH_CHECK(m_vxCandidatesOutputName.initialize());

    msg(MSG::INFO) << "Initialization successful" << endmsg;
    return StatusCode::SUCCESS;
  }


  StatusCode InDetPriVxFinder::execute(const EventContext& ctx) const
  {

    SG::WriteHandle<xAOD::VertexContainer> outputVertices (m_vxCandidatesOutputName, ctx);

    xAOD::VertexContainer*    vertexContainer = 0;
    xAOD::VertexAuxContainer* vertexAuxContainer = 0;
    std::pair< xAOD::VertexContainer*, xAOD::VertexAuxContainer* > vertexContainerPair
	= std::make_pair( vertexContainer, vertexAuxContainer );

    if(m_useTrackParticles){
      SG::ReadHandle<xAOD::TrackParticleContainer> trackParticleCollection(m_tracksName, ctx);
      if(trackParticleCollection.isValid()){

	 vertexContainerPair = m_VertexFinderTool->findVertex ( trackParticleCollection.cptr() );
      }
      else{
	ATH_MSG_ERROR("No TrackParticle Collection with key "<<m_tracksName.key()<<" exists in StoreGate. No Vertexing Possible");
	return StatusCode::FAILURE;
      }
    }
    else{
      SG::ReadHandle<TrackCollection> trackCollection(m_trkTracksName, ctx);
      if(trackCollection.isValid()){
	vertexContainerPair = m_VertexFinderTool->findVertex ( trackCollection.cptr() );
      }
      else{
	ATH_MSG_ERROR("No Trk::Track Collection with key "<<m_trkTracksName.key()<<" exists in StoreGate. No Vertexing Possible");
	return StatusCode::FAILURE;
      }

    }

    // now  re-merge and resort the vertex container and store to SG
    xAOD::VertexContainer* myVertexContainer = 0;
    xAOD::VertexAuxContainer* myVertexAuxContainer = 0;
    std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*  > myVertexContainerPair
      = std::make_pair( myVertexContainer, myVertexAuxContainer );
        
    if (vertexContainerPair.first) {
      //sort xAOD::Vertex container
      
      if(m_doVertexMerging && vertexContainerPair.first->size() > 1) {
        myVertexContainerPair = m_VertexMergingTool->mergeVertexContainer( *vertexContainerPair.first );
        delete vertexContainerPair.first; //also cleans up the aux store
        delete vertexContainerPair.second; 
       	vertexContainerPair = myVertexContainerPair;
      }
      
      if (m_doVertexSorting && vertexContainerPair.first->size() > 1) {	
	myVertexContainerPair = m_VertexCollectionSortingTool->sortVertexContainer(*vertexContainerPair.first);
	delete vertexContainerPair.first; //also cleans up the aux store
        delete vertexContainerPair.second; 
      } else {
	myVertexContainerPair.first = vertexContainerPair.first;
	myVertexContainerPair.second = vertexContainerPair.second;
      }
      
      if (myVertexContainerPair.first == 0) {
	ATH_MSG_ERROR("Vertex container has no associated store.");
	return StatusCode::FAILURE;
      }
      
      if (not myVertexContainerPair.first->hasStore()) {
	ATH_MSG_ERROR("Vertex container has no associated store.");
	return StatusCode::FAILURE;
      }
      
      ATH_MSG_DEBUG("Successfully reconstructed " << myVertexContainerPair.first->size()-1 << " vertices (excluding dummy)");
    }

    ATH_CHECK(outputVertices.record(std::unique_ptr<xAOD::VertexContainer>(myVertexContainerPair.first),std::unique_ptr<xAOD::VertexAuxContainer>(myVertexContainerPair.second)));

    auto NVertices = Monitored::Scalar<int>( "NVertices" , 0 );
    for ( xAOD::VertexContainer::iterator vertexIter = myVertexContainerPair.first->begin();
          vertexIter != myVertexContainerPair.first->end(); ++vertexIter ) {
        if((*vertexIter)->nTrackParticles() > 0 and (*vertexIter)->vertexType() != 0 ){
            NVertices++;
            monitor_vertex( "allVertex", **vertexIter);
            //This expects that vertices are already sorted by SumpT(or different criteria)!!!
            if( vertexIter == myVertexContainerPair.first->begin() ) monitor_vertex( "primVertex", **vertexIter);
        }
    }
    auto mon = Monitored::Group( m_monTool, NVertices);

    return StatusCode::SUCCESS;
  }
  
  StatusCode InDetPriVxFinder::finalize()
  {
    return StatusCode::SUCCESS;
  }

  void InDetPriVxFinder::monitor_vertex( const std::string &prefix, xAOD::Vertex vertex ) const {
     if (prefix == "allVertex"){
         auto x        = Monitored::Scalar<double>( "allVertexX",       vertex.x()               ); 
         auto y        = Monitored::Scalar<double>( "allVertexY",       vertex.y()               ); 
         auto z        = Monitored::Scalar<double>( "allVertexZ",       vertex.z()               ); 
         auto chi2     = Monitored::Scalar<double>( "allVertexChi2",    vertex.chiSquared()      ); 
         auto nDoF     = Monitored::Scalar<double>( "allVertexnDoF",    vertex.numberDoF()       ); 
         auto NTracks  = Monitored::Scalar<int>   ( "allVertexNTracks", vertex.nTrackParticles() );
         auto mon = Monitored::Group(m_monTool,  x, y, z, chi2, nDoF, NTracks );
     }
     else if (prefix == "primVertex"){
         auto x        = Monitored::Scalar<double>( "primVertexX",       vertex.x()               );
         auto y        = Monitored::Scalar<double>( "primVertexY",       vertex.y()               );
         auto z        = Monitored::Scalar<double>( "primVertexZ",       vertex.z()               );
         auto chi2     = Monitored::Scalar<double>( "primVertexChi2",    vertex.chiSquared()      );
         auto nDoF     = Monitored::Scalar<double>( "primVertexnDoF",    vertex.numberDoF()       );
         auto NTracks  = Monitored::Scalar<int>   ( "primVertexNTracks", vertex.nTrackParticles() );
         auto mon = Monitored::Group(m_monTool,  x, y, z, chi2, nDoF, NTracks );
     }
  }
  
} // end namespace InDet

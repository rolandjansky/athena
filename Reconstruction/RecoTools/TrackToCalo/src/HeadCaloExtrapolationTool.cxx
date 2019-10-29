/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
NAME:     HeadCaloExtensionBuilder
PACKAGE:  offline/Reconstruction/RecoTools/TrackToCalo/HeadCaloExtrapolationTool

Based on: offline/Reconstruction/egamma/egammaTrackTools/EMGSFCaloExtensionBuilder

AUTHORS:  Anastopoulos/Capriles
PURPOSE:  Performs Calo Extension for all selected tracks 
 **********************************************************************/
#include "HeadCaloExtrapolationTool.h"
//
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

#include "VxVertex/RecVertex.h"
#include "VxVertex/VxCandidate.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/Vertex.h"

//std includes
#include <algorithm>
#include <cmath>
#include <memory>


Trk::HeadCaloExtensionBuilder::HeadCaloExtensionBuilder(const std::string& name, 
                                                    ISvcLocator* pSvcLocator):
    AthAlgorithm(name, pSvcLocator)
{
}

StatusCode Trk::HeadCaloExtensionBuilder::initialize() 
{
    IToolSvc* myToolSvc;

    ATH_CHECK(m_TrkSelection.retrieve());  
    ATH_CHECK(m_TrkDetailedSelection.retrieve());  
    ATH_CHECK(m_particleCaloExtensionTool.retrieve());

    ATH_CHECK(m_ParticleCacheKey.initialize());
    ATH_CHECK(m_TrkPartContainerKey.initialize());
    ATH_CHECK(m_vertexInputContainer.initialize());

    if (service("ToolSvc", myToolSvc).isFailure()) {
        ATH_MSG_WARNING(" Tool Service Not Found");
        return StatusCode::SUCCESS;
    }

    if(m_TrkSelection.retrieve().isFailure()){
        ATH_MSG_ERROR("initialize: Cannot retrieve " << m_TrkSelection);
        return StatusCode::FAILURE;
    }else {
        ATH_MSG_VERBOSE("Successfully retrieved Extrapolation tool "
                << m_TrkSelection.typeAndName());
    }

    if(m_TrkDetailedSelection.retrieve().isFailure()){
        ATH_MSG_ERROR("initialize: Cannot retrieve " << m_TrkDetailedSelection);
        return StatusCode::FAILURE;
    }else {
        ATH_MSG_VERBOSE("Successfully retrieved Extrapolation tool "
                << m_TrkDetailedSelection.typeAndName());
    }

    if(m_particleCaloExtensionTool.retrieve().isFailure()){
        ATH_MSG_ERROR("initialize: Cannot retrieve " << m_particleCaloExtensionTool);
        return StatusCode::FAILURE;
    } else {
        ATH_MSG_VERBOSE("Successfully retrieved Extrapolation tool "
                << m_particleCaloExtensionTool.typeAndName());
    }
    return StatusCode::SUCCESS;
}  

StatusCode Trk::HeadCaloExtensionBuilder::HeadCaloExtensionBuilder::finalize(){ 
    return StatusCode::SUCCESS;
}

StatusCode Trk::HeadCaloExtensionBuilder::execute()
{
    // defining needed objects
    const xAOD::VertexContainer * vxContainer = 0;
    const xAOD::Vertex*         primaryVertex = 0;

    SG::ReadHandle<xAOD::VertexContainer> vertexInHandle( m_vertexInputContainer );
    SG::ReadHandle<xAOD::TrackParticleContainer> tracks(m_TrkPartContainerKey);

    // checking for tracks and vertices being read correctly
    if (!vertexInHandle.isValid()) {
      ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << vertexInHandle.key());
      return StatusCode::FAILURE;
    }
    if(!tracks.isValid()) {
        ATH_MSG_FATAL("Failed to retrieve TrackParticle container: "<< m_TrkPartContainerKey.key());
        return StatusCode::FAILURE;
    }

    // picking primary vertex
    vxContainer = vertexInHandle.cptr();
    if (vxContainer->size()>0) {
      // simple loop through and get the primary vertex
      xAOD::VertexContainer::const_iterator vxIter    = vxContainer->begin();
      xAOD::VertexContainer::const_iterator vxIterEnd = vxContainer->end();
      for ( size_t ivtx = 0; vxIter != vxIterEnd; ++vxIter, ++ivtx ){
	    // the first and only primary vertex candidate is picked
        if ( (*vxIter)->vertexType() ==  xAOD::VxType::PriVtx){
            primaryVertex = (*vxIter);
            break;
        }
      }
    }
    ATH_MSG_VERBOSE("size of VxPrimaryContainer is: "  << vxContainer->size() );

    // creating and saving the calo extension collection
    SG::WriteHandle<CaloExtensionCollection> lastCache(m_ParticleCacheKey); 
    ATH_CHECK(lastCache.record(std::make_unique<CaloExtensionCollection>()));

    const xAOD::TrackParticleContainer* ptrTracks=tracks.cptr();
    CaloExtensionCollection* ptrPart=lastCache.ptr();
    std::vector<bool> mask (ptrTracks->size(),false);
    for (auto track: *tracks){
      if( static_cast<bool>(m_TrkSelection->accept(*track, nullptr)) || 
          m_TrkDetailedSelection->decision(*track, primaryVertex)    || 
          m_TrkDetailedSelection->decision(*track, (*vxContainer)[0]) ) {
        mask[track->index()] = true;
        // if ( !(static_cast<bool>(m_TrkSelection->accept(*track, nullptr))) &&  m_TrkDetailedSelection->decision(*track, primaryVertex) ){//
        //   ATH_MSG_WARNING("HEADCALODEBUG : Track index " << track->index() << " passed decision but not accept.");//
        // }//
        // if ( !(static_cast<bool>(m_TrkSelection->accept(*track, nullptr))) &&  m_TrkDetailedSelection->decision(*track, (*vxContainer)[0]) ){//
        //   ATH_MSG_WARNING("HEADCALODEBUG : Track index " << track->index() << " passed decision but not accept. (0th elem).");//
        // }//
      }
    }

    ATH_CHECK(m_particleCaloExtensionTool->caloExtensionCollection(*ptrTracks,mask,*ptrPart));

    return StatusCode::SUCCESS;
}
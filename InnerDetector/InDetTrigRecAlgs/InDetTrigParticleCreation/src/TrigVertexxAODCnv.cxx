/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/ITHistSvc.h"

// EDM include(s):
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "VxVertex/VxContainer.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "TrkTrackLink/ITrackLink.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkTrack/LinkToTrack.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

// Local include(s):
#include "InDetTrigParticleCreation/TrigVertexxAODCnv.h"

namespace InDet {

  TrigVertexxAODCnv::TrigVertexxAODCnv( const std::string& name,
    ISvcLocator* pSvcLocator )
  : HLT::FexAlgo( name, pSvcLocator ) 
  {
    declareProperty( "InputVxContainerKey",   m_VxContName = "PrimVx" );
    declareProperty( "OutputVxContainerKey",  m_xVxContName = "xPrimVx" );
  }

  HLT::ErrorCode TrigVertexxAODCnv::hltInitialize() 
  {
    ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
    ATH_MSG_INFO( "Input Vertex Container  = " << m_VxContName );
    ATH_MSG_INFO( "Output Vertex Container = " << m_xVxContName );
    return HLT::OK;
  }

  HLT::ErrorCode TrigVertexxAODCnv::hltExecute(const HLT::TriggerElement*, HLT::TriggerElement* outputTE) 
  {
    int outputLevel = msgLvl();
    
    // Retrieve the AOD vertices:
    const VxContainer *vxc;
    if(HLT::OK != getFeature(outputTE, vxc, m_VxContName)) 
    {
      msg() << MSG::ERROR << "Internal error in the navigation structure during retrieval of the VxContainer with key: " << m_VxContName << ". Do nothing." << endreq;
      return HLT::NAV_ERROR;
    }
    if(!vxc)
    {
      msg() << MSG::ERROR << "No VxContainer with key: " << m_VxContName << " found. Do nothing." << endreq;
      return HLT::NAV_ERROR;
    }
    
    const xAOD::TrackParticleContainer *xtpc;
    if(HLT::OK != getFeature(outputTE, xtpc)){
      msg() << MSG::ERROR << "No xAOD::TrackParticleContainer - cannot create links" << endreq;
      return HLT::NAV_ERROR;
    } 


    // Create the xAOD container and its auxiliary store:
    xAOD::VertexContainer* xaod = new xAOD::VertexContainer();  
    xAOD::VertexAuxContainer aux;
    xaod->setStore( &aux );
     
    // Create the xAOD objects:
    auto itr = vxc->begin();
    auto end = vxc->end();
    Trk::VxCandidate* vtx;
    Trk::VxCandidate dummyVxCandidate(Trk::RecVertex(Amg::Vector3D::Zero(), AmgSymMatrix(3)::Zero()), std::vector<Trk::VxTrackAtVertex*>());
    dummyVxCandidate.setVertexType(Trk::NoVtx);
    for( ; itr != end; ++itr ) 
    {
      if((*itr)->vertexType() == 0)
      {
       	vtx = &dummyVxCandidate;
      }
      else
      {
	vtx = *itr;
      }
              
      // Create the xAOD object:
      xAOD::Vertex* vertex = new xAOD::Vertex();
      xaod->push_back( vertex );
      // Get & set the Position
      vertex->setPosition(vtx->recVertex().position());
      // Get & set the Covariance Position
      vertex->setCovariancePosition(vtx->recVertex().covariancePosition());
      // Get & set the Fit quality
      vertex->setFitQuality((float)(vtx->recVertex().fitQuality().chiSquared()), (float)(vtx->recVertex().fitQuality().doubleNumberDoF ())); 
      //Type of the vertex
      vertex->setVertexType((xAOD::VxType::VertexType)(vtx->vertexType()));
      //Set Links
      unsigned int VTAVsize = vtx->vxTrackAtVertex()->size();
      for (unsigned int i = 0 ; i < VTAVsize ; ++i)
      {
        Trk::VxTrackAtVertex* VTAV = (*(vtx->vxTrackAtVertex()))[i];
        Trk::ITrackLink*      trklink = VTAV->trackOrParticleLink();
        Trk::LinkToTrack* linkToTrackPB = dynamic_cast<Trk::LinkToTrack*>(trklink);  
        if (!linkToTrackPB) 
        {
          ATH_MSG_DEBUG ("Cast of element link failed, skip this track !!!!!");
        } 
        else
        {
          ElementLink<xAOD::TrackParticleContainer> newLink(*xtpc, linkToTrackPB->index());
          //Now set the newlink to the new xAOD vertex
          vertex->addTrackAtVertex(newLink, VTAV->vtxCompatibility()); 
        } 
      }
    }
    
    // Save xAOD vertices:
    if ( HLT::OK !=  attachFeature(outputTE, xaod, m_xVxContName) ) 
    {
      msg() << MSG::ERROR << "Could not attach feature to the TE" << endreq;
      return HLT::NAV_ERROR;
    }
    else 
    {
      if(outputLevel <= MSG::DEBUG)
        msg() << MSG::DEBUG << "Stored xAOD::Vertex container" << endreq;      
    }
    return HLT::OK;
  }

  HLT::ErrorCode TrigVertexxAODCnv::hltFinalize()
  {
     msg() << MSG::DEBUG << "finalize() success" << endreq;
     return HLT::OK;
  }
} // namespace InDet

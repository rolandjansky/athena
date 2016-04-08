/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//
// filename: TrigFTK_VxPrimary.cxx
//
// author: Iwona Grabowska-Bold, Nov 2005
//         Iwona.Grabowska@cern.ch
//
// Description:  Trigger version of the InDetPriVxFinder
//               (see original package documentation).
//
// **************************************************************************
#include "TrigFTK_RecAlgs/TrigFTK_VxPrimary.h"
#include "FTK_DataProviderInterfaces/IFTK_DataProviderSvc.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkParameters/TrackParameters.h"
#include "VxVertex/VxContainer.h"
#include "VxVertex/VxCandidate.h"
#include "TrkEventPrimitives/VertexType.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackingPrimitives.h"

#include "TrkEventPrimitives/ParamDefs.h"

#include "EventPrimitives/EventPrimitivesHelpers.h"

using Amg::Vector3D;

  
TrigFTK_VxPrimary::TrigFTK_VxPrimary(const std::string &n, ISvcLocator *pSvcLoc)
  : HLT::FexAlgo(n, pSvcLoc),
    m_DataProviderSvc("TrigFTK_DataProviderSvc",n),
    m_useRawTracks(false),
    m_useRefittedTracks(false),
    m_getVertexContainer(true),
    m_trackType(ftk::ConvertedTrackType),
    m_vertexContainerName("HLT_PrimVertexFTK"),
    m_vxContainerName("HLT_PrimVxFTK")
  
{
  declareProperty("useRawTracks", m_useRawTracks);
  declareProperty("useRefittedTracks", m_useRefittedTracks);
  declareProperty("getVertexContainer", m_getVertexContainer);
  declareProperty("FTK_DataProvider", m_DataProviderSvc);
  declareProperty("vertexContainerName", m_vertexContainerName);
  declareProperty("vxContainerName", m_vxContainerName);
  
  declareMonitoredVariable("numVertices", m_nVertices    );
  declareMonitoredStdContainer("VertexType", m_VertexType   );
  declareMonitoredStdContainer("numTracksPriVtx", m_nTracksPriVtx   );
  declareMonitoredStdContainer("numTracksPileUp", m_nTracksPileUp   );
  declareMonitoredStdContainer("zOfPriVtx", m_zOfPriVtx);
  declareMonitoredStdContainer("zOfPileUp", m_zOfPileUp);
  declareMonitoredStdContainer("zOfNoVtx", m_zOfNoVtx);
}

//-------------------------------------------------------------------------
TrigFTK_VxPrimary::~TrigFTK_VxPrimary()
{}
//-------------------------------------------------------------------------
HLT::ErrorCode TrigFTK_VxPrimary::hltInitialize() {
    
  msg() << MSG::INFO << "TrigFTK_VxPrimary::initialize(). "<< endreq;
  
  //Set up the FTK Data Provider SVC //
  StatusCode sc= m_DataProviderSvc.retrieve();
  if(sc.isFailure()) {
    ATH_MSG_ERROR("unable to locate FTK_DataProviderSvc");
    return HLT::BAD_JOB_SETUP;
  } else {
    ATH_MSG_INFO("Configured to retrieve FTK tracks FTK_DataProviderSvc");
  }

  
  if (!m_getVertexContainer && m_useRawTracks) {
    ATH_MSG_INFO("Getting Primary vertices from FTK_DataProviderSvc using RAW FTK tracks");
    m_trackType = ftk::RawTrackType;
  } else {
    if (m_useRefittedTracks) {
      ATH_MSG_INFO("Getting Primary vertices from FTK_DataProviderSvc using refitted FTK tracks");
      m_trackType = ftk::RefittedTrackType;
    } else {
      ATH_MSG_INFO("Getting Primary vertices from FTK_DataProviderSvc using converted FTK tracks");
      m_trackType = ftk::ConvertedTrackType;
    }
  }
  
  if (m_getVertexContainer) {
    ATH_MSG_INFO("Attaching primary vertices to TE in xAOD::VertexContainer " << m_vertexContainerName);
  } else {
    ATH_MSG_INFO("Attaching primary vertices to TE in VxContainer" << m_vxContainerName);
  }
  
  return HLT::OK;
}

///////////////////////////////////////////////////////////////////
// Execute HLT algorithm
///////////////////////////////////////////////////////////////////
HLT::ErrorCode TrigFTK_VxPrimary::hltExecute(const HLT::TriggerElement*, HLT::TriggerElement* outputTE) {
  
  
  m_nVertices = 0;
  m_VertexType.clear();
  m_nTracksPriVtx.clear();
  m_nTracksPileUp.clear();
  m_zOfPriVtx.clear();
  m_zOfPileUp.clear();
  m_zOfNoVtx.clear();
  
  int outputLevel = msgLvl();
  
  if(outputLevel <= MSG::DEBUG)
    msg() << MSG::DEBUG << " In execHLTAlgorithm()" << endreq;
  
  if (m_getVertexContainer) {

    
    xAOD::VertexContainer* theVertexContainer = new xAOD::VertexContainer();
    xAOD::VertexAuxContainer	theVertexAux;
    theVertexContainer->setStore(&theVertexAux);

    StatusCode sc = m_DataProviderSvc->getVertexContainer( theVertexContainer, m_useRefittedTracks);

    if (sc != StatusCode::SUCCESS) {
      msg() << MSG::DEBUG << " Error getting VertexContainer StatusCode is " << sc << endreq;
    }

    if ( HLT::OK !=  attachFeature(outputTE, theVertexContainer, m_vertexContainerName) ) {
      msg() << MSG::ERROR << "Could not attach feature to the TE" << endreq;
      
      return HLT::NAV_ERROR;
    }
    
    m_nVertices = theVertexContainer->size();
    if(outputLevel <= MSG::DEBUG){
      msg() << MSG::DEBUG << "Container recorded in StoreGate." << endreq;
      msg() << MSG::DEBUG << "REGTEST: Container size :" << m_nVertices << endreq;
    }    
    
    size_t privtxcount(0), pileupvtxcount(0);
    unsigned int iv = 0;
    for (auto pv =  theVertexContainer->begin(); pv !=  theVertexContainer->end(); pv++, iv++) {
      Vector3D vtx;
      vtx = (*pv)->position();
      const Amg::MatrixX& verr = (*pv)->covariancePosition();

      m_VertexType.push_back(int ((*pv)->vertexType()));

      if ((*pv)->vertexType()==xAOD::VxType::PriVtx){
	++privtxcount;
	if(outputLevel <= MSG::DEBUG){
	  msg() << MSG::DEBUG << "REGTEST " << privtxcount
		<< std::setw(10)
		<< " x=" << vtx.x() << "+/-" << Amg::error(verr, Trk::x)
		<< " y=" << vtx.y() << "+/-" << Amg::error(verr, Trk::y)
		<< " z=" << vtx.z() << "+/-" << Amg::error(verr, Trk::z)
		<< endreq; 
	}
	m_zOfPriVtx.push_back(vtx.z());
 	m_nTracksPriVtx.push_back(int ((*pv)->vxTrackAtVertex().size()));
      } else if ((*pv)->vertexType() == xAOD::VxType::PileUp){
	++pileupvtxcount;
	if(outputLevel <= MSG::DEBUG){
	  msg() << MSG::DEBUG << "REGTEST " << pileupvtxcount
		<< std::setw(10)
		<< " x=" << vtx.x()
		<< " y=" << vtx.y()
		<< " z=" << vtx.z()
		<< endreq; 
	}
	m_zOfPileUp.push_back(vtx.z());
 	m_nTracksPileUp.push_back(int ((*pv)->vxTrackAtVertex().size()));
      } else if ((*pv)->vertexType()==xAOD::VxType::NoVtx){
	m_zOfNoVtx.push_back(vtx.z());
      } else {
	msg() << MSG::DEBUG << "Bad VxCandidate=" << iv << endreq;
      }
    }
    
  } else {
    
    VxContainer* theVxContainer = m_DataProviderSvc->getVxContainer(m_trackType);
    //
    //  Attach resolved tracks to the trigger element.
    
    if ( HLT::OK !=  attachFeature(outputTE, theVxContainer, m_vxContainerName) ) {
	msg() << MSG::ERROR << "Could not attach feature to the TE" << endreq;
	
	return HLT::NAV_ERROR;
    }
    
    m_nVertices = theVxContainer->size();
    if(outputLevel <= MSG::DEBUG){
      msg() << MSG::DEBUG << "Container recorded in StoreGate." << endreq;
      msg() << MSG::DEBUG << "REGTEST: Container size :" << m_nVertices << endreq;
    }    
    
    size_t privtxcount(0), pileupvtxcount(0);
    for (int iv=0; iv<m_nVertices; iv++){
      Trk::VxCandidate *mvtx = theVxContainer->at(iv);
      if ( mvtx ){
	Vector3D vtx;
	vtx = mvtx->recVertex().position();
	const Amg::MatrixX& verr = mvtx->recVertex().covariancePosition();

	m_VertexType.push_back(int (mvtx->vertexType()));

	if (mvtx->vertexType()==Trk::PriVtx){
	  ++privtxcount;
	  if(outputLevel <= MSG::DEBUG){
	    msg() << MSG::DEBUG << "REGTEST " << privtxcount
		  << std::setw(10)
		  << " x=" << vtx.x() << "+/-" << Amg::error(verr, Trk::x)
		  << " y=" << vtx.y() << "+/-" << Amg::error(verr, Trk::y)
		  << " z=" << vtx.z() << "+/-" << Amg::error(verr, Trk::z)
		  << endreq; 
	  }
	  m_zOfPriVtx.push_back(vtx.z());
	  m_nTracksPriVtx.push_back(int (mvtx->vxTrackAtVertex()->size()));
	}
	else if (mvtx->vertexType()==Trk::PileUp){
	  ++pileupvtxcount;
	  if(outputLevel <= MSG::DEBUG){
	    msg() << MSG::DEBUG << "REGTEST " << pileupvtxcount
		  << std::setw(10)
		  << " x=" << vtx.x()
		  << " y=" << vtx.y()
		  << " z=" << vtx.z()
		  << endreq; 
	  }
	  m_zOfPileUp.push_back(vtx.z());
	  m_nTracksPileUp.push_back(int (mvtx->vxTrackAtVertex()->size()));

	}
	else if (mvtx->vertexType()==Trk::NoVtx){
	  m_zOfNoVtx.push_back(vtx.z());
	}
      } else {
	msg() << MSG::DEBUG << "Bad VxCandidate=" << iv << endreq;
      }
    }
  } 
  return HLT::OK;
}
//---------------------------------------------------------------------------
HLT::ErrorCode TrigFTK_VxPrimary::hltFinalize() {
  
  msg() << MSG::INFO << "TrigFTK_VxPrimary::finalize()" << endreq;
  
  return HLT::OK;
}



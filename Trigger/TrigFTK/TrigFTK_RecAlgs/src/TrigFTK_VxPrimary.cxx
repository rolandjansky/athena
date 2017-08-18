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
    m_trackType(ftk::ConvertedTrack),
    m_vertexContainerName("HLT_PrimVertexFTK"),
    m_vxContainerName("HLT_PrimVxFTK"),
    m_useFastVertexTool(false)
{
  declareProperty("useRawTracks", m_useRawTracks);
  declareProperty("useRefittedTracks", m_useRefittedTracks);
  declareProperty("FTK_DataProvider", m_DataProviderSvc);
  declareProperty("vertexContainerName", m_vertexContainerName);
  declareProperty("vxContainerName", m_vxContainerName);
  declareProperty("useFastVertexTool", m_useFastVertexTool);
  
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
    
  msg() << MSG::INFO << "TrigFTK_VxPrimary::initialize(). "<< endmsg;
  
  //Set up the FTK Data Provider SVC //
  StatusCode sc= m_DataProviderSvc.retrieve();
  if(sc.isFailure()) {
    ATH_MSG_ERROR("unable to locate FTK_DataProviderSvc");
    return HLT::BAD_JOB_SETUP;
  } else {
    ATH_MSG_INFO("Configured to retrieve FTK tracks FTK_DataProviderSvc");
  }

  
  if (m_useFastVertexTool) {
    if (m_useRawTracks) {
      ATH_MSG_INFO("Getting Primary vertices from FTK_DataProviderSvc using fast FTK_VertexTool with RAW FTK tracks");
      m_trackType = ftk::RawTrack;
    }  else if (m_useRefittedTracks) {
      ATH_MSG_INFO("Getting Primary vertices from FTK_DataProviderSvc using fast FTK_VertexTool with refitted FTK tracks");
      m_trackType = ftk::RefittedTrack;
    } else {
      ATH_MSG_INFO("Getting Primary vertices from FTK_DataProviderSvc using fast FTK_VertexTool with converted FTK tracks");
      m_trackType = ftk::ConvertedTrack;
    }
  } else {
    if (m_useRefittedTracks) {
      ATH_MSG_INFO("Getting Primary vertices from FTK_DataProviderSvc using refitted FTK tracks");
      m_trackType = ftk::RefittedTrack;
    } else {
      ATH_MSG_INFO("Getting Primary vertices from FTK_DataProviderSvc using converted FTK tracks");
      m_trackType = ftk::ConvertedTrack;
    }
  }
  ATH_MSG_INFO("Attaching primary vertices to TE in xAOD::VertexContainer " << m_vertexContainerName);
  
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
    msg() << MSG::DEBUG << " In execHLTAlgorithm()" << endmsg;
  
  xAOD::VertexContainer* viewVertexContainer = nullptr;
  if (m_useFastVertexTool) {
    viewVertexContainer = m_DataProviderSvc->getFastVertices(m_trackType);
  } else {
    viewVertexContainer = m_DataProviderSvc->getVertexContainer(m_useRefittedTracks);
  }
  

  // view collection is returned. Create a new collection that is not a view so that it can be persitified
  // This can be removed once the conversion to use ViewVectors is completed.

  xAOD::VertexContainer* theVertexContainer = new xAOD::VertexContainer();
  xAOD::VertexAuxContainer    theVertexContainerAux;
  theVertexContainer->setStore(&theVertexContainerAux);
  if (viewVertexContainer != nullptr) {
    for (auto pv =  viewVertexContainer->begin(); pv !=  viewVertexContainer->end(); ++pv) {
      xAOD::Vertex* vert = new xAOD::Vertex(*(*pv));
      theVertexContainer->push_back(vert);
    }
    delete  viewVertexContainer;
  }

  if ( HLT::OK !=  attachFeature(outputTE, theVertexContainer, m_vertexContainerName) ) {
    msg() << MSG::ERROR << "Could not attach feature to the TE" << endmsg;
    delete theVertexContainer;
    return HLT::NAV_ERROR;
  }
  
  m_nVertices = theVertexContainer->size();
  if(outputLevel <= MSG::DEBUG){
    msg() << MSG::DEBUG << "Container recorded in StoreGate." << endmsg;
    msg() << MSG::DEBUG << "REGTEST: Container size :" << m_nVertices << endmsg;
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
	msg() << MSG::DEBUG << "REGTEST "<< iv <<" Primary Vertex #" << privtxcount
	      << std::setw(10)
	      << " x=" << vtx.x() << "+/-" << Amg::error(verr, Trk::x)
	      << " y=" << vtx.y() << "+/-" << Amg::error(verr, Trk::y)
	      << " z=" << vtx.z() << "+/-" << Amg::error(verr, Trk::z)
	      << endmsg; 
      }
      m_zOfPriVtx.push_back(vtx.z());
      m_nTracksPriVtx.push_back(int ((*pv)->vxTrackAtVertex().size()));
    } else if ((*pv)->vertexType() == xAOD::VxType::PileUp){
      ++pileupvtxcount;
      if(outputLevel <= MSG::DEBUG){
	msg() << MSG::DEBUG << "REGTEST "<<iv << " Pileup vertex #"<<pileupvtxcount
	      << std::setw(10)
	      << " x=" << vtx.x()
	      << " y=" << vtx.y()
	      << " z=" << vtx.z()
	      << endmsg; 
      }
      m_zOfPileUp.push_back(vtx.z());
      m_nTracksPileUp.push_back(int ((*pv)->vxTrackAtVertex().size()));
    } else if ((*pv)->vertexType()==xAOD::VxType::NoVtx){
      msg() << MSG::DEBUG << iv << " No Vertex" << endmsg;
      m_zOfNoVtx.push_back(vtx.z());
    } else {
      msg() << MSG::DEBUG << iv << "Bad VxCandidate" << endmsg;
    }
  }
  
  return HLT::OK;
}
//---------------------------------------------------------------------------
HLT::ErrorCode TrigFTK_VxPrimary::hltFinalize() {
  
  msg() << MSG::INFO << "TrigFTK_VxPrimary::finalize()" << endmsg;
  
  return HLT::OK;
}



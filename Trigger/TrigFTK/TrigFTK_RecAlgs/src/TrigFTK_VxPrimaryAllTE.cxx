/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **************************************************************************
#include "TrigFTK_RecAlgs/TrigFTK_VxPrimaryAllTE.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkParameters/TrackParameters.h"
#include "VxVertex/VxContainer.h"
#include "VxVertex/VxCandidate.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkEventPrimitives/VertexType.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackingPrimitives.h"


TrigFTK_VxPrimaryAllTE::TrigFTK_VxPrimaryAllTE(const std::string &n, ISvcLocator *pSvcLoc)
  : HLT::AllTEAlgo(n, pSvcLoc),
    m_DataProviderSvc("TrigFTK_DataProviderSvc",n),
    m_useRawTracks(false),
    m_useRefittedTracks(false),
    m_getVertexContainer(false),
    m_trackType(ftk::ConvertedTrackType),
    m_vertexContainerName("FTK_PrimVertex"),
    m_vxContainerName("FTK_PrimVx")

{
  declareProperty("useRawTracks", m_useRawTracks);
  declareProperty("useRefittedTracks", m_useRefittedTracks);
  declareProperty("getVertexContainer", m_getVertexContainer);
  declareProperty("FTK_DataProvider", m_DataProviderSvc);
  
  declareMonitoredVariable("numTracks", m_nTracks   );
  declareMonitoredVariable("numVertices", m_nVertices    );
  declareMonitoredStdContainer("zOfPriVtx", m_zOfPriVtx);
  declareMonitoredStdContainer("zOfPileUp", m_zOfPileUp);
  declareMonitoredStdContainer("zOfNoVtx", m_zOfNoVtx);
}

//-------------------------------------------------------------------------
TrigFTK_VxPrimaryAllTE::~TrigFTK_VxPrimaryAllTE()
{}
//-------------------------------------------------------------------------
HLT::ErrorCode TrigFTK_VxPrimaryAllTE::hltInitialize() {
  
  msg() << MSG::INFO << "TrigFTK_VxPrimaryAllTE::initialize(). "<< endreq;
  
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
    ATH_MSG_INFO("Attaching primary vertices to TE in VxContainer " << m_vxContainerName);
  }

  return HLT::OK;
}


///////////////////////////////////////////////////////////////////
// Execute HLT algorithm
///////////////////////////////////////////////////////////////////
HLT::ErrorCode TrigFTK_VxPrimaryAllTE::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& inputTE, unsigned int output) {
  
  
  m_nTracks = 0;
  m_nVertices = 0;
  m_zOfPriVtx.clear();
  m_zOfPileUp.clear();
  m_zOfNoVtx.clear();
  HLT::TEVec allTEs;
  
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
    
    //
    //  Attach resolved tracks to the trigger element.
    // create output TE:         
    // Create an output TE seeded by the inputs                                                                                                                
    HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(allTEs, output);
    outputTE->setActiveState(true);
    
    
    HLT::ErrorCode stat = attachFeature(outputTE, theVertexContainer, m_vertexContainerName);
    if (stat != HLT::OK) {
      if (msgLvl() <= MSG::WARNING)
	msg() << MSG::ERROR << "Could not attach feature to the TE" << endreq;
      
      return HLT::NAV_ERROR;
    }
    
    
    
  } else {
    
    VxContainer* theVxContainer = m_DataProviderSvc->getVxContainer(m_trackType);
    
    
    //
    //  Attach resolved tracks to the trigger element.
    // create output TE:         
    // Create an output TE seeded by the inputs                                                                                                                
    HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(allTEs, output);
    outputTE->setActiveState(true);
    
    
    HLT::ErrorCode stat = attachFeature(outputTE, theVxContainer, m_vxContainerName);
    if (stat != HLT::OK) {
      if (msgLvl() <= MSG::WARNING)
	msg() << MSG::ERROR << "Could not attach feature to the TE" << endreq;
      
      return HLT::NAV_ERROR;
    }
    
    
    
    m_nVertices = theVxContainer->size();
    if(outputLevel <= MSG::DEBUG){
      msg() << MSG::DEBUG << "Container recorded in StoreGate." << endreq;
      msg() << MSG::DEBUG << "REGTEST: Container size :" << m_nVertices << endreq;
    }    
    
  } 
  
  // since this is an AllTEAlgo, we have to call the monitoring ourselves:
  afterExecMonitors().ignore();
  
  
  return HLT::OK;
}
//---------------------------------------------------------------------------
HLT::ErrorCode TrigFTK_VxPrimaryAllTE::hltFinalize() {
  
  msg() << MSG::INFO << "TrigFTK_VxPrimaryAllTE::finalize()" << endreq;
  
  return HLT::OK;
}



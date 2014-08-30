/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMinBias/TrigVertexCounter.h"
#include "xAODTrigMinBias/TrigVertexCounts.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/Vertex.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

//---------------------------------------------------------------------------------

TrigVertexCounter::TrigVertexCounter(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::FexAlgo(name, pSvcLocator), 
    m_log(msgSvc(), name),
    m_vtxContainerName(),
    m_vtxNtrks(),
    m_vtxTrkPtSqSum(),
    m_formFeaturesTimer(0),
    m_getVerticesTimer(0),
    m_vxContainer(0) {  
  
  declareProperty("VtxContainerName",  m_vtxContainerName = "xPrimVx");
  declareProperty("pTmin",  m_pTmin = 400., "count tracks above minPt");
  
  // Monitoring of the data stored in TrigVertexCounts
  declareMonitoredStdContainer("vtxNtrks",     m_vtxNtrks); 
  declareMonitoredStdContainer("vtxTrkPtSqSum",m_vtxTrkPtSqSum); 
  declareMonitoredVariable("vtxNvtx",m_vtxNvtx); 
}

//---------------------------------------------------------------------------------

TrigVertexCounter::~TrigVertexCounter(void) {
}

//---------------------------------------------------------------------------------

HLT::ErrorCode TrigVertexCounter::hltInitialize() {
  m_log.setLevel(outputLevel());
  m_log << MSG::INFO << "Initialising this TrigVertexCounter: " << name() << endreq;
  
  // Create timers
  if( timerSvc() ){
    m_formFeaturesTimer = addTimer("formFeatures");
    m_getVerticesTimer = addTimer("getVertices");
  }

  m_log << MSG::INFO << "TrigVertexCounter initialized successfully" << endreq; 
  return HLT::OK;  
}

//---------------------------------------------------------------------------------

HLT::ErrorCode TrigVertexCounter::hltExecute( const HLT::TriggerElement *, HLT::TriggerElement* outputTE ){
  
  // Clear the monitoring variables
  m_vtxNtrks.clear();
  m_vtxTrkPtSqSum.clear();
  m_vtxNvtx = -1;

  if ( msgLvl() <= MSG::DEBUG) {
    m_log << MSG::DEBUG << "Executing this TrigVertexCounter " << name() << endreq;
  } 
  
  if( timerSvc() ){ 
    m_formFeaturesTimer->start();
    m_getVerticesTimer->start();
  }  
  
  HLT::ErrorCode ec = getFeature( outputTE, m_vxContainer, m_vtxContainerName );
  if( ec != HLT::OK ) {
    m_log << MSG::ERROR << "Vertex collection " << m_vtxContainerName << "could not be found " << endreq;
    return HLT::NAV_ERROR;
  }
  else {
    m_log << MSG::DEBUG << "Retrieved successfully vertex collection " << m_vtxContainerName << endreq;
  }

  if( timerSvc() ){ 
    m_getVerticesTimer->stop();
  }  
  
  if( !m_vxContainer || m_vxContainer->size() == 0 ){
    if( msgLvl() <= MSG::DEBUG ){
      m_log << MSG::DEBUG << "VxContainer is null or contains 0 vertices." << endreq;
    }
  }
  else{

    // Loop over the stored primary vertices
    m_vtxNvtx = m_vxContainer->size();
    xAOD::VertexContainer::const_iterator itr  = m_vxContainer->begin();
    xAOD::VertexContainer::const_iterator itr_end = m_vxContainer->end();    
    for (; itr != itr_end; ++itr) {

      // Loop over the vx tracks.
      float ptSqSum = 0.;
      unsigned int trackCount = 0;
      const std::vector< ElementLink< xAOD::TrackParticleContainer > > tpLinks =  (*itr)->trackParticleLinks();

      for (const auto& tplTrk : tpLinks) {
	if ( !(tplTrk.isValid()) ) {
	  m_log << MSG::WARNING << "Got an invalid element link. Continuing..." << endreq;
	  continue;
	}
	
	const xAOD::TrackParticle* trk = *tplTrk;
 
	//m_log << MSG::DEBUG << "Track pt = " << trk->pt() << endreq;

	// If the track fails the minimum pT cut skip it.  
	if(trk->pt() < m_pTmin) continue;
      
	trackCount++; // increment the track counter
	ptSqSum += (trk->pt())*(trk->pt()); // sum(pT^2) of tracks at vertex.
      }

      m_vtxNtrks.push_back(trackCount); // fill number of tracks with pT above minimum pT cut. 
      m_vtxTrkPtSqSum.push_back(ptSqSum/1.e6);  // fill sumpt^2 in GeV for tracks with pT above minimum pT cut.
    }
  }
  
  if( timerSvc() ){
    m_formFeaturesTimer->stop();
  }  
  
  // save feature to output TE:
  xAOD::TrigVertexCounts* vc = new xAOD::TrigVertexCounts;
  
  vc->makePrivateStore();
  vc->setVtxNtrks(m_vtxNtrks);
  vc->setVtxTrkPtSqSum(m_vtxTrkPtSqSum);
  
  HLT::ErrorCode hltStatus = attachFeature( outputTE, vc, "vertexcounts" );
  if(hltStatus != HLT::OK) {
    if(msgLvl() <= MSG::ERROR) {
      m_log << MSG::ERROR << "Write of TrigVertexCounts into outputTE failed" << endreq;
    }
    return hltStatus;
  }

  return HLT::OK;
}

//---------------------------------------------------------------------------------

HLT::ErrorCode TrigVertexCounter::hltFinalize() {
  
  m_log << MSG::INFO << "Finalizing this TrigVertexCounter" << name() << endreq; 
  return HLT::OK;  
}

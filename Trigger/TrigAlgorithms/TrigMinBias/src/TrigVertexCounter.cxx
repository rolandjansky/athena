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
  ATH_MSG_INFO( "Initialising this TrigVertexCounter: " << name()  );
  
  // Create timers
  if( timerSvc() ){
    m_formFeaturesTimer = addTimer("formFeatures");
    m_getVerticesTimer = addTimer("getVertices");
  }

  ATH_MSG_INFO( "TrigVertexCounter initialized successfully"  );
  return HLT::OK;  
}

//---------------------------------------------------------------------------------

HLT::ErrorCode TrigVertexCounter::hltExecute( const HLT::TriggerElement *, HLT::TriggerElement* outputTE ){
  
  // Clear the monitoring variables
  m_vtxNtrks.clear();
  m_vtxTrkPtSqSum.clear();
  m_vtxNvtx = -1;

  ATH_MSG_DEBUG( "Executing this TrigVertexCounter " << name()  );
  
  if( timerSvc() ){ 
    m_formFeaturesTimer->start();
    m_getVerticesTimer->start();
  }  
  
  HLT::ErrorCode ec = getFeature( outputTE, m_vxContainer, m_vtxContainerName );
  if( ec != HLT::OK ) {
    ATH_MSG_ERROR( "Vertex collection " << m_vtxContainerName << "could not be found "  );
    return HLT::NAV_ERROR;
  }
  else {
    ATH_MSG_DEBUG( "Retrieved successfully vertex collection " << m_vtxContainerName  );
  }

  if( timerSvc() ){ 
    m_getVerticesTimer->stop();
  }  
  
  if( !m_vxContainer || m_vxContainer->size() == 0 ){
    ATH_MSG_DEBUG( "VxContainer is null or contains 0 vertices."  );
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
	  ATH_MSG_WARNING( "Got an invalid element link. Continuing..."  );
	  continue;
	}
	
	const xAOD::TrackParticle* trk = *tplTrk;
 
	//m_log << MSG::DEBUG << "Track pt = " << trk->pt() << endmsg;

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
    ATH_MSG_ERROR( "Write of TrigVertexCounts into outputTE failed"  );
    return hltStatus;
  }

  return HLT::OK;
}

//---------------------------------------------------------------------------------

HLT::ErrorCode TrigVertexCounter::hltFinalize() {
  
  ATH_MSG_INFO( "Finalizing this TrigVertexCounter" << name()  );
  return HLT::OK;  
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTauHypo/HLTTauTrackRoiUpdater.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrackSummary/TrackSummary.h"

HLTTauTrackRoiUpdater::HLTTauTrackRoiUpdater(const std::string & name, ISvcLocator* pSvcLocator) 
  : HLT::FexAlgo(name, pSvcLocator)
{
  declareProperty("InputTrackCollection", m_InputTrackColl = "TrigFastTrackFinder_TauCore");
  declareProperty("z0HalfWidth",          m_z0HalfWidth = 0.4);
  declareProperty("nHitPix",              m_nHitPix = 2);
  declareProperty("nSiHoles",             m_nSiHoles = 2);

}

HLTTauTrackRoiUpdater::~HLTTauTrackRoiUpdater()
{

}

HLT::ErrorCode HLTTauTrackRoiUpdater::hltInitialize()
{

  msg() << MSG::DEBUG << "in initialize()" << endreq;
  msg() << MSG::DEBUG << " REGTEST: HLTTauTrackRoiUpdater parameters " << endreq;
  msg() << MSG::DEBUG << " REGTEST: Input Track Collection " << m_InputTrackColl << endreq;
  msg() << MSG::DEBUG << " REGTEST: z0HalfWidth            " << m_z0HalfWidth << endreq;

  return HLT::OK;

}

HLT::ErrorCode HLTTauTrackRoiUpdater::hltFinalize()
{

  msg() << MSG::DEBUG << "in finalize()" << endreq;

  return HLT::OK;

}

HLT::ErrorCode HLTTauTrackRoiUpdater::hltExecute(const HLT::TriggerElement*, HLT::TriggerElement* outputTE)
{

  HLT::ErrorCode status = HLT::OK;

  //get RoI descriptor
  const TrigRoiDescriptor* roiDescriptor = 0;
  status = getFeature(outputTE, roiDescriptor);
  if ( status != HLT::OK || roiDescriptor == 0 ) {
    msg() <<  MSG::WARNING << " Failed to find RoiDescriptor " << endreq;
    return status;
  }

  float leadTrkZ0 = roiDescriptor->zed();


  //look at fast-tracks
  std::vector<const TrackCollection*> vectorFoundTracks;
  const TrackCollection* foundTracks = 0;

  status = getFeatures(outputTE, vectorFoundTracks);

  if (status !=HLT::OK) {
    msg() << MSG::ERROR << "No FastTrackFinder container was found.  Aborting pre-selection." << endreq;
    return HLT::NAV_ERROR;
  }
  else {
    if (vectorFoundTracks.size()<1) {
      msg() << MSG::ERROR << "FastTrackFinder vector was empty.  Aborting pre-selection." << endreq;
      return HLT::NAV_ERROR;
    }
  }

  // Retrieve last container to be appended
  foundTracks = vectorFoundTracks.back();

  if(foundTracks) msg() << MSG::DEBUG << " Input track collection has size " << foundTracks->size() << endreq;
  else msg() << MSG::DEBUG << " Input track collection not found " << endreq;  

  if(foundTracks){

    const Trk::Track *leadTrack = 0;
    const Trk::Perigee *trackPer = 0;
    const Trk::TrackSummary* summary = 0;
    float trkPtMax = 0;
    
    TrackCollection::const_iterator it = foundTracks->begin();
    TrackCollection::const_iterator itEnd = foundTracks->end();

    // Find leading track

 

    for (;it!=itEnd;it++){

      const Trk::Track* track = *it;

      trackPer = track->perigeeParameters();
      summary = track->trackSummary();

 
      if(summary==0){
        msg() << MSG::DEBUG << " track summary not available in RoI updater " << trkPtMax << endreq;
      }

 

      if(trackPer && summary){

 	float trackPt = trackPer->pT();


	if ( trackPt > trkPtMax ) {


	  int nPix  = summary->get(Trk::numberOfPixelHits);
	  if(nPix<0) nPix=0;

	  if(nPix < m_nHitPix) {

	    msg() << MSG::DEBUG << "Track rejected because of nHitPix " << nPix << " < " << m_nHitPix << endreq;

	    continue;

	  }

 
	  int nPixHole = summary->get(Trk::numberOfPixelHoles);
	  if (nPixHole < 0) nPixHole = 0;
	
	  int nSCTHole = summary->get(Trk::numberOfSCTHoles);
	  if (nSCTHole < 0) nSCTHole = 0;
	
	  if((nPixHole + nSCTHole) > m_nSiHoles) {
	    
	    msg() << MSG::DEBUG << "Track rejected because of nSiHoles " << nPixHole + nSCTHole << " > " << m_nSiHoles << endreq;
	    
	    continue;
	  
	  }
	
		  
	  leadTrack = (*it);
	  trkPtMax = trackPt;
	  
	}
	
	
	
      }

    }


    if(leadTrack) {
      msg() << MSG::DEBUG << " leading track pT " << trkPtMax << endreq;
    }
    else msg() << MSG::DEBUG << " no leading track pT found " << endreq;
    
    if(leadTrack){
      leadTrkZ0 = leadTrack->perigeeParameters()->parameters()[Trk::z0];
    }
    
  }
  
  float z0Min = leadTrkZ0 - m_z0HalfWidth;
  float z0Max = leadTrkZ0 + m_z0HalfWidth;

  
  TrigRoiDescriptor *outRoi = new TrigRoiDescriptor(roiDescriptor->roiWord(), roiDescriptor->l1Id(), roiDescriptor->roiId(),
						    roiDescriptor->eta(), roiDescriptor->etaMinus(), roiDescriptor->etaPlus(),
						    roiDescriptor->phi(), roiDescriptor->phiMinus(), roiDescriptor->phiPlus(),
						    leadTrkZ0, z0Min, z0Max);
  
  ATH_MSG_DEBUG("Input RoI " << *roiDescriptor);
  ATH_MSG_DEBUG("Output RoI " << *outRoi);
  
  std::string roiName = "forID2";
  
  if ( HLT::OK !=  attachFeature(outputTE, outRoi, roiName) ) {
    ATH_MSG_ERROR("Could not attach feature to the TE");
    return HLT::NAV_ERROR;
  }
  else {
    ATH_MSG_DEBUG("REGTEST: attached RoI " << roiName << *outRoi);
  }
  
  
  return HLT::OK;
  
}

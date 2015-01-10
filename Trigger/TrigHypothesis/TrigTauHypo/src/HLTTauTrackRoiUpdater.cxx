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


HLTTauTrackRoiUpdater::HLTTauTrackRoiUpdater(const std::string & name, ISvcLocator* pSvcLocator) 
  : HLT::FexAlgo(name, pSvcLocator)
{

  declareProperty("DeltaRLeadTrkRoI",     m_deltaRLeadTrkRoI = 0.1);
  declareProperty("InputTrackCollection", m_InputTrackColl = "TrigFastTrackFinder_TrigInDetTrack_TauCore");
  declareProperty("z0HalfWidth",          m_z0HalfWidth = 0.4);
  declareProperty("minTrackPt",           m_minTrackPt = 1.0*CLHEP::GeV);

}

HLTTauTrackRoiUpdater::~HLTTauTrackRoiUpdater()
{

}

HLT::ErrorCode HLTTauTrackRoiUpdater::hltInitialize()
{

  msg() << MSG::DEBUG << "in initialize()" << endreq;
  msg() << MSG::DEBUG << " REGTEST: HLTTauTrackRoiUpdater parameters " << endreq;
  msg() << MSG::DEBUG << " REGTEST: Input Track Collection " << m_InputTrackColl << endreq;
  msg() << MSG::DEBUG << " REGTEST: dR Lead Track-RoI      " << m_deltaRLeadTrkRoI << endreq;
  msg() << MSG::DEBUG << " REGTEST: z0HalfWidth            " << m_z0HalfWidth << endreq;
  msg() << MSG::DEBUG << " REGTEST: minTrackPt             " << m_minTrackPt << endreq;

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

  float roiEta = roiDescriptor->eta();
  float roiPhi = roiDescriptor->phi();

  float leadTrkZ0 = roiDescriptor->zed();


  //look at fast-tracks
  std::vector<const TrackCollection*> vectorFoundTracks;
  const TrackCollection* foundTracks = 0;

  status = getFeatures(outputTE,vectorFoundTracks);

  if (status !=HLT::OK) {
    msg() << MSG::ERROR << "No FastTrackFinder container was found.  Aborting pre-selection." << endreq;
    return status;
  }
  else {
    if (vectorFoundTracks.size()<1) {
      msg() << MSG::ERROR << "FastTrackFinder vector was empty.  Aborting pre-selection." << endreq;
      return HLT::ERROR;
    }
  }

  // Retrieve last container to be appended
  foundTracks = vectorFoundTracks.back();

  msg() << MSG::DEBUG << " Input track collection has size " << foundTracks->size() << endreq;

  if(foundTracks){

    const Trk::Track *leadTrack = 0;
    const Trk::Perigee *trackPer = 0;
    float trkPtMax = m_minTrackPt;
    
    TrackCollection::const_iterator it = foundTracks->begin();
    TrackCollection::const_iterator itEnd = foundTracks->end();

    // Find leading track

    for (;it!=itEnd;it++){
 
     const Trk::Track* track = *it;
     trackPer = track->perigeeParameters();

     if(trackPer){
	
       float trkEta = trackPer->eta();
       float trkPhi = trackPer->parameters()[Trk::phi];
       double dRTrkTau = sqrt((roiEta-trkEta)*(roiEta-trkEta) + (roiPhi-trkPhi)*(roiPhi-trkPhi));
       
       float trackPt = trackPer->pT()/1000.;
       if ((trackPt > trkPtMax) && dRTrkTau < m_deltaRLeadTrkRoI) {
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
  
  std::string roiName = "forID";
  
  if ( HLT::OK !=  attachFeature(outputTE, outRoi, roiName) ) {
    ATH_MSG_ERROR("Could not attach feature to the TE");
       return HLT::NAV_ERROR;
  }
  else {
    ATH_MSG_DEBUG("REGTEST: attached RoI " << roiName << *outRoi);
  }
  

  return HLT::OK;

}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <cmath>

#include "TrigTauHypo/HLTTauTrackRoiUpdater.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "xAODTrigger/TrigPassBits.h"

HLTTauTrackRoiUpdater::HLTTauTrackRoiUpdater(const std::string & name, ISvcLocator* pSvcLocator) 
  : HLT::FexAlgo(name, pSvcLocator)
{
  declareProperty("InputTrackCollection", m_InputTrackColl = "TrigFastTrackFinder_TauCore");
  declareProperty("z0HalfWidth",          m_z0HalfWidth = 0.4);
  declareProperty("nHitPix",              m_nHitPix = 2);
  declareProperty("nSiHoles",             m_nSiHoles = 2);
  declareProperty("UpdateEta",            m_updateEta = true);
  declareProperty("UpdatePhi",            m_updatePhi = false);
}

HLTTauTrackRoiUpdater::~HLTTauTrackRoiUpdater()
{

}

HLT::ErrorCode HLTTauTrackRoiUpdater::hltInitialize()
{

  msg() << MSG::INFO << "in initialize()" << endmsg;
  msg() << MSG::INFO << " REGTEST: HLTTauTrackRoiUpdater parameters 	" 	<< endmsg;
  msg() << MSG::INFO << " REGTEST: Input Track Collection 		" 	<< m_InputTrackColl << endmsg;
  msg() << MSG::INFO << " REGTEST: z0HalfWidth            		" 	<< m_z0HalfWidth << endmsg;
  msg() << MSG::INFO << " REGTEST: nHitPix            			" 	<< m_nHitPix << endmsg;
  msg() << MSG::INFO << " REGTEST: nSiHoles            		        " 	<< m_nSiHoles << endmsg;
  msg() << MSG::INFO << " REGTEST: UpdateEta            		" 	<< m_updateEta << endmsg;
  msg() << MSG::INFO << " REGTEST: UpdatePhi            		" 	<< m_updatePhi << endmsg;
  return HLT::OK;

}

HLT::ErrorCode HLTTauTrackRoiUpdater::hltFinalize()
{

  msg() << MSG::DEBUG << "in finalize()" << endmsg;

  return HLT::OK;

}

HLT::ErrorCode HLTTauTrackRoiUpdater::hltExecute(const HLT::TriggerElement*, HLT::TriggerElement* outputTE)
{

  HLT::ErrorCode status = HLT::OK;

  //get RoI descriptor
  const TrigRoiDescriptor* roiDescriptor = 0;
  status = getFeature(outputTE, roiDescriptor);
  if ( status != HLT::OK || roiDescriptor == 0 ) {
    msg() <<  MSG::WARNING << " Failed to find RoiDescriptor " << endmsg;
    return status;
  }

  double leadTrkZ0  = roiDescriptor->zed();
  double leadTrkEta = roiDescriptor->eta();
  double leadTrkPhi = roiDescriptor->phi();


  //look at fast-tracks
  std::vector<const TrackCollection*> vectorFoundTracks;
  const TrackCollection* foundTracks = 0;
 
  status = getFeatures(outputTE, vectorFoundTracks);

  if (status !=HLT::OK) {
    msg() << MSG::ERROR << "No FastTrackFinder container was found.  Aborting pre-selection." << endmsg;
    return HLT::NAV_ERROR;
  }
  else {
    if (vectorFoundTracks.size()<1) {
      msg() << MSG::ERROR << "FastTrackFinder vector was empty.  Aborting pre-selection." << endmsg;
      return HLT::NAV_ERROR;
    }
  }

  // Retrieve last container to be appended
  foundTracks = vectorFoundTracks.back();
  // add a passbits container
  std::unique_ptr<xAOD::TrigPassBits> xBits = xAOD::makeTrigPassBits<TrackCollection>(foundTracks);

  if(foundTracks!=0) msg() << MSG::DEBUG << " Input track collection has size " << foundTracks->size() << endmsg;
  else msg() << MSG::DEBUG << " Input track collection not found " << endmsg;  

  const Trk::Track *leadTrack = 0;
  const Trk::Perigee *trackPer = 0;
  const Trk::TrackSummary* summary = 0;
  double trkPtMax = 0;
  
  if(foundTracks!=0){
    
    TrackCollection::const_iterator it = foundTracks->begin();
    TrackCollection::const_iterator itEnd = foundTracks->end();

    // Find leading track

 

    for (;it!=itEnd;it++){

      const Trk::Track* track = *it;

      trackPer = track->perigeeParameters();
      summary = track->trackSummary();

 
      if(summary==0){
        msg() << MSG::DEBUG << " track summary not available in RoI updater " << trkPtMax << endmsg;
      }

 

      if(trackPer && summary){

 	float trackPt = trackPer->pT();


	if ( trackPt > trkPtMax ) {


	  int nPix  = summary->get(Trk::numberOfPixelHits);
	  if(nPix<0) nPix=0;

	  if(nPix < m_nHitPix) {

	    msg() << MSG::DEBUG << "Track rejected because of nHitPix " << nPix << " < " << m_nHitPix << endmsg;

	    continue;

	  }

 
	  int nPixHole = summary->get(Trk::numberOfPixelHoles);
	  if (nPixHole < 0) nPixHole = 0;
	
	  int nSCTHole = summary->get(Trk::numberOfSCTHoles);
	  if (nSCTHole < 0) nSCTHole = 0;
	
	  if((nPixHole + nSCTHole) > m_nSiHoles) {
	    
	    msg() << MSG::DEBUG << "Track rejected because of nSiHoles " << nPixHole + nSCTHole << " > " << m_nSiHoles << endmsg;
	    
	    continue;
	  
	  }
	
		  
	  leadTrack = (*it);
	  trkPtMax = trackPt;
	  xBits->markPassing(*it, foundTracks, true);
	}
	
	
	
      }

    }


    if(leadTrack) {
      msg() << MSG::DEBUG << " leading track pT " << trkPtMax << endmsg;
    }
    else msg() << MSG::DEBUG << " no leading track pT found " << endmsg;
    
    if(leadTrack){
      leadTrkZ0  = leadTrack->perigeeParameters()->parameters()[Trk::z0];
      leadTrkEta = leadTrack->perigeeParameters()->eta();
      leadTrkPhi = leadTrack->perigeeParameters()->parameters()[Trk::phi0];
    }
    
  }
  
  /// always update z
  double z0Min = leadTrkZ0 - m_z0HalfWidth;
  double z0Max = leadTrkZ0 + m_z0HalfWidth;


  /// update eta if required (by default)
  double eta      = roiDescriptor->eta();
  double etaMinus = roiDescriptor->etaMinus();
  double etaPlus  = roiDescriptor->etaPlus();

  if ( leadTrack && m_updateEta ) { 
    eta      = leadTrkEta;
    etaMinus = leadTrkEta - (roiDescriptor->eta() - roiDescriptor->etaMinus() );
    etaPlus  = leadTrkEta + (roiDescriptor->etaPlus() - roiDescriptor->eta() );
  }


  /// update phi if required
  double phi      = roiDescriptor->phi();
  double phiMinus = roiDescriptor->phiMinus();
  double phiPlus  = roiDescriptor->phiPlus();

  
  if ( leadTrack && m_updatePhi ) { 
    phi      = leadTrkPhi;
    /// FIXME: This attempts to handle the phi wrapping correctly across the phi = pi boundary
    ///        but the values themselves would still need to be wrapped - the RoiDescriptor
    ///        should do it in the constructor, but even so, this calculation should still 
    ///        be checked before use  
    if ( roiDescriptor->phi()>=roiDescriptor->phiMinus() ) phiMinus = leadTrkPhi - (roiDescriptor->phi() - roiDescriptor->phiMinus() );
    else                                                   phiMinus = leadTrkPhi - (roiDescriptor->phi() - roiDescriptor->phiMinus() + 2*M_PI );
    
    if ( roiDescriptor->phiPlus()>=roiDescriptor->phi() ) phiPlus  = leadTrkPhi + (roiDescriptor->phiPlus() - roiDescriptor->phi() );
    else                                                  phiPlus  = leadTrkPhi + (roiDescriptor->phiPlus() - roiDescriptor->phi() + 2*M_PI );
  }
  
  TrigRoiDescriptor *outRoi = new TrigRoiDescriptor(roiDescriptor->roiWord(), roiDescriptor->l1Id(), roiDescriptor->roiId(),
						    eta, etaMinus, etaPlus,
						    phi, phiMinus, phiPlus,
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

  if(attachFeature(outputTE, xBits.release(), "passbits") != HLT::OK)   {
    ATH_MSG_ERROR("Could not attach feature to the TE");
    return HLT::NAV_ERROR;
  } else {
    ATH_MSG_DEBUG("REGTEST: attached bits " << xBits.release());
  }
  
  return HLT::OK;
  
}

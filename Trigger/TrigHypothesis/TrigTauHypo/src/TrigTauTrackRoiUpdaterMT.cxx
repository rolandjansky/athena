/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include <cmath>

#include "TrigTauTrackRoiUpdaterMT.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "CxxUtils/phihelper.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "xAODTrigger/TrigPassBits.h"


TrigTauTrackRoiUpdaterMT::TrigTauTrackRoiUpdaterMT(const std::string & name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator)
{
}

TrigTauTrackRoiUpdaterMT::~TrigTauTrackRoiUpdaterMT()
{
}

StatusCode TrigTauTrackRoiUpdaterMT::initialize()
{

  ATH_MSG_DEBUG( "Initializing " << name() << " ... " );

  ATH_MSG_DEBUG( "declareProperty review:"   );
  ATH_MSG_DEBUG( " TrigTauTrackRoiUpdaterMT parameters 	" 	);
  ATH_MSG_DEBUG( " z0HalfWidth               " 	<< m_z0HalfWidth );
  ATH_MSG_DEBUG( " nHitPix            			" 	<< m_nHitPix );
  ATH_MSG_DEBUG( " nSiHoles            		" 	<< m_nSiHoles );
  ATH_MSG_DEBUG( " UpdateEta            		" 	<< m_updateEta );
  ATH_MSG_DEBUG( " UpdatePhi            		" 	<< m_updatePhi );


  ATH_MSG_DEBUG( "Initialising HandleKeys" );
  CHECK( m_roIInputKey.initialize()        );
  CHECK( m_tracksKey.initialize() );
  CHECK( m_roIOutputKey.initialize()  );

  return StatusCode::SUCCESS;

}

StatusCode TrigTauTrackRoiUpdaterMT::execute(const EventContext& ctx) const
{


  ATH_MSG_DEBUG( "Running "<< name() <<" ... " );

  // Prepare Outputs
  auto roICollection = std::make_unique<TrigRoiDescriptorCollection>();

  // Retrieve Input TrackCollection
  SG::ReadHandle< TrackCollection > TrackCollectionHandle = SG::makeHandle( m_tracksKey,ctx );
  CHECK( TrackCollectionHandle.isValid() );
  const TrackCollection *foundTracks = TrackCollectionHandle.get();
  ATH_MSG_DEBUG( "Found " << foundTracks->size() << " fast Tracks found, updating the corresponding RoI ... " );

  if(foundTracks) {
    ATH_MSG_DEBUG( "REGTEST: Size of vector CaloCluster container is " << foundTracks->size());
    if(foundTracks->size()==0) {
      ATH_MSG_DEBUG( "Cannot proceed, size of vector CaloCluster container is " << foundTracks->size());
      return StatusCode::SUCCESS;
    }
  }else {
    ATH_MSG_DEBUG( "no CaloCluster container found " );
    return StatusCode::SUCCESS;
  }

  //get RoI descriptor
  SG::ReadHandle< TrigRoiDescriptorCollection > roisHandle = SG::makeHandle( m_roIInputKey, ctx );
  ATH_MSG_DEBUG("Size of roisHandle: "<<roisHandle->size());
  const TrigRoiDescriptor *roiDescriptor = roisHandle->at(0);

  ATH_MSG_DEBUG( "; RoI ID = " << roiDescriptor->roiId()
       << ": Eta = " << roiDescriptor->eta()
       << ", Phi = " << roiDescriptor->phi() );

  double leadTrkZ0  = roiDescriptor->zed();
  double leadTrkEta = roiDescriptor->eta();
  double leadTrkPhi = roiDescriptor->phi();

  if(foundTracks!=0) ATH_MSG_DEBUG(" Input track collection has size " << foundTracks->size() );
  else ATH_MSG_DEBUG(" Input track collection not found " );  

  const Trk::Track *leadTrack = nullptr;
  const Trk::Perigee *trackPer = nullptr;
  const Trk::TrackSummary* summary = nullptr;
  double trkPtMax = 0;
  
  if(foundTracks!=0){
    
    // Find leading track
    for (const Trk::Track* track : *foundTracks){
      trackPer = track->perigeeParameters();
      summary = track->trackSummary();
      if(summary==nullptr) ATH_MSG_ERROR(" track summary not available in RoI updater " << trkPtMax << " " << name() );
      if(trackPer && summary){
 	     float trackPt = trackPer->pT();
	     if ( trackPt > trkPtMax ) {
	       int nPix  = summary->get(Trk::numberOfPixelHits);
	       if(nPix<0) nPix=0;
	       if(nPix < m_nHitPix) {
            if(msgLvl(MSG::DEBUG))  msg() << MSG::DEBUG <<" Track rejected because of nHitPix " << nPix << " < " << m_nHitPix
            << " " << name() << ": in execute()" << endmsg;
	         continue;
	       }
	       int nPixHole = summary->get(Trk::numberOfPixelHoles);
	       if (nPixHole < 0) nPixHole = 0;
	       int nSCTHole = summary->get(Trk::numberOfSCTHoles);
	       if (nSCTHole < 0) nSCTHole = 0;
	       if((nPixHole + nSCTHole) > m_nSiHoles) {
            if(msgLvl(MSG::DEBUG) )  msg() << MSG::DEBUG <<" Track rejected because of nSiHoles " << nPixHole+nSCTHole << " > " << m_nSiHoles 
            << " " << name() << ": in execute()" << endmsg;
	         continue;
	       }
	       leadTrack = track;
	       trkPtMax = trackPt;
	     }
      }
    }


    if(leadTrack) {
      ATH_MSG_DEBUG(" leading track pT " << trkPtMax );
      leadTrkZ0  = leadTrack->perigeeParameters()->parameters()[Trk::z0];
      leadTrkEta = leadTrack->perigeeParameters()->eta();
      leadTrkPhi = leadTrack->perigeeParameters()->parameters()[Trk::phi0];
    }else ATH_MSG_DEBUG(" no leading track pT found " );
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
    double deltaPhiMinus = roiDescriptor->phi() - roiDescriptor->phiMinus();
    double deltaPhiPlus  = roiDescriptor->phiPlus()-roiDescriptor->phiMinus();
    phiMinus = leadTrkPhi - CxxUtils::wrapToPi(deltaPhiMinus);  
    phiPlus  = leadTrkPhi + CxxUtils::wrapToPi(deltaPhiPlus); 

  }
  
  // Prepare the new RoI
  TrigRoiDescriptor *outRoi = new TrigRoiDescriptor(roiDescriptor->roiWord(), roiDescriptor->l1Id(), roiDescriptor->roiId(),
						    eta, etaMinus, etaPlus,
						    phi, phiMinus, phiPlus,
						    leadTrkZ0, z0Min, z0Max);
  
  ATH_MSG_DEBUG("Input RoI " << *roiDescriptor);
  ATH_MSG_DEBUG("Output RoI " << *outRoi);

  roICollection->push_back(outRoi);

  // Save Outputs
  ATH_MSG_DEBUG( "Saving RoIs to be used as input to Fast Tracking -- TO BE CHANGED -- ::: " << m_roIOutputKey.key() );
  SG::WriteHandle< TrigRoiDescriptorCollection > outputRoiHandle = SG::makeHandle( m_roIOutputKey,ctx );
  CHECK( outputRoiHandle.record( std::move( roICollection ) ) );

  return StatusCode::SUCCESS;

}

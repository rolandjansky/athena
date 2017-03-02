/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigT2HistoPrmVtxAllTE.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2HistoPrmVtx
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro@ge.infn.it
// 
// ************************************************

#include "TrigT2HistoPrmVtx/TrigT2HistoPrmVtxAllTE.h"
#include "TrigT2HistoPrmVtx/HistoVertexHelper.h"
#include "TrigT2HistoPrmVtxBase.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODBase/IParticle.h"

#include "InDetBeamSpotService/IBeamCondSvc.h"



//** ----------------------------------------------------------------------------------------------------------------- **//


TrigT2HistoPrmVtxAllTE::TrigT2HistoPrmVtxAllTE(const std::string& name, ISvcLocator* pSvcLocator) :
  HLT::AllTEAlgo(name, pSvcLocator),
  c( new TrigT2HistoPrmVtxBase(msg(), msgLvl()) ),
  m_constTrigT2HistoPrmVtxBase(0),
  m_trigVertexColl(0)
{
  declareProperty ("AlgoId",             c->m_algo);
  declareProperty ("Instance",           c->m_instance);

  declareProperty ("NumBins",            c->m_nBins);

  declareProperty ("UseBeamSpot",        c->m_useBeamSpot        = false);
  declareProperty ("UseEtaPhiTrackSel",  c->m_useEtaPhiTrackSel  = false);

  declareProperty ("EFTrkSel_Chi2",      c->m_efTrkSelChi2       = 0.0);
  declareProperty ("EFTrkSel_BLayer",    c->m_efTrkSelBLayer     = 1);
  declareProperty ("EFTrkSel_PixHits",   c->m_efTrkSelPixHits    = 2);
  declareProperty ("EFTrkSel_SiHits",    c->m_efTrkSelSiHits     = 7);
  declareProperty ("EFTrkSel_D0",        c->m_efTrkSelD0         = 1*CLHEP::mm);
  declareProperty ("EFTrkSel_Pt",        c->m_efTrkSelPt         = 1*CLHEP::GeV);

  declareMonitoredVariable("PrmVtx",          c->zPrmVtx[0]          = -1);
  declareMonitoredVariable("PrmVtxSigmaAll",  c->zPrmVtxSigmaAll[0]  = -1);
  declareMonitoredVariable("PrmVtxSigma2Trk", c->zPrmVtxSigma2Trk[0] = -1);
  declareMonitoredVariable("TracksPerVtx",    c->nTrackVtx[0]        = -1);
  declareMonitoredVariable("NVtxFound",       c->nVtxFound           = -1);

  declareMonitoredObject("Trk_selectedTracks", m_constTrigT2HistoPrmVtxBase, &TrigT2HistoPrmVtxBase::totSelectedTracks);
  declareMonitoredStdContainer("Trk_stepsToSelect", c->m_listCutApplied);
}


//** ----------------------------------------------------------------------------------------------------------------- **//


TrigT2HistoPrmVtxAllTE::~TrigT2HistoPrmVtxAllTE() {}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigT2HistoPrmVtxAllTE::hltInitialize() {

  // Get message service
  if (msgLvl() <= MSG::INFO) 
    msg() << MSG::INFO << "Initializing TrigT2HistoPrmVtxAllTE, version " << PACKAGE_VERSION << endmsg;
  
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "declareProperty review:" << endmsg;
    msg() << MSG::DEBUG << " AlgoId = "              << c->m_algo << endmsg; 
    msg() << MSG::DEBUG << " Instance = "            << c->m_instance << endmsg; 
    msg() << MSG::DEBUG << " Numbins = "             << c->m_nBins << endmsg; 
    msg() << MSG::DEBUG << " UseBeamSpot = "         << c->m_useBeamSpot << endmsg; 
    msg() << MSG::DEBUG << " UseEtaPhiTrackSel = "   << c->m_useEtaPhiTrackSel << endmsg;

    if (c->m_instance == "_EF" || c->m_instance == "_HLT") {

      msg() << MSG::DEBUG << " EFTrkSel_Chi2 = "     << c->m_efTrkSelChi2 << endmsg; 
      msg() << MSG::DEBUG << " EFTrkSel_BLayer = "   << c->m_efTrkSelBLayer << endmsg; 
      msg() << MSG::DEBUG << " EFTrkSel_PixHits = "  << c->m_efTrkSelPixHits << endmsg; 
      msg() << MSG::DEBUG << " EFTrkSel_SiHits = "   << c->m_efTrkSelSiHits << endmsg;    
      msg() << MSG::DEBUG << " EFTrkSel_D0 = "       << c->m_efTrkSelD0 << endmsg; 
      msg() << MSG::DEBUG << " EFTrkSel_Pt = "       << c->m_efTrkSelPt << endmsg; 

    }
  }

  m_constTrigT2HistoPrmVtxBase = const_cast<const TrigT2HistoPrmVtxBase*>(c);

  return HLT::OK;
}

//** ----------------------------------------------------------------------------------------------------------------- **//


unsigned int TrigT2HistoPrmVtxAllTE::getTrackNumbers(const xAOD::TrackParticleContainer* pointerToEFTrackCollections) {
  
  unsigned int nEFtracks;
  
  if (pointerToEFTrackCollections) {
    nEFtracks = pointerToEFTrackCollections->size();
    if (msgLvl() <= MSG::DEBUG)  
      msg() << MSG::DEBUG << "Found " << nEFtracks << " tracks in the RoI" << endmsg;
  } else {
    nEFtracks = 0;
    if (msgLvl() <= MSG::DEBUG)  
      msg() << MSG::DEBUG << "No tracks in the RoI" << endmsg;
  }
  
  return nEFtracks;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigT2HistoPrmVtxAllTE::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& inputTE, unsigned int output) {

  // since this is an AllTEAlgo, we have to call the monitoring ourselves:
  beforeExecMonitors().ignore();

  if (msgLvl() <= MSG::DEBUG) 
    msg() << MSG::DEBUG << "Executing TrigT2HistoPrmVtxAllTE" << endmsg;

  float zFirstTrack=0;
  float zFirstTrackError=0;

  c->m_totTracks=0;
  c->m_totTracks_All=0;
  c->m_totSelTracks=0;
  c->m_totSelTracks_All=0;

  c->m_hisVtx->reset();

  for (int i = 0; i <3; i++) {
    c->zPrmVtx.at(i) = -200;
    c->zPrmVtxSigmaAll.at(i)  = -200;
    c->zPrmVtxSigma2Trk.at(i) = -200;
    c->nTrackVtx.at(i) = -200;
  }

  // Retrieve beamspot information
  c->m_xBeamSpot = c->m_yBeamSpot = c->m_zBeamSpot = 0;
  c->m_xBeamSpotSigma = c->m_yBeamSpotSigma = c->m_zBeamSpotSigma = 1e-6;
  c->m_xBeamSpotTilt = c->m_yBeamSpotTilt = 0;
  
  if(c->m_useBeamSpot) {
    
    IBeamCondSvc* m_iBeamCondSvc; 
    StatusCode scBS = service("BeamCondSvc", m_iBeamCondSvc);
    
    if (scBS.isFailure() || m_iBeamCondSvc == 0) {
      m_iBeamCondSvc = 0;
      
      if (msgLvl() <= MSG::WARNING) {
	msg() << MSG::WARNING << "Could not retrieve Beam Conditions Service. " << endmsg;
	msg() << MSG::WARNING << "Using origin at ( " << c->m_xBeamSpot << " , " << c->m_yBeamSpot << " , " << c->m_zBeamSpot << " ) " << endmsg;
      }
      
    } else {
      
      Amg::Vector3D m_beamSpot = m_iBeamCondSvc->beamPos();
      
      c->m_xBeamSpot = m_beamSpot.x();
      c->m_yBeamSpot = m_beamSpot.y();
      c->m_zBeamSpot = m_beamSpot.z();
      c->m_xBeamSpotSigma = m_iBeamCondSvc->beamSigma(0);
      c->m_yBeamSpotSigma = m_iBeamCondSvc->beamSigma(1);
      c->m_zBeamSpotSigma = m_iBeamCondSvc->beamSigma(2);
      c->m_xBeamSpotTilt = m_iBeamCondSvc->beamTilt(0);
      c->m_yBeamSpotTilt = m_iBeamCondSvc->beamTilt(1);

      if(msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << "Beam spot from service: x = " << c->m_xBeamSpot << " +/- " << c->m_xBeamSpotSigma << "   "
	      << "y = " << c->m_yBeamSpot << " +/- " << c->m_yBeamSpotSigma << "   "
	      << "z = " << c->m_zBeamSpot << " +/- " << c->m_zBeamSpotSigma << endmsg;
    }
  }
  
  //the outher vector has size 1 in this case
  for (HLT::TEVec::const_iterator inner_it = inputTE[0].begin(); inner_it != inputTE[0].end(); ++inner_it) {
   
    // Get RoI descriptor
    const TrigRoiDescriptor* roiDescriptor; 
    
    if ((getFeature(*inner_it, roiDescriptor) == HLT::OK) && roiDescriptor) {
      if (msgLvl() <= MSG::DEBUG) {
	msg() << MSG::DEBUG << "Using inputTE: "
	      << "RoI id " << roiDescriptor->roiId()
	      << ", Phi = " <<  roiDescriptor->phi()
	      << ", Eta = " << roiDescriptor->eta() << endmsg;
      }
    }
    else {
      if (msgLvl() <= MSG::WARNING) 
	msg() <<  MSG::WARNING << "No RoI for this Trigger Element " << endmsg;
    
      return HLT::NAV_ERROR;
    }
    
    // Create pointers to collections 
    const xAOD::TrackParticleContainer* pointerToEFTrackCollections=0;

    zFirstTrack=0;
    c->m_totTracks=0;
    c->m_totSelTracks=0;
    
    if (c->m_instance == "_EF") { 
      
      if (msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << "Executing TrigT2HistoPrmVtxAllTE at EF" << endmsg;

      HLT::ErrorCode status = getFeature(*inner_it, pointerToEFTrackCollections, ""); 
      if (status != HLT::OK) {
	msg() << MSG::DEBUG << "No tracks retrieved from TrigT2HistoPrmVtxAllTE" << endmsg;
      } 
      else if (msgLvl() <= MSG::DEBUG)  
	msg() << MSG::DEBUG << "Got collection from TrigT2HistoPrmVtxAllTE" << endmsg;   
      
      // Get tracks number
      c->m_totTracks = getTrackNumbers(pointerToEFTrackCollections);
      c->m_totTracks_All += c->m_totTracks;
      c->m_listCutApplied.clear();

      // Retrieve z0 parameters  
      for (unsigned int i = 0; i < c->m_totTracks; i++) {
	
	if (msgLvl() <= MSG::DEBUG && i == 0) 
	  msg() << MSG::DEBUG << "Loop over tracks: retrieving z0" << endmsg;
		
	const xAOD::TrackParticle* track = (*pointerToEFTrackCollections)[i];
	float z0 =  track->z0();
	const xAOD::ParametersCovMatrix_t covTrk = track->definingParametersCovMatrix();
	float ez0 = Amg::error(covTrk, 1);

	if (!c->efTrackSel(track, i, roiDescriptor->eta(), roiDescriptor->phi())) continue;
	
	c->m_totSelTracks++;
	c->m_totSelTracks_All++;

	if (c->m_totSelTracks == 1) {
	  zFirstTrack=z0;
	  zFirstTrackError=ez0;
	}

	if (msgLvl() <= MSG::DEBUG)
	  msg() << MSG::DEBUG << "Track number " << i+1 << " with z0 = " << z0 << " and ez0 = " << ez0 << endmsg;
      
	c->m_hisVtx->fill(z0);
	//std::cout << "PRMVTX-USING track " << z0 << std::endl;
      }
    }
  }

  // Get primary vertex from TrigT2HistoPrmVtxAllTE::findPrmVtx
  if (c->m_totSelTracks_All == 1) {
    c->zPrmVtx.at(0)          = zFirstTrack;
    c->zPrmVtxSigmaAll.at(0)  = zFirstTrackError;
    c->nTrackVtx.at(0)        = 1;
    c->nVtxFound = 1;
  } else {
    c->findPrmVtx();
  }

  if (msgLvl() <= MSG::DEBUG) {
    for (int i = 0; i <3; i++)
      msg() << MSG::DEBUG << "Primary vertex " << i << ": z=" << c->zPrmVtx.at(i) << ", sigma=" << c->zPrmVtxSigmaAll.at(i) 
	    << ", nTracks=" << c->nTrackVtx.at(i) << ", and ntot track in all ROIs" << c->m_totSelTracks_All << endmsg;
  }
  
  if (msgLvl() <= MSG::DEBUG) {

    const EventInfo* pEventInfo;
    
    if ( !store() || store()->retrieve(pEventInfo).isFailure() ) {
      msg()  << MSG::DEBUG << "Failed to get EventInfo " << endmsg;
    } else {
      msg() << MSG::DEBUG << "PrmVtx summary (Run " << pEventInfo->event_ID()->run_number()
	    << "; Event "<< pEventInfo->event_ID()->event_number() << ")" << endmsg;
      msg() << MSG::DEBUG << "REGTEST: Number of RoIs " << inputTE[0].size() << endmsg;
      msg() << MSG::DEBUG << "REGTEST: Tracks: " << c->m_totTracks_All << " reconstructed and " << c->m_totSelTracks_All <<" selected" << endmsg;
      msg() << MSG::DEBUG << "REGTEST: Primary vertex: " << c->zPrmVtx.at(0) << " +/ " << c->zPrmVtxSigmaAll.at(0) << " with " << c->nTrackVtx.at(0) << " tracks" << endmsg;
    }
  }

  // Create the xAOD container and its auxiliary store:
  m_trigVertexColl = new xAOD::VertexContainer();
  xAOD::VertexAuxContainer aux;
  m_trigVertexColl->setStore( &aux );
  xAOD::Vertex* trigVertex = new xAOD::Vertex();
  m_trigVertexColl->push_back(trigVertex); 

  trigVertex->setZ(c->zPrmVtx.at(0) + c->m_zBeamSpot);
//  trigVertex->setX(c->m_xBeamSpot);
//  trigVertex->setY(c->m_yBeamSpot);
  trigVertex->setX(c->m_xBeamSpot + tan(c->m_xBeamSpotTilt) * trigVertex->z());
  trigVertex->setY(c->m_yBeamSpot + tan(c->m_yBeamSpotTilt) * trigVertex->z());
  trigVertex->setVertexType(xAOD::VxType::PriVtx);

  AmgSymMatrix(3) cov;
  cov(0,0) = c->m_xBeamSpotSigma*c->m_xBeamSpotSigma;
  cov(1,1) = c->m_yBeamSpotSigma*c->m_yBeamSpotSigma;
  cov(2,2) = c->zPrmVtxSigmaAll.at(0)*c->zPrmVtxSigmaAll.at(0);
  cov(0,1) = cov(1,0) = 0.0; 
  cov(0,2) = cov(2,0) = 0.0;
  cov(1,2) = cov(2,1) = 0.0;
  trigVertex->setCovariancePosition(cov);

  // Create an output TE seeded by an empty vector
  HLT::TEVec allTEs;
  HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(allTEs, output);
  outputTE->setActiveState(true);

  string key;
  
  if (c->m_instance == "_EF" || c->m_instance == "_HLT" ) key = "EFHistoPrmVtx";

  HLT::ErrorCode stat = attachFeature(outputTE, m_trigVertexColl, key);

  if (stat != HLT::OK) {
    if (msgLvl() <= MSG::WARNING) 
      msg() << MSG::WARNING << "Failed to attach xAOD::VertexCollection to navigation" << endmsg;
    return stat;
  }
  else {
    if (msgLvl() <= MSG::DEBUG) 
      msg() << MSG::DEBUG << "Attached xAOD::VertexCollection to navigation" << endmsg;
  }
  
  // since this is an AllTEAlgo, we have to call the monitoring ourselves:
  afterExecMonitors().ignore();
  
  return HLT::OK;
}

//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigT2HistoPrmVtxAllTE::hltFinalize() {

  if (msgLvl() <= MSG::INFO) 
    msg() << MSG::INFO << "Finalizing TrigT2HistoPrmVtxAllTE" << endmsg;

  return HLT::OK;
}

 


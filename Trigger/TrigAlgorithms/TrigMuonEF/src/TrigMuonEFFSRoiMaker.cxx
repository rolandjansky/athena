/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEFFSRoiMaker.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigNavigation/TriggerElement.h"

#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"

//#include "AthContainers/ConstDataVector.h"

/**
 * Constructor - set up the algorithm
 */
TrigMuonEFFSRoiMaker::TrigMuonEFFSRoiMaker(const std::string & name, ISvcLocator* pSvcLocator) :
  HLT::AllTEAlgo(name, pSvcLocator),
  m_classIDSvc("ClassIDSvc/ClassIDSvc", this->name())
{
  declareProperty ("MuonPtCut", m_ptcut=-1.0);
  declareProperty ("OutputContName", m_efContName);
  declareProperty ("InvertRoI", m_doInvertRoI=false);
  declareProperty ("UseFSRoI" , m_useFS=true);  
  declareProperty ("CreateCrackRoI", m_createCrackRoI=false);
  declareProperty ("CreateFSRoI", m_createFSroi=false);
  declareProperty ("CreateRoIFromL1", m_createRoIfromL1=false);
  declareProperty ("RoISizeEta", m_roiSizeEta=0.1);
  declareProperty ("RoISizePhi", m_roiSizePhi=0.1);
  declareProperty ("RoILabel", m_roiLabel="");

  declareMonitoredVariable("nTrkIn", m_nTrkIn, IMonitoredAlgo::AutoClear);
  declareMonitoredVariable("nRoIOut", m_nRoIOut, IMonitoredAlgo::AutoClear);
}

/**
 * Destructor - clean up when done
 */
TrigMuonEFFSRoiMaker::~TrigMuonEFFSRoiMaker() {

}

/**
 * Initialize the algorithm
 */
HLT::ErrorCode TrigMuonEFFSRoiMaker::hltInitialize() {

  if (msgLvl() <= MSG::INFO) {
    msg() << MSG::INFO << "Initializing TrigMuonEFFSRoiMaker[" << name() << "], version " << PACKAGE_VERSION << endmsg;
    msg() << MSG::INFO << "pT cut on muons = " << m_ptcut << " MeV" << endmsg;
    msg() << MSG::INFO << "Output EF container name = " << m_efContName << endmsg;
    msg() << MSG::INFO << "RoI eta,phi = " << m_roiSizeEta << "," << m_roiSizePhi << endmsg;
    msg() << MSG::INFO << "RoI Label = " << m_roiLabel << endmsg;
    msg() << MSG::INFO << "Invert RoI = " << m_doInvertRoI << endmsg;
    msg() << MSG::INFO << "Use FS RoI = " << m_useFS << endmsg;
    msg() << MSG::INFO << "Create crack RoI = " << m_createCrackRoI << endmsg;
    msg() << MSG::INFO << "Create FS RoI = " << m_createFSroi << endmsg;
    msg() << MSG::INFO << "Create RoI from L1 = " << m_createRoIfromL1 << endmsg;
  }

  if (m_classIDSvc.retrieve().isFailure()) {
    msg() << MSG::WARNING << "Cannot retrieve ClassIDSvc" << endmsg;
    return HLT::BAD_JOB_SETUP;
  }
  else {
    msg() << MSG::INFO << "Retrieved " << m_classIDSvc << endmsg;
  }

  return HLT::OK;
}//hltInitialize

/**
 * Finalize the algorithm
 */
HLT::ErrorCode TrigMuonEFFSRoiMaker::hltFinalize() {

  return HLT::OK;
}//hltFinalize

HLT::ErrorCode TrigMuonEFFSRoiMaker::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& inputTEs, unsigned int output) {

  // reset monitoring variables
  beforeExecMonitors().ignore();
  m_nTrkIn=0;
  m_nRoIOut=0;

  if(m_createFSroi){
    ATH_MSG_DEBUG("creating full scan RoI");
    TrigRoiDescriptor* roiFS = new TrigRoiDescriptor(0.0, -2.5, 2.5, 0.0, -M_PI, M_PI-0.0001);
    HLT::TriggerElement* te = nullptr;
    te = addRoI(output, nullptr ); 
    te->setActiveState(true);
    std::string key;
    m_config->getNavigation()->attachFeature(te, roiFS, HLT::Navigation::ObjectCreatedByNew, key, m_roiLabel);
    return HLT::OK;
  }

  //create RoI from L1 RoIs. Make sure this is run first in the sequence (needed for MGonly chains).
  if(m_createRoIfromL1){
    HLT::TriggerElement* te;
    const HLT::TEVec &tev = inputTEs.back();
    for ( unsigned iTe=0; iTe<tev.size() ; iTe++ ){
      const HLT::TriggerElement* inputTe = tev[iTe];
      
      if(!inputTe){
	msg() << MSG::ERROR << "Invalid TriggerElement pointer = "<< inputTe << endmsg;
	return HLT::ERROR;
      } // end if(!inputTe)
	
	// get the RoiDescriptor
      const TrigRoiDescriptor* roiDescriptor = 0;
      HLT::ErrorCode hltStatus = getFeature(inputTe, roiDescriptor);
      if( hltStatus != HLT::OK ){
	msg()<<MSG::ERROR<<" Failed to find RoiDescriptor "<<endmsg;
	return hltStatus;
      } 
      te = addRoI(output, nullptr);
      te->setActiveState(true); // set this trigger element to be active
      std::string key;
      m_config->getNavigation()->attachFeature(te, roiDescriptor, HLT::Navigation::ObjectCreatedByNew, key, m_roiLabel);	
    }
    return HLT::OK;
  }

  //start of FS roi building
  // we run after FS instance - so should only be one inputTE
  if (inputTEs.size() < 1) {
    msg() << MSG::ERROR << "Got less than one inputTE" << endmsg;
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
  }
  for(auto inputTE : inputTEs){
    ATH_MSG_DEBUG( "inputTE.size() " << inputTE.size() );

    if(inputTE.size() < 1) {
      ATH_MSG_ERROR("Size of the vector<HLT::TriggerElement*> < 1 - no input to process, abort chain");
      return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
    }

    //Create RoI centered around the crack in MS
    if (m_createCrackRoI) {

      TrigRoiDescriptor* newRoI = new TrigRoiDescriptor(0.0, -m_roiSizeEta, m_roiSizeEta, 0.0, -m_roiSizePhi, m_roiSizePhi);
    
      HLT::TriggerElement* outputTE = nullptr;
      if (m_roiLabel == "") {
	//Add RoI with default label
	outputTE = addRoI(output, newRoI);
      } else {
	//Create RoI with no descriptor then attach descriptor as feature with desired label
	std::string key;
	outputTE = addRoI(output, nullptr);
	m_config->getNavigation()->attachFeature(outputTE, newRoI, HLT::Navigation::ObjectCreatedByNew, key, m_roiLabel);
      }

      outputTE->setActiveState(true);
      ATH_MSG_DEBUG("RoI:" << *newRoI);
      return HLT::OK;
    }

    // Get MuonContainer
    const xAOD::MuonContainer* muonContainer(0);
    for (auto inTE : inputTE) {
      HLT::ErrorCode status = getFeature(inTE, muonContainer);

      if(status!=HLT::OK || muonContainer==0) {
	ATH_MSG_WARNING( "Could not get MuonContainer" );
	return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
      }
      ATH_MSG_DEBUG("Got MuonContainer with size = " << muonContainer->size());

      // Loop on container & create ROIs.
      for(auto muon : *muonContainer) {

	const xAOD::TrackParticle* saTrack = muon->primaryTrackParticle();
	if(!saTrack) {
	  ATH_MSG_WARNING("No track particle, skip this muon");
	  continue;
	}
	++m_nTrkIn; // count input tracks
	double phi = saTrack->phi();
	double eta = saTrack->eta();
	double pt = saTrack->pt();

	/* Some debugging of the error matrix - need to fix for Eigen
	   if(msgLvl(MSG::DEBUG)) {
	   const Amg::AmgSymMatrix(5)* em = perigee->covariance();
	   msg(MSG::DEBUG) << "Err[Trk::z0] = " << Amg::error(*em,Trk::z0) << endmsg;
	   msg(MSG::DEBUG) << "Cov[Trk::phi0] = " << em.covValue(Trk::phi0) << endmsg;
	   msg(MSG::DEBUG) << "Cov[Trk::theta] = " << em.covValue(Trk::theta) << endmsg;
	   msg(MSG::DEBUG) << "Cov[Trk::qoverp] = " << em.covValue(Trk::qOverP) << endmsg;
	   }//DEBUG
	   } else {
	   msg(MSG::DEBUG) << "No Trk::MeasuredPerigee" << endmsg;
	   }
	*/

	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Extrapolated track, pT, eta, phi = " << pt << ", " << eta << ", " << phi << endmsg;
      
	if (m_doInvertRoI) {
	  eta = -eta; //Flip RoI in eta
	  if (phi > 0) phi -= M_PI; //Shift phi by pi
	  else phi += M_PI;
        
	  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Reversed extrapolated track, pT, eta, phi = " << pt << ", " << eta << ", " << phi << endmsg;
	}

	// check pT of track
	if( pt < m_ptcut) {
	  ATH_MSG_DEBUG( "Track fails pT cut, ignore" );
	  continue;
	}

	// create a new ROI from extrapolated track - make it active
	double minphi = phi-m_roiSizePhi;
	if(minphi < -M_PI) minphi = 2.0*M_PI + minphi;
	double maxphi = phi+m_roiSizePhi;
	if(maxphi > M_PI) maxphi = maxphi - 2.0*M_PI;
	TrigRoiDescriptor* newRoI = new TrigRoiDescriptor(eta, eta-m_roiSizeEta, eta+m_roiSizeEta, phi, minphi, maxphi);
      
	HLT::TriggerElement* outputTE = nullptr;
	if (m_roiLabel == "") {
	  outputTE = addRoI(output, newRoI);
	} else {
	  std::string key;
	  outputTE = addRoI(output, nullptr);
	  //status =  attachFeature(outputTE, newRoI, m_roiLabel);
	  m_config->getNavigation()->attachFeature(outputTE, newRoI, HLT::Navigation::ObjectCreatedByNew, key, m_roiLabel);
	  //if (status != HLT::OK) {
	  //  ATH_MSG_ERROR("Failed to attach new RoI.");
	  //  return status;
	  // }
	}

	outputTE->setActiveState(true);
	ATH_MSG_DEBUG("Added RoI with eta, phi = " << eta << ", " << phi << ", outputTE = " << outputTE->getId());
	++m_nRoIOut; // count created RoIs

	ATH_MSG_DEBUG("Create output container");
	// make a new MuonContainer with just this muon
	xAOD::MuonContainer* outputContainer = new xAOD::MuonContainer(1);
	xAOD::MuonAuxContainer auxCont;
	outputContainer->setStore(&auxCont);
	// using deep copy for now 
	// maybe better to use ConstDataVector<MuonContainer> - but need to be able to attach to trigger element
	//ConstDataVector<xAOD::MuonContainer>* outputContainer = new ConstDataVector<xAOD::MuonContainer>(1, SG::VIEW_ELEMENTS);
	//(*outputContainer)[0] = const_cast<xAOD::Muon*>(muon);

	// deep copy the muon
	xAOD::Muon* muoncopy = new xAOD::Muon();
	(*outputContainer)[0] =  muoncopy;
	*muoncopy = *muon;

	status = attachFeature(outputTE, outputContainer, m_efContName);
	if ( status != HLT::OK ) {
	  msg() << MSG::ERROR << "Failed to attach MuonContainer" << endmsg;
	  return status;
	} else {
	  ATH_MSG_DEBUG("Successfully attached new MuonContainer to " << outputTE->getId());
	}
      }//loop on muons
    }
  }
  // fill monitor variables "by hand" because this is an allTE algo
  afterExecMonitors().ignore();

  return HLT::OK;  
}//hltExecute

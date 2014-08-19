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
  declareProperty ("OutputContName", m_efContName="EFMuonFromRoiMaker");

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
    msg() << MSG::INFO << "Initializing TrigMuonEFFSRoiMaker[" << name() << "], version " << PACKAGE_VERSION << endreq;
    msg() << MSG::INFO << "pT cut on muons = " << m_ptcut << " MeV" << endreq;
    msg() << MSG::INFO << "Output EF container name = " << m_efContName << endreq;
  }

  if (m_classIDSvc.retrieve().isFailure()) {
    msg() << MSG::WARNING << "Cannot retrieve ClassIDSvc" << endreq;
    return HLT::BAD_JOB_SETUP;
  }
  else {
    msg() << MSG::INFO << "Retrieved " << m_classIDSvc << endreq;
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

  // we run after FS instance - so should only be one inputTE
  if (inputTEs.size() != 1) {
    msg() << MSG::ERROR << "Got more than one inputTE" << endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
  }
  
  std::vector<HLT::TriggerElement*>& inputTE = inputTEs.at(0);
  ATH_MSG_DEBUG( "inputTE.size() " << inputTE.size() );

  if(inputTE.size() < 1) {
    ATH_MSG_ERROR("Size of the vector<HLT::TriggerElement*> < 1 - no input to process, abort chain");
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
  }

  // Get MuonContainer
  const xAOD::MuonContainer* muonContainer(0);
  HLT::ErrorCode status = getFeature(inputTE.front(), muonContainer);

  if(status!=HLT::OK || muonContainer==0) {
    ATH_MSG_WARNING( "Could not get MuonContainer" );
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
  }
  ATH_MSG_DEBUG("Got MuonContainer with size = " << muonContainer->size());

  // Loop on container & create ROIs.
  for(auto muon : *muonContainer) {

    const xAOD::TrackParticle* saTrack = muon->trackParticle(xAOD::Muon::TrackParticleType::MuonSpectrometerTrackParticle);
    if(!saTrack) {
      ATH_MSG_WARNING("No extrapolated track, skip this muon");
      continue;
    }
    ++m_nTrkIn; // count input tracks
    const double phi = saTrack->phi();
    const double eta = saTrack->eta();
    const double pt = saTrack->pt();

    /* Some debugging of the error matrix - need to fix for Eigen
    if(msgLvl(MSG::DEBUG)) {
      const Amg::AmgSymMatrix(5)* em = perigee->covariance();
      msg(MSG::DEBUG) << "Err[Trk::z0] = " << Amg::error(*em,Trk::z0) << endreq;
      msg(MSG::DEBUG) << "Cov[Trk::phi0] = " << em.covValue(Trk::phi0) << endreq;
      msg(MSG::DEBUG) << "Cov[Trk::theta] = " << em.covValue(Trk::theta) << endreq;
      msg(MSG::DEBUG) << "Cov[Trk::qoverp] = " << em.covValue(Trk::qOverP) << endreq;
    }//DEBUG
    } else {
      msg(MSG::DEBUG) << "No Trk::MeasuredPerigee" << endreq;
    }
    */

    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Extrapolated track, pT, eta, phi = " << pt << ", " << eta << ", " << phi << endreq;

    // check pT of track
    if( pt < m_ptcut) {
      ATH_MSG_DEBUG( "Track fails pT cut, ignore" );
      continue;
    }

    // create a new ROI from extrapolated track - make it active
    double minphi = phi-0.1;
    if(minphi < -M_PI) minphi = 2.0*M_PI + minphi;
    double maxphi = phi+0.1;
    if(maxphi > M_PI) maxphi = maxphi - 2.0*M_PI;
    TrigRoiDescriptor* newRoI = new TrigRoiDescriptor(eta, eta-0.1, eta+0.1, phi, minphi, maxphi);
    HLT::TriggerElement* outputTE = addRoI(output, newRoI);
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
      msg() << MSG::ERROR << "Failed to attach MuonContainer" << endreq;
      return status;
    } else {
      ATH_MSG_DEBUG("Successfully attached new MuonContainer to " << outputTE->getId());
    }
  }//loop on muons
  
  // fill monitor variables "by hand" because this is an allTE algo
  afterExecMonitors().ignore();

  return HLT::OK;  
}//hltExecute

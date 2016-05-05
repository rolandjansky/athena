/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ITHistSvc.h"
#include "InDetTrigParticleCreation/TrigTrackingxAODCnv.h"
#include "TrkToolInterfaces/ITrackParticleCreatorTool.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "VxVertex/VxContainer.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "Particle/TrackParticle.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/LinkToTrack.h"
#include "TrkParameters/TrackParameters.h"
#include "Particle/TrackParticleContainer.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/PhiHelper.h"
#include "IRegionSelector/IRegSelSvc.h"
#include "TrkTrackSummary/TrackSummary.h"

#include "Identifier/Identifier.h"
#include "InDetIdentifier/PixelID.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "IdDictDetDescr/IdDictManager.h"

#include <cmath>
#include <algorithm>

namespace InDet
{


  TrigTrackingxAODCnv::TrigTrackingxAODCnv(const std::string &name,
					   ISvcLocator *pSvcLocator)
    : HLT::FexAlgo (name, pSvcLocator),
      m_particleCreatorTool("Trk::ParticleCreatorTool"),
      m_residualCalc("Trk::ResidualPullCalculator"),
      m_tracks(0),
      m_slice_name(""),
      m_mon_doSliceSpecific(true),
      m_mon_counter(0),
      m_mon_prescale(1),
      m_mon_ptmin(1)
  {
    declareProperty("ParticleCreatorTool", m_particleCreatorTool);
    declareProperty("ResidualPullCalculator", m_residualCalc);

    //+++ DQM (SA): monitoring
    declareProperty("MonSliceSpecific", m_mon_doSliceSpecific);
    declareProperty("MonPrescale",      m_mon_prescale);
    declareProperty("MonPtmin",         m_mon_ptmin);
    // Common for all slices
    declareMonitoredStdContainer("trk_d0",     m_dqm_d0);
    declareMonitoredStdContainer("trk_z0",     m_dqm_z0);
    declareMonitoredStdContainer("trk_phi",    m_dqm_phi);
    declareMonitoredStdContainer("trk_eta",    m_dqm_eta);
    declareMonitoredStdContainer("trk_qoverp", m_dqm_qOverP);
    declareMonitoredStdContainer("trk_nPIXHits", m_dqm_npix_hits);
    declareMonitoredStdContainer("trk_nSCTHits", m_dqm_nsct_hits);
    declareMonitoredStdContainer("trk_nTRTHits", m_dqm_ntrt_hits);
    declareMonitoredStdContainer("trk_chi2dof", m_dqm_chi2dof);
    declareMonitoredStdContainer("roi_etaRoi", m_dqm_eta_roi);
    declareMonitoredStdContainer("roi_phiRoi", m_dqm_phi_roi);
    declareMonitoredVariable("roi_nTrkPart",    m_dqm_ntrkpart);
    declareMonitoredVariable("roi_nTracks",        m_dqm_ntrk);
    declareMonitoredVariable("roi_nVertices",     m_dqm_nvertex);
    declareMonitoredVariable("roi_eta", m_roiEta);
    declareMonitoredVariable("roi_etaWidth", m_roiEtaWidth);
    declareMonitoredVariable("roi_phi", m_roiPhi);
    declareMonitoredVariable("roi_phiWidth", m_roiPhiWidth);
    declareMonitoredVariable("roi_z", m_roiZ);
    declareMonitoredVariable("roi_zWidth", m_roiZ_Width);
    // Bjet slice
    declareMonitoredVariable("bj_sumPt",  m_dqm_bj_sumpt);
    // Muon slice
    declareMonitoredVariable("mu_d0",     m_dqm_mu_d0);
    declareMonitoredVariable("mu_z0",     m_dqm_mu_z0);
    declareMonitoredVariable("mu_eta",    m_dqm_mu_eta);
    declareMonitoredVariable("mu_phi",    m_dqm_mu_phi);
    declareMonitoredVariable("mu_qOverP", m_dqm_mu_qOverP);
    declareMonitoredVariable("mu_pt",     m_dqm_mu_pt);
    declareMonitoredVariable("mu_nPIXHits", m_dqm_mu_npix_hits);
    declareMonitoredVariable("mu_nSCTHits", m_dqm_mu_nsct_hits);
    declareMonitoredVariable("mu_nTRTHits", m_dqm_mu_ntrt_hits);
    declareMonitoredVariable("mu_chi2Dof", m_dqm_mu_chi2dof);
    declareMonitoredVariable("mu_etaRoi", m_dqm_mu_eta_roi);
    declareMonitoredVariable("mu_phiRoi", m_dqm_mu_phi_roi);
    // Tau slice
    declareMonitoredStdContainer("ta_roiDR", m_dqm_ta_roidr);
    declareMonitoredVariable("ta_lPt",    m_dqm_ta_L_pt);
    declareMonitoredVariable("ta_fracPt", m_dqm_ta_frac_pt);
    declareMonitoredVariable("ta_sumCh",  m_dqm_ta_sum_ch);
    declareMonitoredVariable("ta_asyCh",  m_dqm_ta_asy_ch);
    declareMonitoredVariable("ta_m",       m_dqm_ta_m);
    //
    declareMonitoredStdContainer("IBLz", m_dqm_ibl_z);
    declareMonitoredStdContainer("IBLresx", m_dqm_ibl_res_x);
    declareMonitoredStdContainer("IBLresy", m_dqm_ibl_res_y);

  }

  TrigTrackingxAODCnv::~TrigTrackingxAODCnv()
  {}

  //----------------------------------
  //          beginRun method:
  //----------------------------------------------------------------------------
  HLT::ErrorCode TrigTrackingxAODCnv::hltBeginRun() {
    msg() << MSG::INFO << "TrigTrackingxAODCnv::beginRun()" << endreq;
    m_mon_counter = 1;

    return HLT::OK;
  }
  //----------------------------------------------------------------------------

  ///////////////////////////////////////////////////////////////////
  // Initialisation
  ///////////////////////////////////////////////////////////////////
  HLT::ErrorCode TrigTrackingxAODCnv::hltInitialize() {

    msg() << MSG::DEBUG << name() << " initialize() " << PACKAGE_VERSION << endreq;

    if (detStore()->retrieve(m_idHelper, "AtlasID").isFailure()) {
      msg(MSG::FATAL) << "Could not get AtlasDetectorID helper" << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }
    
    if (detStore()->retrieve(m_pixelId, "PixelID").isFailure()) {
      msg(MSG::ERROR) << "Could not get PixelID helper !" << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }

    if ( m_particleCreatorTool.retrieve().isFailure() ) {
      msg() << MSG::FATAL << "Failed to retrieve tool " << m_particleCreatorTool << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }
    else{
      msg() << MSG::INFO << "Retrieved tool " << m_particleCreatorTool << endreq;
    }

    if ( m_residualCalc.retrieve().isFailure() ) {
      msg() << MSG::FATAL << "Failed to retrieve tool " << m_residualCalc << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }
    else{
      msg() << MSG::INFO << "Retrieved tool " << m_particleCreatorTool << endreq;
    }


    //+++ DQM (SA): monitoring
    std::string tmp_alg_name = this->name();
    std::vector<std::string> slice_names;
    slice_names.push_back("Bphysics");
    slice_names.push_back("Electron");
    slice_names.push_back("MinBias");
    slice_names.push_back("Photon");
    slice_names.push_back("Bjet");
    slice_names.push_back("FullScan");
    slice_names.push_back("Muon");
    slice_names.push_back("Tau");

    m_slice_name="";

    std::vector<std::string>::const_iterator iter;
    for (iter = slice_names.begin(); iter != slice_names.end(); ++iter) {
      if(tmp_alg_name.find( (*iter), 0) != std::string::npos ) m_slice_name = (*iter);
    }

    return HLT::OK;
  }

  ///////////////////////////////////////////////////////////////////
  // Execute HLT Algorithm
  ///////////////////////////////////////////////////////////////////
  HLT::ErrorCode TrigTrackingxAODCnv::hltExecute(const HLT::TriggerElement*, HLT::TriggerElement* outputTE) {

    int outputLevel = msgLvl();

    if(outputLevel <= MSG::DEBUG)
      msg() << MSG::DEBUG << "execHLTAlgorithm()" << endreq;


    //----------------------------------------------------------------------
    //  Navigate throw the trigger element to retrieve the last TrackCollection
    //----------------------------------------------------------------------

    //const TrackCollection* allTracksFromStoreGate;

    //initialize monitored objects
    m_tracks = 0;
    bool runAlg = true;
    HLT::ErrorCode statCode(HLT::OK);

    if (statCode!=HLT::OK){
      msg() << MSG::ERROR << "ErrorCode check to avoid unchecked SC" << endreq;
    }

    //+++ DQM (SA): monitoring
    ResetMon();

    //
    float tmp_eta_roi = -999;  // Uggly solution, should not need to fill a !!!
    float tmp_phi_roi = -999;  // vector with the same value for each track !!!
    const TrigRoiDescriptor* roi;
    if ( HLT::OK == getFeature(outputTE, roi) && roi ) {
      tmp_eta_roi = roi->eta();
      tmp_phi_roi = roi->phi();
    }

    m_tracks = 0;
    if ( HLT::OK != getFeature(outputTE, m_tracks) ) {
      msg() << MSG::ERROR << " Input track collection could not be found " << endreq;
      runAlg = false;
      statCode = HLT::NAV_ERROR;
    }

    if(!m_tracks){
      if(outputLevel <= MSG::DEBUG)
	msg() << MSG::DEBUG << " Input track collection was not attached. Algorithm not executed!" << endreq;
      runAlg = false;
      statCode = HLT::OK;
    } else {
      if(outputLevel <= MSG::VERBOSE)
	msg() << MSG::VERBOSE << " Input track collection has size " << m_tracks->size() << endreq;
      if ( m_tracks->size() == 0 ) {
	if(outputLevel <= MSG::DEBUG)
	  msg() << MSG::DEBUG << " Input track collection has 0 size. Algorithm not executed!" << endreq;
	runAlg = false;
	statCode=HLT::OK;
      }
    }


    //convert tracks

    xAOD::TrackParticleContainer* tpCont = new xAOD::TrackParticleContainer();
    //    xAOD::TrackParticleAuxContainer* tpAuxCont = new xAOD::TrackParticleAuxContainer();
    xAOD::TrackParticleAuxContainer tpAuxCont; // = new xAOD::TrackParticleAuxContainer();      //this guy should allow reset
    tpCont->setStore( &tpAuxCont );

    if(m_tracks && runAlg) {
      for(unsigned int idtr=0; idtr< m_tracks->size(); ++idtr) {
        const ElementLink<TrackCollection> trackLink(*m_tracks, idtr);

	fillIBLResidual(m_tracks->at(idtr));

        xAOD::TrackParticle* tp = m_particleCreatorTool->createParticle( trackLink, tpCont);

        if((outputLevel <= MSG::DEBUG) && (tp != 0)){
  	  int npix, nsct, ntrt, npixh, nscth;
  	  npix = nsct = ntrt = npixh = nscth = -1;
  	  const Trk::Track *tr = tp->track();
  	  if (tr){
  	    const Trk::TrackSummary *ts = tr->trackSummary();
  	    if (ts){
  	      npix = ts->get(Trk::numberOfPixelHits);
  	      nsct = ts->get(Trk::numberOfSCTHits);
  	      ntrt = ts->get(Trk::numberOfTRTHits);
  	      nscth= ts->get(Trk::numberOfSCTHoles);
  	      npixh= ts->get(Trk::numberOfPixelHoles);
  	    }
  	  }
  
    	  msg() << MSG::DEBUG << "REGTEST: " << std::setw(5) << idtr
    	      << "  pT:  " << std::setw(10) << tp->pt()
    	      << "  eta: " << tp->eta()
    	      << "  phi: " << tp->phi()
    	      << "  d0:  " << tp->d0()
    	      << "  z0:  " << tp->z0()
    	      << "\t" << npix << "/" << nsct << "/" << ntrt << "//" << npixh << "/" << nscth
    	      << endreq;

        }
      }
    }

    if(outputLevel <= MSG::DEBUG){
      msg() << MSG::DEBUG << "REGTEST container size = " << tpCont->size() << endreq;
    }


    if ( HLT::OK !=  attachFeature(outputTE, tpCont, name()) ) {
      msg() << MSG::ERROR << "Could not attach feature to the TE" << endreq;
      return HLT::NAV_ERROR;
    }
    else {
      if(outputLevel <= MSG::DEBUG){
	msg() << MSG::DEBUG << "Stored xAOD::TrackParticle container " << endreq;
      }
    }


    for (xAOD::TrackParticleContainer::iterator itr = tpCont->begin();
     itr != tpCont->end(); ++itr)  {
      FillMonPerTrack(*itr, tmp_eta_roi, tmp_phi_roi);
     }

    if (runAlg){
      //+++ DQM (SA): per RoI quantities
      FillMonPerRoi(roi, tmp_eta_roi, tmp_phi_roi);
      ++m_mon_counter;

      return HLT::OK;
    } else {
      return statCode;
    }



  }

  ///////////////////////////////////////////////////////////////////
  // Finalize
  ///////////////////////////////////////////////////////////////////

  HLT::ErrorCode TrigTrackingxAODCnv::hltFinalize() {

    msg() << MSG::DEBUG << "finalize() success" << endreq;
    return HLT::OK;
  }

  //----------------------------------
  //          endRun method:
  //----------------------------------------------------------------------------
  HLT::ErrorCode TrigTrackingxAODCnv::hltEndRun() {

    msg() << MSG::INFO << "TrigTrackingxAODCnv::endRun()" << endreq;

    return HLT::OK;
  }
  //---------------------------------------------------------------------------


  //---------------------------------------------------------------------------
  //  update the monitoring arrays
  //---------------------------------------------------------------------------
  void TrigTrackingxAODCnv::FillMonPerTrack(const xAOD::TrackParticle *particle,
					    const double &tmp_eta_roi, const double &tmp_phi_roi) {

    //+++ Prescale
    if (m_mon_counter != m_mon_prescale) return;

    //+++ Common for all slices
    float tmp_eta = -999;
    float tmp_phi = -999;

    tmp_eta = particle->eta();
    tmp_phi = particle->phi();
    m_dqm_d0.push_back(particle->d0());
    m_dqm_z0.push_back(particle->z0());
    m_dqm_eta.push_back(tmp_eta);
    m_dqm_phi.push_back(tmp_phi);
    m_dqm_qOverP.push_back(particle->qOverP());

    uint8_t numberOfPixelHits = 0;
    particle->summaryValue(numberOfPixelHits, xAOD::numberOfPixelHits);
    m_dqm_npix_hits.push_back(static_cast<int>(numberOfPixelHits));
    uint8_t numberOfSCTHits = 0;
    particle->summaryValue(numberOfSCTHits, xAOD::numberOfSCTHits);
    m_dqm_nsct_hits.push_back(static_cast<int>(numberOfSCTHits));
    uint8_t numberOfTRTHits = 0;
    particle->summaryValue(numberOfTRTHits, xAOD::numberOfTRTHits);
    m_dqm_ntrt_hits.push_back(static_cast<int>(numberOfTRTHits));

    if(particle->numberDoF()>0) {
      m_dqm_chi2dof.push_back(particle->chiSquared() / particle->numberDoF());
    }

    m_dqm_eta_roi.push_back(tmp_eta_roi);
    m_dqm_phi_roi.push_back(tmp_phi_roi);

    //+++ Slice specific
    if (m_mon_doSliceSpecific) {
      if (m_slice_name == "Tau") {
      	float tmp_deta = tmp_eta - tmp_eta_roi;
      	float tmp_dphi = tmp_phi - tmp_phi_roi;
      	float tmp_roidr = sqrt(tmp_deta*tmp_deta + tmp_dphi*tmp_dphi);
      	m_dqm_ta_roidr.push_back(tmp_roidr);
      }
    }

    return;
  }

  void TrigTrackingxAODCnv::fillIBLResidual(const Trk::Track *track){
    
    const DataVector<const Trk::TrackStateOnSurface>* trackStates=track->trackStateOnSurfaces();    
    
    for (DataVector<const Trk::TrackStateOnSurface>::const_iterator it=trackStates->begin();
	 it!=trackStates->end();
	 it++) {
      if (!(*it)) {
	msg(MSG::WARNING) << "TrackStateOnSurface == Null" << endreq;
	continue;
      }
      
      if ((*it)->type(Trk::TrackStateOnSurface::Measurement) ){
	
	ATH_MSG_VERBOSE ("try to get measurement for track state");
	// Get pointer to measurement on track
	const Trk::MeasurementBase *measurement = (*it)->measurementOnTrack();
	
	
	if(  (*it)->trackParameters() !=0 &&
	     &((*it)->trackParameters()->associatedSurface()) !=0 &&  
	     (*it)->trackParameters()->associatedSurface().associatedDetectorElement() !=0 && 
	     (*it)->trackParameters()->associatedSurface().associatedDetectorElement()->identify() !=0 )
	  {  
	    
	    Identifier id;
	    id = (*it)->trackParameters()->associatedSurface().associatedDetectorElement()->identify();
	    float zmod = (*it)->trackParameters()->associatedSurface().associatedDetectorElement()->center().z();
	    if (m_idHelper->is_pixel(id)) 
	      {
		ATH_MSG_DEBUG("Found pixel module : Associated track parameter");
		if(m_pixelId->is_barrel(id)) 
		  { 
		    ATH_MSG_DEBUG("Found pixel barrel");
		    if(m_pixelId->layer_disk(id) == 0) 
		      {
			if (msgLvl(MSG::DEBUG)) 
			  {
			    msg(MSG::DEBUG) << "Found Innermost Pixel Layer  " << id.get_compact() << endreq;       
			  }
			
			const Trk::ResidualPull* pull = m_residualCalc->residualPull(measurement,(*it)->trackParameters(),Trk::ResidualPull::Unbiased);
			m_dqm_ibl_z.push_back(zmod);
			m_dqm_ibl_res_x.push_back(pull->residual()[Trk::locX]);
			m_dqm_ibl_res_y.push_back(pull->residual()[Trk::locY]);
		      }
		  }
	      }
	  }
      }
    }
   
  }


  void TrigTrackingxAODCnv::FillMonPerRoi(const TrigRoiDescriptor* roi, const double &tmp_eta_roi, const double &tmp_phi_roi) {

    //+++ Prescale
    if (m_mon_counter >= m_mon_prescale) {
      m_mon_counter = 0;
    } else {
      return;
    }


    //+++ Common for all slices
    if (m_tracks) m_dqm_ntrk = m_tracks->size();

    if (roi)
    {
      m_roiEta = roi->eta();
      m_roiEtaWidth = roi->etaPlus() - roi->etaMinus();
      m_roiPhi = roi->phi();
      m_roiPhiWidth = HLT::wrapPhi(roi->phiPlus() - roi->phiMinus());
      m_roiZ = roi->zed();
      m_roiZ_Width = roi->zedPlus() - roi->zedMinus();    
    }

    //+++ Slice specific
    if (m_mon_doSliceSpecific) {
      //+++ Bjet
      if (m_slice_name == "Bjet") {
        const Trk::Perigee* tmpMp = 0;
	if (m_tracks) {
          for (auto track : *m_tracks) {
	    tmpMp = track->perigeeParameters();
	    if (tmpMp) {
	      m_dqm_bj_sumpt += tmpMp->pT()/1000;
	    }
	  }
	}
      //+++ Muon
      } else if (m_slice_name == "Muon") {

	m_dqm_mu_eta_roi = tmp_eta_roi;
	m_dqm_mu_phi_roi = tmp_phi_roi;

	//+++ Find leading track with pT > X
	const Trk::Track* tpL = 0;
	const Trk::Perigee* tmpMp = 0;
	float tmp_pt_max = 0;
	if (m_tracks) {
          for (auto track : *m_tracks) {
	    tmpMp = track->perigeeParameters();
	    if (tmpMp) {
	      float tmp_pt = tmpMp->pT()/1000;
	      if ((tmp_pt > tmp_pt_max) && (tmp_pt > m_mon_ptmin)) {
		tpL = track;
		tmp_pt_max = tmp_pt;
	      }
	    }
	  }
	}

	//+++ Fill leading histograms
	const Trk::TrackSummary * tmpSum = 0;
	const Trk::FitQuality *   tmpQty = 0;
	if (tpL) {
	  tmpMp = tpL->perigeeParameters();
	  tmpSum = tpL->trackSummary();
	  tmpQty = tpL->fitQuality();
	}
	if (tmpMp){
	  m_dqm_mu_d0 = tmpMp->parameters()[Trk::d0];
	  m_dqm_mu_z0 = tmpMp->parameters()[Trk::z0];
	  m_dqm_mu_phi = tmpMp->parameters()[Trk::phi];
	  m_dqm_mu_eta = tmpMp->eta();
	  m_dqm_mu_qOverP = tmpMp->parameters()[Trk::qOverP];
	  m_dqm_mu_pt = tmpMp->pT();
	}
	if (tmpSum){
	  m_dqm_mu_npix_hits = tmpSum->get(Trk::numberOfPixelHits);
	  m_dqm_mu_nsct_hits = tmpSum->get(Trk::numberOfSCTHits);
	  m_dqm_mu_ntrt_hits = tmpSum->get(Trk::numberOfTRTHits);
	}
	if (tmpQty){
	  if (tmpQty->numberDoF()>0) m_dqm_mu_chi2dof = tmpQty->chiSquared() / tmpQty->numberDoF();
	}
      //+++ Tau
      } else if (m_slice_name == "Tau") {

	//+++ Find L and NL pT track and sum charge
	const Trk::Track* tpL = 0;
	const Trk::Track* tpNL = 0;
	const Trk::Perigee* tmpLMp = 0;
	const Trk::Perigee* tmpNLMp = 0;
	float tmp_pt_max = 0;
	int tmp_pos_ch = 0;
	int tmp_neg_ch = 0;
	float tmp_px = 0;
	float tmp_py = 0;
	float tmp_pz = 0;
	float tmp_e  = 0;
	if (m_tracks) {
          for (auto track : *m_tracks) {
	    tmpLMp = track->perigeeParameters();
	    if (tmpLMp) {
	      float tmp_qOverP = tmpLMp->parameters()[Trk::qOverP];
	      float tmp_p  = 0;
	      if (tmp_qOverP != 0) tmp_p  = fabs(1/tmp_qOverP)/1000;
	      float tmp_pt = tmp_p*sin(tmpLMp->parameters()[Trk::theta]);
	      tmp_px += tmp_pt*cos(tmpLMp->parameters()[Trk::phi]);
	      tmp_py += tmp_pt*sin(tmpLMp->parameters()[Trk::phi]);
	      tmp_pz += tmp_p*cos(tmpLMp->parameters()[Trk::theta]);
	      tmp_e  += sqrt(tmp_p*tmp_p + 0.140*0.140);  // All tracks get the pi mass
	      if ((tmp_pt > tmp_pt_max) && (tmp_pt > m_mon_ptmin)) {
		tpNL = tpL;
		tpL  = track;
		tmp_pt_max = tmp_pt;
	      }
	      if (tmp_qOverP < 0) {
		++tmp_neg_ch;
	      } else {
		++tmp_pos_ch;
	      }
	    }
	  }
	}

	//+++ Fill m and pT of L and NL track
	float tmp_p2 = tmp_px*tmp_px + tmp_py*tmp_py + tmp_pz*tmp_pz;
	float tmp_m2 = tmp_e*tmp_e - tmp_p2;
	if (tmp_m2 >= 0) m_dqm_ta_m = sqrt( tmp_m2 );
	if (tpL)  tmpLMp = tpL->perigeeParameters();
	if (tpNL)  tmpNLMp = tpNL->perigeeParameters();
	if (tmpLMp){
	  m_dqm_ta_L_pt = tmpLMp->pT()/1000;
	  if (tmpNLMp) {
	    float tmp_NL_pt = tmpNLMp->pT()/1000;
	    if (m_dqm_ta_L_pt!=0) m_dqm_ta_frac_pt = tmp_NL_pt / m_dqm_ta_L_pt;
	  }
	}

	//+++ Charge Asymmetry
        float tmp_sumpart = tmp_pos_ch + tmp_neg_ch;
        if (tmp_sumpart != 0) {
          m_dqm_ta_sum_ch = tmp_pos_ch - tmp_neg_ch;
          m_dqm_ta_asy_ch = m_dqm_ta_sum_ch / tmp_sumpart;
        } else {
          m_dqm_ta_asy_ch = -999;
        }


      }
    }

    return;
  }

  void TrigTrackingxAODCnv::ResetMon() {

    // Common
    m_dqm_d0.clear();
    m_dqm_z0.clear();
    m_dqm_phi.clear();
    m_dqm_eta.clear();
    m_dqm_qOverP.clear();
    m_dqm_npix_hits.clear();
    m_dqm_nsct_hits.clear();
    m_dqm_ntrt_hits.clear();
    m_dqm_chi2dof.clear();
    m_dqm_eta_roi.clear();
    m_dqm_phi_roi.clear();
    m_dqm_ntrkpart = 0;
    m_dqm_ntrk = 0;
    m_dqm_nvertex = 0;

    m_dqm_ibl_z.clear();
    m_dqm_ibl_res_x.clear();
    m_dqm_ibl_res_y.clear();

    // Bjet
    m_dqm_bj_sumpt = 0;

    // Muon
    m_dqm_mu_d0 = -999;
    m_dqm_mu_z0 = -999;
    m_dqm_mu_eta = -999;
    m_dqm_mu_phi = -999;
    m_dqm_mu_qOverP = -999;
    m_dqm_mu_pt = -999999;
    m_dqm_mu_npix_hits = 0;
    m_dqm_mu_nsct_hits = 0;
    m_dqm_mu_ntrt_hits = 0;
    m_dqm_mu_chi2dof = -999;
    m_dqm_mu_eta_roi = -999;
    m_dqm_mu_phi_roi = -999;
    // Tau
    m_dqm_ta_roidr.clear();
    m_dqm_ta_L_pt = 0;
    m_dqm_ta_frac_pt = 0;
    m_dqm_ta_sum_ch = -999;
    m_dqm_ta_asy_ch = -999;
    m_dqm_ta_m = -999;

    return;
  }


} // end namespace



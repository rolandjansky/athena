/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ITHistSvc.h"
#include "InDetTrigParticleCreation/TrigParticleCreator.h"
#include "TrkToolInterfaces/ITrackParticleCreatorTool.h"
#include "VxVertex/VxContainer.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "Particle/TrackParticle.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/LinkToTrack.h"
#include "TrkParameters/TrackParameters.h"
#include "Particle/TrackParticleContainer.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrkTrackSummary/TrackSummary.h"

#include <cmath>

namespace InDet
{
  //a helper function to obtain TrackParticleContainer for a monitored object
  //as declareMonitoredObject("numTrackParticles",
  //  (const Rec::TrackParticleContainer*&)m_trackPC, &Rec::TrackParticleContainer::size)
  //  does not compile - 

  int getTrackPContainersize(const Rec::TrackParticleContainer* c)  {
    if (c){
      return c->size();
    }
    else
      return 0;
  }

  TrigParticleCreator::TrigParticleCreator(const std::string &name, 
					   ISvcLocator *pSvcLocator)
    : HLT::FexAlgo (name, pSvcLocator),
      m_particleCreatorTool("Trk::ParticleCreatorTool"),
      m_assoTool("Trk::PRD_AssociationTool"),
      m_trackPC(0),
      m_allTracksFromStoreGate(0),
      m_primcontainer(0),
      m_doSharedHits(false),
      m_slice_name(""),
      m_mon_doSliceSpecific(true),
      m_mon_counter(1),
      m_mon_prescale(1),
      m_mon_ptmin(1)
  {
    declareProperty("ParticleCreatorTool", m_particleCreatorTool);
    declareProperty("PRDAssociationTool",  m_assoTool);
    declareProperty("DoSharedHits",        m_doSharedHits);


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

  }

  TrigParticleCreator::~TrigParticleCreator()
  {}

  ///////////////////////////////////////////////////////////////////
  // Initialisation
  ///////////////////////////////////////////////////////////////////
  HLT::ErrorCode TrigParticleCreator::hltInitialize() {

    msg() << MSG::DEBUG << "initialize() success" << endmsg;

    if ( m_particleCreatorTool.retrieve().isFailure() ) {
      msg() << MSG::FATAL << "Failed to retrieve tool " << m_particleCreatorTool << endmsg;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }
    else{
      msg() << MSG::INFO << "Retrieved tool " << m_particleCreatorTool << endmsg;
    }

    if (m_doSharedHits) {
      if ( m_assoTool.retrieve().isFailure() ) {
	msg() << MSG::FATAL << "Failed to retrieve tool " << m_assoTool << endmsg;
	return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
      } else {
	msg() << MSG::INFO << "Retrieved tool " << m_assoTool << endmsg;
      }
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
  HLT::ErrorCode TrigParticleCreator::hltExecute(const HLT::TriggerElement*, HLT::TriggerElement* outputTE) {
 
    int outputLevel = msgLvl();

    if(outputLevel <= MSG::DEBUG)
      msg() << MSG::DEBUG << "execHLTAlgorithm()" << endmsg;


    //----------------------------------------------------------------------
    //  Navigate throw the trigger element to retrieve the last TrackCollection
    //----------------------------------------------------------------------

    //const TrackCollection* allTracksFromStoreGate;

    //initialize monitored objects
    m_allTracksFromStoreGate = 0;
    m_trackPC = 0; 
    m_primcontainer = 0;
    bool runAlg = true;
    HLT::ErrorCode statCode(HLT::OK);

    if (statCode!=HLT::OK){
      msg() << MSG::ERROR << "ErrorCode check to avoid unchecked SC" << endmsg;
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

    m_allTracksFromStoreGate = 0;
    if ( HLT::OK != getFeature(outputTE, m_allTracksFromStoreGate) ) {
      msg() << MSG::ERROR << " Input track collection could not be found " << endmsg;
      runAlg = false;
      statCode = HLT::NAV_ERROR;
    }

    if(!m_allTracksFromStoreGate){
      if(outputLevel <= MSG::DEBUG)
	msg() << MSG::DEBUG << " Input track collection was not attached. Algorithm not executed!" << endmsg;
      runAlg = false;
      statCode = HLT::OK; 
    } else {
      if(outputLevel <= MSG::VERBOSE)
	msg() << MSG::VERBOSE << " Input track collection has size " << m_allTracksFromStoreGate->size() << endmsg;
      if ( m_allTracksFromStoreGate->size() == 0 ) {
	if(outputLevel <= MSG::DEBUG)
	  msg() << MSG::DEBUG << " Input track collection has 0 size. Algorithm not executed!" << endmsg;
	runAlg = false;
	statCode=HLT::OK; 
      }
    }


    //  now prepare for particle creation
    m_trackPC = new Rec::TrackParticleContainer;
    Rec::TrackParticle* aParticle(0);

    if (runAlg){
      /**
	 this is new for b-tagging
      */
      if (m_doSharedHits) {
	// clear prdAssociationTool (this may be altered)
	m_assoTool->reset();
	// loop over tracks and to 
	TrackCollection::const_iterator trackIt    = m_allTracksFromStoreGate->begin();
	TrackCollection::const_iterator trackItEnd = m_allTracksFromStoreGate->end();
	for ( ; trackIt != trackItEnd ; ++trackIt) {
	  if ( (m_assoTool->addPRDs(**trackIt)).isFailure() ) {
	    msg() << MSG::ERROR << "could not add PRDs to association tool" << endmsg;
	  }
	}
      }
   
   
      /**
	 Retrieve Primary Vertex from the TriggerElement.
	 If no primary vertex is there or it is not valid, all TrackParticles will
	 be constructed with m_reconstructedVertex(0).
      */

      m_primcontainer = 0;
      bool primVtxExists = true;
   
      if ( (HLT::OK != getFeature(outputTE, m_primcontainer )) || m_primcontainer == 0 ) {
	msg() << MSG::DEBUG << " Input vertex collection could not be found " << endmsg;
	primVtxExists = false;
      } else {
	if (outputLevel <= MSG::DEBUG)
	  msg() << MSG::DEBUG << "REGTEST: Retrieved input primary vertex container with "
		<< m_primcontainer->size() << " vertices " << endmsg;
     
     
	VxContainer::const_iterator vtxItr=m_primcontainer->begin();
	if (m_primcontainer->size()==0){
	  msg() << MSG::DEBUG
		<< "REGTEST: VxPrimary container empty - it should have at least a dummy vertex " << endmsg; 
	  primVtxExists = false;
	}
	else if ((*vtxItr)->recVertex().position()[0] == 0.) {
	  primVtxExists = false;
	  if(outputLevel <= MSG::DEBUG){
	    msg() << MSG::DEBUG << "REGTEST: There is a VxPrimary container " 
		  << " in StoreGate, but the fitted vertex is exactly " << endmsg;
	    msg() << MSG::DEBUG << "(0,0,0) which means no primary vertex was found."
		  << endmsg;
	  }
	}
	if(outputLevel <= MSG::DEBUG)
	  msg() << MSG::DEBUG << "REGTEST: Found primary vertex!!! " << endmsg;
      }
   
      std::map<const Trk::Track*, bool> trackWasUsed;
   
      if (!primVtxExists) {
	for (TrackCollection::const_iterator alltr = m_allTracksFromStoreGate->begin() ;
	     alltr != m_allTracksFromStoreGate->end()   ; ++alltr) {
       
	  aParticle = m_particleCreatorTool->createParticle(*alltr);
	  if (aParticle!=0 && !trackWasUsed[*alltr]){ 
	    m_trackPC->push_back(aParticle);
	    trackWasUsed[*alltr] = true;

	  } else {
	    delete aParticle;
	    msg() << MSG::DEBUG << "An attempt to add a duplicate TP in !primVtxExists" << endmsg;
	  }
	}
      }
      else if (primVtxExists) {
	// here we have to loop over the tracks assigned to vertices first and 
	// then look at the tracks not used in any fit and assign them to the
	// closest vertex in z dir.
     
	// primary vertex first
	VxContainer::const_iterator vtxItr=m_primcontainer->begin();
	for (unsigned int i = 0 ; i < (*vtxItr)->vxTrackAtVertex()->size() ; ++i) {
       
	  // const Trk::Track* tmpTrack((*((*vtxItr)->vxTrackAtVertex()))[i]->origTrack());
	  Trk::ITrackLink* tlink((*((*vtxItr)->vxTrackAtVertex()))[i]->trackOrParticleLink());
	  Trk::LinkToTrack *l2track = dynamic_cast<Trk::LinkToTrack*>(tlink);
	  const Trk::Track* ttrack;
	  if (l2track){
	    ttrack = l2track->cachedElement();
	  } else {
	    ttrack = 0;
	    msg() << MSG::WARNING << "Navigation from vertex to track failed " << endmsg;
	  }

	  aParticle = 0;
	  if (ttrack)
	    aParticle = m_particleCreatorTool->createParticle(ttrack, *vtxItr, Trk::PriVtx);
	  if (aParticle!=0 && !trackWasUsed[ttrack] ){
	    m_trackPC->push_back(aParticle);
	    trackWasUsed[ttrack] = true;

	  } else {
	    delete aParticle;
	    msg() << MSG::DEBUG << "An attempt to add a duplicate TP in primVtxExists" << endmsg;
	  }

	}

	// then loop over pile up vertices
	if (m_primcontainer->size()>1)  {
	  for (vtxItr=m_primcontainer->begin()+1; vtxItr!=m_primcontainer->end(); 
	       vtxItr++) {
	    for (unsigned int i = 0 ; i < (*vtxItr)->vxTrackAtVertex()->size() ; 
		 ++i) {

	      Trk::ITrackLink *tlink((*((*vtxItr)->vxTrackAtVertex()))[i]->trackOrParticleLink());
	      Trk::LinkToTrack *l2track =  dynamic_cast<Trk::LinkToTrack*>(tlink);
	      const Trk::Track* ttrack;
	      if (l2track)
		ttrack = l2track->cachedElement();
	      else {
		ttrack = 0;
		msg() << MSG::WARNING << "Navigation from vertex to track failed " << endmsg;
	      }

	      aParticle = 0;
	      if (ttrack)
		aParticle = m_particleCreatorTool->createParticle(ttrack, *vtxItr, Trk::PileUp);

	      if (aParticle!=0 &&  !trackWasUsed[ttrack]){
		m_trackPC->push_back(aParticle);
		trackWasUsed[ttrack] = true;
	      } else {
		delete aParticle;
		msg() << MSG::DEBUG << "An attempt to add a duplicate TP in primVtxExists" << endmsg;
	      }

	    }
	  }

	  // create TP with the rest of the tracks which have not been used in any vertex fit
	  // TP created with those tracks will point to the closest vertex in z direction.
	  // AW: 9th Oct 2006: Dont know if that is a good idea ... up to recently all TrackParticles point to prim vertex
	  if (trackWasUsed.size() != m_allTracksFromStoreGate->size()) {
	    for (TrackCollection::const_iterator
		   alltr  = m_allTracksFromStoreGate->begin() ;
		 alltr != m_allTracksFromStoreGate->end()   ; ++alltr) {
	      if (!trackWasUsed[(*alltr)]) {
		// do z match to vertex here!!!!!!!!!!!!!
		double deltaZ(10000000.);
		const Trk::VxCandidate* closestVxCandidate(0);
		for (vtxItr=m_primcontainer->begin(); vtxItr!=m_primcontainer->end();
		     vtxItr++)  {
		  double vtxZ0((*vtxItr)->recVertex().position()[2]);
		  double trkZ0((*alltr)->perigeeParameters()->parameters()[Trk::z0]);
		  double diff(fabs(vtxZ0 - trkZ0));
		  if (diff < deltaZ) {
		    deltaZ = diff;
		    closestVxCandidate = (*vtxItr);
		  }
		}
		if (deltaZ<100000000. && closestVxCandidate!=0) {
		  aParticle = m_particleCreatorTool->createParticle((*alltr), closestVxCandidate, Trk::NoVtx);  

		  if (aParticle!=0){ 
		    m_trackPC->push_back(aParticle); 
		    trackWasUsed[*alltr] = true;
		  }
		}
	      }
	    }
	  }
	}
	/// no loop over pile up and no z track match is needed if there was only the primary vertex
	else {
	  if (trackWasUsed.size() != m_allTracksFromStoreGate->size()) {
	    for (TrackCollection::const_iterator
		   alltr  = m_allTracksFromStoreGate->begin() ;
		 alltr != m_allTracksFromStoreGate->end()   ; ++alltr) {
	      if (!trackWasUsed[(*alltr)]) {
		vtxItr=m_primcontainer->begin();

		aParticle = m_particleCreatorTool->createParticle((*alltr), *vtxItr, Trk::NoVtx);
		if (aParticle!=0) { 
		  m_trackPC->push_back(aParticle);
		  trackWasUsed[*alltr] = true;
		}
	      }
	    }
	  }    
	}
      } // end if prim vtx in StoreGate

      /**
	 These TrackParticles will be persistified!
	 Therefore we have to assign a collection to the ElementLink<TrackCollection> datamember of it
      */
      for (Rec::TrackParticleContainer::iterator itr = m_trackPC->begin();
	   itr != m_trackPC->end(); ++itr)  {
	// TrackParticle has a wrapped method setStorableObject which
	// calls the ElementLink::setStorableObject
	(*itr)->setStorableObject(m_allTracksFromStoreGate);

	//+++ DQM (SA): per track quantities
	FillMonPerTrack(*itr, tmp_eta_roi, tmp_phi_roi);

	if (primVtxExists) 
	  {
	    (*itr)->setStorableObject(m_primcontainer);
	  }
      }
    }
  
    //
    //  Attach resolved tracks to the trigger element.
    //  std::string sgkey;
    //if ( HLT::OK !=  recordAndAttachFeature(outputTE, m_trackPC, sgkey, "TrackPart") ) {
    if ( HLT::OK !=  attachFeature(outputTE, m_trackPC, name()) ) {
      msg() << MSG::ERROR << "Could not attach feature to the TE" << endmsg;
    
      return HLT::NAV_ERROR;
    }
    else {
      if(outputLevel <= MSG::DEBUG){
	msg() << MSG::DEBUG << "Stored TrackParticle container " << endmsg;
	msg() << MSG::DEBUG << "REGTEST: TrackParticle container contains "
	      << m_trackPC->size() << " particles." << endmsg;

	//do detailed output of TrackParticles for REGTEST comparison
	Rec::TrackParticleContainer::const_iterator itrp = m_trackPC->begin();
	Rec::TrackParticleContainer::const_iterator itrp_last = m_trackPC->end();

	size_t count(0);
	for (; itrp!= itrp_last; ++itrp){
	  const Trk::Perigee *mp = (*itrp)->measuredPerigee();
	  int npix, nsct, ntrt, npixh, nscth;
	  npix = nsct = ntrt = npixh = nscth = -1;
	  const Trk::Track *tr = (*itrp)->originalTrack();
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
	  count++;
	  if (mp){
	    msg() << MSG::DEBUG << "REGTEST: " << std::setw(5) << count
		  << "  pT:  " << std::setw(10) << mp->pT()
		  << "  eta: " << mp->eta()
		  << "  phi: " << mp->parameters()[Trk::phi]
		  << "\t" << npix << "/" << nsct << "/" << ntrt << "//" << npixh << "/" << nscth 
		  << endmsg;
	  }
	}
      }
    }

    if (runAlg){
      //+++ DQM (SA): per RoI quantities
      FillMonPerRoi(tmp_eta_roi, tmp_phi_roi);
      ++m_mon_counter; 

      return HLT::OK;
    } else {
      return statCode;
    }

    /*
    //for the following symLink trackPC has to be present in SG.
    //It fails in the online or when offline ReferenceAllClasses=False unless
    //recordAndAttachFeature is used instead of attachFeature
    //try to comment to see who needs it
    const INavigable4MomentumCollection* theNav4s = 0;
    StatusCode sc = store()->symLink(m_trackPC,theNav4s);
    if (sc.isFailure()){
    msg() << MSG::WARNING << "Could not symLink TrigTrackParticleContainer to INavigable4MomentumCollection" 
    << endmsg;
    }
    */




  }
  ///////////////////////////////////////////////////////////////////
  // Finalize
  ///////////////////////////////////////////////////////////////////
  
  HLT::ErrorCode TrigParticleCreator::hltFinalize() {

    msg() << MSG::DEBUG << "finalize() success" << endmsg;
    return HLT::OK;
  }

  //---------------------------------------------------------------------------
  //  update the monitoring arrays
  //---------------------------------------------------------------------------
  void TrigParticleCreator::FillMonPerTrack(const Rec::TrackParticle *particle, 
					    const double &tmp_eta_roi, const double &tmp_phi_roi) {	
  
    //+++ Prescale
    if (m_mon_counter != m_mon_prescale) return;

    //+++ Common for all slices
    float tmp_eta = -999;
    float tmp_phi = -999;
    const Trk::Perigee *tmpMp = particle->measuredPerigee();
    const Trk::TrackSummary *tmpSum = particle->trackSummary();
    const Trk::FitQuality *tmpQty = particle->fitQuality();
    if (tmpMp){
      tmp_eta = tmpMp->eta();
      tmp_phi = tmpMp->parameters()[Trk::phi];
      m_dqm_d0.push_back(tmpMp->parameters()[Trk::d0]);
      m_dqm_z0.push_back(tmpMp->parameters()[Trk::z0]);
      m_dqm_eta.push_back(tmp_eta);
      m_dqm_phi.push_back(tmp_phi);
      m_dqm_qOverP.push_back(tmpMp->parameters()[Trk::qOverP]);
    }
    if (tmpSum){
      m_dqm_npix_hits.push_back(tmpSum->get(Trk::numberOfPixelHits));
      m_dqm_nsct_hits.push_back(tmpSum->get(Trk::numberOfSCTHits));
      m_dqm_ntrt_hits.push_back(tmpSum->get(Trk::numberOfTRTHits));
    }
    if (tmpQty){
      if(tmpQty->numberDoF()>0) m_dqm_chi2dof.push_back(tmpQty->chiSquared() / tmpQty->numberDoF());
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


  void TrigParticleCreator::FillMonPerRoi(const double &tmp_eta_roi, const double &tmp_phi_roi) {	
    
    //+++ Prescale
    if (m_mon_counter >= m_mon_prescale) { 
      m_mon_counter = 1;
    } else {
      return;       
    }

 
    //+++ Common for all slices
    if (m_allTracksFromStoreGate) m_dqm_ntrk = m_allTracksFromStoreGate->size();
    if (m_primcontainer) m_dqm_nvertex = m_primcontainer->size();
    if (m_trackPC) m_dqm_ntrkpart = m_trackPC->size();


    //+++ Slice specific
    if (m_mon_doSliceSpecific) {  
      //+++ Bjet
      if (m_slice_name == "Bjet") {
	const Trk::Perigee * tmpMp = 0;
	if (m_trackPC) {
	  for (Rec::TrackParticleContainer::const_iterator tpItr = m_trackPC->begin(); tpItr != m_trackPC->end(); ++tpItr) {
	    tmpMp = (*tpItr)->measuredPerigee();
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
	const Rec::TrackParticle * tpL = 0;
	const Trk::Perigee * tmpMp = 0;
	float tmp_pt_max = 0;
	if (m_trackPC) {
	  for (Rec::TrackParticleContainer::const_iterator tpItr = m_trackPC->begin(); tpItr != m_trackPC->end(); ++tpItr) {
	    tmpMp = (*tpItr)->measuredPerigee();
	    if (tmpMp) {
	      float tmp_pt = tmpMp->pT()/1000;
	      if ((tmp_pt > tmp_pt_max) && (tmp_pt > m_mon_ptmin)) {
		tpL = (*tpItr);
		tmp_pt_max = tmp_pt;
	      }
	    } 
	  }
	}
	
	//+++ Fill leading histograms
	const Trk::TrackSummary * tmpSum = 0;
	const Trk::FitQuality *   tmpQty = 0;
	if (tpL) {
	  tmpMp = tpL->measuredPerigee();
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
	const Rec::TrackParticle * tpL = 0;
	const Rec::TrackParticle * tpNL = 0;
	const Trk::Perigee * tmpLMp = 0;
	const Trk::Perigee * tmpNLMp = 0;
	float tmp_pt_max = 0;
	int tmp_pos_ch = 0;
	int tmp_neg_ch = 0;
	float tmp_px = 0;
	float tmp_py = 0;
	float tmp_pz = 0;
	float tmp_e  = 0;
	if (m_trackPC) {
	  for (Rec::TrackParticleContainer::const_iterator tpItr = m_trackPC->begin(); tpItr != m_trackPC->end(); ++tpItr) {
	    tmpLMp = (*tpItr)->measuredPerigee();
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
		tpL  = (*tpItr);
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
	if (tpL)  tmpLMp = tpL->measuredPerigee();
	if (tpNL)  tmpNLMp = tpNL->measuredPerigee();
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

  void TrigParticleCreator::ResetMon() {

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



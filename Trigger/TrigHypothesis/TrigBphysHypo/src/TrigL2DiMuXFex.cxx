// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBPhysHypo/TrigL2DiMuXFex.cxx
 **
 **   Description: L2 hypothesis algorithms for B+ -> K+ mu+ mu-  
 **                Create TrigL2Bphys
 **
 **   Author: Cristina Adorisio (Cristina.Adorisio@cern.ch)
 **
 **   Created:   08.10.2007
 **   Modified:  25.02.2008(new TrigDiMuon class)
 **              07.03.2008(monitoring histograms added)
 **                
 **************************************************************************/ 
 
#include "TrigBphysHypo/TrigL2DiMuXFex.h"
#include "TrigBphysHypo/BtrigUtils.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

// additions of xAOD objects
#include "xAODEventInfo/EventInfo.h"

/*---------------------------------------------------------------------------------*/
TrigL2DiMuXFex::TrigL2DiMuXFex(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::FexAlgo(name, pSvcLocator),
  m_L2vertFitter("TrigL2VertexFitter",this),
  m_vertexingTool("TrigVertexingTool",this)
{
  // Read cuts
  declareProperty("AcceptAll", m_acceptAll = false );  //default=true

  declareProperty("LowerMuMuMassCut", m_lowerMuMuMassCut = 200.0);  //default=200.0  
  declareProperty("UpperMuMuMassCut", m_upperMuMuMassCut = 5000.0);  //default=5000.0
  declareProperty("LowerKplusMuMuMassCut", m_lowerKplusMuMuMassCut = 5000.0);  //default=5000.0
  declareProperty("UpperKplusMuMuMassCut", m_upperKplusMuMuMassCut = 5600.0);  //default=5600.0

  declareProperty("doVertexFit", m_doVertexFit = true ); //default=true

   // variables for monitoring histograms
  declareMonitoredStdContainer("MuMuMass",      mon_dimumass  , AutoClear);
  declareMonitoredStdContainer("KplusMuMuMass", mon_kdimumass , AutoClear);
  
  m_trigBphysColl = new TrigL2BphysContainer();
  
}

/*------------------------------*/
TrigL2DiMuXFex::~TrigL2DiMuXFex()
{ 
  delete m_trigBphysColl;
}

/*-------------------------------------------*/
HLT::ErrorCode TrigL2DiMuXFex::hltInitialize()
{
  msg() << MSG::INFO << "|----------------------- INFO FROM TrigL2DiMuXFex --------------------|" << endreq;
  msg() << MSG::INFO << "Initializing TrigL2DiMuXFex" << endreq;
  msg() << MSG::INFO << "AcceptAll            = " 
	<< (m_acceptAll==true ? "True" : "False") << endreq; 
  msg() << MSG::INFO << "LowerMuMuMassCut     = " << m_lowerMuMuMassCut << endreq;
  msg() << MSG::INFO << "UpperMuMuMassCut     = " << m_upperMuMuMassCut << endreq;
  msg() << MSG::INFO << "LowerKplusMuMuMassCut    = " << m_lowerKplusMuMuMassCut << endreq;
  msg() << MSG::INFO << "UpperKplusMuMuMassCut    = " << m_upperKplusMuMuMassCut << endreq;
  msg() << MSG::INFO << "doVertexFit              = " 
	<< (m_doVertexFit==true ? "True" : "False") << endreq; 
  msg() << MSG::INFO << "|---------------------------------------------------------------------|" << endreq;
 
  m_lastEvent=0;
  m_lastEvent_DiMu=0;
  m_lastEventPassed=0;
  m_lastEventPassedMuMu=0;
  m_lastEventPassedKMuMu=0;

  m_countTotalEvents=0;
  m_countTotalEvents_DiMu=0;
  m_countTotalRoI=0;
  m_countTotalRoI_DiMu=0;

  m_countPassedEvents=0;
  m_countPassedRoIs=0;
  m_countPassedEventsMuMu=0;
  m_countPassedEventsKMuMu=0;

  m_countPassedMuMuMassCut=0;
  m_countPassedKMuMuMassCut=0;
  m_countPassedKMuMuVertexCut=0;

  if ( m_doVertexFit ){
    StatusCode sc = m_L2vertFitter.retrieve();
    if ( sc.isFailure() ) {
      msg() << MSG::FATAL << "Unable to locate TrigL2VertexFitter tool" << endreq;
      return HLT::BAD_JOB_SETUP;
    } 
    else {
      msg() << MSG::INFO << "TrigL2VertexFitter retrieved"<< endreq;
    }
    
    sc = m_vertexingTool.retrieve();
    if ( sc.isFailure() ) {
      msg() << MSG::FATAL << "Unable to locate TrigVertexingTool tool" << endreq;
      return HLT::BAD_JOB_SETUP;
    } else {
      msg() << MSG::INFO << "TrigVertexingTool retrieved"<< endreq;
    }
  }

  // add timers
  if ( timerSvc() ) {
    m_TotTimer    = addTimer("L2DiMuXFex_Tot");
    m_VtxFitTimer = addTimer("L2DiMuXFex_KplusMuMuVFit");
  }  
  return HLT::OK;
}

/*---------------------------------------------*/
HLT::ErrorCode TrigL2DiMuXFex::hltFinalize()
{
  msg() << MSG::INFO << "Finalizing TrigL2DiMuXFex" << endreq;
  
  msg() << MSG::INFO << "|----------------------- SUMMARY FROM TrigL2DiMuXFex -----------------|" << endreq;
  msg() << MSG::INFO << "Run on events / RoIs " << m_countTotalEvents  << " / " << m_countTotalRoI <<  endreq;
  msg() << MSG::INFO << "Run on events / RoIs " << m_countTotalEvents_DiMu  << " / " << m_countTotalRoI_DiMu << " from DiMuon algo" <<  endreq;
  if ( !m_doVertexFit ) {
    msg() << MSG::INFO << "Passed events / RoIs " << m_countPassedEvents << " / " << m_countPassedRoIs <<  endreq;
  } else {
    msg() << MSG::INFO << "Passed events / RoIs " << m_countPassedEventsKMuMu << " / " << m_countPassedKMuMuVertexCut <<  endreq;
  }
  msg() << MSG::INFO << "Passed MuMu Invariant Mass Cut ( events / RoIs ) : "      << m_countPassedEventsMuMu  << " / " << m_countPassedMuMuMassCut << endreq;
  msg() << MSG::INFO << "Passed KplusMuMu Invariant Mass Cut ( events / RoIs ) : " << m_countPassedEvents      << " / " << m_countPassedKMuMuMassCut << endreq;
  if ( m_doVertexFit )
    msg() << MSG::INFO << "Passed KplusMuMu vertexing ( events / RoIs ) : "          << m_countPassedEventsKMuMu << " / " << m_countPassedKMuMuVertexCut << endreq;
  msg() << MSG::INFO << "|---------------------------------------------------------------------|" << endreq;

  return HLT::OK;
}

//----------------------------------------------------------------------------------------------------------
HLT::ErrorCode TrigL2DiMuXFex::hltExecute(const HLT::TriggerElement* /*inputTE*/, HLT::TriggerElement* outputTE)
{
  if ( timerSvc() ) 
    m_TotTimer->start();


  if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Executing TrigL2DiMuXFex" << endreq;

  bool result      = false;
  bool resultMuMu  = false;
  bool resultKMuMu = false;

  bool PassedMuMuPair    = false;
  bool PassedKMuMuMass   = false;
  bool PassedKMuMuVertex = false;

  std::vector<const TrigInDetTrackFitPar*> particleOK;
  std::vector<const TrigInDetTrack*> v_particleOK;

    // Retrieve event info
    int IdRun   = 0;
    int IdEvent = 0;
    
    // JW - Try to get the xAOD event info
    const EventInfo* pEventInfo(0);
    const xAOD::EventInfo *evtInfo(0);
    if ( store()->retrieve(evtInfo).isFailure() ) {
        if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Failed to get xAOD::EventInfo " << endreq;
        // now try the old event ifo
        if ( store()->retrieve(pEventInfo).isFailure() ) {
            if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Failed to get EventInfo " << endreq;
            if ( timerSvc() )
                m_TotTimer->stop();
            return HLT::NAV_ERROR;
        } else {
            IdRun   = pEventInfo->event_ID()->run_number();
            IdEvent = pEventInfo->event_ID()->event_number();
            if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Run " << IdRun << " Event " << IdEvent << endreq;
        }// found old event info
    }else { // found the xAOD event info
        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Run " << evtInfo->runNumber()
            << " Event " << evtInfo->eventNumber() << endreq;
        IdRun   = evtInfo->runNumber();
        IdEvent = evtInfo->eventNumber();
    } // get event ifo

    
    // Accept-All mode: temporary patch; should be done with force-accept
  if ( m_acceptAll ) {
    if ( msgLvl() <= MSG::DEBUG ){
      msg() << MSG::DEBUG << "AcceptAll property is set: taking all events" << endreq;
    }
  } else {
    if ( msgLvl() <= MSG::DEBUG ){
      msg() << MSG::DEBUG << "AcceptAll property not set: applying selection" << endreq;
    }
  }

  // get RoI descriptor
  const TrigRoiDescriptor* roiDescriptor = 0;
  if ( getFeature(outputTE, roiDescriptor) != HLT::OK )
    roiDescriptor = 0;
  if ( roiDescriptor ) {
    if ( msgLvl() <= MSG::DEBUG ){
      msg() << MSG::DEBUG 
	    << "Using outputTE ( " << outputTE << " ) ->getId(): " << outputTE->getId() << endreq;
      msg() << MSG::DEBUG << "RoI id " << roiDescriptor->roiId()
	    << " LVL1 id " << roiDescriptor->l1Id()
	    << " located at   phi = " <<  roiDescriptor->phi()
	    << ", eta = " << roiDescriptor->eta() << endreq;
    }
  } else {
    if ( msgLvl() <= MSG::WARNING ) {
      msg() << MSG::WARNING << "No RoI for this Trigger Element!" << endreq;
    }
    if ( timerSvc() ) 
      m_TotTimer->stop();    
    return HLT::NAV_ERROR;
  }

  if ( IdEvent != m_lastEvent ) {
    m_countTotalEvents++;
    m_lastEvent=IdEvent;
  }
  m_countTotalRoI++;

  //  create vector for TrigL2Bphys particles and vertexes
  if ( !m_trigBphysColl )
    m_trigBphysColl = new TrigL2BphysContainer();
  else 
    m_trigBphysColl->clear();

  // get vector for TrigL2Bphys particles
  const TrigL2BphysContainer* trigBphysColl;
  HLT::ErrorCode status = getFeature ( outputTE, trigBphysColl );
  if ( status != HLT::OK ) {
    if ( msgLvl() <= MSG::WARNING ) {
      msg() << MSG::WARNING << "Failed to get TrigBphysics collection" << endreq;
    }
    return HLT::OK;
  } else {
    if ( trigBphysColl == NULL ) {
      if ( msgLvl() <= MSG::DEBUG ) {
	msg() << MSG::DEBUG << "No Bphys particles found" << endreq;
      }
      return HLT::OK;
    }
    if ( msgLvl() <= MSG::DEBUG ) {
      msg() << MSG::DEBUG << "Got TrigBphys collection with " << trigBphysColl->size() << " Bphys particle candidates" << endreq;
    }
    // to count events from TrigDiMuon
    if ( IdEvent != m_lastEvent_DiMu ) {
      m_countTotalEvents_DiMu++;
      m_lastEvent_DiMu=IdEvent;
    }
    m_countTotalRoI_DiMu++;
  }

  TrigL2BphysContainer::const_iterator endPair = trigBphysColl->end();
  for ( TrigL2BphysContainer::const_iterator thePair = trigBphysColl->begin(); thePair != endPair; ++thePair ){

    //if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "particleType = " << (*thePair)->particleType() << endreq;
    if ( (*thePair)->particleType() == TrigL2Bphys::JPSIMUMU) {

      float mass = (*thePair)->mass();
      if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "MuMu Invariant Mass = " << mass << endreq;

      // apply mass cut
      if ( mass < m_lowerMuMuMassCut || mass > m_upperMuMuMassCut) {
	if ( msgLvl()<= MSG::DEBUG ){
	  msg() << MSG::DEBUG << "Combination discarded by mass cut: "
		<< mass << " MeV" << endreq;
	}
	particleOK.clear(); //to have only the muon pair in the vector
	v_particleOK.clear(); 
	continue;
      } else {
	mon_dimumass.push_back((mass*0.001));
	PassedMuMuPair=true;
	resultMuMu = true;
	if( msgLvl() <= MSG::DEBUG ) {
	  msg() << MSG::DEBUG << "MuMu combination retained! Invariant mass value: "
		<< mass << " MeV" << endreq;
	}
	particleOK.clear(); //to have only the muon pair in the vector
	v_particleOK.clear(); 
      }

      // JK 22-05-08 changes for new EDM
      //      const TrigVertex* p_vtx = (*thePair)->pVertex();
      //      if ( p_vtx != NULL ) {
      // if vertex exists check tracks
      //	TrackInVertexList* p_tracks=p_vtx->tracks();
      const ElementLinkVector<TrigInDetTrackCollection> trackVector = (*thePair)->trackVector();
      if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "Number of tracks in vertex:  " << trackVector.size() << endreq;
      
      //std::list<const TrigInDetTrack*>::iterator vtrkIt=p_tracks->begin();	
      //std::list<const TrigInDetTrack*>::iterator vtrkItEnd=p_tracks->end();	
      ElementLinkVector<TrigInDetTrackCollection>::const_iterator vtrkIt=trackVector.begin();
      ElementLinkVector<TrigInDetTrackCollection>::const_iterator vtrkItEnd=trackVector.end();
      for ( int itrk=0; vtrkIt != vtrkItEnd; ++vtrkIt, ++itrk ) {
	const TrigInDetTrackFitPar* p_param=(*(*vtrkIt))->param();
	if ( p_param == NULL ) {
	  if( msgLvl() <= MSG::WARNING ) {
	    msg() << MSG::WARNING << "No track parameters for track  " << itrk << endreq;
	  }
	  continue;
	}
	if ( (*(*vtrkIt))->chi2() == 100000000.000 ) {
	  if( msgLvl() <= MSG::WARNING ) msg() << MSG::WARNING << "Chi2 of muon track " << itrk << " is 1e8 !" << endreq;
	  continue;
	}
	if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "Parameters for track " << itrk << " : pT = " 
	      << p_param->pT() << " phi = " << p_param->phi0() << " eta = " 
	      << p_param->eta() << endreq;
	particleOK.push_back(p_param);	  
	v_particleOK.push_back(*(*vtrkIt)); 
      }
      
      //-----------------------Get vector of tracks ----------------------
      std::vector<const TrigInDetTrackCollection*> vectorOfTrackCollections;
      status = getFeatures( outputTE, vectorOfTrackCollections );
      
      if ( trigBphysColl != NULL ){
	if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "TrigBphys collection with " << trigBphysColl->size() << " muon pairs candidates" << endreq;
	
	if ( status != HLT::OK ) {
	  if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Failed to get InDetTrackCollections " << endreq;
	} else {
	  if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Got " << vectorOfTrackCollections.size() << " InDetTrackCollections" << endreq;
	}
	
	std::vector<const TrigInDetTrackCollection*>::iterator lastTrackColl = vectorOfTrackCollections.end();
	for ( std::vector<const TrigInDetTrackCollection*>::iterator iterTrackColl = vectorOfTrackCollections.begin(); iterTrackColl != lastTrackColl; ++iterTrackColl ){
	  
	  TrigInDetTrackCollection::const_iterator track= (*iterTrackColl)->begin();
	  TrigInDetTrackCollection::const_iterator lastTrack= (*iterTrackColl)->end();
	  if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Found " << (*iterTrackColl)->size() << " tracks" << endreq;	 
	  if ( (*iterTrackColl)->size() <= 2 )
	    continue;
	  
	  for ( int itrk=0; track !=lastTrack; ++itrk, ++track ) {
	    
	    unsigned int algoId = (*track)->algorithmId();
	    if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Info on InDet tracks, track: " << itrk << ", algo " << algoId
		  << " pT = " << (*track)->param()->pT() << " phi = " << (*track)->param()->phi0()
		  << " eta = " << (*track)->param()->eta() << endreq;

	    if ( (*track)->chi2() == 100000000.000 ) continue;
	    
	    particleOK.push_back((*track)->param()); 
	    v_particleOK.push_back((*track));        
	    std::vector<double> massHypo;
	    massHypo.clear();
	    massHypo.push_back(MUMASS);
	    massHypo.push_back(MUMASS);
	    massHypo.push_back(KPLUSMASS);
	    
	    if ( std::fabs(particleOK[2]->pT() - particleOK[0]->pT()) < EPSILON || std::fabs(particleOK[2]->pT() - particleOK[1]->pT()) < EPSILON ){
	      particleOK.pop_back();
	      v_particleOK.pop_back();
	      continue;
	    } 
	    if ( particleOK[2]->pT() < 0 ){   // only positive tracks (B+ case) 
	      particleOK.pop_back();
	      v_particleOK.pop_back();
	      continue;
	    }
	    if ( std::fabs(particleOK[2]->pT() - particleOK[0]->pT()) > EPSILON && std::fabs(particleOK[2]->pT() - particleOK[1]->pT()) > EPSILON ){
	      double BmassHypo = InvMass(particleOK, massHypo);
	      if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "3 particles inv mass = " << BmassHypo << endreq;
	      particleOK.pop_back();
	      v_particleOK.pop_back();
	      
	      // apply mass cut
	      if ( BmassHypo < m_lowerKplusMuMuMassCut || BmassHypo > m_upperKplusMuMuMassCut ) {
		if ( msgLvl()<= MSG::DEBUG ){
		  msg() << MSG::DEBUG << "3 particles combination discarded by mass cut: "
			<< BmassHypo << " MeV" << endreq;
		}
		continue;
	      } else {
		mon_kdimumass.push_back((BmassHypo*0.001));
		PassedKMuMuMass = true;
		result = true;
		if( msgLvl() <= MSG::DEBUG ) {
		  msg() << MSG::DEBUG << "3 particles combination retained! Invariant mass value: "
			<< BmassHypo << " MeV" << endreq;
		}
	      }
	      
	      
	      //store new TrigL2Bphys collection
	      TrigL2Bphys* trigPartBmumuX = new TrigL2Bphys(0, 0., 0., TrigL2Bphys::BMUMUX, BmassHypo);
	      if( msgLvl() <= MSG::DEBUG ) {
		msg()  << MSG::DEBUG << "Create Bphys particle with roI Id " << trigPartBmumuX->roiId()
		       << " phi = " << trigPartBmumuX->phi() << " eta = " << trigPartBmumuX->eta()
		       << " mass = " << trigPartBmumuX->mass() 
		       << " particle type (trigPartBmumuX->particleType()) " << trigPartBmumuX->particleType() << endreq;
	      }
	      // need to find index of tracks in collection
	      TrigInDetTrackCollection::const_iterator it = (*iterTrackColl)->begin(),e = (*iterTrackColl)->end();
	      int itr1=-99;
	      int itr2=-99;
	      int itr3=-99;
	      for (int itrk=0; it != e; ++it,++itrk) {
		if (v_particleOK[0] == (*it)) itr1=itrk;
		if (v_particleOK[1] == (*it)) itr2=itrk;
		if (v_particleOK[2] == (*it)) itr3=itrk;
	      }
	      if (itr1==-99 || itr2==-99 || itr3==-99) msg() << MSG::INFO << "Something is worng, can't find tracks in original container" << endreq;
	      ElementLink<TrigInDetTrackCollection> track1EL(*(*iterTrackColl),itr1);
	      ElementLink<TrigInDetTrackCollection> track2EL(*(*iterTrackColl),itr2);
	      ElementLink<TrigInDetTrackCollection> track3EL(*(*iterTrackColl),itr3);
	      
	      trigPartBmumuX->addTrack(track1EL);
	      trigPartBmumuX->addTrack(track2EL);
	      trigPartBmumuX->addTrack(track3EL);
	      
	      if ( m_doVertexFit ){
		
		if ( timerSvc() ) 
		  m_VtxFitTimer->start();
		
		// creating empty TrigL2Vertex
		TrigL2Vertex* vL2Kmumu = new TrigL2Vertex();
		
		TrigVertex* p_KmumuV = NULL;
		
		// adding TrigInDetTrack* to the vertex 
		StatusCode sc_v = m_vertexingTool->addTrack(v_particleOK[0], vL2Kmumu, Trk::muon);
		if(sc_v.isFailure()){
		  if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Failed to add track 1 to vertexingTool" << endreq;
		}
		sc_v = m_vertexingTool->addTrack(v_particleOK[1], vL2Kmumu, Trk::muon);
		if(sc_v.isFailure()){
		  if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Failed to add track 2 to vertexingTool" << endreq;
		}
		sc_v = m_vertexingTool->addTrack(v_particleOK[2], vL2Kmumu, Trk::kaon);
		if(sc_v.isFailure()){
		  if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Failed to add track 3 to vertexingTool" << endreq;
		}
		
		// vertex fit
		StatusCode sc_vert = m_L2vertFitter->fit(vL2Kmumu);
		
		if ( sc_vert.isSuccess() ) {
		  //invariant mass calculation
		  sc_vert = m_vertexingTool->calculateInvariantMass(vL2Kmumu);
		  if ( sc_vert.isSuccess() ) {
		    //mother particle creation
		    sc_vert = m_vertexingTool->createMotherParticle(vL2Kmumu);
		    if ( sc_vert.isSuccess() ) {
		      if ( msgLvl() <= MSG::DEBUG) {
			msg() << MSG::DEBUG << "B+ created:" << endreq;
			msg() << MSG::DEBUG << "B+ particle parameters : " 
			    << "  d0 =" << vL2Kmumu->m_getMotherTrack()->a0() 
			    << "  z0 = " << vL2Kmumu->m_getMotherTrack()->z0() 
			    << "  phi0 = " << vL2Kmumu->m_getMotherTrack()->phi0() 
			    << "  eta = " << vL2Kmumu->m_getMotherTrack()->eta() 
			    << "  pT = " << vL2Kmumu->m_getMotherTrack()->pT() << endreq;
		      }
		      p_KmumuV = m_vertexingTool->createTrigVertex(vL2Kmumu);
		      
		      if ( p_KmumuV != NULL ){
			if ( msgLvl() <= MSG::DEBUG) {
			  msg() << MSG::DEBUG << "REGTEST: Kmumu vertex Fit: x = " << p_KmumuV->x() << " , y = " << p_KmumuV->y() << " , z = " << p_KmumuV->z() << endreq;
			  msg() << MSG::DEBUG << "REGTEST: Kmumu invariant mass = " << p_KmumuV->mass() << endreq;
			}
			trigPartBmumuX->fitmass(p_KmumuV->mass());
			trigPartBmumuX->fitchi2(p_KmumuV->chi2());
			trigPartBmumuX->fitndof(p_KmumuV->ndof());
			trigPartBmumuX->fitx(p_KmumuV->x());
			trigPartBmumuX->fity(p_KmumuV->y());
			trigPartBmumuX->fitz(p_KmumuV->z());
			if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "Added vertex information to B particle" << endreq;
			
			m_trigBphysColl->push_back(trigPartBmumuX);
			if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Added particle to Bphys particle collection" << endreq;
			
			resultKMuMu = true;
			PassedKMuMuVertex = true;
			delete p_KmumuV;
			
		      } else {
			if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG <<"Failed to create Kmumu vertex" << endreq;
			delete p_KmumuV;
			delete trigPartBmumuX;
		      }
		    } else {
		      if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG <<"Failed to create mother particle of the Kmumu L2vertex" << endreq;
		      delete p_KmumuV;
		      delete trigPartBmumuX;
		    }
		  } else {
		    if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG <<"Failed to calculate invariant mass for the Kmumu L2vertex" << endreq;
		    delete p_KmumuV;
		    delete trigPartBmumuX;
		  }
		} else {
		  if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG <<"Failed to fit the Kmumu L2vertex" << endreq;
		  delete p_KmumuV;    
		  delete trigPartBmumuX;
		}
		delete vL2Kmumu;
		if ( timerSvc() ) 
		  m_VtxFitTimer->stop();
		
	      } else {  //end if ( m_doVertexFit ) 
		
		m_trigBphysColl->push_back(trigPartBmumuX);
		if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Added particle to Bphys particle collection" << endreq;
		
	      }  //end if ( m_doVertexFit ) else
	      
	    } // end if to control tracks are not the same of muons	      
	  } //end for iterator over InDetTrackCollection - itrk
	} // end fot over vector InDetTrackCollection -  iterTrackColl
      } // end if ( trigBphysColl != NULL )
	// JK 22-05-08 changes for new EDM      } // end if (p_vtx !=NULL)
      
    } // end if ( (*thePair)->particleType() == TrigL2Bphys::JPSIMUMU) 
  } // end iterator over pairs TrigL2Bphys Collection
  
  if ( ( m_trigBphysColl != 0 ) && ( m_trigBphysColl->size() > 0 ) ) {
    if ( msgLvl() <= MSG::DEBUG) msg()  << MSG::DEBUG << "REGTEST: Store Bphys Collection size: " << m_trigBphysColl->size() << endreq;
    HLT::ErrorCode sc = attachFeature(outputTE, m_trigBphysColl, "L2DiMuXFex" );
    if ( sc != HLT::OK ) {
      msg() << MSG::WARNING << "Failed to store trigBphys Collection" << endreq;
      if ( timerSvc() ) 
	m_TotTimer->stop();        
      return sc;
    }
  } else {
    if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST: No Bphys Collection to store"  << endreq;
    delete m_trigBphysColl;
  }
  m_trigBphysColl=0;
 
  if ( PassedMuMuPair ) m_countPassedMuMuMassCut++;
  if ( PassedKMuMuMass ) m_countPassedKMuMuMassCut++;
  if ( PassedKMuMuVertex ) m_countPassedKMuMuVertexCut++;
  if ( resultMuMu == true ){
    if ( IdEvent != (int) m_lastEventPassedMuMu ) {
      m_countPassedEventsMuMu++;
      m_lastEventPassedMuMu=IdEvent;
    }
  }
  if ( resultKMuMu == true ) {
    if ( IdEvent != (int) m_lastEventPassedKMuMu ) {
      m_countPassedEventsKMuMu++;
      m_lastEventPassedKMuMu=IdEvent;
    }
  }
  if ( result == true ){
    m_countPassedRoIs++;
    if ( IdEvent != (int) m_lastEventPassed ) {
      m_countPassedEvents++;
      m_lastEventPassed=IdEvent;
    }
  }
  
  if ( timerSvc() ) 
    m_TotTimer->stop();    
  
  return HLT::OK;
}


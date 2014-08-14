// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBPhysHypo/TrigEFBMuMuHypo.cxx
 **
 **   Description: EF hypothesis algorithms for B0_s -> mu+ mu-  
 **             
 **************************************************************************/ 
 
#include "TrigMuonEvent/CombinedMuonFeature.h"
//#include "TrigMuonEvent/TrigMuonEFContainer.h"
#include "TrigBphysHypo/TrigEFBMuMuFex.h"
#include "TrigBphysHypo/BtrigUtils.h"
//#include "TrigParticle/TrigEFBphysContainer.h"
//#include "TrigBphysHypo/TrigEFB.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include <math.h>
#include "EventInfo/EventInfo.h" 
#include "EventInfo/EventID.h" 
#include "TrigTimeAlgs/TrigTimerSvc.h"
//#include "TrigNavigation/Navigation.h"

#include <iostream>
//class ISvcLocator;

#include "MuidEvent/MuidTrackContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
//#include "MuidEvent/Muid_ClassDEF.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkParameters/TrackParameters.h"
#include "Particle/TrackParticleContainer.h"
#include <sstream>
#include "VxVertex/VxCandidate.h"

#include "CLHEP/Vector/ThreeVector.h"

// additions of xAOD objects
#include "xAODEventInfo/EventInfo.h"

TrigEFBMuMuFex::TrigEFBMuMuFex(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::ComboAlgo(name, pSvcLocator),
  m_fitterSvc("Trk::TrkVKalVrtFitter/VertexFitterTool",this),
  m_trigBphysColl(NULL)
{

  // Read cuts

  declareProperty("AcceptAll",    m_acceptAll=true);
  declareProperty("OppositeSign", m_oppositeCharge=true);
  declareProperty("LowerMassCut", m_lowerMassCut=2000.0);
  declareProperty("UpperMassCut", m_upperMassCut=10000.0);
  declareProperty("ApplyUpperMassCut", m_ApplyupperMassCut=true);
  declareProperty("VertexFitterTool", m_fitterSvc);
  declareProperty("MuonAlgo"    , m_muonAlgo="TrigMoore");
  declareProperty("noId"        , m_noId              = false);
  
  declareMonitoredVariable("DeltaEtaRoIs", mon_dEtaRoI    );
  declareMonitoredVariable("DeltaPhiRoIs", mon_dPhiRoI    );
  declareMonitoredVariable("DeltaEtaMuMu", mon_dEtaMuMu   );
  declareMonitoredVariable("DeltaPhiMuMu", mon_dPhiMuMu   );
  declareMonitoredVariable("pTsumMuMu",    mon_pTsum      );
  declareMonitoredVariable("pTMu1",        mon_mu1pT      );
  declareMonitoredVariable("pTMu2",        mon_mu2pT      );
  declareMonitoredVariable("etaMu1",       mon_mu1eta     );
  declareMonitoredVariable("etaMu2",       mon_mu2eta     );
  declareMonitoredVariable("phiMu1",       mon_mu1phi     );
  declareMonitoredVariable("phiMu2",       mon_mu2phi     );
  declareMonitoredVariable("MuMumass",     mon_MuMumass   );
  declareMonitoredVariable("BmassFit",     mon_BmassFit   );
  declareMonitoredVariable("VtxChi2",      mon_Chi2       );
 
}

TrigEFBMuMuFex::~TrigEFBMuMuFex()
{ }

HLT::ErrorCode TrigEFBMuMuFex::hltInitialize()
{
  if (msgLvl() <= MSG::DEBUG) {
   msg() << MSG::DEBUG << "Initialization ..." << endreq;
   msg() << MSG::DEBUG << "AcceptAll            = " 
	 << (m_acceptAll==true ? "True" : "False") << endreq; 
    msg() << MSG::DEBUG << "noId          = "
          << (m_noId==true ? "True" : "False") << endreq;
   msg() << MSG::DEBUG << "OppositeCharge       = " 
	 << (m_oppositeCharge==true ? "True" : "False") << endreq; 
   msg() << MSG::DEBUG << "LowerMassCut         = " << m_lowerMassCut << endreq;
   msg() << MSG::DEBUG << "UpperMassCut         = " << m_upperMassCut << endreq;
   msg() << MSG::DEBUG << "ApplyUpperMassCut         = " << m_ApplyupperMassCut << endreq;
  }
  if ( timerSvc() ) {
        m_BmmHypTot = addTimer("EFBmmHypTot");
        m_BmmHypVtx = addTimer("EFBmmHypVtxFit");
        }
                     
       
  m_lastEvent=-1;
  m_lastEventPassed=-1;
  m_countTotalEvents=0;
  m_countTotalRoI=0;
  m_countPassedEvents=0;
  m_countPassedRoIs=0;
  m_countPassedmumuPairsEv=0;
  m_countPassedBsMassEv=0;
  m_countPassedVtxFitEv=0;
  m_countPassedmumuPairs2R=0;
  m_countPassedBsMass2R=0;
  m_countPassedVtxFit2R=0;

  if (m_fitterSvc.retrieve().isFailure()) {
    msg() << MSG::ERROR << "Can't find Trk::TrkVKalVrtFitter" << endreq;
    return StatusCode::SUCCESS; 
  } else {
    if (msgLvl() <= MSG::DEBUG) {
     msg() << MSG::DEBUG << "Trk::TrkVKalVrtFitter found" << endreq;
    }
    m_VKVFitter = dynamic_cast<Trk::TrkVKalVrtFitter*>(&(*m_fitterSvc)); 
  }
  return HLT::OK;
}

HLT::ErrorCode TrigEFBMuMuFex::hltFinalize()
{
  msg() << MSG::INFO << "in finalize()" << endreq;
  MsgStream log(msgSvc(), name());
  
  msg() << MSG::INFO << "|----------------------- SUMMARY FROM TrigEFBMuMuFex -------------|" << endreq;
  msg() << MSG::INFO << "Run on events/2xRoIs " << m_countTotalEvents << "/" << m_countTotalRoI <<  endreq;
  msg() << MSG::INFO << "Passed events/2xRoIs " << m_countPassedEvents << "/" << m_countPassedRoIs <<  endreq;
  msg() << MSG::INFO << "Passed MuMu pairs:events/2xRoIs "  << m_countPassedmumuPairsEv<<"/"<<m_countPassedmumuPairs2R << endreq;
  msg() << MSG::INFO << "Passed BsMass: events/2xRoIs "<< m_countPassedBsMassEv<<"/"<<m_countPassedBsMass2R<<endreq;
  msg() << MSG::INFO << "Passed Vtx Fit: events/2xRoIs "<<m_countPassedVtxFitEv<< "/"<<m_countPassedVtxFit2R<< endreq;

  return HLT::OK;
}

//-------------------------------------------------------------------------------------
HLT::ErrorCode TrigEFBMuMuFex::acceptInputs(HLT::TEConstVec& inputTE, bool& pass) {
  
  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "Running TrigEFBMuMuFex::acceptInputs" << endreq;
  
  
  bool vtxpass=false;
  bool mumuIDpass=false;
  bool PassedBsMass=false;

  mon_dEtaRoI = -10.;
  mon_dPhiRoI = -10.;
  mon_dEtaMuMu = -10.;
  mon_dPhiMuMu = -10.;
  mon_pTsum = -10.;
  mon_mu1pT = -10.;
  mon_mu2pT = -10.;
  mon_mu1eta = -10.;
  mon_mu2eta = -10.;
  mon_mu1phi = -10.;
  mon_mu2phi = -10.;
  mon_MuMumass = -1.;
  mon_BmassFit = -1;
  mon_Chi2 = -1.;

  if ( timerSvc() )    m_BmmHypTot->start();
  //const double BSMASS = 5369.3;
    
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
            //mon_Errors.push_back( ERROR_No_EventInfo );
        } else {
            IdRun   = pEventInfo->event_ID()->run_number();
            IdEvent = pEventInfo->event_ID()->event_number();
            if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Run " << IdRun << " Event " << IdEvent << " using algo " << m_muonAlgo <<  endreq;
        }// found old event info
    }else { // found the xAOD event info
        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Run " << evtInfo->runNumber()
            << " Event " << evtInfo->eventNumber() << " using algo " << m_muonAlgo << endreq;
        IdRun   = evtInfo->runNumber();
        IdEvent = evtInfo->eventNumber();
    } // get event ifo

  
  
  if ( inputTE.size() != 2 ) {
    msg() << MSG::ERROR << "Got different than 2 number of input TEs" << endreq;
    if ( timerSvc() )    m_BmmHypTot->stop();
    return HLT::BAD_JOB_SETUP;
  }
  
  
  const TrigRoiDescriptor *roi1(0);
  const TrigRoiDescriptor *roi2(0);
  
  // get them from the navigation
  if ( getFeature(inputTE.front(), roi1) != HLT::OK ) {
    msg() << MSG::ERROR << "Navigation error while getting RoI descriptor 1" << endreq; 
    if ( timerSvc() ) m_BmmHypTot->stop();
    return HLT::NAV_ERROR;
  }
  if ( roi1 == NULL ) {
    msg() << MSG::ERROR << "Navigation error while getting RoI descriptor 1 (retrieved null pointer)" << endreq; 
    if ( timerSvc() ) m_BmmHypTot->stop();
    return HLT::NAV_ERROR;
  }
  
  if ( getFeature(inputTE.back(), roi2) != HLT::OK ) {
    msg() << MSG::ERROR << "Navigation error while getting RoI descriptor 2" << endreq; 
    if ( timerSvc() )    m_BmmHypTot->stop();
    return HLT::NAV_ERROR;
  }
  if ( roi2 == NULL ) {
    msg() << MSG::ERROR << "Navigation error while getting RoI descriptor 2 (retrieved null pointer)" << endreq; 
    if ( timerSvc() ) m_BmmHypTot->stop();
    return HLT::NAV_ERROR;
  }
  
  
  if ( msgLvl() <= MSG::DEBUG ){
    msg() << MSG::DEBUG
	  << "Using inputTEs: "<< inputTE.front() <<  " and "  << inputTE.back() << " with Ids " << inputTE.front()->getId()<<" AND "<<inputTE.back()->getId()
	  << "; RoI IDs = "   << roi1->roiId()<<" AND "<<roi2->roiId()
	  << ": Eta1 = "      << roi1->eta()<<" Eta2= "<<roi2->eta()
	  << ", Phi1 = "      << roi1->phi()<<" Phi2= "<<roi2->phi()
	  << endreq;
  }

  mon_dEtaRoI = fabs(roi1->eta() - roi2->eta() );
  mon_dPhiRoI = roi1->phi() - roi2->phi();
  while (mon_dPhiRoI >  M_PI) mon_dPhiRoI -= 2*M_PI;
  while (mon_dPhiRoI < -M_PI) mon_dPhiRoI += 2*M_PI;
  mon_dPhiRoI = fabs(mon_dPhiRoI);

  // m u o n s
  
//  std::vector<const Trk::Track*> muidIDtracksPos;
//  std::vector<const Trk::Track*> muidIDtracksNeg;
  std::vector<const Trk::Track*> muidIDtracks1;
  std::vector<const Trk::Track*> muidIDtracks2;
  
  const DataHandle<MuidTrackContainer> MuidTracksEF1,MuidTracksEF2;


  
  //  MUID COMBINED track


  if (m_muonAlgo == "TrigMoore") {

     std::ostringstream s;
     s<<roi1->roiId()+1;
     if (StatusCode::SUCCESS != store()->retrieve(MuidTracksEF1,"MuidCombinedHybRoI"+s.str()))
       { msg()<< MSG::WARNING
	   << "MuidCombinedHyb:Could not retrieve COMBID Track container roiId= "<<roi1->roiId()<<" s= "<<s.str()
	   << endreq; //store()->dump();
       } else { 
         if ( msgLvl() <= MSG::DEBUG )
             msg() << MSG::DEBUG << "MuidTracks 1 found size()= "<<MuidTracksEF1->size()<< endreq; 
         for (MuidTrackContainer::const_iterator tr = MuidTracksEF1->begin();
	    tr != MuidTracksEF1->end(); tr++){
             if ( msgLvl() <= MSG::DEBUG )
	       msg() << MSG::DEBUG << " Muon 1 code is < " << (*tr)->muonCode() <<" >"<< endreq;
           if ((*tr)->muonCode()>100)
	     { 
	      //if((*tr)->indetTrack()->perigeeParameters()->charge() > 0 ) addUnique(muidIDtracksPos,(*tr)->indetTrack());
	      //else addUnique(muidIDtracksNeg,(*tr)->indetTrack());
	      addUnique(muidIDtracks1,(*tr)->indetTrack());
	  
	        if ( msgLvl() <= MSG::DEBUG )
		  msg() << MSG::DEBUG <<" track 1 code is "<<(*tr)->muonCode() << endreq;
	      const Trk::Perigee* perigee = (*tr)->indetTrack()->perigeeParameters();
	        if ( msgLvl() <= MSG::DEBUG )   
		  msg() << MSG::DEBUG 
		  << " Comb muon 1 pt/eta/phi " << perigee->pT()
                << " / " << perigee->eta() << " / " <<  perigee->parameters()[Trk::phi0]
	          << endreq;
             }
	 }
       }
     s.clear();
     std::ostringstream s1;
     s1<<roi2->roiId()+1;
  
     if (StatusCode::SUCCESS != store()->retrieve(MuidTracksEF2,"MuidCombinedHybRoI"+s1.str()))
      { msg()<< MSG::WARNING 
	   << "MuidCombinedHyb:Could not retrieve COMBID Track container roiId= "<<roi2->roiId()<<" s= "<<s1.str()
	   << endreq;
      } else {
          if ( msgLvl() <= MSG::DEBUG )
	    msg() << MSG::DEBUG << "MuidTracks 2 found size()= "<<MuidTracksEF2->size()<< endreq; 
        for (MuidTrackContainer::const_iterator tr = MuidTracksEF2->begin();
	    tr != MuidTracksEF2->end(); tr++){
             if ( msgLvl() <= MSG::DEBUG )
	       msg() << MSG::DEBUG << " Muon 2 code is < " << (*tr)->muonCode() <<" >"<< endreq;
           if ((*tr)->muonCode()>100)
	    { 
	     //if((*tr)->indetTrack()->perigeeParameters()->charge() > 0 ) addUnique(muidIDtracksPos,(*tr)->indetTrack());
	     //else addUnique(muidIDtracksNeg,(*tr)->indetTrack());
	     addUnique(muidIDtracks2,(*tr)->indetTrack());
	       if ( msgLvl() <= MSG::DEBUG )
		 msg() << MSG::DEBUG <<" track 2 code is "<<(*tr)->muonCode() << endreq;
	     const Trk::Perigee* perigee = (*tr)->indetTrack()->perigeeParameters();
	       if ( msgLvl() <= MSG::DEBUG )
		 msg() << MSG::DEBUG 
		 << " Comb muon 2 pt/eta/phi " << perigee->pT()
		 << " / " << perigee->eta() << " / " <<  perigee->parameters()[Trk::phi0]
	         << endreq;
	    }
	}
      }
  } else {   // TrigMuSuperEF

    // Retrieve the MuonFInfo container
    std::vector<const TrigMuonEFInfoContainer*> MuEFTracksEF1;
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Try to retrieve EFInfo container of muon 1" << endreq;
    HLT::ErrorCode status = getFeatures(inputTE.front(), MuEFTracksEF1);
    if ( status != HLT::OK ) {
      if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Failed to get EFInfo feature of muon 1, exiting" << endreq;
      return HLT::OK;
    }
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Got MuonEF (1) Feature, size = " << MuEFTracksEF1.size() << endreq;
    for ( unsigned int i_mu=0; i_mu<MuEFTracksEF1.size(); i_mu++ ) {
      std::vector<const Trk::Track*> idTrks;
      status = GetTrigMuonEFInfoTracks(MuEFTracksEF1[i_mu], idTrks, msg());
      if ( status != HLT::OK ) return status;
      for ( unsigned int i_trk=0; i_trk<idTrks.size(); i_trk++ ) {
        addUnique(muidIDtracks1, idTrks[i_trk]);
        const Trk::Perigee* perigee = idTrks[i_trk]->perigeeParameters();
        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG
                                            << " Comb muon 1 pt/eta/phi " << perigee->pT()
                                            << " / " << perigee->eta() << " / " << perigee->parameters()[Trk::phi]
                                            << endreq;
      }
    }

    if (m_noId) { // using standalone tracks
     msg() << MSG::DEBUG << "Try to retieve standalone track 2" << endreq; 
      const TrackCollection *MuEFSATracks;
    if (HLT::OK != getFeature(inputTE.back(), MuEFSATracks,"forCB"))
    { 
      msg()<< MSG::WARNING << " Could not retrieve SA muon tracks using getFeature " << endreq; 
    } else {
      if (MuEFSATracks==0) {
	  if ( msgLvl() <= MSG::DEBUG )
	    msg() << MSG::DEBUG << "NUll pointer for MuEFSATracks" << endreq; 
          return HLT::OK;
      }

      if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "MuEFSATracks RoI 2 found size()= " << MuEFSATracks->size()<< endreq; 

      for (TrackCollection::const_iterator tr = MuEFSATracks->begin();
	 tr != MuEFSATracks->end(); tr++){

	  const Trk::Perigee* perigee = (*tr)->perigeeParameters();
	    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG 
		<< " SA muon 2 pt/eta/phi " << perigee->pT()
		<< " / " << perigee->eta() << " / " <<  perigee->parameters()[Trk::phi0]
	        << endreq;

	     addUnique(muidIDtracks2,(*tr));

      } // end loop over muEFSAtracks
    
    } // end if tracks found


    } else {   // !m_noId so we can use Combined muon tracks

      // Retrieve the MuonFInfo container
      std::vector<const TrigMuonEFInfoContainer*> MuEFTracksEF2;
      if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Try to retrieve EFInfo container of muon 2" << endreq;
      status = getFeatures(inputTE.back(), MuEFTracksEF2);
      if ( status != HLT::OK ) {
        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Failed to get EFInfo feature of muon 2, exiting" << endreq;
        return HLT::OK;
      }
      if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Got MuonEF (2) Feature, size = " << MuEFTracksEF2.size() << endreq;
      for ( unsigned int i_mu=0; i_mu<MuEFTracksEF2.size(); i_mu++ ) {
        std::vector<const Trk::Track*> idTrks;
        HLT::ErrorCode status = GetTrigMuonEFInfoTracks(MuEFTracksEF2[i_mu], idTrks, msg());
        if ( status != HLT::OK ) return status;
        for ( unsigned int i_trk=0; i_trk<idTrks.size(); i_trk++ ) {
          addUnique(muidIDtracks2, idTrks[i_trk]);
          const Trk::Perigee* perigee = idTrks[i_trk]->perigeeParameters();
          if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG
                                              << " Comb muon 2 pt/eta/phi " << perigee->pT()
                                              << " / " << perigee->eta() << " / " << perigee->parameters()[Trk::phi]
                                              << endreq;
        }
      }
    } // end if else m_noId
  } // end if else on m_muonAlgo
  
  
  
  //======= do vertexing  ==========================================================  
    //if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG <<" muidIDtracksPos.size()= "<<muidIDtracksPos.size()<<endreq;
    //if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG <<" muidIDtracksNeg.size()= "<<muidIDtracksNeg.size()<<endreq;
  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG <<" muidIDtracks1.size()= "<<muidIDtracks1.size()<<" muidIDtracks2.size()= "<<muidIDtracks2.size() << endreq;
  
  
  
  //std::vector<double> InpMass; InpMass.push_back(105.6);InpMass.push_back(105.6);
  TrigEFBphys* trigPartBmumu (NULL);
  //  TrigEFB* trigPartBmumu (NULL);
  
//  m_VertexColl = new VxContainer();
  m_trigBphysColl = new TrigEFBphysContainer();
 
//  if(muidIDtracksPos.size()> 0 && muidIDtracksNeg.size() > 0 )
//  if(muidIDtracks.size()> 1 )

  if(muidIDtracks1.size()>= 1 && muidIDtracks2.size()>= 1) {
      mumuIDpass = true;
//      std::vector<const Trk::Track*>::iterator pItr=muidIDtracksPos.begin();
//      std::vector<const Trk::Track*>::iterator mItr=muidIDtracksNeg.begin();

// changes to allow noId option JK 13/9/11
//      std::vector<const Trk::Track*>::iterator pItr=muidIDtracks.begin();
//      std::vector<const Trk::Track*>::iterator mItr;
//      for(; pItr != muidIDtracks.end(); pItr++)
//        {  mItr = pItr; ++mItr;
//	for( ; mItr != muidIDtracks.end(); mItr++)
//	  {

      std::vector<const Trk::Track*>::iterator pItr=muidIDtracks1.begin();
      for(int iTrk=0; pItr != muidIDtracks1.end(); pItr++,iTrk++)  {
//          mItr = pItr; ++mItr;
      std::vector<const Trk::Track*>::iterator mItr=muidIDtracks2.begin();

	  for(int jTrk=0 ; mItr != muidIDtracks2.end(); mItr++,jTrk++)
	  {
            if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"Tracks:  " << iTrk << "  "  << jTrk << " " << *pItr << " " << *mItr << endreq;                               
	    if (mItr==pItr) {
              if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"Tracks same:  " << iTrk << "  " <<  jTrk << endreq;                               
	      continue;

	    } else {
              double dPhi=fabs((*mItr)->perigeeParameters()->parameters()[Trk::phi] -
			       (*pItr)->perigeeParameters()->parameters()[Trk::phi]);
              if (dPhi > M_PI) dPhi = 2.*M_PI - dPhi;
      
              if( dPhi < 0.02 &&
	           fabs((*mItr)->perigeeParameters()->eta() -
			(*pItr)->perigeeParameters()->eta()) < 0.02 ) 
	      { 

                if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"Tracks same parameters :  " << iTrk << "  " <<  jTrk << endreq;                               
	        continue;
	      }
	    }

	    if(m_oppositeCharge && (*pItr)->perigeeParameters()->charge()*(*mItr)->perigeeParameters()->charge() > 0) continue;

	    Trk::VxCandidate * myVxCandidate ( 0 );
	    std::vector<const Trk::Track*> pair;
          Amg::Vector3D vtx(0.,0.,0.);
          Trk::Vertex vertex ( vtx );

	    pair.push_back(*pItr);
	    pair.push_back(*mItr);
	    //	    m_fitterSvc->->setDefault();
	    //m_fitterSvc->setMassInputParticles( InpMass );
	    myVxCandidate = m_fitterSvc->fit(pair,vertex);
	    if(myVxCandidate) {
	      vtxpass = true;
	      float massMuMu = invariantMass(*pItr,*mItr);
	      mon_MuMumass = ((massMuMu*0.001));
              if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"Create Bphys from tracks " << iTrk << "  " << jTrk << " Mass: " << massMuMu <<endreq;                               
	      trigPartBmumu = new TrigEFBphys( 0, 0., 0., TrigEFBphys::BMUMU, massMuMu);
	      //	      trigPartBmumu = new TrigEFB( 0, 0., 0., TrigEFB::BMUMU, massMuMu,myVxCandidate);
	      //m_VertexColl->push_back(myVxCandidate);
                  std::vector<int> trkIndices;                                                                                                                    
                  double invariantMass=0, invariantMassError=0;                                                                                                   
                  for (int i=0;i<(int)pair.size();++i) {trkIndices.push_back(1);}                                                                                 
                  if (!(m_VKVFitter->VKalGetMassError(trkIndices,invariantMass,invariantMassError).isSuccess())) {                                                
    //                mon_Errors.push_back( ERROR_CalcInvMass_Fails );                                                                                              
                  if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"Warning from VKaVrt - cannot calculate uncertainties!"<<endreq;                               
                  } else {
                  mon_BmassFit = invariantMass/1000;
                  }
              mon_Chi2 =  myVxCandidate->recVertex().fitQuality().chiSquared();
	      if(m_lowerMassCut < massMuMu && ((massMuMu < m_upperMassCut) || (!m_ApplyupperMassCut) )) 
		PassedBsMass = true;
	        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Sec Vtx at ("
		    << myVxCandidate->recVertex().position() [0] 
		    << myVxCandidate->recVertex().position() [1] 
		    << myVxCandidate->recVertex().position() [2] <<  ") with chi2 = "
		    << myVxCandidate->recVertex().fitQuality().chiSquared() << " ("
		    << myVxCandidate->vxTrackAtVertex()->size() << " tracks)" 
		    << " Mass= "<<massMuMu<< endreq;
	      trigPartBmumu->fitmass( (float) invariantMass );
	      trigPartBmumu->fitchi2(myVxCandidate->recVertex().fitQuality().chiSquared());
              trigPartBmumu->fitx(myVxCandidate->recVertex().position() [0]);
              trigPartBmumu->fity(myVxCandidate->recVertex().position() [1]);
              trigPartBmumu->fitz(myVxCandidate->recVertex().position() [2]);
	      
	      // JK look for tracks in TrackParticle containers and add to TrigEFBphys
              bool FoundMTrack=false;
              bool FoundPTrack=false;

	      
             ///////////////// Get Track Particles from RoI 1/////////////////
             std::vector<const Rec::TrackParticleContainer *> vectorTrkParticleCont;

             HLT::ErrorCode status = getFeatures(inputTE.front(), vectorTrkParticleCont);
             if(status != HLT::OK) {
                msg() << MSG::WARNING << "Failed to get TrackParticleContainer's from the trigger element" << endreq;
             } else {
                  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Got " << vectorTrkParticleCont.size()
	            << " TrackParticle Collections" << endreq;
             }  

             //if (vectorTrkParticleCont.size() ==0) return HLT::OK ;

             const Rec::TrackParticleContainer* TrkParticleCont = vectorTrkParticleCont.front();


               if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Now loop over TrackPartciles to find tracks " << endreq;

             Rec::TrackParticleContainer::const_iterator trkIt =  TrkParticleCont->begin();
             Rec::TrackParticleContainer::const_iterator lastTrkIt = TrkParticleCont->end();
               if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Found tracks, ntrack= " << TrkParticleCont->size() << endreq;

             for (int itrk=0 ; trkIt != lastTrkIt; itrk++, trkIt++) {
		const Trk::Track* track = (*trkIt)->originalTrack();
                  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "original track " << track << endreq; 
                if (track == *pItr && !FoundPTrack) {
                   ElementLink<Rec::TrackParticleContainer> trackEL(*TrkParticleCont,itrk);
                   trigPartBmumu->addTrack(trackEL);
                     if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG	<< "track matches positive track, will store in EFBPhys "<< endreq;
		   FoundPTrack=true;
		}
                if (track == *mItr && !FoundMTrack) {
                   ElementLink<Rec::TrackParticleContainer> trackEL(*TrkParticleCont,itrk);
                   trigPartBmumu->addTrack(trackEL);
                     if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG	<< "track matches negative track, will store in EFBPhys "<< endreq;
		   FoundMTrack=true;
		}
		

	     } // end loop over track partciles      
	      

             ///////////////// Get Track Particles from RoI 2/////////////////
	     // JK 26/9/2011 Only if NOT m_noId
	     if (!m_noId) {
             std::vector<const Rec::TrackParticleContainer *> vectorTrkParticleCont2;
             status = getFeatures(inputTE.back(), vectorTrkParticleCont2);
             if(status != HLT::OK) {
                msg() << MSG::WARNING << "Failed to get TrackParticleContainer's from the trigger element" << endreq;
             } else {
                  if ( msgLvl() <= MSG::DEBUG )  msg() << MSG::DEBUG << " Got " << vectorTrkParticleCont2.size()
	            << " TrackParticle Collections" << endreq;
             }  


             TrkParticleCont = vectorTrkParticleCont2.front();


               if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Now loop over TrackPartciles to find tracks " << endreq;

             trkIt =  TrkParticleCont->begin();
             lastTrkIt = TrkParticleCont->end();
               if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Found tracks, ntrack= " << TrkParticleCont->size() << endreq;

             for (int itrk=0 ; trkIt != lastTrkIt; itrk++, trkIt++) {

		const Trk::Track* track = (*trkIt)->originalTrack();
                  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "original track " << track << endreq; 
                if (track == *pItr && !FoundPTrack) {
                   ElementLink<Rec::TrackParticleContainer> trackEL(*TrkParticleCont,itrk);
                   trigPartBmumu->addTrack(trackEL);
                     if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG	<< "track matches positive track, will store in EFBPhys "<< endreq;
		}
                if (track == *mItr && !FoundMTrack) {
                   ElementLink<Rec::TrackParticleContainer> trackEL(*TrkParticleCont,itrk);
                   trigPartBmumu->addTrack(trackEL);
                     if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG	<< "track matches negative track, will store in EFBPhys "<< endreq;
		}
	     } // end loop over track partciles      
	      
	     } // end if (!m_noId)
             m_trigBphysColl->push_back(trigPartBmumu);
            
            delete myVxCandidate;
            myVxCandidate = 0;
	    } // end  if(myVxCandidate)

	    mon_dEtaMuMu = ( fabs( (*pItr)->perigeeParameters()->eta() - (*mItr)->perigeeParameters()->eta()) );
	    mon_dPhiMuMu = ( (*pItr)->perigeeParameters()->parameters()[Trk::phi0] -  (*mItr)->perigeeParameters()->parameters()[Trk::phi0]);
	    while (mon_dPhiMuMu >  M_PI) mon_dPhiMuMu -= 2*M_PI;
	    while (mon_dPhiMuMu < -M_PI) mon_dPhiMuMu += 2*M_PI;
	    mon_dPhiMuMu = fabs(mon_dPhiMuMu);
	    
	    mon_mu1pT  = fabs((*pItr)->perigeeParameters()->pT())*0.001;
	    mon_mu2pT  = fabs((*mItr)->perigeeParameters()->pT())*0.001;
	    mon_mu1eta = (*pItr)->perigeeParameters()->eta();
	    mon_mu2eta = (*mItr)->perigeeParameters()->eta();
	    mon_mu1phi = (*pItr)->perigeeParameters()->parameters()[Trk::phi0];
	    mon_mu2phi = (*mItr)->perigeeParameters()->parameters()[Trk::phi0];
	    
	    mon_pTsum = ((mon_mu1pT + mon_mu2pT));

            if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<" At end of loop for tracks2:  " << iTrk << "  "  << jTrk <<   " " << *pItr << " " << *mItr << endreq;                               

	  } //end mItr loop
            if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<" At end of loop for tracks1:  " << iTrk << "  "  <<   " " << *pItr << endreq;                               
      } // end loop over pItr      
  } //end if both track containers contain >= 1 track
  
  
  //  create vector for TrigL2Bphys particles
//  if(vtxpass) {
//    m_trigBphysColl->push_back(trigPartBmumu);
///  } 
  if (IdEvent !=  m_lastEvent) {
    m_countTotalEvents++;
    m_lastEvent=IdEvent;
  }
  m_countTotalRoI++;
  
  
  
  pass = true;
  if(vtxpass) m_countPassedVtxFit2R++;
  if(mumuIDpass) m_countPassedmumuPairs2R++; 
  if (PassedBsMass) m_countPassedBsMass2R++;
  m_countPassedRoIs++;
  if(IdEvent != m_lastEventPassed) {
    m_countPassedEvents++;
    m_lastEventPassed = IdEvent;
    if(vtxpass) m_countPassedVtxFitEv++;
    if(mumuIDpass) m_countPassedmumuPairsEv++;
    if (PassedBsMass) m_countPassedBsMassEv++;
  }
  if ( timerSvc() )  m_BmmHypTot->stop();    
  return HLT::OK;
}




HLT::ErrorCode TrigEFBMuMuFex::hltExecute(HLT::TEConstVec&, HLT::TriggerElement* outputTE)
{
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " In TrigEFBMuMu hltExecute" << endreq;

    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "    m_trigBphysColl " << m_trigBphysColl << endreq;
 
  if ((m_trigBphysColl!=0) && (m_trigBphysColl->size() > 0)) {
      if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "REGTEST: Store Bphys Collection size: " << m_trigBphysColl->size() << endreq;

    HLT::ErrorCode sc = attachFeature(outputTE, m_trigBphysColl, "EFBMuMuFex" );
    if(sc != HLT::OK) {
      msg()  << MSG::WARNING << "Failed to store trigBphys Collection" << endreq;
      return HLT::ERROR;
    }
  } else {
      if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "REGTEST: no bphys collection to store "  << endreq;
    delete m_trigBphysColl;

  }
  m_trigBphysColl = 0;
  return HLT::OK;
}

double TrigEFBMuMuFex::invariantMass(const Trk::Track* muon1, const Trk::Track* muon2) {

  double pT1 = muon1->perigeeParameters()->pT();
  double pT2 = muon2->perigeeParameters()->pT();

    //  double Px1   = pT1*muon1->perigeeParameters()->cosPhi();
    //  double Py1   = pT1*muon1->perigeeParameters()->sinPhi();
    //  double Pz1   = pT1*muon1->perigeeParameters()->cotTheta();
    //
    //  double Px2   = pT2*muon2->perigeeParameters()->cosPhi();
    //  double Py2   = pT2*muon2->perigeeParameters()->sinPhi();
    //  double Pz2   = pT2*muon2->perigeeParameters()->cotTheta();
  
    double Px1   = muon1->perigeeParameters()->momentum()[Trk::px];
    double Py1   = muon1->perigeeParameters()->momentum()[Trk::py];
    double Pz1   = muon1->perigeeParameters()->momentum()[Trk::pz];
    
    double Px2   = muon2->perigeeParameters()->momentum()[Trk::px];
    double Py2   = muon2->perigeeParameters()->momentum()[Trk::py];
    double Pz2   = muon2->perigeeParameters()->momentum()[Trk::pz];

    
  double m2   = 11151.36;
  
  double E = sqrt( pT1*pT1 + Pz1*Pz1 + m2) + sqrt( pT2*pT2 + Pz2*Pz2 + m2);
  double pX = Px1 + Px2;
  double pY = Py1 + Py2;
  double pZ = Pz1 + Pz2;
  
  double mass =  E*E - pX*pX - pY*pY - pZ*pZ ; 
  mass = mass > 0 ? sqrt(mass) : 0;  

  return mass;
}
 
void TrigEFBMuMuFex::addUnique(std::vector<const Trk::Track*>& tracks, const Trk::Track* trkIn)
{
//  std::cout<<" in addUnique : trkIn pT= "<<trkIn->perigeeParameters()->pT()<<std::endl;
  std::vector<const Trk::Track*>::iterator tItr;
  for( tItr = tracks.begin(); tItr != tracks.end(); tItr++)
    {
      double dPhi=fabs((*tItr)->perigeeParameters()->parameters()[Trk::phi] -
		       trkIn->perigeeParameters()->parameters()[Trk::phi]);
      if (dPhi > M_PI) dPhi = 2.*M_PI - dPhi;
      
      if( fabs(dPhi) < 0.02 &&
	  fabs((*tItr)->perigeeParameters()->eta() -
	       trkIn->perigeeParameters()->eta()) < 0.02 ) 
	{ //std::cout<<" TrigEFBMuMuFex addUnique: the SAME tracks! pT= "<<
	    //trkIn->perigeeParameters()->pT()<<" and "<<
	    //(*tItr)->perigeeParameters()->pT()<<std::endl;
      return;
	}
    } 
  tracks.push_back(trkIn);       
}


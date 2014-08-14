// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBphysHypo/TrigL2MultiMuFex.cxx
 **
 **   Description: EF hypothesis algorithms for 3mu and 4mu signatures  
 **             
 **************************************************************************/ 
 
#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "TrigBphysHypo/TrigEFMultiMuFex.h"


#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

//#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"
//#include "GeneratorObjects/McEventCollection.h"  
//#include "HepMC/GenEvent.h"                      
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

#include "TrigBphysHypo/Constants.h"
#include "TrigBphysHypo/BtrigUtils.h"

// additions of xAOD objects
#include "xAODEventInfo/EventInfo.h"


TrigEFMultiMuFex::TrigEFMultiMuFex(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::ComboAlgo(name, pSvcLocator),
  m_fitterSvc("Trk::TrkVKalVrtFitter/VertexFitterTool",this),
  m_trigBphysColl(NULL)
{

  // Read cuts

  declareProperty("AcceptAll",    m_acceptAll=true);
  declareProperty("NInputMuon"     , m_NInputMuon    = 3 );
  declareProperty("OppositeSign", m_oppositeCharge=true);
  declareProperty("NMassMuon"     , m_NMassMuon    = 2 );
  declareProperty("LowerMassCut", m_lowerMassCut=2000.0);
  declareProperty("UpperMassCut", m_upperMassCut=10000.0);
  declareProperty("VertexFitterTool", m_fitterSvc);
  declareProperty("MuonAlgo", m_muonAlgo="TrigMoore");
  
  declareMonitoredStdContainer("pTMu1",        mon_mu1pT       , AutoClear   );
  declareMonitoredStdContainer("pTMu2",        mon_mu2pT       , AutoClear  );
  declareMonitoredStdContainer("MuMumass",     mon_MuMumass    , AutoClear  );
  declareMonitoredStdContainer("FitMass",      mon_FitMass     , AutoClear );
  declareMonitoredStdContainer("VtxChi2",      mon_Chi2        , AutoClear  );
 
}

TrigEFMultiMuFex::~TrigEFMultiMuFex()
{ }

HLT::ErrorCode TrigEFMultiMuFex::hltInitialize()
{
  if (msgLvl() <= MSG::DEBUG) {
   msg() << MSG::DEBUG << "Initialization ..." << endreq;
   msg() << MSG::DEBUG << "AcceptAll            = " 
	 << (m_acceptAll==true ? "True" : "False") << endreq; 
    msg() << MSG::DEBUG << "Number of input muons expected   = " << m_NInputMuon << endreq;
    msg() << MSG::DEBUG << "Number of muons used for mass   = " << m_NMassMuon << endreq;
   msg() << MSG::DEBUG << "OppositeCharge       = " 
	 << (m_oppositeCharge==true ? "True" : "False") << endreq; 
   msg() << MSG::DEBUG << "LowerMassCut         = " << m_lowerMassCut << endreq;
   msg() << MSG::DEBUG << "UpperMassCut         = " << m_upperMassCut << endreq;
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

HLT::ErrorCode TrigEFMultiMuFex::hltFinalize()
{
  msg() << MSG::INFO << "in finalize()" << endreq;
  MsgStream log(msgSvc(), name());
  
  msg() << MSG::INFO << "|----------------------- SUMMARY FROM TrigEFMultiMuFex -------------|" << endreq;
  msg() << MSG::INFO << "Run on events/2xRoIs " << m_countTotalEvents << "/" << m_countTotalRoI <<  endreq;
  msg() << MSG::INFO << "Passed events/2xRoIs " << m_countPassedEvents << "/" << m_countPassedRoIs <<  endreq;
  msg() << MSG::INFO << "Passed MuMu pairs:events/2xRoIs "  << m_countPassedmumuPairsEv<<"/"<<m_countPassedmumuPairs2R << endreq;
  msg() << MSG::INFO << "Passed BsMass: events/2xRoIs "<< m_countPassedBsMassEv<<"/"<<m_countPassedBsMass2R<<endreq;
  msg() << MSG::INFO << "Passed Vtx Fit: events/2xRoIs "<<m_countPassedVtxFitEv<< "/"<<m_countPassedVtxFit2R<< endreq;

  return HLT::OK;
}

//-------------------------------------------------------------------------------------
HLT::ErrorCode TrigEFMultiMuFex::acceptInputs(HLT::TEConstVec& inputTE, bool& pass) {
  
  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "Running TrigEFMultiMuFex::acceptInputs" << endreq;
  
  
  //bool vtxpass=false;
  //bool mumuIDpass=false;
  //bool PassedBsMass=false;


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

  
  
  if ( inputTE.size() != m_NInputMuon ) {
    msg() << MSG::ERROR << "Got wrong number of input TEs, expect " << m_NInputMuon << " got " << inputTE.size() << endreq;
    if ( timerSvc() )    m_BmmHypTot->stop();
    return HLT::BAD_JOB_SETUP;
  }
  
 
  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Number of input TEs " << inputTE.size() << endreq;
  /*
 
  const TrigRoiDescriptor *roi1(0);
  const TrigRoiDescriptor *roi2(0);
  
  // get them from the navigation
  if ( getFeature(inputTE.front(), roi1) != HLT::OK ) {
    msg() << MSG::ERROR << "Navigation error while getting RoI descriptor 1" << endreq; 
    if ( timerSvc() )    m_BmmHypTot->stop();
    return HLT::NAV_ERROR;
  }
  
  if ( getFeature(inputTE.back(), roi2) != HLT::OK ) {
    msg() << MSG::ERROR << "Navigation error while getting RoI descriptor 2" << endreq; 
    if ( timerSvc() )    m_BmmHypTot->stop();
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
  std::vector<const Trk::Track*> muidIDtracks; 
  const DataHandle<MuidTrackContainer> MuidTracksEF1,MuidTracksEF2;
  const MuidTrackContainer* MuEFTracksEF1;
  const MuidTrackContainer* MuEFTracksEF2;


  
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
	      addUnique(muidIDtracks,(*tr)->indetTrack());
	  
	        if ( msgLvl() <= MSG::DEBUG )
		  msg() << MSG::DEBUG <<" track 1 code is "<<(*tr)->muonCode() << endreq;
	      const Trk::Perigee* perigee = (*tr)->indetTrack()->perigeeParameters();
	        if ( msgLvl() <= MSG::DEBUG )   
		  msg() << MSG::DEBUG 
		  << " Comb muon 1 pt/eta/phi " << perigee->pT()
		  << " / " << perigee->eta() << " / " <<  acos(perigee->cosPhi())  
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
	     addUnique(muidIDtracks,(*tr)->indetTrack());
	       if ( msgLvl() <= MSG::DEBUG )
		 msg() << MSG::DEBUG <<" track 2 code is "<<(*tr)->muonCode() << endreq;
	     const Trk::Perigee* perigee = (*tr)->indetTrack()->perigeeParameters();
	       if ( msgLvl() <= MSG::DEBUG )
		 msg() << MSG::DEBUG 
		 << " Comb muon 2 pt/eta/phi " << perigee->pT()
		 << " / " << perigee->eta() << " / " <<  acos(perigee->cosPhi())  
	         << endreq;
	    }
	}
      }
  } else {   //TrigMuonEF

    if (HLT::OK != getFeature(inputTE.front(), MuEFTracksEF1,"MuonCombEF"))
    { 
      msg()<< MSG::WARNING << " Could not retieve MuidTrackConatiner1 using getFeature " << endreq; 
    } else { 
      if (MuEFTracksEF1==0) {
           if ( msgLvl() <= MSG::DEBUG )
	     msg() << MSG::DEBUG << "NUll pointer for MuEFTracksEF1" << endreq; 
         return HLT::OK;
      } 
        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "MuEFTracks 1 found size()= "<<MuEFTracksEF1->size()<< endreq; 
      for (MuidTrackContainer::const_iterator tr = MuEFTracksEF1->begin();
	 tr != MuEFTracksEF1->end(); tr++){
        if ( msgLvl() <= MSG::DEBUG )
	  msg() << MSG::DEBUG << " Muon 1 code is < " << (*tr)->muonCode() <<" >"<< endreq;
     // if ((*tr)->muonCode()>=100)
//	{ 
	  //if((*tr)->indetTrack()->perigeeParameters()->charge() > 0 ) addUnique(muidIDtracksPos,(*tr)->indetTrack());
	  //else addUnique(muidIDtracksNeg,(*tr)->indetTrack());
	  addUnique(muidIDtracks,(*tr)->indetTrack());
	  
//	    if ( msgLvl() <= MSG::DEBUG )  msg() << MSG::DEBUG <<" track 1 code is "<<(*tr)->muonCode() << endreq;
	  const Trk::Perigee* perigee = (*tr)->indetTrack()->perigeeParameters();
	    if ( msgLvl() <= MSG::DEBUG )
	      msg() << MSG::DEBUG 
	      << " Comb muon 1 pt/eta/phi " << perigee->pT()
	      << " / " << perigee->eta() << " / " <<  acos(perigee->cosPhi())  
	      << endreq;
//        }
    }
    }

    if (HLT::OK != getFeature(inputTE.back(), MuEFTracksEF2,"MuonCombEF"))
    { 
      msg()<< MSG::WARNING << " Could not retieve MuidTrackConatiner2 using getFeature " << endreq; 
    } else {
      if (MuEFTracksEF2==0) {
	  if ( msgLvl() <= MSG::DEBUG )
	    msg() << MSG::DEBUG << "NUll pointer for MuEFTracksEF2" << endreq; 
        return HLT::OK;
      }

        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "MuEFTracks 2 found size()= " << MuEFTracksEF2->size()<< endreq; 

      for (MuidTrackContainer::const_iterator tr = MuEFTracksEF2->begin();
	 tr != MuEFTracksEF2->end(); tr++){
        if ( msgLvl() <= MSG::DEBUG )
	  msg() << MSG::DEBUG << " Muon 2 code is < " << (*tr)->muonCode() <<" >"<< endreq;
//      if ((*tr)->muonCode()>=100)
//	{ 
	  //if((*tr)->indetTrack()->perigeeParameters()->charge() > 0 ) addUnique(muidIDtracksPos,(*tr)->indetTrack());
	  //else addUnique(muidIDtracksNeg,(*tr)->indetTrack());
	  addUnique(muidIDtracks,(*tr)->indetTrack());
//	    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG <<" track 2 code is "<<(*tr)->muonCode() << endreq;
	  const Trk::Perigee* perigee = (*tr)->indetTrack()->perigeeParameters();
	    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG 
		<< " Comb muon 2 pt/eta/phi " << perigee->pT()
		<< " / " << perigee->eta() << " / " <<  acos(perigee->cosPhi())  
	        << endreq;
//        }
    }
    
    }
  } // end if else on m_muonAlgo
  
  
  
  //======= do vertexing  ==========================================================  
    //if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG <<" muidIDtracksPos.size()= "<<muidIDtracksPos.size()<<endreq;
    //if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG <<" muidIDtracksNeg.size()= "<<muidIDtracksNeg.size()<<endreq;
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG <<" muidIDtracks.size()= "<<muidIDtracks.size()<<endreq;
  
  
  
  //std::vector<double> InpMass; InpMass.push_back(105.6);InpMass.push_back(105.6);
  TrigEFBphys* trigPartBmumu (NULL);
  //  TrigEFB* trigPartBmumu (NULL);
  
//  m_VertexColl = new VxContainer();
  m_trigBphysColl = new TrigEFBphysContainer();
 
//  if(muidIDtracksPos.size()> 0 && muidIDtracksNeg.size() > 0 )
  if(muidIDtracks.size()> 1 )
    {
      mumuIDpass = true;
//      std::vector<const Trk::Track*>::iterator pItr=muidIDtracksPos.begin();
//      std::vector<const Trk::Track*>::iterator mItr=muidIDtracksNeg.begin();
      std::vector<const Trk::Track*>::iterator pItr=muidIDtracks.begin();
      std::vector<const Trk::Track*>::iterator mItr;
      for(; pItr != muidIDtracks.end(); pItr++)
        {  mItr = pItr; ++mItr;
	for( ; mItr != muidIDtracks.end(); mItr++)
	  {

	    if(m_oppositeCharge && (*pItr)->perigeeParameters()->charge()*(*mItr)->perigeeParameters()->charge() > 0) continue;

	    Trk::VxCandidate * myVxCandidate ( 0 );
	    std::vector<const Trk::Track*> pair;
	    Hep3Vector vtx ( 0.,0.,0. );
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
	      trigPartBmumu = new TrigEFBphys( 0, 0., 0., TrigEFBphys::MULTIMU, massMuMu);
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
	      
             m_trigBphysColl->push_back(trigPartBmumu);
            
            delete myVxCandidate;
            myVxCandidate = 0;
	    } // end  if(myVxCandidate)

	    mon_dEtaMuMu = ( fabs( (*pItr)->perigeeParameters()->eta() - (*mItr)->perigeeParameters()->eta()) );
	    mon_dPhiMuMu = ( (*pItr)->perigeeParameters()->parameters()[Trk::phi0] -  (*mItr)->perigeeParameters()->parameters()[Trk::phi0]);
	    while (mon_dPhiMuMu >  M_PI) mon_dPhiMuMu -= 2*M_PI;
	    while (mon_dPhiMuMu < -M_PI) mon_dPhiMuMu += 2*M_PI;
	    mon_dPhiMuMu = fabs(mon_dPhiMuMu);
	    
	    mon_mu1pT = ((fabs((*pItr)->perigeeParameters()->pT()))*0.001);
	    mon_mu2pT = ((fabs((*mItr)->perigeeParameters()->pT()))*0.001);
	    
	    mon_pTsum = ((mon_mu1pT + mon_mu2pT));
	  }
	  }       
    }
  
  
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

  */
  pass=true;
  return HLT::OK;

}




HLT::ErrorCode TrigEFMultiMuFex::hltExecute(HLT::TEConstVec& inputTE , HLT::TriggerElement* outputTE)
{
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " In TrigEFMultiMu hltExecute" << endreq;

  m_trigBphysColl = new TrigEFBphysContainer();

  if (m_NInputMuon==3) {
   if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Call processTriMuon " << endreq;

    processTriMuon(inputTE);
  }

    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "    m_trigBphysColl " << m_trigBphysColl << endreq;
 
  if ((m_trigBphysColl!=0) && (m_trigBphysColl->size() > 0)) {
      if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "REGTEST: Store Bphys Collection size: " << m_trigBphysColl->size() << endreq;

    HLT::ErrorCode sc = attachFeature(outputTE, m_trigBphysColl, "EFMultiMuFex" );
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

void TrigEFMultiMuFex::processTriMuon(HLT::TEConstVec& inputTE)
{
  if ( msgLvl() <= MSG::VERBOSE ) msg() << MSG::VERBOSE << " In processTriMuon " << endreq;

  std::vector<const TrigMuonEFInfoContainer*> muon[3];
  std::vector<const Trk::Track*> muidIDtracks;

  for ( int i_te=0; i_te<3; i_te++ ) {

    if ( getFeatures(inputTE[i_te], muon[i_te]) != HLT::OK ) {
      msg() << MSG::DEBUG << "Failed to get MuonEFInfoContainer for TE " << i_te << endreq;
      return;
    }

    const Trk::Perigee* perigee;
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "MuonEFInfocontainer " << i_te << ": size" << muon[i_te].size() << endreq;
    for ( unsigned int i_mu=0; i_mu<muon[i_te].size(); i_mu++ ) {
      std::vector<const Trk::Track*> idTrks;
      HLT::ErrorCode status = GetTrigMuonEFInfoTracks(muon[i_te][i_mu], idTrks, msg());
      if ( status != HLT::OK ) return;
      for ( unsigned int i_trk=0; i_trk<idTrks.size(); i_trk++ ) {
        addUnique(muidIDtracks, idTrks[i_trk]);
        perigee = idTrks[i_trk]->perigeeParameters();
        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG  << "muon" << i_te << ", pointer and indet track pointer " << muon[i_te][i_mu] << " " << idTrks[i_trk] << endreq;
        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG
                                            << " Comb muon " << i_te << " pt/eta/phi " << perigee->pT()
                                            << " / " << perigee->eta() << " / " << perigee->parameters()[Trk::phi]
                                            << endreq;
      }
    }
  }

  if ( msgLvl() <= MSG::DEBUG )msg() << MSG::DEBUG << "Number of unique tracks : " << muidIDtracks.size() << endreq;


      
      // Create vector for TrigEFBphys particles
	// m_trigBphysColl = new TrigEFBphysContainer();

      double Mass;
      //      TrigEFBphys *TrigPart=NULL;

      if (m_NMassMuon ==2 && muidIDtracks.size()> 1) {   // check mass of opposite sign muon pairs

        std::vector<const Trk::Track*>::iterator pItr=muidIDtracks.begin();
        std::vector<const Trk::Track*>::iterator mItr;
        for(; pItr != muidIDtracks.end(); pItr++)
          {  mItr = pItr; ++mItr;
	  for( ; mItr != muidIDtracks.end(); mItr++)
	    { 
	      if( (*pItr)->perigeeParameters()->charge()*(*mItr)->perigeeParameters()->charge() > 0) continue;
              if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Muons are opp. charge will calculate mass " << endreq;

  	      mon_mu1pT.push_back((fabs((*pItr)->perigeeParameters()->pT()))*0.001);
	      mon_mu2pT.push_back((fabs((*mItr)->perigeeParameters()->pT()))*0.001);

	      //Mass = invariantMass(*pItr,*mItr);
	      Mass = getInvMass2(*pItr,*mItr);
              
              TrigEFBphys* TrigPart2 = checkInvMass2(*pItr, *mItr, Mass);

	      if (TrigPart2!=NULL){
	        findTracks(inputTE, *pItr, *mItr, TrigPart2); //find original inner detector tracks and add to TrigEFBphys
	        m_trigBphysColl->push_back(TrigPart2);
	      } else {
                delete TrigPart2;
	      }

	    }
	  }
      }

      if (m_NMassMuon ==3 && muidIDtracks.size()>2 ) {   // check mass of all 3 muons

        std::vector<const Trk::Track*>::iterator pItr=muidIDtracks.begin();
        std::vector<const Trk::Track*>::iterator mItr;
        std::vector<const Trk::Track*>::iterator nItr;
        for(; pItr != muidIDtracks.end(); pItr++)
          {  mItr = pItr; ++mItr;
	  for( ; mItr != muidIDtracks.end(); mItr++)
	    { nItr = mItr; ++nItr;
	      for( ; nItr != muidIDtracks.end(); nItr++) {
	      float charge1 =  (*pItr)->perigeeParameters()->charge();
	      float charge2 =  (*mItr)->perigeeParameters()->charge();
	      float charge3 =  (*nItr)->perigeeParameters()->charge();

	      // check charge
	      if( (charge1*charge2 > 0.) &&(charge1*charge3 > 0.)&&(charge3*charge2 > 0.) ) continue;
              if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Muons are opp. charge will calculate mass " << endreq;
	      Mass = getInvMass3(*pItr,*mItr,*nItr);
              
              TrigEFBphys* TrigPart3 = checkInvMass3(*pItr, *mItr,*nItr, Mass);

	      if (TrigPart3!=NULL){
	        findTracks(inputTE, *pItr, *mItr, *nItr,TrigPart3); //find original inner detector tracks and add to TrigEFBphys
	        m_trigBphysColl->push_back(TrigPart3);
	      }

	      }
	    }
	  }
      }
      
      return;
}


double TrigEFMultiMuFex::getInvMass2(const Trk::Track *muon1,const Trk::Track *muon2)
{
   if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " In getInvMass2 " << endreq;
   double Mass;
   std::vector<const Trk::Track*> inputtrks;
      std::vector<double> massHypo ;
      massHypo.push_back( MUMASS );
      massHypo.push_back( MUMASS );

      inputtrks.push_back(muon1);
      inputtrks.push_back(muon2);
      Mass=InvMass(inputtrks, massHypo);
      if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Muons 1 and 2 mass =  " << Mass << endreq;
      mon_MuMumass.push_back(Mass/1000.);
   return Mass;
}


 double TrigEFMultiMuFex::getInvMass3(const Trk::Track *muon1,const Trk::Track *muon2,const Trk::Track *muon3)
{
   if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " In getInvMass3 " << endreq;
   double Mass;
   std::vector<const Trk::Track*> inputtrks;
      std::vector<double> massHypo ;
      massHypo.push_back( MUMASS );
      massHypo.push_back( MUMASS );
      massHypo.push_back( MUMASS );

      inputtrks.push_back(muon1);
      inputtrks.push_back(muon2);
      inputtrks.push_back(muon3);
      Mass=InvMass(inputtrks, massHypo);
      if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Muons 1, 2 and 3 have mass =  " << Mass << endreq;
      mon_MuMumass.push_back(Mass/1000.);

   return Mass;
}

TrigEFBphys* TrigEFMultiMuFex::checkInvMass2(const Trk::Track* muon1, const Trk::Track* muon2, double Mass)
{
  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " In checkInvMass2, cuts " << m_lowerMassCut << " " << m_upperMassCut <<endreq;

   if ( Mass < m_lowerMassCut || Mass > m_upperMassCut  ) {
     if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Mass : " << Mass << " cut failed  "  << endreq;
      return NULL;
   }
   if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Mass " << Mass << " : cut passed, create TrigEFBphys  "  << endreq;

   TrigEFBphys* TrigPart = new TrigEFBphys( 0, 0., 0., TrigEFBphys::MULTIMU, Mass); //should really be type MULTIMU - needs changes to TrigPartcile


   // do vertex fit
   Trk::VxCandidate * myVxCandidate ( 0 );
   std::vector<const Trk::Track*> pair;
    Amg::Vector3D vtx(0.,0.,0.);
   Trk::Vertex vertex ( vtx );

   pair.push_back(muon1);
   pair.push_back(muon2);
   myVxCandidate = m_fitterSvc->fit(pair,vertex);

   if ( myVxCandidate ) {
	    TrigPart->fitchi2(myVxCandidate->recVertex().fitQuality().chiSquared());
            TrigPart->fitx(myVxCandidate->recVertex().position() [0]);
            TrigPart->fity(myVxCandidate->recVertex().position() [1]);
            TrigPart->fitz(myVxCandidate->recVertex().position() [2]);

            std::vector<int> trkIndices;                  
	    double fitMass=0, fitMassError=0;
	    for (int i=0;i<(int)pair.size();++i) {trkIndices.push_back(1);} 
            if (!(m_VKVFitter->VKalGetMassError(trkIndices,fitMass,fitMassError).isSuccess())) {   
                  if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"Warning from VKaVrt - cannot calculate uncertainties!"<<endreq;                               
                  } else {
	      if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"Fit mass "<< fitMass << endreq;                               
	      TrigPart->fitmass( (float) fitMass );
              mon_FitMass.push_back(fitMass/1000.);
              mon_Chi2.push_back(myVxCandidate->recVertex().fitQuality().chiSquared());
	    }
	    delete myVxCandidate;
	    myVxCandidate=0;
   }

   return TrigPart;
}


TrigEFBphys* TrigEFMultiMuFex::checkInvMass3(const Trk::Track* muon1, const Trk::Track* muon2, const Trk::Track* muon3, double Mass)
{
  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " In checkInvMass3, cuts " << m_lowerMassCut << " " << m_upperMassCut <<endreq;

   if ( Mass < m_lowerMassCut || Mass > m_upperMassCut  ) {
     if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Mass : " << Mass << " cut failed  "  << endreq;
      return NULL;
   }
   if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Mass " << Mass << " : cut passed, create TrigEFBphys  "  << endreq;

   TrigEFBphys* TrigPart = new TrigEFBphys( 0, 0., 0., TrigEFBphys::MULTIMU, Mass); //should really be type MULTIMU - needs changes to TrigPartcile


   // do vertex fit
   Trk::VxCandidate * myVxCandidate ( 0 );
   std::vector<const Trk::Track*> pair;
    Amg::Vector3D vtx(0.,0.,0.);
    Trk::Vertex vertex ( vtx );

   pair.push_back(muon1);
   pair.push_back(muon2);
   pair.push_back(muon3);
   myVxCandidate = m_fitterSvc->fit(pair,vertex);

   if ( myVxCandidate ) {
	    TrigPart->fitchi2(myVxCandidate->recVertex().fitQuality().chiSquared());
            TrigPart->fitx(myVxCandidate->recVertex().position() [0]);
            TrigPart->fity(myVxCandidate->recVertex().position() [1]);
            TrigPart->fitz(myVxCandidate->recVertex().position() [2]);

            std::vector<int> trkIndices;                  
	    double fitMass=0, fitMassError=0;
	    for (int i=0;i<(int)pair.size();++i) {trkIndices.push_back(1);} 
            if (!(m_VKVFitter->VKalGetMassError(trkIndices,fitMass,fitMassError).isSuccess())) {   
                  if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"Warning from VKaVrt - cannot calculate uncertainties!"<<endreq;                               
                  } else {
	      if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"Fit mass "<< fitMass << endreq;                               
	      TrigPart->fitmass( (float) fitMass );
              mon_FitMass.push_back(fitMass/1000.);
              mon_Chi2.push_back(myVxCandidate->recVertex().fitQuality().chiSquared());
	    }


	    delete myVxCandidate;
   }

   return TrigPart;
}



void TrigEFMultiMuFex::findTracks(HLT::TEConstVec& inputTE, const Trk::Track* muon1, const Trk::Track* muon2, TrigEFBphys* TrigPart)
{

     // JK look for tracks in TrackParticle containers and add to TrigEFBphys
     bool FoundTrack1=false;
     bool FoundTrack2=false;
	      
     for(unsigned int iroi=0; iroi < inputTE.size() && !(FoundTrack1 && FoundTrack2); iroi++) {
	 ///////////////// Get Track Particles from RoI /////////////////
         std::vector<const Rec::TrackParticleContainer *> vectorTrkParticleCont;

         HLT::ErrorCode status = getFeatures(inputTE[iroi], vectorTrkParticleCont);
         if(status != HLT::OK) {
	     if ( msgLvl() <= MSG::WARNING ) msg() << MSG::WARNING << "Failed to get TrackParticleContainer's from the trigger element" << iroi << endreq;
         } else {
	     if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " RoI " << iroi << " Got " << vectorTrkParticleCont.size()
	            << " TrackParticle Collections" << endreq;
         }  

         const Rec::TrackParticleContainer* TrkParticleCont = vectorTrkParticleCont.front();

         if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Now loop over TrackParticles to find tracks " << endreq;

         Rec::TrackParticleContainer::const_iterator trkIt =  TrkParticleCont->begin();
         Rec::TrackParticleContainer::const_iterator lastTrkIt = TrkParticleCont->end();
         if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Found tracks, ntrack= " << TrkParticleCont->size() << endreq;

         if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "trying to match: " << muon1 << "  "  << muon2 << endreq; 
               for (int itrk=0 ; trkIt != lastTrkIt && !(FoundTrack1 && FoundTrack2); itrk++, trkIt++) {
	  	  const Trk::Track* track = (*trkIt)->originalTrack();
                  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "original track " << track << endreq; 

                  if (track == muon1 && !FoundTrack1) {
                   ElementLink<Rec::TrackParticleContainer> trackEL(*TrkParticleCont,itrk);
                   TrigPart->addTrack(trackEL);
                     if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG	<< "track matches track 1, will store in EFBPhys "<< endreq;
		   FoundTrack1=true;
		}
                if (track == muon2 && !FoundTrack2) {
                   ElementLink<Rec::TrackParticleContainer> trackEL(*TrkParticleCont,itrk);
                   TrigPart->addTrack(trackEL);
                     if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG	<< "track matches track 2, will store in EFBPhys "<< endreq;
		   FoundTrack2=true;
		}
		

	       } // end loop over track partciles      
     } // end loop over RoIs


  return;
}


void TrigEFMultiMuFex::findTracks(HLT::TEConstVec& inputTE, const Trk::Track* muon1, const Trk::Track* muon2, const Trk::Track* muon3, TrigEFBphys* TrigPart)
{

     // JK look for tracks in TrackParticle containers and add to TrigEFBphys
     bool FoundTrack1=false;
     bool FoundTrack2=false;
     bool FoundTrack3=false;
	      
     for(unsigned int iroi=0; iroi < inputTE.size() && !(FoundTrack1 && FoundTrack2 && FoundTrack3); iroi++) {
	 ///////////////// Get Track Particles from RoI /////////////////
         std::vector<const Rec::TrackParticleContainer *> vectorTrkParticleCont;

         HLT::ErrorCode status = getFeatures(inputTE[iroi], vectorTrkParticleCont);
         if(status != HLT::OK) {
	     if ( msgLvl() <= MSG::WARNING ) msg() << MSG::WARNING << "Failed to get TrackParticleContainer's from the trigger element" << iroi << endreq;
         } else {
	     if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " RoI " << iroi << " Got " << vectorTrkParticleCont.size()
	            << " TrackParticle Collections" << endreq;
         }  

         const Rec::TrackParticleContainer* TrkParticleCont = vectorTrkParticleCont.front();

         if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Now loop over TrackParticles to find tracks " << endreq;

         Rec::TrackParticleContainer::const_iterator trkIt =  TrkParticleCont->begin();
         Rec::TrackParticleContainer::const_iterator lastTrkIt = TrkParticleCont->end();
         if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Found tracks, ntrack= " << TrkParticleCont->size() << endreq;

         if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "trying to match: " << muon1 << "  "  << muon2 << "   " << muon3 << endreq; 
               for (int itrk=0 ; trkIt != lastTrkIt && !(FoundTrack1 && FoundTrack2); itrk++, trkIt++) {
	  	  const Trk::Track* track = (*trkIt)->originalTrack();
                  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "original track " << track << endreq; 

                  if (track == muon1 && !FoundTrack1) {
                   ElementLink<Rec::TrackParticleContainer> trackEL(*TrkParticleCont,itrk);
                   TrigPart->addTrack(trackEL);
                     if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG	<< "track matches track 1, will store in EFBPhys "<< endreq;
		   FoundTrack1=true;
		}
                if (track == muon2 && !FoundTrack2) {
                   ElementLink<Rec::TrackParticleContainer> trackEL(*TrkParticleCont,itrk);
                   TrigPart->addTrack(trackEL);
                     if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG	<< "track matches track 2, will store in EFBPhys "<< endreq;
		   FoundTrack2=true;
		}
                  if (track == muon3 && !FoundTrack3) {
                   ElementLink<Rec::TrackParticleContainer> trackEL(*TrkParticleCont,itrk);
                   TrigPart->addTrack(trackEL);
                     if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG	<< "track matches track 3, will store in EFBPhys "<< endreq;
		   FoundTrack3=true;
		}
		

	       } // end loop over track partciles      
     } // end loop over RoIs


  return;
}


double TrigEFMultiMuFex::invariantMass(const Trk::Track* muon1, const Trk::Track* muon2) {
    /* JW EDM
  double pT1 = muon1->perigeeParameters()->pT();
  double pT2 = muon2->perigeeParameters()->pT();

  double Px1   = pT1*muon1->perigeeParameters()->cosPhi();  
  double Py1   = pT1*muon1->perigeeParameters()->sinPhi();
  double Pz1   = pT1*muon1->perigeeParameters()->cotTheta();
  
  double Px2   = pT2*muon2->perigeeParameters()->cosPhi();
  double Py2   = pT2*muon2->perigeeParameters()->sinPhi();
  double Pz2   = pT2*muon2->perigeeParameters()->cotTheta();
  */
    const Trk::Perigee* per1 = muon1->perigeeParameters();
    const Trk::Perigee* per2 = muon2->perigeeParameters();
    
    double pT1   = per1->pT();
    double pT2   = per2->pT();
    
    double Px1   = per1->momentum()[Trk::px];
    double Py1   = per1->momentum()[Trk::py];
    double Pz1   = per1->momentum()[Trk::pz];
    
    double Px2   = per2->momentum()[Trk::px];
    double Py2   = per2->momentum()[Trk::py];
    double Pz2   = per2->momentum()[Trk::pz];

    
  double m2   = 11151.36;
  
  double E = sqrt( pT1*pT1 + Pz1*Pz1 + m2) + sqrt( pT2*pT2 + Pz2*Pz2 + m2);
  double pX = Px1 + Px2;
  double pY = Py1 + Py2;
  double pZ = Pz1 + Pz2;
  
  double mass =  E*E - pX*pX - pY*pY - pZ*pZ ; 
  mass = mass > 0 ? sqrt(mass) : 0;  

  return mass;
}
 
void TrigEFMultiMuFex::addUnique(std::vector<const Trk::Track*>& tracks, const Trk::Track* trkIn)
{
//  std::cout<<" in addUnique : trkIn pT= "<<trkIn->perigeeParameters()->pT()<<std::endl;
  std::vector<const Trk::Track*>::iterator tItr;
  for( tItr = tracks.begin(); tItr != tracks.end(); tItr++)
    {
      double dPhi=fabs((*tItr)->perigeeParameters()->parameters()[Trk::phi] -
		       trkIn->perigeeParameters()->parameters()[Trk::phi]);
      if (dPhi > M_PI) dPhi = 2.*M_PI - dPhi;
      // may need to tweak these cuts JK
      if( fabs(dPhi) < 0.005 &&
	  (fabs((*tItr)->perigeeParameters()->eta() - trkIn->perigeeParameters()->eta()) < 0.005) &&
	  ((*tItr)->perigeeParameters()->pT() - trkIn->perigeeParameters()->pT() < 10.) ) 
	{ //std::cout<<" TrigEFMultiMuFex addUnique: the SAME tracks! pT= "<<
	    //trkIn->perigeeParameters()->pT()<<" and "<<
	    //(*tItr)->perigeeParameters()->pT()<<std::endl;
      return;
	}
    } 
  tracks.push_back(trkIn);       
}


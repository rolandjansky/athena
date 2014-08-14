/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     TrigL2JpsieeFex.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBphysHypo
// author :  Julie Kirk, Ricardo Neves
// ********************************************************************

#include <math.h> 
//#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"

#include "StoreGate/StoreGateSvc.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TrigBphysHypo/TrigL2JpsieeFex.h"
#include "TrigBphysHypo/BtrigUtils.h"
#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"

#include "TrigInDetToolInterfaces/ITrigInDetTrackExtrapolator.h"

#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigInDetTrackHelper.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"
#include "TrigVertexFitter/ITrigVertexingTool.h"

// additions of xAOD objects
#include "xAODEventInfo/EventInfo.h"


class ISvcLocator;

using namespace std;

/*-------------------------------------------*/
TrigL2JpsieeFex::TrigL2JpsieeFex(const std::string & name, ISvcLocator* pSvcLocator):
HLT::FexAlgo(name, pSvcLocator),   
m_vertFitter("TrigVertexFitter",this),
m_L2vertFitter("TrigL2VertexFitter",this),
m_vertexingTool("TrigVertexingTool",this)
/*-------------------------------------------*/
{
   
   // Read cuts 
   declareProperty( "AcceptAll", m_AcceptAll  = false );
   
   // Track cuts
   declareProperty("TrackPTthr", m_TrackPTthr=1000.);
   //   declareProperty("TrackAlgoId", m_TrackAlgoId=2); no longer needed - use different chains for IdScan/SiTrack
   
   // Track Pair cuts  
   declareProperty("dEtaTrackPair", m_dEtaTrackPair=1.);
   declareProperty("dPhiTrackPair", m_dPhiTrackPair=1.);
   declareProperty("dz0TrackPair", m_dz0TrackPair=2.);
   declareProperty("pTsumTrackPair", m_pTsumTrackPair=3000.);
   
   // Jpsi Mass cuts
   declareProperty("JpsiMasslow", m_JpsiMasslow=1500.);
   declareProperty("JpsiMasshigh", m_JpsiMasshigh=3800.);
   
   // Cluster-RoI match cuts
   declareProperty( "dEtaClusRoI", m_dEtaClusRoI = 0.2 );
   declareProperty( "dPhiClusRoI", m_dPhiClusRoI = 0.1 );
   
   // Extrapolator params
   declareProperty( "RCalBarrelFace",       m_RCAL = 1470.0*CLHEP::mm );
   declareProperty( "ZCalEndcapFace",       m_ZCAL = 3800.0*CLHEP::mm );
   declareProperty( "TrackExtrapolatorName", m_trackExtrapolatorName = "TrigInDetTrackExtrapolator");
   
   // Calo-Id cuts  
   declareProperty( "CaloTrackdEoverPLow",  m_calotrackdeoverp_low = 0.5);
   declareProperty( "CaloTrackdEoverPHigh", m_calotrackdeoverp_high = 3.5);
   declareProperty( "CaloTrackdETA",        m_calotrackdeta = 0.4);
   declareProperty( "CaloTrackdPHI",        m_calotrackdphi = 0.4 );    
   declareProperty( "TRTcut", m_TRTCut = 0.1 );
   
   // Vertexing
   declareProperty("doVertexFit", m_doVertexFit=true);
   declareProperty("TrigVertexFitter", m_vertFitter);
   declareProperty("TrigL2VertexFitter", m_L2vertFitter);
   declareProperty("TrigVertexingTool", m_vertexingTool, "TrigVertexingTool");
   
   // variables for monitoring histograms
   declareMonitoredVariable(    "Ntrack" ,     mon_Ntrack);
   declareMonitoredStdContainer("trackPt" ,    mon_trackPt         , AutoClear);
   declareMonitoredStdContainer("dEta",        mon_dEta            , AutoClear);
   declareMonitoredStdContainer("dPhi",        mon_dPhi            , AutoClear);
   declareMonitoredStdContainer("dz0",         mon_dz0             , AutoClear);
   declareMonitoredStdContainer("Ptsum",       mon_Ptsum           , AutoClear);
   declareMonitoredStdContainer("JpsiMass",    mon_JpsiMass        , AutoClear);
   declareMonitoredStdContainer("JpsiFitMass", mon_JpsiFitMass     , AutoClear);
   
   declareMonitoredStdContainer("RoIEta",      mon_vRoIEta         , AutoClear);
   declareMonitoredStdContainer("RoIPhi",      mon_vRoIPhi         , AutoClear);
   declareMonitoredStdContainer("calo_dEta",   mon_vcalo_dEta      , AutoClear);
   declareMonitoredStdContainer("calo_dPhi",   mon_vcalo_dPhi      , AutoClear);
   declareMonitoredStdContainer("TRTRatio",    mon_vTRTRatio       , AutoClear);
   declareMonitoredStdContainer("etoverpt",    mon_vetoverpt       , AutoClear);
   declareMonitoredStdContainer("dEtaCalo",    mon_vdEtaCalo       , AutoClear);
   declareMonitoredStdContainer("dPhiCalo",    mon_vdPhiCalo       , AutoClear);
   declareMonitoredStdContainer("Eta",         mon_Eta             , AutoClear);
   declareMonitoredStdContainer("Phi",         mon_Phi             , AutoClear);
   
   // zero counters
   m_lastEvent = 999;
   m_lastEventPassed = 999;
   m_countTotalEvents=0;
   m_countTotalRoI=0;
   m_countPassedEvents=0;
   m_countPassedRoIs=0;
   m_countPassedTrackPair=0;
   m_countPassedJpsiMass=0;
   
   m_trigBphysColl=0;
   m_VertexColl=0;
   
}

/*-------------------------------------------*/
TrigL2JpsieeFex::~TrigL2JpsieeFex()
/*-------------------------------------------*/
{
   delete m_trigBphysColl;
   delete m_VertexColl;
}

/*-------------------------------------------*/
HLT::ErrorCode TrigL2JpsieeFex::hltInitialize()
/*-------------------------------------------*/
{
   StatusCode sc = toolSvc()->retrieveTool(m_trackExtrapolatorName,
   m_trackExtrapolatorName,
   m_trackExtrapolator);
   
   if ( sc.isFailure() ) {
      msg() << MSG::FATAL << "Unable to locate TrackExtrapolator tool " << m_trackExtrapolatorName  << endreq;
      return HLT::BAD_JOB_SETUP;
   }
   
   if ( msgLvl() <= MSG::INFO ) {
      msg() << MSG::INFO << "===============TrigL2JpsieeFex init values===============" << endreq;
      
      msg() << MSG::INFO << "AcceptAll            = "
      << (m_AcceptAll==true ? "True" : "False") << endreq;
      //msg() << MSG::INFO
	//      << "Using track algo " << m_TrackAlgoId
	// << endreq;
      msg() << MSG::INFO 
      << "Track pT cut " << m_TrackPTthr
      << endreq;
      msg() << MSG::INFO 
      << "Track pair cuts: dEta<" << m_dEtaTrackPair << " dPhi<" << m_dPhiTrackPair 
      << " dz0<" << m_dz0TrackPair << " pTsum>" << m_pTsumTrackPair  
      << endreq;
      msg() << MSG::INFO 
      << "Jpsi mass cuts: " << m_JpsiMasslow << " < Mass(phi) < " << m_JpsiMasshigh 
      << endreq;
      msg() << MSG::INFO 
      << "Cluster-RoI matching cuts: deta=" << m_dEtaClusRoI << ", dphi=" << m_dPhiClusRoI 
      << endreq;
      msg() << MSG::INFO 
      << "Track ET/pT cuts: " << m_calotrackdeoverp_low << " < ET/pT < " <<  m_calotrackdeoverp_high
      <<endreq;
      msg() << MSG::INFO 
      << "Calo-ID cuts: deta=" << m_calotrackdeta << ", dphi=" <<  m_calotrackdphi
      <<endreq;
      msg() << MSG::INFO 
      << "TRT cut:" << m_TRTCut
      <<endreq;  
      msg() << MSG::INFO 
      << "Initialization completed successfully" 
      << endreq;
   }
   // retrieving the vertex fitting tool
   sc = m_vertFitter.retrieve();
   if ( sc.isFailure() ) {
      msg() << MSG::FATAL << "Unable to locate TrigVertexFitter tool " << endreq;
      return HLT::BAD_JOB_SETUP;
   } else {
      msg() << MSG::INFO << "TrigVertexFitter retrieved"<< endreq;
   }
   
   //  sc = toolSvc()->retrieveTool("TrigL2VertexFitter","TrigL2VertexFitter",m_L2vertFitter);
   sc = m_L2vertFitter.retrieve();
   if ( sc.isFailure() ) {
      msg() << MSG::FATAL << "Unable to locate TrigL2VertexFitter tool " << endreq;
      return HLT::BAD_JOB_SETUP;
   } 
   else {
      msg() << MSG::INFO << "TrigL2VertexFitter retrieved"<< endreq;
   }
   
   //  sc = toolSvc()->retrieveTool("TrigVertexingTool","TrigVertexingTool",m_vertexingTool);
   sc = m_vertexingTool.retrieve();
   if ( sc.isFailure() ) {
      msg() << MSG::FATAL << "Unable to locate TrigVertexingTool tool " << endreq;
      return HLT::BAD_JOB_SETUP;
   } else {
      if ( msgLvl() <= MSG::INFO ) msg() << MSG::INFO << "TrigVertexingTool retrieved"<< endreq;
   }
   
   return HLT::OK;
}


/*-------------------------------------------*/
HLT::ErrorCode TrigL2JpsieeFex::hltFinalize()
/*-------------------------------------------*/
{
   
   msg() << MSG::INFO << "in finalize()" << endreq;
   msg() << MSG::INFO << "Run on events/RoIs " << m_countTotalEvents << "/" << m_countTotalRoI <<  endreq;
   msg() << MSG::INFO << "Passed events/RoIs " << m_countPassedEvents << "/" << m_countPassedRoIs <<  endreq;
   msg() << MSG::INFO << "RoIs Passed track pair: "  << m_countPassedTrackPair << endreq;
   msg() << MSG::INFO << "RoIs Passed JpsiMass: "  << m_countPassedJpsiMass << endreq;
   
   return HLT::OK;
}

/*--------------------------------------------------------------*/
HLT::ErrorCode TrigL2JpsieeFex::acceptInput(const HLT::TriggerElement* inputTE, bool& pass)
/*--------------------------------------------------------------*/
{
   bool PassedTrackPair=false;
   bool PassedJpsiMass=false;
   bool result = false;
   //   const double EMASS = 0.511003;
   //   const double JPSIMASS = 3096.;
   
   HLT::ErrorCode stat = HLT::OK;
   
   mon_Ntrack=-99;   
   
   if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " In Jpsiee FEX acceptInput" << endreq;
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
            if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Run " << IdRun << " Event " << IdEvent << endreq;
        }// found old event info
    }else { // found the xAOD event info
        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Run " << evtInfo->runNumber()
            << " Event " << evtInfo->eventNumber() << endreq;
        IdRun   = evtInfo->runNumber();
        IdEvent = evtInfo->eventNumber();
    } // get event ifo
   
   
   
   // Accept-All mode: temporary patch; should be done with force-accept 
   if (m_AcceptAll) {
      if ( msgLvl() <= MSG::DEBUG )
	 msg() << MSG::DEBUG << "AcceptAll property is set: taking all events" 
      << endreq;
      
      pass = true;
   } 
   else {
      if ( msgLvl() <= MSG::DEBUG )
	 msg() << MSG::DEBUG << "AcceptAll property not set: applying selection" 
      << endreq;
      pass=false;
   }
   
   // get RoI descriptor
   const TrigRoiDescriptor* roiDescriptor = 0;
   if (getFeature(inputTE, roiDescriptor, "initialRoI") != HLT::OK) roiDescriptor = 0;
   
   if ( !roiDescriptor ) {
      if ( msgLvl() <= MSG::WARNING) {
	 msg() <<  MSG::WARNING << "No RoI for this Trigger Element! " << endreq;
      }    
      
      return HLT::NAV_ERROR;
   }
   
   // fill local variables for RoI reference position   
   double etaRef = roiDescriptor->eta();
   double phiRef = roiDescriptor->phi();
   mon_vRoIEta.push_back(etaRef);
   mon_vRoIPhi.push_back(phiRef);
   
   if ( msgLvl() <= MSG::DEBUG ){
      msg() << MSG::DEBUG 
      << "Using inputTE("<< inputTE <<")->getId(): " << inputTE->getId()
      << "; RoI ID = "   << roiDescriptor->roiId()
      << ": Eta = "      << etaRef
      << ", Phi = "      << phiRef
      << endreq;
   } 
   
   
   // retrieve TrigEMCluster using the Navigation methods
   const TrigEMCluster* t2calo_clus;
   stat = getFeature(inputTE, t2calo_clus);
   
   if ( stat != HLT::OK || !t2calo_clus ) {
      if ( msgLvl() <= MSG::WARNING)
	 msg() << MSG::WARNING << "Failed to get TrigEMCluster" << endreq;
      return HLT::MISSING_FEATURE;
   }
   // copy relevant quantities to local variables
   float calo_eta, calo_phi, calo_et;
   calo_eta = t2calo_clus->eta();
   calo_phi = t2calo_clus->phi();
   calo_et  = t2calo_clus->et();
   
   // cluster found: print debug information
   if ( msgLvl() <= MSG::DEBUG ) (t2calo_clus)->print(msg());
   
   
   // calculate eta and phi distances (deal with angle diferences > Pi)
   float dPhi =  calo_phi - phiRef;
   dPhi = ( fabs(dPhi) < M_PI ? dPhi : 2*M_PI - dPhi );
   
   float dEta = fabs( etaRef - calo_eta );
   
   mon_vcalo_dEta.push_back(dEta);
   mon_vcalo_dPhi.push_back(dPhi);
   
   // figure out what eta bin this cluster is in
   
   //   float absEta = fabs(calo_eta);
   //
   //   int etaBin = 0;
   //   for (unsigned int iBin = 0; iBin < m_etabin.size(); iBin++ )
   //     if ( absEta > m_etabin[iBin] && absEta < m_etabin[iBin+1] ) etaBin = iBin;
   // 
   //   if ( msgLvl() <= MSG::DEBUG )
   //     msg() << MSG::DEBUG << "eta bin used for cuts: " << etaBin << endreq;
   // 
   // Accept only L2 em clusters which are "close" to the one provided by L1
   
   
   if ( dEta > m_dEtaClusRoI || dPhi > m_dPhiClusRoI ) {
      if ( msgLvl() <= MSG::DEBUG ) {
	 msg() << MSG::DEBUG << "Cluster doesn't match RoI! Leaving." << endreq;
      }
      return HLT::OK;
   }
   
   //  create vector for TrigL2Bphys particles
   delete m_trigBphysColl;
   delete m_VertexColl;
   m_trigBphysColl = new TrigL2BphysContainer();
   m_VertexColl = new TrigVertexCollection();
   
   //  Get vector of tracks 
   
   std::vector<const TrigInDetTrackCollection*> vectorOfTrackCollections;
   HLT::ErrorCode status = getFeatures(inputTE, vectorOfTrackCollections);
   if(status != HLT::OK ) {
      if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Failed to get InDetTrackCollections " << endreq;
   } else {
      if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Got " << vectorOfTrackCollections.size()
      << " InDetTrackCollections" << endreq;
   }
   std::vector<const TrigInDetTrackCollection*>::iterator pTrackColl =  vectorOfTrackCollections.begin();
   std::vector<const TrigInDetTrackCollection*>::iterator lastTrackColl = vectorOfTrackCollections.end();
   if (IdEvent != (int) m_lastEvent) {
      m_countTotalEvents++;
      m_lastEvent=IdEvent;
   }
   m_countTotalRoI++;
   
   
   for ( ; pTrackColl != lastTrackColl;  pTrackColl++) {         // loopover collections
      
      TrigInDetTrackCollection::const_iterator track= (*pTrackColl)->begin();
      TrigInDetTrackCollection::const_iterator lastTrack= (*pTrackColl)->end();
      
      if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG
      << "Found tracks, ntrack= "
      << (*pTrackColl)->size()
      << endreq;
      mon_Ntrack=(*pTrackColl)->size();
      
      std::vector<bool> GoodTrack((*pTrackColl)->size(), false);	// cluster tracks
      std::vector<bool> GoodTRTTrack((*pTrackColl)->size(),false);    // TRT tracks
      int nGoodTrack=0;
      int nGoodTRTTrack=0;
      float etoverpt;
      
      // check for good tracks
      mon_trackPt.reserve(mon_Ntrack);
      for (int itrk=0; track !=lastTrack; ++itrk, track++) {     // loop over tracks
	 GoodTrack[itrk]=false;
	 GoodTRTTrack[itrk]=false;
	 
	 unsigned int algoId = (*track)->algorithmId();
	 float trackPt =  fabs( (*track)->param()->pT() );
     float trackChi2 = (*track)->chi2();
	 
     mon_trackPt.push_back(trackPt/CLHEP::GeV);
     mon_Eta.push_back( (*track)->param()->eta() );
     mon_Phi.push_back( (*track)->param()->phi0() );	 
	 
	 if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "track " << itrk << " pT " << trackPt << " chi2 " << trackChi2 << endreq;
	 if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "track pT vector " << mon_trackPt << endreq;
	 
	 // pT && Algo cut
	 // JK no longer needto checkalgoid	 if( algoId == m_TrackAlgoId  && trackPt > m_TrackPTthr ) {
	 if( trackPt > m_TrackPTthr && trackChi2 < 1e7) {
	    if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE <<"Passed Algo & Pt" <<endreq;
	    
	    // check TRT track
	    float TRTRatio = 0;
	    if ((*track)->NStrawHits() > 0)
	       TRTRatio = (float) (*track)->NTRHits() / (*track)->NStrawHits();
	    mon_vTRTRatio.push_back(TRTRatio);
	    if(TRTRatio > m_TRTCut) {
	       GoodTRTTrack[itrk]=true;
	       nGoodTRTTrack++;
	       if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG <<"Good TRT track, track: "<< itrk <<endreq;
	    }	  

	    
	    // extrapolate track using tool
	    double phiAtCalo = 0, etaAtCalo = 0;
	    StatusCode extsc = m_trackExtrapolator->extrapolateToCalo((*track),
	    m_RCAL, m_ZCAL,
	    phiAtCalo, etaAtCalo);
	    
	    // if extrapolation failed for some reason, don't do calo-track matching cuts
	    // and instead reject electron already here (jump to next electron)
	    if ( extsc.isFailure() ) {
	       if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "extrapolator failed "<<endreq;
	       // jump to next electron
	       continue;
	    } else {
	       // all ok: do track-matching cuts
	       if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "extrapolated eta/phi=" << etaAtCalo
		  << "/" << phiAtCalo << endreq;
	       
	       // match in eta
	       float dEtaCalo = fabs(etaAtCalo - calo_eta);
	       mon_vdEtaCalo.push_back(dEtaCalo);
	       if ( dEtaCalo < m_calotrackdeta ) {
		  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "passed eta match cut" << endreq;
		  
		  // match in phi: deal with differences larger than Pi
		  float dPhiCalo =  fabs(phiAtCalo - calo_phi);
		  dPhiCalo = ( dPhiCalo < M_PI ? dPhiCalo : 2*M_PI - dPhiCalo );
		  mon_vdPhiCalo.push_back(dPhiCalo);
		  if ( dPhiCalo < m_calotrackdphi ) {
		     	    // ET/pT cut
		     etoverpt = fabs(calo_et/trackPt);
		     mon_vetoverpt.push_back(etoverpt);
		     if ( etoverpt >= m_calotrackdeoverp_low && etoverpt <= m_calotrackdeoverp_high ) {
			if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Passed ET/PT cuts" << endreq;
			if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "Now extrapolating to Calo" << endreq;
			
			if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "passed phi match cut" << endreq;
			// all cuts passed
			GoodTrack[itrk]=true;
			nGoodTrack++;
			if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Good Cluster track, track: " << itrk << " pT = "  <<  (*track)->param()->pT()  << endreq;
		     }
		  }
	       }
	    }				
	    
	 } else {
	    if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG
	    << "Bad track, track: " << itrk << " algo " << algoId << " pT = " 
	    <<  (*track)->param()->pT() 
	    << endreq;
	    
	 } // if good algo && pT && good et/pt && good matching
      } // end loop over tracks
      
      
      if (nGoodTrack<1 && nGoodTRTTrack<1) continue;    // Only one Calo and TRT track each needed
      
      if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Good Cluster Track=" << nGoodTrack << " Good TRT Track=" << nGoodTRTTrack << endreq;
      
      // loop over tracks and form combinations
      TrigInDetTrackCollection::const_iterator track1= (*pTrackColl)->begin();
      TrigInDetTrackCollection::const_iterator lastTrack1= (*pTrackColl)->end();
      for (int itrk=0; track1 !=lastTrack1; ++itrk, track1++) {
	 TrigInDetTrackCollection::const_iterator track2= (*pTrackColl)->begin();
	 TrigInDetTrackCollection::const_iterator lastTrack2= (*pTrackColl)->end();
	 for (int jtrk=0; track2 !=lastTrack2; ++jtrk, track2++) {
	    if (jtrk<=itrk) continue;   // Don't count same pair twice nor the same track with itself --R
	    if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE
	    << "Check tracks "
		  << itrk << " goodtrack = " << GoodTrack[itrk] << " and " << jtrk
		  << " goodtrttrack = " << GoodTRTTrack[jtrk] <<endreq;
	    if (!((GoodTrack[itrk] && GoodTRTTrack[jtrk]) ||(GoodTrack[jtrk] && GoodTRTTrack[itrk]) ) ) continue;   //match a Cluster track with a TRT track --R

	    if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE
	    << "Good tracks "
	    << itrk << " and " << jtrk << " now check sign " 
	    << endreq;
	    if (((*track1)->param()->pT()>0. && (*track2)->param()->pT()>0.) || 
	       ((*track1)->param()->pT()<0. && (*track2)->param()->pT()<0.))  continue;
	    
	    if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE
	    << "Tracks " << itrk << " and " << jtrk << " are opposite sign, make track pair cuts "
	    << endreq;
	    if (! PassTrackPair( (*track1)->param(), (*track2)->param() )) continue;
	    if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG
	    << " Pass track pair cuts, now form Jpsi mass " 
	    << endreq;
	    PassedTrackPair=true;
	    std::vector<const TrigInDetTrackFitPar*> inputtrks;
	    std::vector<double> massHypo ;
	    inputtrks.push_back((*track1)->param());
	    inputtrks.push_back((*track2)->param());
	    massHypo.push_back( EMASS );
	    massHypo.push_back( EMASS );
	    
	    double JpsiMass=InvMass(inputtrks, massHypo);
	    mon_JpsiMass.push_back((JpsiMass)*0.001);
	    if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG
	    << " Jpsi_mass =  " << JpsiMass 
	    << endreq;
	    if (JpsiMass> m_JpsiMasslow && JpsiMass<m_JpsiMasshigh) {
	       PassedJpsiMass=true;
	       result=true;
	       TrigL2Bphys* trigPartJpsi = new TrigL2Bphys(roiDescriptor->roiId(), roiDescriptor->eta(), roiDescriptor->phi(), TrigL2Bphys::JPSIEE, JpsiMass);
	       if ( msgLvl() <= MSG::DEBUG) msg()  << MSG::DEBUG << "Create Bphys particle with roIId " << trigPartJpsi->roiId()  
	       << " phi, eta " << trigPartJpsi->phi() << " " << trigPartJpsi->eta() <<
	       " vertex type " << trigPartJpsi->particleType() << endreq;
	       
	       
	       //14-5-08 add tracks to L2bphys rather than vertex
	       ElementLink<TrigInDetTrackCollection> track1EL(*(*pTrackColl),itrk);
	       ElementLink<TrigInDetTrackCollection> track2EL(*(*pTrackColl),jtrk);
	       trigPartJpsi->addTrack(track1EL);
	       trigPartJpsi->addTrack(track2EL);
	       
	       // new vertexing
	       TrigL2Vertex* pL2V=new TrigL2Vertex();
	       if (m_doVertexFit) 
	       {
		  // vertex fitting
		  TrigVertex* p_JpsiV=NULL;
		  
		  StatusCode sc = m_vertexingTool->addTrack((*track1),pL2V,Trk::electron);
		  if(sc.isFailure())
		  {
		     if ( msgLvl() <= MSG::DEBUG) msg() <<MSG::DEBUG <<"Failed to add track 1 to vertexingTool"<<endreq;
		  }
		  sc = m_vertexingTool->addTrack((*track2),pL2V,Trk::electron);
		  if(sc.isFailure())
		  {
		     if ( msgLvl() <= MSG::DEBUG) msg() <<MSG::DEBUG <<"Failed to add track 2 to vertexingTool"<<endreq;
		  }
		  //bool Jpsi_created=false;
		  sc=m_L2vertFitter->fit(pL2V);
		  
		  
		  if(sc.isFailure()) 
		  {
		     if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG <<"TrigL2VertexFitter failed"<<endreq;
		  } 
		  else 
		  {			    
		     sc=m_vertexingTool->calculateInvariantMass(pL2V);
		     if(sc.isSuccess()) 
		     {
			sc=m_vertexingTool->createMotherParticle(pL2V);
			if(sc.isSuccess()) 
			{
			   //Jpsi_created=true;
			   if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Jpsi created: pT "<<pL2V->m_getMotherTrack()->pT() << endreq;
			   //  Jpsi_track->param(pL2V->m_getMotherTrack());
			}
			p_JpsiV=m_vertexingTool->createTrigVertex(pL2V);
		     }
		  }
		  if(p_JpsiV==NULL) 
		  {
		     if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG
		     << " vertex fit failed for tracks  " << itrk << " and " << jtrk  
		     << endreq;
		     delete pL2V;
		     delete p_JpsiV;
		     delete trigPartJpsi;
		     continue;
		  } 
		  else 
		  {
		     if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "Jpsi vertex Fit: x= y= z=" << p_JpsiV->x() << " " << p_JpsiV->y() 
		     << " " << p_JpsiV->z() << endreq;
		     if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Jpsi vertex fit mass = " << p_JpsiV->mass() << endreq;
		     if (p_JpsiV->massVariance() !=0) {  
			if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "Jpsi mass pull = " 
			<< (p_JpsiV->mass()-JPSIMASS)/p_JpsiV->massVariance() << endreq;
		     } else {
			if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "Jpsi mass variance is 0, can't calculate the pull" << endreq;
		     }
		     mon_JpsiFitMass.push_back((p_JpsiV->mass())*0.001);
		     double chi2prob=1.0-Genfun::CumulativeChiSquare(p_JpsiV->ndof())(p_JpsiV->chi2());
		     if ( msgLvl() <= MSG::VERBOSE) {
		       msg() << MSG::VERBOSE << "Chi2 probability of the fit = " << chi2prob << endreq;
		       msg() << MSG::VERBOSE << "SigmaX =  SigmaY =  SigmaZ = " << 
			 sqrt(p_JpsiV->cov()[0]) << " " << 
			 sqrt(p_JpsiV->cov()[2]) << " " << 
			 sqrt(p_JpsiV->cov()[5]) << endreq;
		     }
		     m_VertexColl->push_back(p_JpsiV);
		     if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "added vertex to vertex collection" << endreq;
		     
		     // add vertex results to L2Bphys
		     trigPartJpsi->fitmass(p_JpsiV->mass());
		     trigPartJpsi->fitchi2(p_JpsiV->chi2());
		     trigPartJpsi->fitndof(p_JpsiV->ndof());
		     trigPartJpsi->fitx(p_JpsiV->x());
		     trigPartJpsi->fity(p_JpsiV->y());
		     trigPartJpsi->fitz(p_JpsiV->z());
		     if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "added vertex results to Jpsi particle" << endreq;
		  }
		  
	       } // end if doVertexFit
	       m_trigBphysColl->push_back(trigPartJpsi);
	       if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "added Jpsi to bphys collection" << endreq;
	       delete pL2V;
	    } // if pass Jpsi mass
	 } // track 2 loop
      } // track1 loop
   } //end of track collection loop
   if (PassedTrackPair) m_countPassedTrackPair++;
   if (PassedJpsiMass) m_countPassedJpsiMass++;
   if (result) {
      m_countPassedRoIs++;
      if (IdEvent!= (int) m_lastEventPassed) {
	 m_countPassedEvents++;
	 m_lastEventPassed=IdEvent;
      }
   }              
   
   if (m_VertexColl!=0) {
      if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " vertex collection size : " << m_VertexColl->size() << " m_VertexColl " << m_VertexColl << endreq;
   }
   if (m_trigBphysColl != 0) {
     if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " bphys collection size : " << m_trigBphysColl->size() << " m_trigBphysColl " << m_trigBphysColl << endreq;
   }
   
   // Reach this point successfully
   if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Run: " << IdRun << " Event: " << IdEvent << "  result is " << result << endreq;
   
   //  result=true;
   
   if (!m_AcceptAll) pass = result; 
   
   return HLT::OK;
}


HLT::ErrorCode TrigL2JpsieeFex::hltExecute(const HLT::TriggerElement*, HLT::TriggerElement* outputTE)
{
   if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " In Jpsiee FEX hltExecute" << endreq;
   
   // get RoI descriptor
   const TrigRoiDescriptor* roiDescriptor = 0;
   if (getFeature(outputTE, roiDescriptor,"initialRoI") != HLT::OK) roiDescriptor = 0;
   
   if ( !roiDescriptor ) {
      if ( msgLvl() <= MSG::WARNING) {
	 msg() <<  MSG::WARNING << "No RoI for this Trigger Element! " << endreq;
      }    
      
      return HLT::NAV_ERROR;
   }
   
   if ( msgLvl() <= MSG::DEBUG ){
      msg() << MSG::DEBUG 
      << "Using outputTE("<< outputTE <<")->getId(): " << outputTE->getId()
      << "; RoI ID = "   << roiDescriptor->roiId()
      << ": Eta = "      << roiDescriptor->eta()
      << ", Phi = "      << roiDescriptor->phi()
      << endreq;
   } 
   
   
   if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "m_vertexColl " << m_VertexColl << "    m_trigBphysColl " << m_trigBphysColl << endreq;
   
   delete  m_VertexColl;
   m_VertexColl=0;
   
   if ((m_trigBphysColl!=0) && (m_trigBphysColl->size() > 0)) {
      if ( msgLvl() <= MSG::DEBUG) msg()  << MSG::DEBUG << "REGTEST: Store Bphys Collection size: " << m_trigBphysColl->size() << endreq;
      //std::string bphysKey ="";
      //	HLT::ErrorCode sc = recordAndAttachFeature(outputTE, m_trigBphysColl, bphysKey, "L2JpsieeFex" );
      HLT::ErrorCode sc = attachFeature(outputTE, m_trigBphysColl, "L2JpsieeFex" );
      if(sc != HLT::OK) {
	 msg()  << MSG::WARNING << "Failed to store trigBphys Collection" << endreq;
	 return sc;
      }
   } else {
      if ( msgLvl() <= MSG::DEBUG) msg()  << MSG::DEBUG << "REGTEST: no bphys collection to store "  << endreq;
      delete  m_trigBphysColl;
   }
   m_trigBphysColl=0;
   
   return HLT::OK;
}

/*-------------------------------------------*/
bool TrigL2JpsieeFex::PassTrackPair(const TrigInDetTrackFitPar* param_1, const TrigInDetTrackFitPar* param_2)
/*-------------------------------------------*/
{
   bool passed=false;
   
   if ( msgLvl() <= MSG::VERBOSE) {
     msg() << MSG::VERBOSE
	   << "PassTrackPair: track 1 : pt= " << param_1->pT() 
	   <<  " phi0 = " << param_1->phi0() << "  eta = " << param_1->eta() 
	   << "   z0 = " << param_1->z0()
	   << endreq;
     msg() << MSG::VERBOSE
	   << "PassTrack Pair track 2 : pt= " << param_2->pT() 
	   <<  " phi0 = " << param_2->phi0() << "  eta = " << param_2->eta() 
	   << "   z0 = " << param_2->z0() 
	   << endreq;
   }
   double deta = param_1->eta() - param_2->eta();
   double dphi = param_1->phi0() - param_2->phi0();
   double absdphi = fabs(dphi);
   if (2.* M_PI -absdphi < absdphi) {
      if (dphi > 0) { 
	 dphi = dphi - 2.* M_PI;
      } else { 
	 dphi = 2* M_PI - absdphi;
      }
   }	
   dphi=fabs(dphi);
   
   double dz0  = param_1->z0() - param_2->z0();
   double ptsum= fabs(param_1->pT()) + fabs(param_2->pT());
   
   
   if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE
   << " deta = " << deta << "    dphi = " << dphi << "    dz0 = " << dz0 << "    ptsum = " << ptsum 
   << endreq;
   
   mon_dEta.push_back(deta);
   mon_dPhi.push_back(dphi);
   mon_dz0.push_back(dz0);
   mon_Ptsum.push_back(ptsum/1000.);
   
   if (fabs(deta)< m_dEtaTrackPair && /*fabs(dphi)<m_dPhiTrackPair &&*/ fabs(dz0)<m_dz0TrackPair && ptsum > m_pTsumTrackPair) 
      passed=true;
   
   
   
   if ( msgLvl() <= MSG::VERBOSE)  msg() << MSG::VERBOSE << "Track pairs passed: " << passed << endreq;
   
   return passed;
}

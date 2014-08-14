/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigL2DsPhiPiFex.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBphysHypo
// author :  Julie Kirk
// ********************************************************************

#include <math.h>
//#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"

#include "StoreGate/StoreGateSvc.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "TrigBphysHypo/TrigL2DsPhiPiFex.h"
#include "TrigBphysHypo/BtrigUtils.h"
#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"

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

/*------------------------------------------------------------------------------------*/
TrigL2DsPhiPiFex::TrigL2DsPhiPiFex(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::FexAlgo(name, pSvcLocator),
  m_vertFitter("TrigVertexFitter",this),
  m_L2vertFitter("TrigL2VertexFitter",this),
  m_vertexingTool("TrigVertexingTool",this)
/*------------------------------------------------------------------------------------*/
{
  // Read cuts
  declareProperty( "AcceptAll", m_acceptAll  = false );
  declareProperty("TrackPTthr", m_trackPtthr=1.4);
  //  declareProperty("TrackAlgoId", m_trackalgoID=2); no longer needed - have separate chains for IdSCan/SiTrk
  declareProperty("dEtaTrackPair", m_dEta_cut=0.2);
  declareProperty("dPhiTrackPair", m_dPhi_cut=0.2);
  declareProperty("dz0TrackPair", m_dz0_cut=3.);
  declareProperty("pTsumTrackPair", m_pTsum_cut=0.);
  declareProperty("EtaBins", m_Etabin);
  declareProperty("PhiMasslow_cut", m_PhiMasslow_cut);
  declareProperty("PhiMasshigh_cut", m_PhiMasshigh_cut);
  declareProperty("DsMasslow_cut", m_DsMasslow_cut);
  declareProperty("DsMasshigh_cut", m_DsMasshigh_cut);
  declareProperty("doVertexFit", m_doVertexFit=true);
  declareProperty("NumberDsToKeep", m_NumberDsToKeep=10);
  declareProperty("PhiChi2Prob_cut", m_PhiChi2_cut=-1.);
  declareProperty("DsChi2Prob_cut" , m_DsChi2_cut=-1.);


  declareProperty("TrigVertexFitter", m_vertFitter);
  declareProperty("TrigL2VertexFitter", m_L2vertFitter);
  declareProperty("TrigVertexingTool", m_vertexingTool, "TrigVertexingTool");

  // variables for monitoring histograms
  declareMonitoredVariable(    "Ntrack",                 mon_Ntrack);
  declareMonitoredStdContainer("trackPt",                mon_trackPt                , AutoClear);
  declareMonitoredStdContainer("dEta",                   mon_dEta                   , AutoClear);
  declareMonitoredStdContainer("dPhi",                   mon_dPhi                   , AutoClear);
  declareMonitoredStdContainer("dz0",                    mon_dz0                    , AutoClear);
  declareMonitoredStdContainer("Ptsum",                  mon_Ptsum                  , AutoClear);
  declareMonitoredStdContainer("PhiMass",                mon_PhiMass                , AutoClear);
  declareMonitoredStdContainer("PhiFitMass",             mon_PhiFitMass             , AutoClear);
  declareMonitoredStdContainer("DsMass",                 mon_DsMass                 , AutoClear);
  declareMonitoredStdContainer("DsFitMass",              mon_DsFitMass              , AutoClear);
  declareMonitoredStdContainer("PhiChi2prob",            mon_PhiChi2prob            , AutoClear);
  declareMonitoredStdContainer("PhiChi2prob_max",        mon_PhiChi2prob_max        , AutoClear);
  declareMonitoredStdContainer("DsChi2prob",             mon_DsChi2prob             , AutoClear);
  declareMonitoredStdContainer("DsChi2prob_max",         mon_DsChi2prob_max         , AutoClear);
  declareMonitoredStdContainer("PhiChi2prob_DsPass",     mon_PhiChi2prob_DsPass     , AutoClear);
  declareMonitoredStdContainer("PhiChi2prob_max_DsPass", mon_PhiChi2prob_max_DsPass , AutoClear);

  // zero counters
  m_lastEvent = 999;
  m_lastEventPassed = 999;
  m_countTotalEvents=0;
  m_countTotalRoI=0;
  m_countPassedEvents=0;
  m_countPassedRoIs=0;
  m_countPassedTrackPair=0;
  m_countPassedPhiMass=0;
  m_countPassedDsMass=0;
  m_countPassedPhiChi2=0;
  m_countPassedDsChi2=0;
  m_countPassedPhiVertex=0;
  m_countPassedDsVertex=0;

  m_trigBphysColl=0;
  m_trigPhiColl=0;
  m_VertexColl=0;
}

/*-------------------------------------------*/
TrigL2DsPhiPiFex::~TrigL2DsPhiPiFex(){
/*-------------------------------------------*/
  delete m_trigBphysColl;
  delete m_trigPhiColl;
  delete m_VertexColl;
}

/*-------------------------------------------*/
HLT::ErrorCode TrigL2DsPhiPiFex::hltInitialize(){
/*-------------------------------------------*/
  // print out properties, cuts

  msg() << MSG::INFO << "AcceptAll            = "  << (m_acceptAll==true ? "True" : "False") << endreq;
  //  msg() << MSG::INFO << "Using track algo "       << m_trackalgoID   << endreq;
  msg() << MSG::INFO << "Track pT cut "           << m_trackPtthr    << endreq;
  msg() << MSG::INFO << "Track pair cuts: dEta<"  << m_dEta_cut
                     << " dPhi<" << m_dPhi_cut
                     << " dz0<"  << m_dz0_cut
                     << " pTsum>" << m_pTsum_cut << endreq;
  msg() << MSG::INFO << "Phi mass cuts: " << m_PhiMasslow_cut << " < Mass(phi) < " << m_PhiMasshigh_cut << endreq;
  msg() << MSG::INFO << "Ds mass cuts : " << m_DsMasslow_cut  << " < Mass(Ds)  < " << m_DsMasshigh_cut  << endreq;
  msg() << MSG::INFO << "Chi2 Cuts: Chi2(Phi) > " << m_PhiChi2_cut  << "; Chi2(Ds) >  " << m_DsChi2_cut  << endreq;
  if (m_PhiChi2_cut==-1.) msg() << MSG::INFO << "Chi2 Cut : Chi2(Phi) : disabled " << endreq;
  if (m_DsChi2_cut ==-1.) msg() << MSG::INFO << "Chi2 Cut : Chi2(Ds)  : disabled " << endreq;
  if (m_doVertexFit)      msg() << MSG::INFO << "Vertex fit: enabled" << endreq;
  else                    msg() << MSG::INFO << "Vertex fit: disabled" << endreq;

  msg() << MSG::INFO << "Initialization completed successfully" << endreq;

  for (int iBin = 0; iBin < 3; iBin++)
    if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Bin: " << iBin << " Eta " << m_Etabin[iBin] << endreq;

// retrieving the vertex fitting tool

  StatusCode sc = m_vertFitter.retrieve();
  if ( sc.isFailure() ) {
    msg() << MSG::FATAL << "Unable to locate TrigVertexFitter tool " << endreq;
    return HLT::BAD_JOB_SETUP;
  } else {
    msg() << MSG::INFO << "TrigVertexFitter retrieved"<< endreq;
  }

  sc = m_L2vertFitter.retrieve();
  if ( sc.isFailure() ) {
    msg() << MSG::FATAL << "Unable to locate TrigL2VertexFitter tool " << endreq;
    return HLT::BAD_JOB_SETUP;
  }
  else {
    msg() << MSG::INFO << "TrigL2VertexFitter retrieved"<< endreq;
  }

  sc = m_vertexingTool.retrieve();
  if ( sc.isFailure() ) {
    msg() << MSG::FATAL << "Unable to locate TrigVertexingTool tool " << endreq;
    return HLT::BAD_JOB_SETUP;
  } else {
    msg() << MSG::INFO << "TrigVertexingTool retrieved"<< endreq;
  }

  // add timers
  if ( timerSvc() ) {
    m_TotTimer = addTimer("L2DsPhiPiFexTot");
    m_VtxFitTimer = addTimer ("L2DsPhiPiVFit");
  }
  return HLT::OK;
}

/*-------------------------------------------*/
HLT::ErrorCode TrigL2DsPhiPiFex::hltFinalize(){
/*-------------------------------------------*/
  if ( msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "--> summary output <-- " << endreq;

  // use brackets to use cool merging tool
    msg() << MSG::DEBUG << "in finalize()" << endreq;
    msg() << MSG::DEBUG << "Run on events (RoIs)  : " << m_countTotalEvents    << " ( " << m_countTotalRoI   << " ) " << endreq;
    msg() << MSG::DEBUG << "Passed events (RoIs)  : " << m_countPassedEvents   << " ( " << m_countPassedRoIs << " ) " << endreq;
    msg() << MSG::DEBUG << "RoIs Passed track pair: " << m_countPassedTrackPair<< endreq;
    msg() << MSG::DEBUG << "RoIs Passed PhiMass   : " << m_countPassedPhiMass  << endreq;
    msg() << MSG::DEBUG << "RoIs Passed PhiVertex : " << m_countPassedPhiVertex<< endreq;
    msg() << MSG::DEBUG << "RoIs Passed PhiChi2   : " << m_countPassedPhiChi2  << endreq;
    msg() << MSG::DEBUG << "RoIs Passed DsMass    : " << m_countPassedDsMass   << endreq;
    msg() << MSG::DEBUG << "RoIs Passed DsVertex  : " << m_countPassedDsVertex << endreq;
    msg() << MSG::DEBUG << "RoIs Passed DsChi2    : " << m_countPassedDsChi2   << endreq;
  }
  return HLT::OK;
}

/*--------------------------------------------------------------*/
//HLT::ErrorCode TrigL2DsPhiPiFex::acceptInput(const HLT::TriggerElement* inputTE, bool& pass){
/*--------------------------------------------------------------*/

HLT::ErrorCode TrigL2DsPhiPiFex::hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE)
{
  if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " In DsPhiPIFEX hltExecute" << endreq;
  

  bool PassedTrackPair=false;
  bool PassedPhiMass=false;
  bool PassedDsMass=false;
  bool PassedPhiChi2=false;
  bool PassedDsChi2=false;
  bool PassedPhiVertex=false;
  bool PassedDsVertex=false;
  bool result = false;
  //  const double PIMASS = 139.57;
  //  const double KMASS = 493.677;
  //  const double PHIMASS = 1020.;
  //  const double DSMASS = 1968.2;

  if (timerSvc() ) m_TotTimer->start();

  mon_Ntrack=-99;

  if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " In DsPhiPIFEX acceptInput" << endreq;
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
  if (m_acceptAll) {
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "AcceptAll property is set: taking all events" << endreq;
    //pass = true;
  }
  else {
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "AcceptAll property not set: applying selection" << endreq;
    //    pass=false;
  }

  // get RoI descriptor
  const TrigRoiDescriptor* roiDescriptor = 0;
  if (getFeature(inputTE, roiDescriptor) != HLT::OK) roiDescriptor = 0;

  if ( !roiDescriptor ) {
    if ( msgLvl() <= MSG::WARNING) {
      msg() <<  MSG::WARNING << "No RoI for this Trigger Element! " << endreq;
    }
    if ( timerSvc() ) m_TotTimer->stop();
    return HLT::NAV_ERROR;
  }

  if ( msgLvl() <= MSG::DEBUG ){
    msg() << MSG::DEBUG << "Using inputTE("<< inputTE <<")->getId(): " << inputTE->getId()
                        << "; RoI ID = "   << roiDescriptor->roiId()
                        << ": Eta = "      << roiDescriptor->eta()
                        << ", Phi = "      << roiDescriptor->phi() << endreq;
  }

//  create vector for TrigL2Bphys particles
  delete m_trigBphysColl;
  delete m_trigPhiColl;
  delete m_VertexColl;
  m_trigBphysColl = new TrigL2BphysContainer();
  TrigL2BphysContainer* m_trigDsColl      = new TrigL2BphysContainer();
  TrigL2BphysContainer* m_trigTempPhiColl = new TrigL2BphysContainer();
  m_trigPhiColl   = new TrigL2BphysContainer();
  m_VertexColl    = new TrigVertexCollection();

  //  TrigL2Bphys* trigL2BphysSave = new TrigL2Bphys((*pTrackColl)->RoI_ID(), roiDescriptor->eta(), roiDescriptor->phi(), TrigL2Bphys::PHIKK, PHIMASS);


  ///////////////// Get vector of tracks /////////////////
  /// changed to use new steering. NB used to use outputTE with old steering!! /////


  std::vector<const TrigInDetTrackCollection*> vectorOfTrackCollections;

  HLT::ErrorCode status = getFeatures(inputTE, vectorOfTrackCollections);

  if(status != HLT::OK ) {
    if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Failed to get InDetTrackCollections " << endreq;
  } else {
    if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Got " << vectorOfTrackCollections.size() << " InDetTrackCollections" << endreq;
  }

  std::vector<const TrigInDetTrackCollection*>::iterator pTrackColl    = vectorOfTrackCollections.begin();
  std::vector<const TrigInDetTrackCollection*>::iterator lastTrackColl = vectorOfTrackCollections.end();

  ////////////////////////////////////////////////////////////

  if (IdEvent != (int) m_lastEvent) {
    m_countTotalEvents++;
    m_lastEvent=IdEvent;
  }
  m_countTotalRoI++;

  double chi2prob_max = -1.;
  double chi2prob_phi_max = -1.;
  double chi2prob_phi_max_dspass = -1.;

  for ( ; pTrackColl != lastTrackColl;  pTrackColl++) {

    TrigInDetTrackCollection::const_iterator track= (*pTrackColl)->begin();
    TrigInDetTrackCollection::const_iterator lastTrack= (*pTrackColl)->end();

    if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Found tracks, ntrack= " << (*pTrackColl)->size() << endreq;
    mon_Ntrack=(*pTrackColl)->size();
    std::vector<bool> GoodTrack((*pTrackColl)->size(), false);
    int nGoodTrack=0;
		// check for good tracks
    mon_trackPt.reserve(mon_Ntrack);
    for (int itrk=0; track !=lastTrack; ++itrk, track++) {
      GoodTrack[itrk]=false;
      unsigned int algoId = (*track)->algorithmId();
      float trackPt = fabs( (*track)->param()->pT() );
      float trackChi2 = (*track)->chi2();
      mon_trackPt.push_back(trackPt/CLHEP::GeV);

      // JK no longer need to check algo id      if( algoId == m_trackalgoID  && trackPt > m_trackPtthr ) {
      if( trackPt > m_trackPtthr  && trackChi2 < 1e7) {
        GoodTrack[itrk]=true;
        nGoodTrack++;
        if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Good track, track: " << itrk
                            << " pT = "  << (*track)->param()->pT()
                            << " phi = " << (*track)->param()->phi0()
                            << " eta = " << (*track)->param()->eta() 
	                    << " chi2 " << trackChi2 <<  endreq;
      } else {
        if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Bad track, track: " << itrk << " algo " << algoId << " pT = " <<  (*track)->param()->pT() << " chi2 " << trackChi2 << endreq;
      } // if good algo && pT
    } // end loop over tracks

    if (nGoodTrack<3) continue;

		// loop over tracks and form combinations
    TrigInDetTrackCollection::const_iterator track1    = (*pTrackColl)->begin();
    TrigInDetTrackCollection::const_iterator lastTrack1= (*pTrackColl)->end();
    for (int itrk=0; track1 !=lastTrack1; ++itrk, track1++) {
      TrigInDetTrackCollection::const_iterator track2= (*pTrackColl)->begin();
      TrigInDetTrackCollection::const_iterator lastTrack2= (*pTrackColl)->end();
      for (int jtrk=0; track2 !=lastTrack2; ++jtrk, track2++) {
        if (jtrk<=itrk) continue;
        if ( msgLvl() <= MSG::VERBOSE)   msg() << MSG::VERBOSE << "Check tracks " << itrk << " and " << jtrk << endreq;
        if (!(GoodTrack[itrk] && GoodTrack[jtrk]) ) continue;
        if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "Good tracks "  << itrk << " and " << jtrk << " now check sign " << endreq;
        if (((*track1)->param()->pT()>0. && (*track2)->param()->pT()>0.) ||
               ((*track1)->param()->pT()<0. && (*track2)->param()->pT()<0.))  continue;

        
        if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "Tracks " << itrk << " and " << jtrk << " are opposite sign, make track pair cuts " << endreq;
        if (! PassTrackPair( (*track1)->param(), (*track2)->param() )) continue;
        if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Tracks " << itrk << " and " << jtrk << " pass track pair cuts, now form KK mass "   << endreq;
        PassedTrackPair=true;
        std::vector<const TrigInDetTrackFitPar*> inputtrks;
        std::vector<double> massHypo ;
        inputtrks.push_back((*track1)->param());
        inputtrks.push_back((*track2)->param());
        massHypo.push_back( KMASS );
        massHypo.push_back( KMASS );

        double PhiMass=InvMass(inputtrks, massHypo);
        mon_PhiMass.push_back(PhiMass);
        if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << " Phi_mass =  " << PhiMass  << endreq;
        int KetaBin;
        for (int iBin = 0; iBin < 3; iBin++)
          if ( fabs((*track1)->param()->eta()) > m_Etabin[iBin] ||  fabs((*track2)->param()->eta()) > m_Etabin[iBin] ) KetaBin = iBin;

        if ( msgLvl() <= MSG::DEBUG) msg()  << MSG::DEBUG << "track eta: " << (*track1)->param()->eta() << " " << (*track2)->param()->eta() << " K eta Bin : " << KetaBin << endreq;

        if (PhiMass> m_PhiMasslow_cut[KetaBin] && PhiMass<m_PhiMasshigh_cut[KetaBin]) {
          PassedPhiMass=true;

          TrigL2Bphys* trigPartPhi = new TrigL2Bphys((*pTrackColl)->RoI_ID(), roiDescriptor->eta(), roiDescriptor->phi(), TrigL2Bphys::PHIKK, PhiMass);
          if ( msgLvl() <= MSG::DEBUG) msg()  << MSG::DEBUG << "Create Bphys particle with roIId " << trigPartPhi->roiId()
              <<" mass " << PhiMass << " phi, eta " << trigPartPhi->phi() << " " << trigPartPhi->eta() <<
              " vertex type " << trigPartPhi->particleType() << endreq;

          //14-5-08 add tracks to L2bphys rather than vertex
          ElementLinkVector<TrigInDetTrackCollection> trackVector;
          ElementLink<TrigInDetTrackCollection> track1EL(*(*pTrackColl),itrk);
          ElementLink<TrigInDetTrackCollection> track2EL(*(*pTrackColl),jtrk);
          trackVector.push_back(track1EL);
          trackVector.push_back(track2EL);
          // trigPartPhi->trackVector(trackVector);
          trigPartPhi->addTrack(track1EL);
          trigPartPhi->addTrack(track2EL);

          // new vertexing
          double chi2probPhi= -1.;
          if (m_doVertexFit) {
            // vertex fitting
            //TrigVertex* p_phiV=m_vertFitter->fit(tList_phi,vPH);
            TrigL2Vertex* pL2V=new TrigL2Vertex();
            TrigVertex* p_phiV=NULL;

	    bool addTracks=true;
            StatusCode sc = m_vertexingTool->addTrack((*track1),pL2V,Trk::kaon);
            if(sc.isFailure()){
              if ( msgLvl() <= MSG::DEBUG) msg() <<MSG::DEBUG <<"Failed to add track 1 to vertexingTool pT, chi2 " <<(*track1)->param()->pT() << " " << (*track1)->chi2() << endreq;
	      addTracks=false;
            }
            sc = m_vertexingTool->addTrack((*track2),pL2V,Trk::kaon);
            if(sc.isFailure()){
              if ( msgLvl() <= MSG::DEBUG) msg() <<MSG::DEBUG <<"Failed to add track 2 to vertexingTool pT, chi2 " <<(*track2)->param()->pT() << " " << (*track2)->chi2() << endreq;
	      addTracks=false;
            }

            //bool phi_created=false;

	    // only do vertex fit if tracks successfully added to vertexing Tool
	    if (addTracks) {       
            sc=m_L2vertFitter->fit(pL2V);

// setting up the constraint
//m_vertexingTool->setMassConstraint(pL2V,(*track1),(*track2),PHIMASS);
// mass constrained fit
//StatusCode sc=m_L2vertFitter->fitWithConstraints(pL2V);

            if(sc.isFailure()) {
              if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG <<"TrigL2VertexFitter failed"<<endreq;
            } else {

              sc=m_vertexingTool->calculateInvariantMass(pL2V);
              if(sc.isSuccess()) {
                p_phiV=m_vertexingTool->createTrigVertex(pL2V);
                sc=m_vertexingTool->createMotherParticle(pL2V);
                if(sc.isSuccess()) {
                  //phi_created=true;
                  if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "Phi created: pT "<<pL2V->m_getMotherTrack()->pT() << endreq;
                }
              }
            }
	    } // end if (addTracks)
            delete pL2V;

            if(p_phiV==NULL) {
              if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " vertex fit failed for tracks  " << itrk << " and " << jtrk << endreq;
	      //              delete pL2V;
              delete p_phiV;
              delete trigPartPhi;
              continue;
            } else {
              if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "Phi vertex Fit: x= y= z=" << p_phiV->x() << " " << p_phiV->y()
                  << " " << p_phiV->z() << endreq;
              if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Phi vertex fit mass = " << p_phiV->mass() << " chi2 " << p_phiV->chi2() << " ndof " << p_phiV->ndof() <<  endreq;
              if (p_phiV->massVariance() != 0) {
                if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "Phi mass pull = "
                    << (p_phiV->mass()-PHIMASS)/p_phiV->massVariance() << endreq;
              } else {
                if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "Phi mass variance is zero, can't calculate the pull" << endreq;
              }


	      // histogram variable
              mon_PhiFitMass.push_back(p_phiV->mass());
	      // add vertex results to L2Bphys
              trigPartPhi->fitmass(p_phiV->mass());
              trigPartPhi->fitchi2(p_phiV->chi2());
              trigPartPhi->fitndof(p_phiV->ndof());
              trigPartPhi->fitx(p_phiV->x());
              trigPartPhi->fity(p_phiV->y());
              trigPartPhi->fitz(p_phiV->z());

              chi2probPhi=1.0-Genfun::CumulativeChiSquare(p_phiV->ndof())(p_phiV->chi2());
              if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Chi2 probability of the Phi fit = " << chi2probPhi << endreq;

              PassedPhiVertex=true;
              if (m_PhiChi2_cut!=-1. && chi2probPhi<m_PhiChi2_cut) {
                msg() << MSG::INFO << "Phi Candidate Chi2 lower than cut value" << endreq;
                delete p_phiV;
                delete trigPartPhi;
                continue;
              }
              PassedPhiChi2=true;
              if ( chi2probPhi > chi2prob_phi_max) {
                  chi2prob_phi_max = chi2probPhi;
              }

              if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "SigmaX =  SigmaY =  SigmaZ = " <<
                  sqrt(p_phiV->cov()[0]) << " " <<
                  sqrt(p_phiV->cov()[2]) << " " <<
                  sqrt(p_phiV->cov()[5]) << endreq;

              mon_PhiChi2prob.push_back(chi2probPhi);

              m_VertexColl->push_back(p_phiV);
              if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "added vertex to vertex collection" << endreq;
              int index = m_VertexColl->size()-1;
	      // JK 26-04-08 use ElementLink
              ElementLink<TrigVertexCollection> PhiVertexEL(*m_VertexColl,index);

            }


          } // end if doVertexFit


          m_trigPhiColl->push_back(trigPartPhi);
          int PhiIndex = m_trigPhiColl->size() - 1;


     	  // if pass phi mass then now make Ds mass
          TrigInDetTrackCollection::const_iterator track3= (*pTrackColl)->begin();
          TrigInDetTrackCollection::const_iterator lastTrack3= (*pTrackColl)->end();
          for (int ktrk=0 ; track3 != lastTrack3  ; ++ktrk, track3++) {
            if (!(GoodTrack[ktrk]) ) continue;
            if (track3==track1 || track3==track2) continue;
            if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "add track " << ktrk << endreq;
            inputtrks.push_back((*track3)->param());
            massHypo.push_back( PIMASS );
            double DsMass = InvMass(inputtrks, massHypo);
            mon_DsMass.push_back(DsMass);
            if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << " Ds mass =  " << DsMass  << endreq;

	    // JK 26-04-08 change to ElementLink
            ElementLink<TrigL2BphysContainer> trigPartPhiEL(*m_trigPhiColl,PhiIndex);

            TrigL2Bphys* trigPartDs = new TrigL2Bphys(roiDescriptor->roiId(), roiDescriptor->eta(), roiDescriptor->phi(), TrigL2Bphys::DSPHIPI, DsMass, trigPartPhiEL);

            float piEta =fabs((*track3)->param()->eta()) ;
            int DsEtaBin=KetaBin;
            if ( (piEta > fabs((*track1)->param()->eta())) || (piEta > fabs((*track2)->param()->eta()))) {
              for (int iBin = 0; iBin < 3; iBin++)
                if ( fabs((*track3)->param()->eta()) > m_Etabin[iBin] ) DsEtaBin = iBin;
            }


            if ( msgLvl() <= MSG::VERBOSE) msg()  << MSG::VERBOSE << "track eta: " << (*track1)->param()->eta() << " " << (*track2)->param()->eta() << " " << (*track3)->param()->eta() << " Ds eta Bin : " << DsEtaBin << endreq;
            if (DsMass > m_DsMasslow_cut[DsEtaBin] && DsMass < m_DsMasshigh_cut[DsEtaBin]) {
              if ( msgLvl() <= MSG::DEBUG) {
		msg() << MSG::DEBUG << " add track " << ktrk << " mass =  " << DsMass
                  << " EVENT PASSES!! " << endreq;
		msg()  << MSG::DEBUG << "Ds Bphys particle with roIId " << trigPartDs->roiId()
                  << " mass " << DsMass << " phi, eta " << trigPartDs->phi() << " " << trigPartDs->eta() <<
                  " vertex type " << trigPartDs->particleType() << endreq;
	      }
              PassedDsMass=true;

            } else {
              inputtrks.pop_back();
              massHypo.pop_back();
              delete trigPartDs;
              continue;
            }
            bool GoodDsFit=false;

            if (m_doVertexFit) {
              TrigL2Vertex* pL2Ds=new TrigL2Vertex();
              TrigVertex* p_DsV=NULL;
   	      bool addTracks=true;


              StatusCode sc = m_vertexingTool->addTrack((*track1),pL2Ds,Trk::kaon);
              if(sc.isFailure()){
                if ( msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"Failed to add track 1 to vertexingTool"<<endreq;
	        addTracks=false;
              }
              sc = m_vertexingTool->addTrack((*track2),pL2Ds,Trk::kaon);
              if(sc.isFailure()){
                if ( msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"Failed to add track 2 to vertexingTool"<<endreq;
	        addTracks=false;
              }
			 //   m_vertexingTool->setMassConstraint(pL2Ds,(*track1),(*track2),PHIMASS);

              sc = m_vertexingTool->addTrack((*track3),pL2Ds,Trk::pion);
              if(sc.isFailure()){
                if ( msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"Failed to add track 3 to vertexingTool"<<endreq;
	        addTracks=false;
              }

	      if (addTracks) {
              sc=m_L2vertFitter->fit(pL2Ds);

              if(sc.isFailure()){
                if ( msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"TrigL2VertexFitter failed"<<endreq;
              } else {

                sc=m_vertexingTool->calculateInvariantMass(pL2Ds);
                if(sc.isSuccess()) {
                  sc=m_vertexingTool->createMotherParticle(pL2Ds);
                  if(sc.isSuccess()){
                    GoodDsFit=true;
                    if ( msgLvl() <= MSG::VERBOSE) {
		      msg()<<MSG::VERBOSE <<"Ds created:"<<endreq;
		      msg() << MSG::VERBOSE << "Ds particle parameters"<<endreq;
		      msg() << MSG::VERBOSE << "d0="<<pL2Ds->m_getMotherTrack()->a0()<<
                        "  z0="<<pL2Ds->m_getMotherTrack()->z0()<<
                        "  phi0="<<pL2Ds->m_getMotherTrack()->phi0()<<
                        "  eta="<<pL2Ds->m_getMotherTrack()->eta()<<
                        "  pT="<<pL2Ds->m_getMotherTrack()->pT()<<endreq;
		    }
                    p_DsV=m_vertexingTool->createTrigVertex(pL2Ds);
                  }
                }
              }
	      }
              delete pL2Ds;

              if(p_DsV==NULL) {
                if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " vertex fit failed for tracks  " << itrk << " and " << jtrk
		      << " and " << ktrk << " GoodDsFit: " << GoodDsFit << endreq;
                delete p_DsV;
                
              } else {

                if ( msgLvl() <= MSG::DEBUG) {
		  msg() << MSG::DEBUG << "REGTEST: Ds vertex Fit: x= y= z=" << p_DsV->x() << " " << p_DsV->y() << " " << p_DsV->z() << endreq;
		  msg() << MSG::DEBUG << "REGTEST: Ds mass = " << p_DsV->mass() << " chi2 " <<  p_DsV->chi2() << " ndof " <<  p_DsV->ndof() << endreq;
		}
                if (p_DsV->massVariance() != 0) {
                  if ( msgLvl() <= MSG::VERBOSE)  msg() << MSG::VERBOSE << "Ds mass pull = " << (p_DsV->mass()-DSMASS)/p_DsV->massVariance() << endreq;
                } else {
                  if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "Ds mass variance is zero, can't calculate the pull" << endreq;
                }

                double chi2prob=1.0-Genfun::CumulativeChiSquare(p_DsV->ndof())(p_DsV->chi2());

                if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Chi2 probability of the Ds fit = " << chi2prob << endreq;
                PassedDsVertex=true;
                if (m_DsChi2_cut!=-1. && chi2prob<m_DsChi2_cut) {
                  if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Ds Candidate Chi2 lower than cut value" << endreq;
                  delete p_DsV;
		  delete trigPartDs;
                  continue;
                }
                PassedDsChi2=true;

                if ( chi2prob > chi2prob_max) {
                  chi2prob_max = chi2prob;   
                }
                if ( chi2probPhi > chi2prob_phi_max_dspass) {
                  chi2prob_phi_max_dspass = chi2probPhi;
                }
                if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "SigmaX =  SigmaY =  SigmaZ = " <<
                    sqrt(p_DsV->cov()[0]) << " " <<
                    sqrt(p_DsV->cov()[2]) << " " <<
                    sqrt(p_DsV->cov()[5]) << endreq;

                // add vertex results to L2Bphys
                float fitmass=p_DsV->mass();
                float fitchi2=p_DsV->chi2();
                int fitndof=p_DsV->ndof();
                trigPartDs->fitmass(fitmass);
                trigPartDs->fitchi2(fitchi2);
                trigPartDs->fitndof(fitndof);
                trigPartDs->fitx(p_DsV->x());
                trigPartDs->fity(p_DsV->y());
                trigPartDs->fitz(p_DsV->z());
                // monitoring histogram variable
                mon_DsFitMass.push_back(p_DsV->mass());
                mon_DsChi2prob.push_back(chi2prob);
                mon_PhiChi2prob_DsPass.push_back(chi2probPhi); 


                m_VertexColl->push_back(p_DsV);
                int DsIndex = m_VertexColl->size()-1;
                // JK 26-04-08 change to ElementLink
                ElementLink<TrigVertexCollection> DsVertexEL(*m_VertexColl,DsIndex);

                ElementLink<TrigInDetTrackCollection> track3EL(*(*pTrackColl),ktrk);
                trackVector.push_back(track3EL);

                trigPartDs->addTrack(track1EL);
                trigPartDs->addTrack(track2EL);
                trigPartDs->addTrack(track3EL);

              } // if vertex present
            } // end if doVertexFit

            inputtrks.pop_back();
            massHypo.pop_back();
            if (DsMass > m_DsMasslow_cut[DsEtaBin] && DsMass < m_DsMasshigh_cut[DsEtaBin]) {
              if (!m_doVertexFit || (m_doVertexFit && GoodDsFit)){
                if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "pass mass cuts, put Ds and Phi into Bphys collection " << endreq;
                m_trigDsColl->push_back(trigPartDs);
                result=true;
              } else {
		if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "no vertex fit or failed, delete trigPartDs " << endreq;
                delete trigPartDs;
              }
            } else {
              delete trigPartDs;
            }
          } // loop over third track
        } // if pass phi mass
      } // track 2 loop
    } // track1 loop


  } //end of track collection loop


  if (chi2prob_phi_max !=-1.) {
    if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " chi2prob_phi_max = " << chi2prob_phi_max << "  Event = " << IdEvent << endreq;
    mon_PhiChi2prob_max.push_back(chi2prob_phi_max);
  }
  if (chi2prob_max !=-1.) {
    if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " chi2prob_max = " << chi2prob_max << "  Event = " << IdEvent << endreq;
    mon_DsChi2prob_max.push_back(chi2prob_max);
  }
  if (chi2prob_phi_max_dspass !=-1.) {
    if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " chi2prob_phi_max_dspass = " << chi2prob_phi_max_dspass << "  Event = " << IdEvent << endreq;
    mon_PhiChi2prob_max_DsPass.push_back(chi2prob_phi_max_dspass); 
  }

  if (PassedTrackPair) m_countPassedTrackPair++;
  if (PassedPhiMass)   m_countPassedPhiMass++;
  if (PassedDsMass)    m_countPassedDsMass++;
  if (PassedPhiVertex) m_countPassedPhiVertex++;
  if (PassedDsVertex)  m_countPassedDsVertex++;
  if (PassedPhiChi2)   m_countPassedPhiChi2++;
  if (PassedDsChi2)    m_countPassedDsChi2++;

  if (result) {
    m_countPassedRoIs++;
    if (IdEvent!= (int) m_lastEventPassed) {
      m_countPassedEvents++;
      m_lastEventPassed=IdEvent;
    }
  }

    if ( msgLvl() <= MSG::DEBUG) {
      if (m_VertexColl != 0) msg() << MSG::DEBUG << " vertex collection size : " << m_VertexColl->size() << " m_VertexColl " << m_VertexColl << endreq;

      if (m_trigDsColl != 0) msg() << MSG::DEBUG << "Ds bphys collection size : " << m_trigDsColl->size() << " m_trigDsColl " << m_trigDsColl << endreq;
    }

  /*************************************
  // for now forget the sorting

	      // sort best 10 Ds
		// keep all Ds and store
  if (m_trigDsColl->size() <= m_NumberDsToKeep ) {
     if ( msgLvl() <= MSG::DEBUG)  msg() << MSG::DEBUG << "Less than " << m_NumberDsToKeep << " ds, will keep them all " << endreq;
  for (TrigL2BphysContainer::const_iterator dsIter = m_trigDsColl->begin(); dsIter !=  m_trigDsColl->end(); ++dsIter) {
  if ( msgLvl() <= MSG::VERBOSE) {
    msg() << MSG::VERBOSE << "*dsIter: " << *dsIter << endreq;
    msg() << MSG::VERBOSE << "*dsIter mass " << (*dsIter)->mass() << endreq;
  }
  TrigL2Bphys* trigPart = new TrigL2Bphys(*(*dsIter));
  if ( msgLvl() <= MSG::VERBOSE) {
    msg() << MSG::VERBOSE << "trigPart " << trigPart << endreq;
    msg() << MSG::VERBOSE << "trigPart mass " << trigPart->mass() << endreq;
  }
  m_trigBphysColl->push_back(trigPart);
}
} else {
                // sort by mass
   if ( msgLvl() <= MSG::DEBUG)   msg() << MSG::DEBUG << "More than " << m_NumberDsToKeep << " ds, will only keep best " << endreq;
  vector< pair<float,TrigL2Bphys*> > massdiff;
  if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << " Bphys collection before sort " << endreq;
  for (TrigL2BphysContainer::const_iterator bphysIter = m_trigDsColl->begin();
  bphysIter !=  m_trigDsColl->end(); ++bphysIter) {

  if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << " Bphys particle type: " << (*bphysIter)->particleType() << " with mass " << (*bphysIter)->mass() << "mass diff " << (*bphysIter)->mass()-DSMASS << endreq;
  pair<float,TrigL2Bphys*> diff(fabs((*bphysIter)->mass()-DSMASS),(*bphysIter));
  massdiff.push_back(diff);
}

  for (unsigned int i = 0 ; i<massdiff.size() ; i++) {
  if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "mass diff " << massdiff[i].first << " posn " << massdiff[i].second << endreq;
}
  std::sort(massdiff.begin(),massdiff.end());
  if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "after sort " << endreq;
  for (unsigned int i = 0 ; i<massdiff.size() ; i++) {
  if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "mass diff " << massdiff[i].first  << " posn " << massdiff[i].second << endreq;
}


  for (unsigned int i=0 ; i< m_NumberDsToKeep ; i++) {
  if ( msgLvl() <= MSG::VERBOSE) {
    msg() << MSG::VERBOSE << "massdiff[i].second " << massdiff[i].second << endreq;
    msg() << MSG::VERBOSE << "massdiff[i].second mass " << (massdiff[i].second)->mass() << endreq;
  }
  TrigL2Bphys* trigPart = new TrigL2Bphys(*(massdiff[i].second));
  if ( msgLvl() <= MSG::VERBOSE) {
    msg() << MSG::VERBOSE << "trigPart " << trigPart << endreq;
    msg() << MSG::VERBOSE << "trigPart mass " << trigPart->mass() << endreq;
  }
  m_trigBphysColl->push_back(trigPart);
}
		//                delete &massdiff;
}


	      // only store the Phis we need
  vector< pair<const TrigL2Bphys*,TrigL2Bphys*> > oldnew;
  if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << " Bphys collection before sort " << endreq;
  for (TrigL2BphysContainer::const_iterator bphysIter = m_trigBphysColl->begin();
  bphysIter !=  m_trigBphysColl->end(); ++bphysIter) {
  if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << " phi pointer: " << (*bphysIter)->pSecondDecay() << " Ds mass " << (*bphysIter)->mass() << " phi mass " << (*bphysIter)->pSecondDecay()->mass() << endreq;
		// check if phi already in collection
  bool phiInColl=false;
  for (TrigL2BphysContainer::const_iterator PhiIter = m_trigPhiColl->begin();
  PhiIter !=  m_trigPhiColl->end(); ++PhiIter) {
  if ( *PhiIter == (*bphysIter)->pSecondDecay()) {
  phiInColl=true;
     if ( msgLvl() <= MSG::DEBUG)  msg() << MSG::DEBUG << " phi already in collection " << (*bphysIter)->pSecondDecay() << endreq;
}
} // end loop over phi collection
  if (!phiInColl) {
  TrigL2Bphys* trigPart = new TrigL2Bphys(*((*bphysIter)->pSecondDecay()));
  if ( msgLvl() <= MSG::VERBOSE) {
    msg() << MSG::VERBOSE << "trigPart " << trigPart << endreq;
    msg() << MSG::VERBOSE << "trigPart mass " << trigPart->mass() << endreq;
  }
  m_trigPhiColl->push_back(trigPart);
		  // change the pointer in the Ds
  pair<const TrigL2Bphys*,TrigL2Bphys*> link((*bphysIter)->pSecondDecay(),trigPart);
  oldnew.push_back(link);
  (*bphysIter)->pSecondDecay(trigPart);
} else {
		  // just change pointer in Ds to new one
  for (unsigned int i = 0 ; i<oldnew.size() ; i++) {
  if ( (*bphysIter)->pSecondDecay() == oldnew[i].first) {
  (*bphysIter)->pSecondDecay(oldnew[i].second);
}
}
}


} // end loop over Ds collection
        */
	      // for now forget the sorting and store all Ds and phi



     if ( msgLvl() <= MSG::DEBUG)  msg() << MSG::DEBUG << "Keep all Ds for now  " << endreq;
  for (TrigL2BphysContainer::const_iterator dsIter = m_trigDsColl->begin(); dsIter !=  m_trigDsColl->end(); ++dsIter) {
    if ( msgLvl() <= MSG::VERBOSE) {
      msg() << MSG::VERBOSE << "*dsIter: " << *dsIter << endreq;
      msg() << MSG::VERBOSE << "*dsIter mass " << (*dsIter)->mass() << endreq;
    }
    TrigL2Bphys* trigPart = new TrigL2Bphys(*(*dsIter));
    if ( msgLvl() <= MSG::VERBOSE) {
      msg() << MSG::VERBOSE << "trigPart " << trigPart << endreq;
      msg() << MSG::VERBOSE << "trigPart mass " << trigPart->mass() << endreq;
    }
    m_trigBphysColl->push_back(trigPart);
  }

  if ( msgLvl() <= MSG::DEBUG) {
  if (m_trigBphysColl != 0) msg() << MSG::DEBUG << " bphys collection size : " << m_trigBphysColl->size() << " m_trigBphysColl " << m_trigBphysColl << endreq;
  if (m_trigPhiColl != 0) msg() << MSG::DEBUG << " bphys phi collection size : " << m_trigPhiColl->size() << " m_trigPhiColl " << m_trigPhiColl << endreq;
  }
  for (TrigL2BphysContainer::const_iterator bphysIter = m_trigBphysColl->begin();
       bphysIter !=  m_trigBphysColl->end(); ++bphysIter) {

            if ( msgLvl() <= MSG::DEBUG)  msg() << MSG::DEBUG << " phi pointer: " << (*bphysIter)->pSecondDecay() << " Ds mass " << (*bphysIter)->mass() << " phi mass " << (*bphysIter)->pSecondDecay()->mass() << endreq;
       }
  
  delete m_trigDsColl;
  delete m_trigTempPhiColl;

  
  // Reach this point successfully
     if ( msgLvl() <= MSG::DEBUG)  msg() << MSG::DEBUG << "Run: " << IdRun << " Event: " << IdEvent << "  result is " << result << endreq;
  
  //  result=true;
  
  //if (!m_acceptAll) pass = result;
  
  

  
  /*
  // get RoI descriptor
  const TrigRoiDescriptor* roiDescriptor = 0;
  if (getFeature(outputTE, roiDescriptor) != HLT::OK) roiDescriptor = 0;
  
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
  */

     if ( msgLvl() <= MSG::DEBUG)  msg() << MSG::DEBUG << "m_vertexColl " << m_VertexColl << "    m_trigBphysColl " << m_trigBphysColl << "    m_trigPhiColl " << m_trigPhiColl << endreq;

  delete m_VertexColl;
  m_VertexColl=0;

  if ((m_trigBphysColl!=0) && (m_trigBphysColl->size() > 0)) {
       if ( msgLvl() <= MSG::DEBUG)  msg()  << MSG::DEBUG << "REGTEST: Store Bphys Collection size: " << m_trigBphysColl->size() << endreq;
        //std::string bphysKey ="";
	//	HLT::ErrorCode sc = recordAndAttachFeature(outputTE, m_trigBphysColl, bphysKey, "L2DsPhiPiFex" );
    HLT::ErrorCode sc = attachFeature(outputTE, m_trigBphysColl, "L2DsPhiPiFexDs" );
    if(sc != HLT::OK) {
      msg()  << MSG::WARNING << "Failed to store trigBphys Collection" << endreq;
      return sc;
    }
  } else {
       if ( msgLvl() <= MSG::DEBUG)  msg()  << MSG::DEBUG << "REGTEST: no bphys collection to store "  << endreq;
    delete m_trigBphysColl;
  }
  m_trigBphysColl=0;

  if ((m_trigPhiColl!=0) && (m_trigPhiColl->size() > 0)) {
       if ( msgLvl() <= MSG::DEBUG)  msg()  << MSG::DEBUG << "REGTEST: Store Phi Collection size: " << m_trigPhiColl->size() << endreq;
    HLT::ErrorCode sc = attachFeature(outputTE, m_trigPhiColl, "L2DsPhiPiFexPhi" );
    if(sc != HLT::OK) {
      msg()  << MSG::WARNING << "Failed to store trigPhi Collection" << endreq;
      return sc;
    }
  } else {
       if ( msgLvl() <= MSG::DEBUG) msg()  << MSG::DEBUG << "REGTEST: no phi collection to store "  << endreq;
    delete m_trigPhiColl;
  }
  m_trigPhiColl=0;
  
  return HLT::OK;
}

/*-------------------------------------------*/
bool TrigL2DsPhiPiFex::PassTrackPair(const TrigInDetTrackFitPar* param_1, const TrigInDetTrackFitPar* param_2)
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


  if ( msgLvl() <= MSG::VERBOSE)  msg() << MSG::VERBOSE
      << " deta = " << deta << "    dphi = " << dphi << "    dz0 = " << dz0 << "    ptsum = " << ptsum
      << endreq;

  mon_dEta.push_back(fabs(deta));
  mon_dPhi.push_back(dphi);
  mon_dz0.push_back(fabs(dz0));
  mon_Ptsum.push_back(ptsum/1000.);

  if (fabs(deta)< m_dEta_cut && fabs(dphi)<m_dPhi_cut && fabs(dz0)<m_dz0_cut && ptsum > m_pTsum_cut)
    passed=true;



  if ( msgLvl() <= MSG::VERBOSE)  msg() << MSG::VERBOSE << "Track pairs passed: " << passed << endreq;

  return passed;
}


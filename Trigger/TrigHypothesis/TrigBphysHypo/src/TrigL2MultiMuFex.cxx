/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// *******************************************************************
//
// NAME:     TrigL2MultiMuFex.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBphysHypo
// AUTHOR:   Julie Kirk
//
// *******************************************************************

#include <math.h>

#include "GaudiKernel/StatusCode.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TrigMuonEvent/CombinedMuonFeature.h"

#include "TrigBphysHypo/TrigL2MultiMuFex.h"

#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigInDetTrackFitPar.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"

#include "TrigVertexFitter/ITrigVertexingTool.h"

#include "TrigParticle/TrigL2Bphys.h"

#include "TrigBphysHypo/Constants.h"
#include "TrigBphysHypo/BtrigUtils.h"

// additions of xAOD objects
#include "xAODEventInfo/EventInfo.h"

//class ISvcLocator;

/*--------------------------------------------------------------------------------*/
TrigL2MultiMuFex::TrigL2MultiMuFex(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::ComboAlgo(name, pSvcLocator),
  m_L2vertFitter("TrigL2VertexFitter",this),
  m_vertexingTool("TrigVertexingTool",this)
/*--------------------------------------------------------------------------------*/
{

  // Read properties - boolean switches
  declareProperty("AcceptAll"        , m_acceptAll         = true );
  declareProperty("NInputMuon"     , m_NInputMuon    = 3 );
  declareProperty("OppositeSign"     , m_oppositeCharge    = true );
  declareProperty("NMassMuon"     , m_NMassMuon    = 2 );
  declareProperty("doVertexFit"      , m_doVertexFit       = true );

  // Read properties - mass widnow cuts
  declareProperty("LowerMassCut", m_lowerMassCut =  2000.0 );
  declareProperty("UpperMassCut", m_upperMassCut = 10000.0 );

  // Read properties - vertexing tools
  declareProperty("TrigL2VertexFitter", m_L2vertFitter );
  declareProperty("TrigVertexingTool" , m_vertexingTool, "TrigVertexingTool" );

  // Variables for monitoring histograms
  declareMonitoredStdContainer("Errors"                 , mon_Errors                  , AutoClear);
  declareMonitoredStdContainer("Acceptance"             , mon_Acceptance              , AutoClear);
  //  declareMonitoredStdContainer("ROIEta"                 , mon_ROIEta                  , AutoClear);
  // declareMonitoredStdContainer("ROIPhi"                 , mon_ROIPhi                  , AutoClear);
  // - two combined muons
  declareMonitoredStdContainer("MucombPt"               , mon_MucombPt                , AutoClear);
  declareMonitoredStdContainer("MutrkPt"                , mon_MutrkPt                 , AutoClear);
  //  declareMonitoredStdContainer("MutrkPt_wideRange"      , mon_MutrkPt_wideRange       , AutoClear);
  declareMonitoredStdContainer("MutrkEta"               , mon_MutrkEta                , AutoClear);
  declareMonitoredStdContainer("MutrkPhi"               , mon_MutrkPhi                , AutoClear);
  //  declareMonitoredStdContainer("Mutrk1Mutrk2dEta"       , mon_Mutrk1Mutrk2dEta        , AutoClear);
  //declareMonitoredStdContainer("Mutrk1Mutrk2dPhi"       , mon_Mutrk1Mutrk2dPhi        , AutoClear);
  //declareMonitoredStdContainer("Mutrk1Mutrk2dR"         , mon_Mutrk1Mutrk2dR          , AutoClear);
  //declareMonitoredStdContainer("SumPtMutrk12"           , mon_SumPtMutrk12            , AutoClear);
  declareMonitoredStdContainer("InvMass_comb"           , mon_InvMass_comb            , AutoClear);
  declareMonitoredStdContainer("InvMass_comb_wideRange" , mon_InvMass_comb_wideRange  , AutoClear);
  declareMonitoredStdContainer("FitMass"                , mon_FitMass                 , AutoClear);
  declareMonitoredStdContainer("FitMass_wideRange"      , mon_FitMass_wideRange       , AutoClear);
  declareMonitoredStdContainer("InvMass_comb_okFit"     , mon_InvMass_comb_okFit      , AutoClear);
  declareMonitoredStdContainer("Chi2toNDoF"             , mon_Chi2toNDoF              , AutoClear);
  declareMonitoredStdContainer("FitTotalPt"             , mon_FitTotalPt              , AutoClear);
  declareMonitoredStdContainer("SumPtMutrk12_okFit"     , mon_SumPtMutrk12_okFit      , AutoClear);
  declareMonitoredStdContainer("FitVtxR"                , mon_FitVtxR                 , AutoClear);
  declareMonitoredStdContainer("FitVtxZ"                , mon_FitVtxZ                 , AutoClear);

  // Initialize the collections
  m_trigBphysColl = NULL;
  m_VertexColl    = NULL;
}

/*------------------------------*/
TrigL2MultiMuFex::~TrigL2MultiMuFex()
/*------------------------------*/
{
  // TODO: Delete the collections ?
  //delete m_trigBphysColl;
  //delete m_VertexColl;
}

/*-------------------------------------------*/
HLT::ErrorCode TrigL2MultiMuFex::hltInitialize()
/*-------------------------------------------*/
{

  // Print out properties, cuts
  if ( msgLvl() <= MSG::DEBUG ) {
    msg() << MSG::DEBUG << "Initialization ..." << endreq;
    msg() << MSG::DEBUG << "AcceptAll          = "
          << (m_acceptAll==true ? "True" : "False") << endreq;
    msg() << MSG::DEBUG << "Number of input muons expected   = " << m_NInputMuon << endreq;
    msg() << MSG::DEBUG << "Number of muons for mass   = " << m_NMassMuon << endreq;
    msg() << MSG::DEBUG << "OppositeCharge     = "
          << (m_oppositeCharge==true ? "True" : "False") << endreq;
    msg() << MSG::DEBUG << "LowerMassCut       = " << m_lowerMassCut << endreq;
    msg() << MSG::DEBUG << "UpperMassCut       = " << m_upperMassCut << endreq;
  }

  // Retrieving the vertex fitting tool
  // StatusCode sc = toolSvc()->retrieveTool("TrigVertexFitter","TrigVertexFitter",m_vertFitter);
  // if ( sc.isFailure() ) {
  //   msg() << MSG::FATAL << "Unable to locate TrigVertexFitter tool " << endreq;
  //   return HLT::BAD_JOB_SETUP;
  // }

  StatusCode sc = m_L2vertFitter.retrieve();
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

  // Add the timers
  if ( timerSvc() ) {
    m_BmmHypTot = addTimer("BmmHypTot");
    m_BmmHypVtx = addTimer("BmmHypVtxFit");
  }

  // TODO: shouldn't be initialized in the constructor (does it matter ?)
  m_lastEvent            = -1;
  m_lastEventPassed      = -1;
  m_countTotalEvents     = 0;
  m_countTotalRoI        = 0;
  m_countPassedEvents    = 0;
  m_countPassedRoIs      = 0;
  m_countPassedmumuPairs = 0;
  m_countPassedBsMass    = 0;
  m_countPassedVtxFit    = 0;

  return HLT::OK;
}

/*-----------------------------------------*/
HLT::ErrorCode TrigL2MultiMuFex::hltFinalize()
/*-----------------------------------------*/
{

  msg() << MSG::INFO << "in finalize()" << endreq;
  MsgStream log(msgSvc(), name());

  msg() << MSG::INFO << "|----------------------- SUMMARY FROM TrigL2MultiMuFex -------------|" << endreq;
  msg() << MSG::INFO << "Run on events/2xRoIs " << m_countTotalEvents << "/" << m_countTotalRoI <<  endreq;
  msg() << MSG::INFO << "Passed events/2xRoIs " << m_countPassedEvents << "/" << m_countPassedRoIs <<  endreq;
  msg() << MSG::INFO << "RoIs Passed MuMu pairs: "  << m_countPassedmumuPairs << endreq;
  msg() << MSG::INFO << "RoIs Passed BsMass: "  << m_countPassedBsMass << endreq;
  msg() << MSG::INFO << "RoIs Passed Vtx Fit: "  << m_countPassedVtxFit << endreq;

  return HLT::OK;
}

// Define the bins for error-monitoring histogram
#define ERROR_No_EventInfo               0
#define ERROR_WrongNum_Input_TE          1
#define ERROR_GetMuonFailed              2
#define ERROR_AddTrack_Fails             3
#define ERROR_CalcInvMass_Fails          4
#define ERROR_BphysColl_Fails            5

// Define the bins for acceptance-monitoring histogram
#define ACCEPT_Input                  0
#define ACCEPT_GotMuons               1
#define ACCEPT_InvMass_Cut            2


/*------------------------------------------------------------------------------*/
HLT::ErrorCode TrigL2MultiMuFex::acceptInputs(HLT::TEConstVec& inputTE, bool& pass)
/*------------------------------------------------------------------------------*/
{

  
  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Running TrigL2MultiMuFex::acceptInputS" << endreq;
  mon_Acceptance.push_back( ACCEPT_Input );

  if ( m_acceptAll ) {
    pass = true;
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Accept property is set: taking all the events" << endreq;
    return HLT::OK;
  }

  // Processing timers
  if ( timerSvc() ) m_BmmHypTot->start();

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
            mon_Errors.push_back( ERROR_No_EventInfo );
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

  // Check consistency of the number of input Trigger Elements
  if ( inputTE.size() != m_NInputMuon ) {
    msg() << MSG::ERROR << "Got wrong number of input TEs, expect " << m_NInputMuon << " got " << inputTE.size() << endreq;
    if ( timerSvc() ) {
      m_BmmHypTot->stop();
    }
    mon_Errors.push_back( ERROR_WrongNum_Input_TE );
    return HLT::BAD_JOB_SETUP;
  }

  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Number of input TEs " << inputTE.size() << endreq;
 
  pass = true;
  return HLT::OK;
}



/*----------------------------------------------------------------------------------------*/
HLT::ErrorCode TrigL2MultiMuFex::hltExecute(HLT::TEConstVec& inputTE, HLT::TriggerElement* outputTE)
/*----------------------------------------------------------------------------------------*/
{
   if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " In hltExecute " << endreq;

  m_trigBphysColl = new TrigL2BphysContainer();

  if (m_NInputMuon==3) {
   if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Call processTriMuon " << endreq;

    processTriMuon(inputTE);
  }

      if ((m_trigBphysColl!=0) && (m_trigBphysColl->size() > 0)) {
	//  mon_Acceptance.push_back( ACCEPT_BphysColl_not_Empty );
    if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "REGTEST: Store Bphys Collection size: " << m_trigBphysColl->size() << endreq;

    HLT::ErrorCode sc = attachFeature(outputTE, m_trigBphysColl, "L2MultiMuFex" );
    if(sc != HLT::OK) {
      msg()  << MSG::WARNING << "Failed to store trigBphys Collection" << endreq;
      mon_Errors.push_back( ERROR_BphysColl_Fails );
      return HLT::ERROR;
    }
  } else {
    if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "REGTEST: no bphys collection to store "  << endreq;
    delete m_trigBphysColl;
  }

      
  return HLT::OK;
}


void TrigL2MultiMuFex::processTriMuon(HLT::TEConstVec& inputTE)
{
   if ( msgLvl() <= MSG::VERBOSE ) msg() << MSG::VERBOSE << " In processTriMuon " << endreq;
      const HLT::TriggerElement* te1 = inputTE[0];
      const HLT::TriggerElement* te2 = inputTE[1];
      const HLT::TriggerElement* te3 = inputTE[2];

      const CombinedMuonFeature *muon1;
      const CombinedMuonFeature *muon2;
      const CombinedMuonFeature *muon3;

      if(getFeature(te1,muon1)!= HLT::OK) {
	msg() <<MSG::DEBUG << "Failed to get muon Feature for TE 1" << endreq;
        mon_Errors.push_back( ERROR_GetMuonFailed );
	return;
      }
      if ( muon1 == NULL ) {
        msg() <<MSG::DEBUG << "NULL pointer of muon Feature for TE 1" << endreq;
        mon_Errors.push_back( ERROR_GetMuonFailed );
        return;
      }
      if(getFeature(te2,muon2)!= HLT::OK) {
	msg() <<MSG::DEBUG << "Failed to get muon Feature for TE 2" << endreq;
        mon_Errors.push_back( ERROR_GetMuonFailed );
	return;
      }
      if ( muon2 == NULL ) {
        msg() <<MSG::DEBUG << "NULL pointer of muon Feature for TE 2" << endreq;
        mon_Errors.push_back( ERROR_GetMuonFailed );
        return;
      }
      if(getFeature(te3,muon3)!= HLT::OK) {
	msg() <<MSG::DEBUG << "Failed to get muon Feature for TE 3" << endreq;
        mon_Errors.push_back( ERROR_GetMuonFailed );
	return;
      }
      if ( muon3 == NULL ) {
        msg() <<MSG::DEBUG << "NULL pointer of muon Feature for TE 3" << endreq;
        mon_Errors.push_back( ERROR_GetMuonFailed );
        return;
      }

        mon_Acceptance.push_back( ACCEPT_GotMuons );

      //monitoring
      mon_MucombPt.push_back( muon1->ptq() / CLHEP::GeV );
      mon_MucombPt.push_back( muon2->ptq() / CLHEP::GeV );
      mon_MucombPt.push_back( muon3->ptq() / CLHEP::GeV );
      mon_MutrkPt.push_back( fabs(muon1->IDTrack()->param()->pT()) / CLHEP::GeV );
      mon_MutrkPt.push_back( fabs(muon2->IDTrack()->param()->pT()) / CLHEP::GeV );
      mon_MutrkPt.push_back( fabs(muon3->IDTrack()->param()->pT()) / CLHEP::GeV );
      mon_MutrkEta.push_back( muon1->IDTrack()->param()->eta() );
      mon_MutrkEta.push_back( muon2->IDTrack()->param()->eta() );
      mon_MutrkEta.push_back( muon3->IDTrack()->param()->eta() );
      mon_MutrkPhi.push_back( muon1->IDTrack()->param()->phi0() );
      mon_MutrkPhi.push_back( muon2->IDTrack()->param()->phi0() );
      mon_MutrkPhi.push_back( muon3->IDTrack()->param()->phi0() );


      if ( msgLvl() <= MSG::DEBUG ) {
	msg() << MSG::DEBUG << "1st CombinedMuonFeature: addr=" << muon1
            << " pt=" << muon1->ptq() << " trkAddr=" << muon1->IDTrack()
            << " trackParamAddr=" << muon1->IDTrack()->param()
            << endreq;
	msg() << MSG::DEBUG << " Track 1 pT eta phi : " << muon1->IDTrack()->param()->pT() << "  " <<  muon1->IDTrack()->param()->eta() << "  " <<  muon1->IDTrack()->param()->phi0() << endreq;


	msg() << MSG::DEBUG << "2nd CombinedMuonFeature: addr=" << muon1
            << " pt=" << muon2->ptq() << " trkAddr=" << muon2->IDTrack()
            << " trackParamAddr=" << muon2->IDTrack()->param()
            << endreq;
	msg() << MSG::DEBUG << " Track 2 pT eta phi : " << muon2->IDTrack()->param()->pT() << "  " <<  muon2->IDTrack()->param()->eta() << "  " <<  muon2->IDTrack()->param()->phi0() << endreq;

	msg() << MSG::DEBUG << "3rd CombinedMuonFeature: addr=" << muon1
            << " pt=" << muon3->ptq() << " trkAddr=" << muon3->IDTrack()
            << " trackParamAddr=" << muon3->IDTrack()->param()
            << endreq;
	msg() << MSG::DEBUG << " Track 3 pT eta phi : " << muon3->IDTrack()->param()->pT() << "  " <<  muon3->IDTrack()->param()->eta() << "  " <<  muon3->IDTrack()->param()->phi0() << endreq;
      }

 

      if (m_NMassMuon ==2) {   // check mass of opposite sign muon pairs

	m_passInvMass=false;
        double Mass;
	Mass = getInvMass2(muon1, muon2);
        
	TrigL2Bphys *TrigPart1 = checkInvMass2(muon1, muon2, Mass); //check mass cut and create TrigL2Bphys
        if (TrigPart1!=NULL) {
	  m_trigBphysColl->push_back(TrigPart1);
	} else {
	  delete TrigPart1;
	}

	Mass = getInvMass2(muon1, muon3);
	TrigL2Bphys *TrigPart2 = checkInvMass2(muon1, muon3, Mass);
        if (TrigPart2!=NULL) {
	  m_trigBphysColl->push_back(TrigPart2);
	} else {
	  delete TrigPart2;
	}

	Mass = getInvMass2(muon2, muon3);
	TrigL2Bphys *TrigPart3 = checkInvMass2(muon2, muon3, Mass);
        if (TrigPart3!=NULL) {
	  m_trigBphysColl->push_back(TrigPart3);
	} else {
	  delete TrigPart3;
	}
	if (m_passInvMass) {
           mon_Acceptance.push_back( ACCEPT_InvMass_Cut );
	}
      }

      if (m_NMassMuon == 3) {   // check mass of all 3 muons
	m_passInvMass=false;
        double Mass;
	Mass = getInvMass3(muon1, muon2, muon3);
	TrigL2Bphys *TrigPart = checkInvMass3(muon1, muon2, muon3, Mass); //check mass cut and create TrigL2Bphys
        if (TrigPart!=NULL) {
	  m_trigBphysColl->push_back(TrigPart);
	} else {
	  delete TrigPart;
	}
	if (m_passInvMass) {
           mon_Acceptance.push_back( ACCEPT_InvMass_Cut );
	}
      }

      return;
}


double TrigL2MultiMuFex::getInvMass2(const CombinedMuonFeature *muon1,const CombinedMuonFeature *muon2)
{
  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " In getInvMass2 muon pT: " << muon1->IDTrack()->param()->pT() << "  " <<  muon2->IDTrack()->param()->pT() << endreq;
   double Mass;
     std::vector<const TrigInDetTrackFitPar*> inputtrks;
      std::vector<double> massHypo ;
      massHypo.push_back( MUMASS ); 
      massHypo.push_back( MUMASS );
   // check pair of opposite sign muons
   if (muon1->IDTrack()->param()->pT() * muon2->IDTrack()->param()->pT() < 0.) {
     if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Muons 1 and 2 opp. charge will calculate mass " << endreq;
      inputtrks.push_back(muon1->IDTrack()->param());
      inputtrks.push_back(muon2->IDTrack()->param());
      Mass=InvMass(inputtrks, massHypo);
      if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Muons 1 and 2 mass =  " << Mass << endreq;
      mon_InvMass_comb.push_back(Mass/1000.);    

   }
   return Mass;
}

double TrigL2MultiMuFex::getInvMass3(const CombinedMuonFeature *muon1,const CombinedMuonFeature *muon2, const CombinedMuonFeature *muon3)
{
   if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " In getInvMass3 " << endreq;
   double Mass;
     std::vector<const TrigInDetTrackFitPar*> inputtrks;
      std::vector<double> massHypo ;
      massHypo.push_back( MUMASS );
      massHypo.push_back( MUMASS );
      massHypo.push_back( MUMASS );
   // check pair of opposite sign muons
      double muon1_pT=muon1->IDTrack()->param()->pT();
      double muon2_pT=muon2->IDTrack()->param()->pT();
      double muon3_pT=muon3->IDTrack()->param()->pT();

      if ((muon1_pT * muon2_pT < 0.) || (muon1_pT * muon3_pT < 0.) || (muon3_pT * muon2_pT < 0.)) {
     if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Muons opp. charge will calculate mass " << endreq;
      inputtrks.push_back(muon1->IDTrack()->param());
      inputtrks.push_back(muon2->IDTrack()->param());
      inputtrks.push_back(muon3->IDTrack()->param());
      Mass=InvMass(inputtrks, massHypo);
      if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Muons mass =  " << Mass << endreq;
      mon_InvMass_comb.push_back(Mass/1000.);    

   }
   return Mass;
}


TrigL2Bphys* TrigL2MultiMuFex::checkInvMass2(const CombinedMuonFeature *muon1,const CombinedMuonFeature *muon2, double Mass)
{
  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " In checkInvMass2, cuts " << m_lowerMassCut << " " << m_upperMassCut <<endreq;

   if ( Mass < m_lowerMassCut || Mass > m_upperMassCut  ) {
     if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Mass : " << Mass << " cut failed  "  << endreq;
      return NULL;
   }
   if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Mass cut passed, creat TrigL2Bphys  "  << endreq;
   m_passInvMass=true;
   TrigL2Bphys* trigPart = new TrigL2Bphys( 0, 0., 0., TrigL2Bphys::MULTIMU, Mass); //should really be type MULTIMU - needs changes to TrigPartcile


  //  Add element-links to tracks
  ElementLink<TrigInDetTrackCollection> track1EL=muon1->IDTrackLink();
  ElementLink<TrigInDetTrackCollection> track2EL=muon2->IDTrackLink();
    if ( msgLvl() <= MSG::DEBUG ) {
      msg() << MSG::DEBUG << "Just check track links... " << endreq;
      msg() << MSG::DEBUG << "Track 1 pT " << (*track1EL)->param()->pT()
            << " eta: " << (*track1EL)->param()->eta()
            << " phi: " << (*track1EL)->param()->phi0() << endreq;
      msg() << MSG::DEBUG << "Track 2 pT " << (*track2EL)->param()->pT()
            << " eta: " << (*track2EL)->param()->eta()
            << " phi: " << (*track2EL)->param()->phi0() << endreq;
    }
    trigPart->addTrack(track1EL);
    trigPart->addTrack(track2EL);

    //vertexing
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Now do vertexing" << endreq;

    TrigL2Vertex* pL2V    = new TrigL2Vertex();
    TrigVertex*   p_mumuV = NULL;
    StatusCode sc;
    // Add tracks to the vertexer
    bool addTracks = true;
    if (muon1->IDTrack()->chi2() < 1e7 ) {
       sc = m_vertexingTool->addTrack(muon1->IDTrack(),pL2V,Trk::muon);
        if(sc.isFailure()){
          if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Failed to add track 1 to vertexingTool" << endreq;
          addTracks = false;
          mon_Errors.push_back( ERROR_AddTrack_Fails );
        }
    } else {
     addTracks = false;
    }    
    
    if (muon2->IDTrack()->chi2() < 1e7 ) {
      sc = m_vertexingTool->addTrack(muon2->IDTrack(),pL2V,Trk::muon);
      if(sc.isFailure()){
        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Failed to add track 2 to vertexingTool" << endreq;
        addTracks = false;
        mon_Errors.push_back( ERROR_AddTrack_Fails );
      }
    } else {
        addTracks = false;
    }
    // DO THE VERTEX-FIT
    if ( addTracks) {
      sc = m_L2vertFitter->fit(pL2V);
    }

    // Check the result
    if ( sc.isFailure() || (!addTracks) ) {
      if ( msgLvl() <= MSG::DEBUG )msg() << MSG::DEBUG << "TrigL2VertexFitter failed" << endreq;
    } else {
      // Calculate invariant mass
      sc = m_vertexingTool->calculateInvariantMass(pL2V);
      if ( sc.isSuccess() ) {
        // Create mother particle
	//        sc = m_vertexingTool->createMotherParticle(pL2V);
        //if ( sc.isSuccess() ) {
        //  if ( msgLvl() <= MSG::VERBOSE ) msg() << MSG::VERBOSE << "Bs created: pT "<<pL2V->m_getMotherTrack()->pT() << endreq;
        //} else {
        //  mon_Errors.push_back( ERROR_CalcMother_Fails );
	// }
        p_mumuV = m_vertexingTool->createTrigVertex(pL2V);
      } else {
	mon_Errors.push_back( ERROR_CalcInvMass_Fails );
      }
    }
    delete pL2V;

    if ( p_mumuV == NULL ) {
      if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " vertex fit failed for tracks  " << endreq;
      delete p_mumuV;
    } else {
      double chi2prob = 0.;
      chi2prob = 1.0 - Genfun::CumulativeChiSquare(p_mumuV->ndof())(p_mumuV->chi2());
      if ( msgLvl() <= MSG::DEBUG ) {
        msg() << MSG::DEBUG << "REGTEST: mumu vertex Fit: x= y= z=" << p_mumuV->x() << " " << p_mumuV->y() << " " << p_mumuV->z() << endreq;
        msg() << MSG::DEBUG << "REGTEST: mumu mass = " << p_mumuV->mass() << endreq;
        if (p_mumuV->massVariance() !=0) {
          msg() << MSG::DEBUG << "REGTEST: mumu mass pull = " << (p_mumuV->mass()-BSMASS)/p_mumuV->massVariance() << endreq;
        } else {
          msg() << MSG::DEBUG << "REGTEST: MuMu mass variance is 0 can't calculate the pull " << endreq;
        }

        msg() << MSG::DEBUG << "REGTEST: Chi2 vtx fit = " << p_mumuV->chi2() << ", prob. " << chi2prob << endreq;
        msg() << MSG::DEBUG << "SigmaX =  SigmaY =  SigmaZ = " << sqrt(p_mumuV->cov()[0]) << " " << sqrt(p_mumuV->cov()[2]) << " " << sqrt(p_mumuV->cov()[5]) << endreq;

      }
      trigPart->fitmass(p_mumuV->mass());
      trigPart->fitchi2(p_mumuV->chi2());
      trigPart->fitndof(p_mumuV->ndof());
      trigPart->fitx(p_mumuV->x());
      trigPart->fity(p_mumuV->y());
      trigPart->fitz(p_mumuV->z());
      mon_FitMass.push_back(p_mumuV->mass()/1000.);      
      mon_Chi2toNDoF.push_back(p_mumuV->chi2()/ p_mumuV->ndof());      
      delete p_mumuV;
    }


    return trigPart;
}

TrigL2Bphys* TrigL2MultiMuFex::checkInvMass3(const CombinedMuonFeature *muon1,const CombinedMuonFeature *muon2, const CombinedMuonFeature *muon3, double Mass)
{
  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " In checkInvMass3, cuts " << m_lowerMassCut << " " << m_upperMassCut <<endreq;

   if ( Mass < m_lowerMassCut || Mass > m_upperMassCut  ) {
     if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Mass : " << Mass << " cut failed  "  << endreq;
      return NULL;
   }
   if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Mass cut passed, creat TrigL2Bphys  "  << endreq;
   m_passInvMass=true;

   TrigL2Bphys* trigPart = new TrigL2Bphys( 0, 0., 0., TrigL2Bphys::MULTIMU, Mass); //should really be type MULTIMU - needs changes to TrigPartcile


  //  Add element-links to tracks
  ElementLink<TrigInDetTrackCollection> track1EL=muon1->IDTrackLink();
  ElementLink<TrigInDetTrackCollection> track2EL=muon2->IDTrackLink();
  ElementLink<TrigInDetTrackCollection> track3EL=muon3->IDTrackLink();
    if ( msgLvl() <= MSG::DEBUG ) {
      msg() << MSG::DEBUG << "Just check track links... " << endreq;
      msg() << MSG::DEBUG << "Track 1 pT " << (*track1EL)->param()->pT()
            << " eta: " << (*track1EL)->param()->eta()
            << " phi: " << (*track1EL)->param()->phi0() << endreq;
      msg() << MSG::DEBUG << "Track 2 pT " << (*track2EL)->param()->pT()
            << " eta: " << (*track2EL)->param()->eta()
            << " phi: " << (*track2EL)->param()->phi0() << endreq;
      msg() << MSG::DEBUG << "Track 3 pT " << (*track3EL)->param()->pT()
            << " eta: " << (*track3EL)->param()->eta()
            << " phi: " << (*track3EL)->param()->phi0() << endreq;
    }
    trigPart->addTrack(track1EL);
    trigPart->addTrack(track2EL);
    trigPart->addTrack(track3EL);

    //vertexing
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Now do vertexing" << endreq;

    TrigL2Vertex* pL2V    = new TrigL2Vertex();
    TrigVertex*   p_mumuV = NULL;
    StatusCode sc;
    // Add tracks to the vertexer
    bool addTracks = true;
    if (muon1->IDTrack()->chi2() < 1e7 ) {
       sc = m_vertexingTool->addTrack(muon1->IDTrack(),pL2V,Trk::muon);
        if(sc.isFailure()){
          if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Failed to add track 1 to vertexingTool" << endreq;
          mon_Errors.push_back( ERROR_AddTrack_Fails );
          addTracks = false;
        }
    } else {
     addTracks = false;
    }    
    
    if (muon2->IDTrack()->chi2() < 1e7 ) {
      sc = m_vertexingTool->addTrack(muon2->IDTrack(),pL2V,Trk::muon);
      if(sc.isFailure()){
        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Failed to add track 2 to vertexingTool" << endreq;
        addTracks = false;
        mon_Errors.push_back( ERROR_AddTrack_Fails );
      }
    } else {
        addTracks = false;
    }

    if (muon3->IDTrack()->chi2() < 1e7 ) {
      sc = m_vertexingTool->addTrack(muon3->IDTrack(),pL2V,Trk::muon);
      if(sc.isFailure()){
        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Failed to add track 3 to vertexingTool" << endreq;
        addTracks = false;
        mon_Errors.push_back( ERROR_AddTrack_Fails );
      }
    } else {
        addTracks = false;
    }

    // DO THE VERTEX-FIT
    if ( addTracks) {
      sc = m_L2vertFitter->fit(pL2V);
    }

    // Check the result
    if ( sc.isFailure() || (!addTracks) ) {
      if ( msgLvl() <= MSG::DEBUG )msg() << MSG::DEBUG << "TrigL2VertexFitter failed" << endreq;
    } else {
      // Calculate invariant mass
      sc = m_vertexingTool->calculateInvariantMass(pL2V);
      if ( sc.isSuccess() ) {
        // Create mother particle
	//        sc = m_vertexingTool->createMotherParticle(pL2V);
        //if ( sc.isSuccess() ) {
        //  if ( msgLvl() <= MSG::VERBOSE ) msg() << MSG::VERBOSE << "Bs created: pT "<<pL2V->m_getMotherTrack()->pT() << endreq;
        //} else {
        //  mon_Errors.push_back( ERROR_CalcMother_Fails );
	// }
        p_mumuV = m_vertexingTool->createTrigVertex(pL2V);
      } else {
        mon_Errors.push_back( ERROR_CalcInvMass_Fails );
      }
    }
    delete pL2V;

    if ( p_mumuV == NULL ) {
      if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " vertex fit failed for tracks  " << endreq;
      delete p_mumuV;
    } else {
      double chi2prob = 0.;
      chi2prob = 1.0 - Genfun::CumulativeChiSquare(p_mumuV->ndof())(p_mumuV->chi2());
      if ( msgLvl() <= MSG::DEBUG ) {
        msg() << MSG::DEBUG << "REGTEST: mumu vertex Fit: x= y= z=" << p_mumuV->x() << " " << p_mumuV->y() << " " << p_mumuV->z() << endreq;
        msg() << MSG::DEBUG << "REGTEST: mumu mass = " << p_mumuV->mass() << endreq;
        if (p_mumuV->massVariance() !=0) {
          msg() << MSG::DEBUG << "REGTEST: mumu mass pull = " << (p_mumuV->mass()-BSMASS)/p_mumuV->massVariance() << endreq;
        } else {
          msg() << MSG::DEBUG << "REGTEST: MuMu mass variance is 0 can't calculate the pull " << endreq;
        }

        msg() << MSG::DEBUG << "REGTEST: Chi2 vtx fit = " << p_mumuV->chi2() << ", prob. " << chi2prob << endreq;
        msg() << MSG::DEBUG << "SigmaX =  SigmaY =  SigmaZ = " << sqrt(p_mumuV->cov()[0]) << " " << sqrt(p_mumuV->cov()[2]) << " " << sqrt(p_mumuV->cov()[5]) << endreq;

      }
      trigPart->fitmass(p_mumuV->mass());
      trigPart->fitchi2(p_mumuV->chi2());
      trigPart->fitndof(p_mumuV->ndof());
      trigPart->fitx(p_mumuV->x());
      trigPart->fity(p_mumuV->y());
      trigPart->fitz(p_mumuV->z());
      
      mon_FitMass.push_back(p_mumuV->mass()/1000.);      
      mon_Chi2toNDoF.push_back(p_mumuV->chi2()/ p_mumuV->ndof());      

      delete p_mumuV;
    }


    return trigPart;
}


bool TrigL2MultiMuFex::isUnique(const  TrigInDetTrack* id1, const  TrigInDetTrack* id2)
{
 float dEta = fabs( id1->param()->eta() - id2->param()->eta() );
 float dPhi = id1->param()->phi0() - id2->param()->phi0();
 while  (dPhi > M_PI) dPhi -= 2*M_PI;
 while  (dPhi < -M_PI) dPhi += 2*M_PI;
 if(dEta < 0.02 && fabs(dPhi) < 0.02 ) return false;
 return true;
}

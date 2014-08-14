/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// *******************************************************************
//
// NAME:     TrigL2BMuMuFex.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBphysHypo
// AUTHOR:   Sergey Sivoklokov
//
// *******************************************************************

#include <math.h>

#include "GaudiKernel/StatusCode.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TrigMuonEvent/CombinedMuonFeature.h"

#include "TrigBphysHypo/TrigL2BMuMuFex.h"

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
TrigL2BMuMuFex::TrigL2BMuMuFex(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::ComboAlgo(name, pSvcLocator),
  m_L2vertFitter("TrigL2VertexFitter",this),
  m_vertexingTool("TrigVertexingTool",this)
/*--------------------------------------------------------------------------------*/
{

  // Read properties - boolean switches
  declareProperty("AcceptAll"        , m_acceptAll         = true );
  declareProperty("OppositeSign"     , m_oppositeCharge    = true );
  declareProperty("ApplyUpperMassCut", m_ApplyupperMassCut = true );
  declareProperty("doVertexFit"      , m_doVertexFit       = true );
  declareProperty("noId"             , m_noId              = false);

  // Read properties - mass widnow cuts
  declareProperty("LowerMassCut", m_lowerMassCut =  2000.0 );
  declareProperty("UpperMassCut", m_upperMassCut = 10000.0 );

  // Read properties - vertexing tools
  declareProperty("TrigL2VertexFitter", m_L2vertFitter );
  declareProperty("TrigVertexingTool" , m_vertexingTool, "TrigVertexingTool" );

  // Variables for monitoring histograms
  declareMonitoredStdContainer("Errors"                 , mon_Errors                  , AutoClear);
  declareMonitoredStdContainer("Acceptance"             , mon_Acceptance              , AutoClear);
  declareMonitoredStdContainer("ROIEta"                 , mon_ROIEta                  , AutoClear);
  declareMonitoredStdContainer("ROIPhi"                 , mon_ROIPhi                  , AutoClear);
  declareMonitoredStdContainer("Roi1Roi2dEta"           , mon_Roi1Roi2dEta            , AutoClear);
  declareMonitoredStdContainer("Roi1Roi2dPhi"           , mon_Roi1Roi2dPhi            , AutoClear);
  declareMonitoredStdContainer("Roi1Roi2dR"             , mon_Roi1Roi2dR              , AutoClear);
  // - two combined muons
  declareMonitoredStdContainer("MucombROIdR"            , mon_MucombROIdR             , AutoClear);
  declareMonitoredStdContainer("MucombTrkdR"            , mon_MucombTrkdR             , AutoClear);
  declareMonitoredStdContainer("MucombPt"               , mon_MucombPt                , AutoClear);
  declareMonitoredStdContainer("MutrkPt"                , mon_MutrkPt                 , AutoClear);
  declareMonitoredStdContainer("MutrkPt_wideRange"      , mon_MutrkPt_wideRange       , AutoClear);
  declareMonitoredStdContainer("MutrkEta"               , mon_MutrkEta                , AutoClear);
  declareMonitoredStdContainer("MutrkPhi"               , mon_MutrkPhi                , AutoClear);
  declareMonitoredStdContainer("Mutrk1Mutrk2dEta"       , mon_Mutrk1Mutrk2dEta        , AutoClear);
  declareMonitoredStdContainer("Mutrk1Mutrk2dPhi"       , mon_Mutrk1Mutrk2dPhi        , AutoClear);
  declareMonitoredStdContainer("Mutrk1Mutrk2dR"         , mon_Mutrk1Mutrk2dR          , AutoClear);
  declareMonitoredStdContainer("SumPtMutrk12"           , mon_SumPtMutrk12            , AutoClear);
  declareMonitoredStdContainer("InvMass_comb"           , mon_InvMass_comb            , AutoClear);
  declareMonitoredStdContainer("InvMass_comb_wideRange" , mon_InvMass_comb_wideRange  , AutoClear);
  declareMonitoredStdContainer("FitMass"                , mon_FitMass                 , AutoClear);
  declareMonitoredStdContainer("FitMass_wideRange"      , mon_FitMass_wideRange       , AutoClear);
  declareMonitoredStdContainer("InvMass_comb_okFit"     , mon_InvMass_comb_okFit      , AutoClear);
  declareMonitoredStdContainer("Chi2toNDoF"             , mon_Chi2toNDoF              , AutoClear);
//  declareMonitoredStdContainer("Chi2toNDoFProb"         , mon_Chi2toNDoFProb          , AutoClear);
  declareMonitoredStdContainer("FitTotalPt"             , mon_FitTotalPt              , AutoClear);
  declareMonitoredStdContainer("SumPtMutrk12_okFit"     , mon_SumPtMutrk12_okFit      , AutoClear);
  declareMonitoredStdContainer("FitVtxR"                , mon_FitVtxR                 , AutoClear);
  declareMonitoredStdContainer("FitVtxZ"                , mon_FitVtxZ                 , AutoClear);
//  declareMonitoredVariable(    "VertexingTime"          , mon_VertexingTime );
//  // - one combined + one standalone muon
//  declareMonitoredStdContainer("MustandROIdR"           , mon_MustandROIdR            , AutoClear);
//  declareMonitoredStdContainer("MustandPt"              , mon_MustandPt               , AutoClear);
//  declareMonitoredStdContainer("MustandPt_wideRange"    , mon_MustandPt_wideRange     , AutoClear);
//  declareMonitoredStdContainer("MustandEta"             , mon_MustandEta              , AutoClear);
//  declareMonitoredStdContainer("MustandPhi"             , mon_MustandPhi              , AutoClear);
//  declareMonitoredStdContainer("MutrkMustanddEta"       , mon_MutrkMustanddEta        , AutoClear);
//  declareMonitoredStdContainer("MutrkMustanddPhi"       , mon_MutrkMustanddPhi        , AutoClear);
//  declareMonitoredStdContainer("MutrkMustanddR"         , mon_MutrkMustanddR          , AutoClear);
//  declareMonitoredStdContainer("SumPtMutrkMustand"      , mon_SumPtMutrkMustand       , AutoClear);
//  declareMonitoredStdContainer("InvMass_stand"          , mon_InvMass_stand           , AutoClear);
//  declareMonitoredStdContainer("InvMass_stand_wideRange", mon_InvMass_stand_wideRange , AutoClear);

  // Initialize the collections
  m_trigBphysColl = NULL;
  m_VertexColl    = NULL;
}

/*------------------------------*/
TrigL2BMuMuFex::~TrigL2BMuMuFex()
/*------------------------------*/
{
  // TODO: Delete the collections ?
  //delete m_trigBphysColl;
  //delete m_VertexColl;
}

/*-------------------------------------------*/
HLT::ErrorCode TrigL2BMuMuFex::hltInitialize()
/*-------------------------------------------*/
{

  // Print out properties, cuts
  if ( msgLvl() <= MSG::DEBUG ) {
    msg() << MSG::DEBUG << "Initialization ..." << endreq;
    msg() << MSG::DEBUG << "AcceptAll          = "
          << (m_acceptAll==true ? "True" : "False") << endreq;
    msg() << MSG::DEBUG << "noId          = "
          << (m_noId==true ? "True" : "False") << endreq;
    msg() << MSG::DEBUG << "OppositeCharge     = "
          << (m_oppositeCharge==true ? "True" : "False") << endreq;
    msg() << MSG::DEBUG << "LowerMassCut       = " << m_lowerMassCut << endreq;
    msg() << MSG::DEBUG << "UpperMassCut       = " << m_upperMassCut << endreq;
    msg() << MSG::DEBUG << "ApplyUpperMassCut  = " << m_ApplyupperMassCut << endreq;
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
HLT::ErrorCode TrigL2BMuMuFex::hltFinalize()
/*-----------------------------------------*/
{

  msg() << MSG::INFO << "in finalize()" << endreq;
  MsgStream log(msgSvc(), name());

  msg() << MSG::INFO << "|----------------------- SUMMARY FROM TrigL2BMuMuFex -------------|" << endreq;
  msg() << MSG::INFO << "Run on events/2xRoIs " << m_countTotalEvents << "/" << m_countTotalRoI <<  endreq;
  msg() << MSG::INFO << "Passed events/2xRoIs " << m_countPassedEvents << "/" << m_countPassedRoIs <<  endreq;
  msg() << MSG::INFO << "RoIs Passed MuMu pairs: "  << m_countPassedmumuPairs << endreq;
  msg() << MSG::INFO << "RoIs Passed BsMass: "  << m_countPassedBsMass << endreq;
  msg() << MSG::INFO << "RoIs Passed Vtx Fit: "  << m_countPassedVtxFit << endreq;

  return HLT::OK;
}

// Define the bins for error-monitoring histogram
#define ERROR_No_EventInfo               0
#define ERROR_Not_2_Input_TE             1
#define ERROR_No_RoI_1                   2
#define ERROR_No_RoI_2                   3
#define ERROR_Mu1_not_Combined           4
#define ERROR_Mu1_not_Standalone         5
#define ERROR_Mu1_Standalone_Invalid     6
#define ERROR_Mu2_not_Combined           7
#define ERROR_Mu2_not_Standalone         8
#define ERROR_Mu2_Standalone_Invalid     9
#define ERROR_MuMu_not_Both_Combined    10
#define ERROR_MuMu_not_Comb_Standalone  11
#define ERROR_AddTrack_Fails            12
#define ERROR_CalcInvMass_Fails         13
#define ERROR_CalcMother_Fails          14
#define ERROR_CalcMassPull_Fails        15
#define ERROR_BphysColl_Fails           16

// Define the bins for acceptance-monitoring histogram
#define ACCEPT_Input                  0
#define ACCEPT_Got_RoIs               1
#define ACCEPT_Mu1_Combined           2
#define ACCEPT_Mu1_Standalone         3
#define ACCEPT_Mu2_Combined           4
#define ACCEPT_Mu2_Standalone         5
#define ACCEPT_MuMu_Both_Combined     6
#define ACCEPT_MuMu_Comb_Standalone   7
#define ACCEPT_Mu1_IDTrack            8
#define ACCEPT_Mu2_IDTrack            9
#define ACCEPT_MuMu_Both_IDTracks    10
#define ACCEPT_CombMu_IDTrack        11
#define ACCEPT_DiMuon_Pair           12
#define ACCEPT_vtxpass_is_TRUE       13
#define ACCEPT_mumuIDpass_is_TRUE    14
#define ACCEPT_Opp_Charge            15
#define ACCEPT_InvMass_Cut           16
#define ACCEPT_AddTrack_Test         17
#define ACCEPT_Vertexing             18
#define ACCEPT_CalcInvMass           19
#define ACCEPT_CalcMother            20
#define ACCEPT_Vertex_not_NULL       21
// Separator                         22
#define ACCEPT_BphysColl_not_Empty   23

/*------------------------------------------------------------------------------*/
HLT::ErrorCode TrigL2BMuMuFex::acceptInputs(HLT::TEConstVec& inputTE, bool& pass)
/*------------------------------------------------------------------------------*/
{

  // Global flags
  bool mumuIDpass = true;
  bool vtxpass    = true;
  if (m_noId) vtxpass = false;

  // Initialize the monitoring variables
  mon_TotalRunTime = 0;
  mon_VertexingTime = 0;

  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Running TrigL2BMuMuFex::acceptInputS" << endreq;
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
  if ( inputTE.size() != 2 ) {
    msg() << MSG::ERROR << "Got different than 2 number of input TEs" << endreq;
    if ( timerSvc() ) {
      m_BmmHypTot->stop();
      mon_TotalRunTime = m_BmmHypTot->elapsed();
    }
    mon_Errors.push_back( ERROR_Not_2_Input_TE );
    return HLT::BAD_JOB_SETUP;
  }

  // Get the RoIs from the navigation
  const TrigRoiDescriptor *roi[2] = { 0, 0 };
  for ( int i=0; i<2; i++ ) {
    if ( getFeature(inputTE.front(), roi[i]) != HLT::OK ) {
      msg() << MSG::ERROR << "Navigation error while getting RoI descriptor " << i+1 << endreq;
      if ( timerSvc() ) {
        m_BmmHypTot->stop();
        mon_TotalRunTime = m_BmmHypTot->elapsed();
      }
      if ( !i ) mon_Errors.push_back( ERROR_No_RoI_1 );
      else      mon_Errors.push_back( ERROR_No_RoI_2 );
      return HLT::NAV_ERROR;
    }
    if ( roi[i] == NULL ) {
      msg() << MSG::ERROR << "Navigation error while getting RoI descriptor " << i+1 << " (null pointer)" << endreq;
      if ( timerSvc() ) {
        m_BmmHypTot->stop();
        mon_TotalRunTime = m_BmmHypTot->elapsed();
      }
      if ( !i ) mon_Errors.push_back( ERROR_No_RoI_1 );
      else      mon_Errors.push_back( ERROR_No_RoI_2 );
      return HLT::NAV_ERROR;
    }
  }

  mon_Acceptance.push_back( ACCEPT_Got_RoIs );

  // Print RoIs debugging information
  if ( msgLvl() <= MSG::DEBUG ) {
    msg() << MSG::DEBUG << "Using inputTEs: " << inputTE.front()->getId() << " AND " << inputTE.back()->getId()
          << "; RoI IDs = "   << roi[0]->roiId() << " AND "    << roi[1]->roiId()
          << ": Eta1 = "      << roi[0]->eta()  << " Eta2 = " << roi[1]->eta()
          << ", Phi1 = "      << roi[0]->phi()  << " Phi2 = " << roi[1]->phi()
          << endreq;
  }

  // Monitoring of the RoIs opening angle
  mon_ROIEta.push_back( roi[0]->eta() );
  mon_ROIPhi.push_back( roi[0]->phi() );
  mon_ROIEta.push_back( roi[1]->eta() );
  mon_ROIPhi.push_back( roi[1]->phi() );
  float dEtaRoI = roi[0]->eta() - roi[1]->eta();
  float dPhiRoI = roi[0]->phi() - roi[1]->phi();
  while ( dPhiRoI >  M_PI ) dPhiRoI -= 2*M_PI;
  while ( dPhiRoI < -M_PI ) dPhiRoI += 2*M_PI;
  mon_Roi1Roi2dEta.push_back( fabs(dEtaRoI) );
  mon_Roi1Roi2dPhi.push_back( fabs(dPhiRoI) );
  mon_Roi1Roi2dR  .push_back( sqrt(dEtaRoI*dEtaRoI + dPhiRoI*dPhiRoI) );

  m_muon1 = m_muon2 = NULL;
  ElementLink<MuonFeatureContainer> muonFeatureEL;
  const MuonFeature*  pMuonFeature(NULL);
  const CombinedMuonFeature *m_muon(NULL);
  int i_comb = 0; // for noId chains, index of the RoI with combined muon feature (should be always 0, but better check it)

  /* JK allow one muon to be a muonfeature rather than a combined muon
  if ( getFeature(inputTE.front(), m_muon1) != HLT::OK ) {
    msg() << MSG::ERROR << "Navigation error while getting CombinedMuonFeature 1" << endreq;
    if ( timerSvc() ) m_BmmHypTot->stop();
    return HLT::NAV_ERROR;
  }

  if ( getFeature(inputTE.back(), m_muon2) != HLT::OK ) {
    msg() << MSG::ERROR << "Navigation error while getting CombinedMuonFeature 2" << endreq;
    if ( timerSvc() ) m_BmmHypTot->stop();
    return HLT::NAV_ERROR;
  } */

  // Get the muons combined/standalone features
  const CombinedMuonFeature *muonX[2] = { m_muon1, m_muon2 };

  if ( !m_noId ) {
    for ( int i = 0; i < 2; i++ ) {
      if ( (getFeature(inputTE.at(i), muonX[i]) != HLT::OK) || !muonX[i] ) {
        msg() << MSG::ERROR << "Navigation error while getting CombinedMuonFeature " << i+1 << endreq;
        if ( timerSvc() ) {
          m_BmmHypTot->stop();
          mon_TotalRunTime = m_BmmHypTot->elapsed();
        }
        if ( i == 0 ) mon_Errors.push_back( ERROR_Mu1_not_Combined );
        else          mon_Errors.push_back( ERROR_Mu2_not_Combined );
        return HLT::NAV_ERROR;
      } else {
        msg() << MSG::DEBUG << "successfully got muon combined feature for RoI " << i+1 << endreq;
        // Monitoring of the combined muons w.r.t. ROI
        if ( i == 0 ) mon_Acceptance.push_back( ACCEPT_Mu1_Combined );
        else          mon_Acceptance.push_back( ACCEPT_Mu2_Combined );
        float dEtaCombRoI = ((i==0)?(roi[0]->eta()):(roi[1]->eta())) - muonX[i]->eta();
        float dPhiCombRoI = ((i==0)?(roi[0]->phi()):(roi[1]->phi())) - muonX[i]->phi();
        while ( dPhiCombRoI >  M_PI ) dPhiCombRoI -= 2*M_PI;
        while ( dPhiCombRoI < -M_PI ) dPhiCombRoI += 2*M_PI;
        mon_MucombROIdR.push_back( sqrt(dEtaCombRoI*dEtaCombRoI + dPhiCombRoI*dPhiCombRoI) );
      }
    } //end loop over RoIs

  } else { // m_noId

    for ( i_comb=0; i_comb<2; i_comb++ ) {
      if ( (getFeature(inputTE.at(i_comb), muonX[i_comb]) == HLT::OK) && muonX[i_comb] ) break;
      muonX[i_comb] = NULL;
    }
    if ( i_comb > 1 ) {
      msg() << MSG::ERROR << "Navigation error while getting CombinedMuonFeatures 1 or 2" << endreq;
      if ( timerSvc() ) {
        m_BmmHypTot->stop();
        mon_TotalRunTime = m_BmmHypTot->elapsed();
      }
      mon_Errors.push_back( ERROR_Mu1_not_Combined );
      mon_Errors.push_back( ERROR_Mu2_not_Combined );
      return HLT::NAV_ERROR;
    } else {
      msg() << MSG::DEBUG << "successfully got muon combined feature for RoI " << i_comb+1 << endreq;
      // Monitoring of the combined muons w.r.t. ROI
      if ( !i_comb ) mon_Acceptance.push_back( ACCEPT_Mu1_Combined );
      else           mon_Acceptance.push_back( ACCEPT_Mu2_Combined );
      float dEtaCombRoI = roi[i_comb]->eta() - muonX[i_comb]->eta();
      float dPhiCombRoI = roi[i_comb]->phi() - muonX[i_comb]->phi();
      while ( dPhiCombRoI >  M_PI ) dPhiCombRoI -= 2*M_PI;
      while ( dPhiCombRoI < -M_PI ) dPhiCombRoI += 2*M_PI;
      mon_MucombROIdR.push_back( sqrt(dEtaCombRoI*dEtaCombRoI + dPhiCombRoI*dPhiCombRoI) );
    }

    // Look for standalone muon in 2nd RoI
    if ( HLT::OK != getFeatureLink<MuonFeatureContainer, MuonFeature>(inputTE.at(1-i_comb), muonFeatureEL) ) {
      msg() << MSG::ERROR << " getFeatureLink fails to get MuonFeature" << endreq;
      if ( timerSvc() ) {
        m_BmmHypTot->stop();
        mon_TotalRunTime = m_BmmHypTot->elapsed();
      }
      if ( !i_comb ) mon_Errors.push_back( ERROR_Mu2_not_Standalone );
      else           mon_Errors.push_back( ERROR_Mu1_not_Standalone );
      return HLT::ERROR;
    }
    if ( !muonFeatureEL.isValid() ) {
      msg() << MSG::ERROR << " getFeatureLink finds no TrigMuonFeature (EL invalid)" << endreq;
      if ( timerSvc() ) {
        m_BmmHypTot->stop();
        mon_TotalRunTime = m_BmmHypTot->elapsed();
      }
      if ( !i_comb ) mon_Errors.push_back( ERROR_Mu2_Standalone_Invalid );
      else           mon_Errors.push_back( ERROR_Mu1_Standalone_Invalid );
      return HLT::NAV_ERROR;
    }
    pMuonFeature = *muonFeatureEL;
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG
        << "RoI " << 2-i_comb << ", the MuonFeature is saddress = " << pMuonFeature ->saddress()
        << " / pt = "       << pMuonFeature->pt()
        << " / radius = "   << pMuonFeature->radius()
        << " / eta = "      << pMuonFeature->eta()
        << " / phi = "      << pMuonFeature->phi()
        << " / dir_phi = "  << pMuonFeature->dir_phi()
        << " / zeta = "     << pMuonFeature->zeta()
        << " / dir_zeta = " << pMuonFeature->dir_zeta() << endreq;
    if ( !i_comb ) mon_Acceptance.push_back( ACCEPT_Mu2_Standalone );
    else           mon_Acceptance.push_back( ACCEPT_Mu1_Standalone );
    // Monitoring of the standalone muon w.r.t. ROI
    float dEtaStandRoI = roi[1-i_comb]->eta() - pMuonFeature->eta();
    float dPhiStandRoI = roi[1-i_comb]->phi() - pMuonFeature->phi();
    while ( dPhiStandRoI >  M_PI ) dPhiStandRoI -= 2*M_PI;
    while ( dPhiStandRoI < -M_PI ) dPhiStandRoI += 2*M_PI;
    mon_MustandROIdR.push_back( sqrt(dEtaStandRoI*dEtaStandRoI + dPhiStandRoI*dPhiStandRoI) );
  }

  m_muon1 = muonX[0];
  m_muon2 = muonX[1];

  // Check if we have the requested objects
  if ( !m_noId ) {
    // Need two combined muons
    if ( m_muon1 && m_muon2 && roi[0] && roi[1] ) {
      if ( msgLvl() <= MSG::DEBUG ) {
        msg() << MSG::DEBUG << "Got all necessary objects " << m_muon1 << " " << m_muon2 << endreq;
        msg() << MSG::DEBUG << "               for TE's " << inputTE.front() << " and " << inputTE.back() << endreq;
      }
      mon_Acceptance.push_back( ACCEPT_MuMu_Both_Combined );
    } else {
      msg() << MSG::ERROR << "Can't get all necessary objects rois: " << ((roi[0] && roi[1])?"OK": "MISSING")
            << " muons: " <<  ((m_muon1 && m_muon2)?"OK": "MISSING")  << endreq;
      if ( timerSvc() ) {
        m_BmmHypTot->stop();
        mon_TotalRunTime = m_BmmHypTot->elapsed();
      }
      mon_Errors.push_back( ERROR_MuMu_not_Both_Combined );
      return HLT::BAD_JOB_SETUP;
    }
  } else {
    // The noId flag means one RoI only has a muon feature - no combined track
    if ( muonX[i_comb] && roi[0] && roi[1] && pMuonFeature ) {
      if ( msgLvl() <= MSG::DEBUG ) {
        msg() << MSG::DEBUG << "NoId version, Got all necessary objects " << m_muon1 << " " << m_muon2 << " " << pMuonFeature << endreq;
        msg() << MSG::DEBUG << "               for TE's " << inputTE.front() << " and " << inputTE.back() << endreq;
      }
      m_muon = muonX[i_comb];
      mon_Acceptance.push_back( ACCEPT_MuMu_Comb_Standalone );
    } else {
      msg() << MSG::ERROR << "NoId version, Can't get all necessary objects rois: " << ((roi[0] && roi[1])?"OK": "MISSING")
            << " combined muon: " << (m_muon1?"OK": "MISSING") << " standalone muons: " << (pMuonFeature?"OK": "MISSING") << endreq;
      if ( timerSvc() ) {
        m_BmmHypTot->stop();
        mon_TotalRunTime = m_BmmHypTot->elapsed();
      }
      mon_Errors.push_back( ERROR_MuMu_not_Comb_Standalone );
      return HLT::BAD_JOB_SETUP;
    }
  }

  // Debug dump of the gained information
  if ( msgLvl() <= MSG::DEBUG ) {
    if ( !m_noId ) {
      msg() << MSG::DEBUG << "1st CombinedMuonFeature: addr=" << m_muon1
            << " pt=" << m_muon1->ptq() << " trkAddr=" << m_muon1->IDTrack();
      if ( m_muon1->IDTrack() ) msg() << MSG::DEBUG << " trackParamAddr=" << m_muon1->IDTrack()->param();
      else                      msg() << MSG::DEBUG << " trackParamAddr=0";
      msg() << MSG::DEBUG << endreq;
      msg() << MSG::DEBUG << "2nd CombinedMuonFeature: addr=" << m_muon2
            << " pt=" << m_muon2->ptq() << " trkAddr=" << m_muon2->IDTrack();
      if ( m_muon2->IDTrack() ) msg() << MSG::DEBUG << " trackParamAddr=" << m_muon2->IDTrack()->param();
      else                      msg() << MSG::DEBUG << " trackParamAddr=0";
      msg() << MSG::DEBUG << endreq;
    } else {
      msg() << MSG::DEBUG << "CombinedMuonFeature: addr=" << m_muon
            << " pt=" << m_muon->ptq() << " trkAddr=" << m_muon->IDTrack();
      if ( m_muon->IDTrack() ) msg() << MSG::DEBUG << " trackParamAddr=" << m_muon->IDTrack()->param();
      else                      msg() << MSG::DEBUG << " trackParamAddr=0";
      msg() << MSG::DEBUG << endreq;
      msg() << MSG::DEBUG << "StandaloneMuonFeature: addr=" << pMuonFeature
            << " pt=" << pMuonFeature->pt()
            << endreq;
    }
  }

  // Check if we are still in the same event
  if ( IdEvent !=  m_lastEvent ) {
    m_countTotalEvents++;
    m_lastEvent=IdEvent;
  }
  m_countTotalRoI++;

  // Get the Inner Detector tracks associated with the muons
  if ( !m_noId ) {
    if ( m_muon1->IDTrack() ) mon_Acceptance.push_back( ACCEPT_Mu1_IDTrack );
    if ( m_muon2->IDTrack() && isUnique(m_muon1->IDTrack(),m_muon2->IDTrack())) mon_Acceptance.push_back( ACCEPT_Mu2_IDTrack );
    if ( !(m_muon1->IDTrack() && m_muon2->IDTrack() && isUnique(m_muon1->IDTrack(),m_muon2->IDTrack())) ) {
      if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Missing track(s) from ID or they are the same" << endreq;
      vtxpass    = false;
      mumuIDpass = false;
      // if ( timerSvc() ) m_BmmHypTot->stop();
      // return HLT::OK;
    } else {
      mon_Acceptance.push_back( ACCEPT_MuMu_Both_IDTracks );
      // Monitoring of the combined muons w.r.t. ID tracks
      for ( int i=0; i<2; i++ ) {
        float dEtaMucombTrk = muonX[i]->IDTrack()->param()->eta()  - muonX[i]->eta();
        float dPhiMucombTrk = muonX[i]->IDTrack()->param()->phi0() - muonX[i]->phi();
        while ( dPhiMucombTrk >  M_PI ) dPhiMucombTrk -= 2*M_PI;
        while ( dPhiMucombTrk < -M_PI ) dPhiMucombTrk += 2*M_PI;
        mon_MucombTrkdR.push_back( sqrt(dEtaMucombTrk*dEtaMucombTrk + dPhiMucombTrk*dPhiMucombTrk) );
        mon_MucombPt.push_back( muonX[i]->ptq() / CLHEP::GeV );
        // Monitoring of the ID tracks
        mon_MutrkPt_wideRange.push_back( fabs(muonX[i]->IDTrack()->param()->pT()) / CLHEP::GeV );
        mon_MutrkPt          .push_back( fabs(muonX[i]->IDTrack()->param()->pT()) / CLHEP::GeV );
        mon_MutrkEta.push_back( muonX[i]->IDTrack()->param()->eta() );
        mon_MutrkPhi.push_back( muonX[i]->IDTrack()->param()->phi0() );
      }
      float dEtaMuMu = m_muon1->IDTrack()->param()->eta()  - m_muon2->IDTrack()->param()->eta(); // TODO: put opening angle calculation int the ToolBox)
      float dPhiMuMu = m_muon1->IDTrack()->param()->phi0() - m_muon2->IDTrack()->param()->phi0();
      while ( dPhiMuMu >  M_PI ) dPhiMuMu -= 2*M_PI;
      while ( dPhiMuMu < -M_PI ) dPhiMuMu += 2*M_PI;
      mon_Mutrk1Mutrk2dEta.push_back( fabs(dEtaMuMu) );
      mon_Mutrk1Mutrk2dPhi.push_back( fabs(dPhiMuMu) );
      mon_Mutrk1Mutrk2dR  .push_back( sqrt(dEtaMuMu*dEtaMuMu + dPhiMuMu*dPhiMuMu) );
      mon_SumPtMutrk12.push_back( (fabs(m_muon1->IDTrack()->param()->pT()) + fabs(m_muon2->IDTrack()->param()->pT())) / CLHEP::GeV );
    }
  } else {
    if( !(m_muon->IDTrack()) ) {
      if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Missing track(s) from ID" << endreq;
      vtxpass    = false;
      mumuIDpass = false;
      // if ( timerSvc() ) m_BmmHypTot->stop();
      // return HLT::OK;
    } else {
      mon_Acceptance.push_back( ACCEPT_CombMu_IDTrack );
      // Monitoring of the combined / standalone muons w.r.t. ID tracks
      float dEtaMucombTrk = m_muon->IDTrack()->param()->eta()  - m_muon->eta();
      float dPhiMucombTrk = m_muon->IDTrack()->param()->phi0() - m_muon->phi();
      while ( dPhiMucombTrk >  M_PI ) dPhiMucombTrk -= 2*M_PI;
      while ( dPhiMucombTrk < -M_PI ) dPhiMucombTrk += 2*M_PI;
      mon_MucombTrkdR.push_back( sqrt(dEtaMucombTrk*dEtaMucombTrk + dPhiMucombTrk*dPhiMucombTrk) );
      mon_MucombPt.push_back( m_muon->ptq() / CLHEP::GeV );
      // Monitoring of the ID tracks
      mon_MutrkPt_wideRange  .push_back( fabs(m_muon->IDTrack()->param()->pT()) / CLHEP::GeV );
      mon_MustandPt_wideRange.push_back( fabs(pMuonFeature->pt()) / CLHEP::GeV );
      mon_MutrkPt  .push_back( fabs(m_muon->IDTrack()->param()->pT()) / CLHEP::GeV );
      mon_MustandPt.push_back( fabs(pMuonFeature->pt()) / CLHEP::GeV );
      mon_MutrkEta  .push_back( m_muon->IDTrack()->param()->eta() );
      mon_MustandEta.push_back( pMuonFeature->eta() );
      mon_MutrkPhi  .push_back( m_muon->IDTrack()->param()->phi0() );
      mon_MustandPhi.push_back( pMuonFeature->phi() );
      float dEtaMuMu = m_muon->IDTrack()->param()->eta()  - pMuonFeature->eta();
      float dPhiMuMu = m_muon->IDTrack()->param()->phi0() - pMuonFeature->phi();
      while ( dPhiMuMu >  M_PI ) dPhiMuMu -= 2*M_PI;
      while ( dPhiMuMu < -M_PI ) dPhiMuMu += 2*M_PI;
      mon_MutrkMustanddEta.push_back( fabs(dEtaMuMu) );
      mon_MutrkMustanddPhi.push_back( fabs(dPhiMuMu) );
      mon_MutrkMustanddR  .push_back( sqrt(dEtaMuMu*dEtaMuMu + dPhiMuMu*dPhiMuMu) );
      mon_SumPtMutrkMustand.push_back( (fabs(m_muon->IDTrack()->param()->pT()) + fabs(pMuonFeature->pt())) / CLHEP::GeV );
    }
  }

  mon_Acceptance.push_back( ACCEPT_DiMuon_Pair );
  if ( vtxpass    ) mon_Acceptance.push_back( ACCEPT_vtxpass_is_TRUE );
  if ( mumuIDpass ) mon_Acceptance.push_back( ACCEPT_mumuIDpass_is_TRUE );

    msg() << MSG::DEBUG << "JWW: charge check - !noId" << endreq;
    if ( !m_noId ) {
        double mu1ptq, mu2ptq;
        if ( m_muon1->IDTrack() && m_muon1->IDTrack()->param() ) {
            mu1ptq = m_muon1->IDTrack()->param()->pT();
            msg() << MSG::DEBUG << "JWW: mu1ptq from IDTrack()->param()->pT(): " << mu1ptq << endreq;
            msg() << MSG::DEBUG << "JWW: " << m_muon1->charge() << "  " << m_muon1->pt() << endreq;
        } else {
            mu1ptq = m_muon1->ptq();
            msg() << MSG::DEBUG << "JWW: mu1ptq from ptq(): " << mu1ptq << endreq;
            msg() << MSG::DEBUG << "JWW: " << m_muon1->charge() << "  " << m_muon1->pt() << " "   << mu1ptq << endreq;

        }
        if ( m_muon2->IDTrack() && m_muon2->IDTrack()->param() ){
            m_muon2->IDTrack()->param()->pT();
            msg() << MSG::DEBUG << "JWW: mu2ptq from IDTrack()->param()->pT(): " << mu2ptq << endreq;
            msg() << MSG::DEBUG << "JWW: " << m_muon2->charge() << "  " << m_muon2->pt() << " " << endreq;

        } else{
            mu2ptq = m_muon2->ptq();
            msg() << MSG::DEBUG << "JWW: mu2ptq from ptq(): " << mu2ptq << endreq;
            msg() << MSG::DEBUG << "JWW: " << m_muon2->charge() << "  " << m_muon2->pt() << " "   << mu2ptq << endreq;
        }
        msg() << MSG::DEBUG << "JWW: muon1 \n" << *m_muon1 << endreq;
        msg() << MSG::DEBUG << "JWW: muon2 \n" << *m_muon2 << endreq;

    
    } // !noid
  // Check opposite charge
  if ( m_oppositeCharge ) {
    double mu1ptq, mu2ptq;
    if ( !m_noId ) {
      if ( m_muon1->IDTrack() && m_muon1->IDTrack()->param() ) mu1ptq = m_muon1->IDTrack()->param()->pT();
      else                                                     mu1ptq = m_muon1->ptq();
      if ( m_muon2->IDTrack() && m_muon2->IDTrack()->param() ) mu2ptq = m_muon2->IDTrack()->param()->pT();
      else                                                     mu2ptq = m_muon2->ptq();
    } else {
      if ( m_muon->IDTrack() && m_muon->IDTrack()->param() ) mu1ptq = m_muon->IDTrack()->param()->pT();
      else                                                   mu1ptq = m_muon->ptq();
      mu2ptq = pMuonFeature->pt();
    }
    if ( mu1ptq * mu2ptq > 0 ) {
      if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Combination discarded by opposite charge check" << endreq;
      pass = false;
      if ( timerSvc() ){
        m_BmmHypTot->stop();
        mon_TotalRunTime = m_BmmHypTot->elapsed();
      }
      return HLT::OK;
    }
  }
  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "opposite charge cut passed" << endreq;
  mon_Acceptance.push_back( ACCEPT_Opp_Charge );

  // Calculate the invariant mass
  double mass;
  if ( !m_noId ) {
    mass = invariantMass(m_muon1, m_muon2);
    mon_InvMass_comb          .push_back(mass / CLHEP::GeV);
    mon_InvMass_comb_wideRange.push_back(mass / CLHEP::GeV);
  } else {
    mass = invariantMass(m_muon, pMuonFeature);
    mon_InvMass_stand          .push_back(mass / CLHEP::GeV);
    mon_InvMass_stand_wideRange.push_back(mass / CLHEP::GeV);
  }

  // Apply the cut on the mass
  if ( mass < m_lowerMassCut || (mass > m_upperMassCut && m_ApplyupperMassCut) ) {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "Combination discarded by mass cut: " << mass << " MeV" << endreq;
    pass = false;
    if ( timerSvc() ) {
      m_BmmHypTot->stop();
      mon_TotalRunTime = m_BmmHypTot->elapsed();
    }
    return HLT::OK;
  } else {
    m_countPassedBsMass++;
  }
  mon_Acceptance.push_back( ACCEPT_InvMass_Cut );

  // Start the vertexing timer
  if ( timerSvc() ) m_BmmHypVtx->start();

  // Found pair with good mass, now make L2Bphys object
  TrigL2Bphys* trigPartBmumu = new TrigL2Bphys( 0, 0., 0., TrigL2Bphys::BMUMU, mass); // TODO: shouldn't there be RoIs parameters ?

  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << " Create Bphys particle with RoIId " << trigPartBmumu->roiId()
          << " phi, eta "    << trigPartBmumu->phi() << " " << trigPartBmumu->eta()
          << " vertex type " << trigPartBmumu->particleType() << endreq;

  // JK Add element-links to tracks
  if ( !m_noId ) {
    ElementLink<TrigInDetTrackCollection> track1EL=m_muon1->IDTrackLink();
    ElementLink<TrigInDetTrackCollection> track2EL=m_muon2->IDTrackLink();
    if ( msgLvl() <= MSG::VERBOSE ) {
      msg() << MSG::VERBOSE << "Just check track links... " << endreq;
      msg() << MSG::VERBOSE << "Track 1 pT " << (*track1EL)->param()->pT()
            << " eta: " << (*track1EL)->param()->eta()
            << " phi: " << (*track1EL)->param()->phi0() << endreq;
      msg() << MSG::VERBOSE << "Track 2 pT " << (*track2EL)->param()->pT()
            << " eta: " << (*track2EL)->param()->eta()
            << " phi: " << (*track2EL)->param()->phi0() << endreq;
    }
    trigPartBmumu->addTrack(track1EL);
    trigPartBmumu->addTrack(track2EL);
  } else {
    ElementLink<TrigInDetTrackCollection> track1EL=m_muon->IDTrackLink();
    if ( msgLvl() <= MSG::VERBOSE ){
      msg() << MSG::VERBOSE << "Just check track links... " << endreq;
      msg() << MSG::VERBOSE << "Track 1 pT " << (*track1EL)->param()->pT()
            << " eta: " << (*track1EL)->param()->eta()
            << " phi: " << (*track1EL)->param()->phi0() << endreq;
    }
    trigPartBmumu->addTrack(track1EL);
  }

  ///////////// Now do vertexing /////////////////

  if ( vtxpass ) {

    // Create the vertexer
    TrigL2Vertex* pL2V    = new TrigL2Vertex();
    TrigVertex*   p_mumuV = NULL;
    StatusCode sc;
    // Add tracks to the vertexer
    bool addTracks = true;
    for ( int i=0; i<2; i++ ) {
      if (muonX[i]->IDTrack()->chi2() < 1e7 ) {
        sc = m_vertexingTool->addTrack(muonX[i]->IDTrack(), pL2V, Trk::muon);
        if ( sc.isFailure() ) {
          if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Failed to add track " << i+1 << " to vertexingTool" << endreq;
          addTracks = false;
          mon_Errors.push_back( ERROR_AddTrack_Fails );
        }
      } else {
        addTracks = false;
        mon_Errors.push_back( ERROR_AddTrack_Fails );
      }
    }

    // DO THE VERTEX-FIT
    if ( addTracks ) {
      mon_Acceptance.push_back( ACCEPT_AddTrack_Test );
      sc = m_L2vertFitter->fit(pL2V);
    }

    // Check the result
    if ( sc.isFailure() || (!addTracks) ) {
      if ( msgLvl() <= MSG::DEBUG )msg() << MSG::DEBUG << "TrigL2VertexFitter failed" << endreq;
    } else {
      mon_Acceptance.push_back( ACCEPT_Vertexing );
      // Calculate invariant mass
      sc = m_vertexingTool->calculateInvariantMass(pL2V);
      if ( sc.isSuccess() ) {
        mon_Acceptance.push_back( ACCEPT_CalcInvMass );
        // Create mother particle
        sc = m_vertexingTool->createMotherParticle(pL2V);
        if ( sc.isSuccess() ) {
          mon_Acceptance.push_back( ACCEPT_CalcMother );
          if ( msgLvl() <= MSG::VERBOSE ) msg() << MSG::VERBOSE << "Bs created: pT "<<pL2V->m_getMotherTrack()->pT() << endreq;
        } else {
          mon_Errors.push_back( ERROR_CalcMother_Fails );
        }
        p_mumuV = m_vertexingTool->createTrigVertex(pL2V);
      } else {
        mon_Errors.push_back( ERROR_CalcInvMass_Fails );
      }
    }
    delete pL2V;

    if ( p_mumuV == NULL ) {
      if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " vertex fit failed for tracks" << endreq;
      // delete pL2V;
      delete p_mumuV;
      // delete trigPartBmumu;
      vtxpass = false;
      // continue;
    } else {
      mon_Acceptance.push_back( ACCEPT_Vertex_not_NULL );
      double chi2prob = 0.;
      chi2prob = 1.0 - Genfun::CumulativeChiSquare(p_mumuV->ndof())(p_mumuV->chi2());
      if ( msgLvl() <= MSG::DEBUG ) {
        msg() << MSG::DEBUG << "REGTEST: mumu vertex Fit: x= y= z=" << p_mumuV->x() << " " << p_mumuV->y() << " " << p_mumuV->z() << endreq;
        msg() << MSG::DEBUG << "REGTEST: Bmumu mass = " << p_mumuV->mass() << endreq;
        if (p_mumuV->massVariance() !=0) {
          msg() << MSG::DEBUG << "REGTEST: Bmumu mass pull = " << (p_mumuV->mass()-BSMASS)/p_mumuV->massVariance() << endreq;
        } else {
          msg() << MSG::DEBUG << "REGTEST: MuMu mass variance is 0 can't calculate the pull" << endreq;
          mon_Errors.push_back( ERROR_CalcMassPull_Fails );
        }
        // msg() << MSG::DEBUG << "REGTEST: Chi2 probability of the fit = " << chi2prob << endreq;
        msg() << MSG::DEBUG << "REGTEST: Chi2 vtx fit = " << p_mumuV->chi2() << endreq;
        msg() << MSG::DEBUG << "SigmaX =  SigmaY =  SigmaZ = " << sqrt(p_mumuV->cov()[0]) << " " << sqrt(p_mumuV->cov()[2]) << " " << sqrt(p_mumuV->cov()[5]) << endreq;
      }

      m_VertexColl = new TrigVertexCollection();
      m_VertexColl->push_back(p_mumuV);

      // JK 28/4/08 changes for ElementLinks
      ElementLink<TrigVertexCollection> BMuMuVertexEL(*m_VertexColl,0);
      // trigPartBmumu->pVertex(p_mumuV);
      // 14-05-08 trigPartBmumu->pVertex(BMuMuVertexEL);

      // TODO: need to add tracks as well
      trigPartBmumu->fitmass(p_mumuV->mass());
      trigPartBmumu->fitchi2(p_mumuV->chi2());
      trigPartBmumu->fitndof(p_mumuV->ndof());
      trigPartBmumu->fitx(p_mumuV->x());
      trigPartBmumu->fity(p_mumuV->y());
      trigPartBmumu->fitz(p_mumuV->z());

      // Monitoring of the fit results
      mon_FitMass_wideRange .push_back( p_mumuV->mass() / CLHEP::GeV );
      mon_FitMass           .push_back( p_mumuV->mass() / CLHEP::GeV );
      mon_InvMass_comb_okFit.push_back( mass / CLHEP::GeV );
      mon_Chi2toNDoF        .push_back( p_mumuV->chi2() / p_mumuV->ndof() );
      mon_Chi2toNDoFProb    .push_back( chi2prob );
      if ( p_mumuV->getMotherTrack() != NULL ) {
        mon_FitTotalPt.push_back( p_mumuV->getMotherTrack()->pT() / CLHEP::GeV );
        mon_SumPtMutrk12_okFit.push_back( (fabs(m_muon1->IDTrack()->param()->pT()) + fabs(m_muon2->IDTrack()->param()->pT())) / CLHEP::GeV );
      }
      mon_FitVtxR.push_back( sqrt(p_mumuV->x()*p_mumuV->x() + p_mumuV->y()*p_mumuV->y()) / CLHEP::mm );
      mon_FitVtxZ.push_back( p_mumuV->z() / CLHEP::mm );
    }
    // Stop the vertexing timer
    if ( timerSvc() ) {
      m_BmmHypVtx->stop();
      mon_VertexingTime = m_BmmHypVtx->elapsed();
    }
  }

  // Create vector for TrigL2Bphys particles
  m_trigBphysColl = new TrigL2BphysContainer();

  // Append this combination to the result
  m_trigBphysColl->push_back(trigPartBmumu);

  // Update various counters
  pass = true;
  if ( vtxpass ) {
    m_countPassedVtxFit++;
    delete m_VertexColl;
  }
  if ( mumuIDpass ) m_countPassedmumuPairs++;
  m_countPassedRoIs++;
  if(IdEvent != m_lastEventPassed) {
    m_countPassedEvents++;
    m_lastEventPassed = IdEvent;
  }
  m_VertexColl = 0;

  if ( timerSvc() ) {
    m_BmmHypTot->stop();
    mon_TotalRunTime = m_BmmHypTot->elapsed();
  }

  msg() << MSG::DEBUG << "End of acceptInput, pass is " << pass << endreq;
  return HLT::OK;
}



/*----------------------------------------------------------------------------------------*/
HLT::ErrorCode TrigL2BMuMuFex::hltExecute(HLT::TEConstVec& , HLT::TriggerElement* outputTE)
/*----------------------------------------------------------------------------------------*/
{

  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " In TrigL2BMuMu hltExecute" << endreq;

  if ((m_trigBphysColl!=0) && (m_trigBphysColl->size() > 0)) {
    mon_Acceptance.push_back( ACCEPT_BphysColl_not_Empty );
    if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "REGTEST: Store Bphys Collection size: " << m_trigBphysColl->size() << endreq;
    // std::string bphysKey ="";
    // HLT::ErrorCode sc = recordAndAttachFeature(outputTE, m_trigBphysColl, bphysKey, "L2DsPhiPiFex" );
    HLT::ErrorCode sc = attachFeature(outputTE, m_trigBphysColl, "L2BMuMuFex" );
    if(sc != HLT::OK) {
      msg()  << MSG::WARNING << "Failed to store trigBphys Collection" << endreq;
      mon_Errors.push_back( ERROR_BphysColl_Fails );
      return HLT::ERROR;
    }
  } else {
    if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "REGTEST: no bphys collection to store "  << endreq;
  }


  return HLT::OK;
}



/*-----------------------------------------------------------------------------------------------------*/
double TrigL2BMuMuFex::invariantMass(const CombinedMuonFeature* muon1, const CombinedMuonFeature* muon2)
/*-----------------------------------------------------------------------------------------------------*/
{

  // Check pointers
  // if(!(muon1->IDTrack()) || !(muon2->IDTrack())) return 0;

  double eta1,eta2,Pt1,Pt2,ph1,ph2;
  if(muon1->IDTrack()){
    eta1 = muon1->IDTrack()->param()->eta();
    Pt1  = fabs(muon1->IDTrack()->param()->pT());
    ph1  = muon1->IDTrack()->param()->phi0();
    //       std::cout<<" IDtrack-1  pT,eta,phi= "<<Pt1<<" "<<eta1<<" "<<ph1<<std::endl;
    //     } else if( muon1->muFastTrack() ) {
    //       Pt1  = fabs(muon1->pt());
    //       eta1 = muon1->muFastTrack()->eta();
    //       ph1  = muon1->muFastTrack()->phi();
    //       std::cout<<"  muFast-1  pT,eta,phi= "<<Pt1<<" "<<eta1<<" "<<ph1<<std::endl;
  } else return 0;

  if(muon2->IDTrack()){
    eta2 = muon2->IDTrack()->param()->eta();
    Pt2  = fabs(muon2->IDTrack()->param()->pT());
    ph2  = muon2->IDTrack()->param()->phi0();
    //       std::cout<<" IDtrack-2  pT,eta,phi= "<<Pt2<<" "<<eta2<<" "<<ph2<<std::endl;
    //    } else if( muon2->muFastTrack() ) {
    //       Pt2  = fabs(muon2->pt());
    //       eta2 = muon1->muFastTrack()->eta();
    //       ph2  = muon1->muFastTrack()->phi();
    //       std::cout<<" muFast-2  pT,eta,phi= "<<Pt2<<" "<<eta2<<" "<<ph2<<std::endl;
  } else return 0;

  // get parameters
  //double eta1 = muon1->IDTrack()->param()->eta();
  //double eta2 = muon2->IDTrack()->param()->eta();
  //double Pt1  = fabs(1/muon1->ptq());
  //double Pt2  = fabs(1/muon2->ptq());
  //double Pt1  = fabs(muon1->ptq());
  //double Pt2  = fabs(muon2->ptq());
  //double ph1  = muon1->IDTrack()->param()->phi0();
  //double ph2  = muon2->IDTrack()->param()->phi0();

  double the1 = 2*atan2((double)exp(-eta1),1.);
  double the2 = 2*atan2((double)exp(-eta2),1.);
  double P1   = fabs(Pt1)/sin(the1);
  double P2   = fabs(Pt2)/sin(the2);
  double m1   = 105.6;
  double m2   = 105.6;

  double p1[3],p2[3];
  p1[0]  = Pt1*cos(ph1);
  p1[1]  = Pt1*sin(ph1);
  p1[2]  = P1*cos(the1);
  p2[0]  = Pt2*cos(ph2);
  p2[1]  = Pt2*sin(ph2);
  p2[2]  = P2*cos(the2);

  // evaluate mass
  double Ptot1 = sqrt(std::pow(p1[0],2)+std::pow(p1[1],2)+std::pow(p1[2],2));
  double Ptot2 = sqrt(std::pow(p2[0],2)+std::pow(p2[1],2)+std::pow(p2[2],2));
  double e1 = sqrt(Ptot1*Ptot1+m1*m1);
  double e2 = sqrt(Ptot2*Ptot2+m2*m2);
  double mass = sqrt(m1*m1 + m2*m2 + 2*e1*e2 - 2*p1[0]*p2[0] - 2*p1[1]*p2[1] - 2*p1[2]*p2[2]);

  // std::cout<<" Itogo: Mass= "<<mass<<std::endl;

  return mass;
}

/*---------------------------------------------------------------------------------------------*/
double TrigL2BMuMuFex::invariantMass(const CombinedMuonFeature* muon1, const MuonFeature* muon2)
/*---------------------------------------------------------------------------------------------*/
{

  double eta1,eta2,Pt1,Pt2,ph1,ph2;
  if(muon1->IDTrack()){
    eta1 = muon1->IDTrack()->param()->eta();
    Pt1  = fabs(muon1->IDTrack()->param()->pT());
    ph1  = muon1->IDTrack()->param()->phi0();
    msg() << MSG::DEBUG << "in invariant mass, muon 1 pT, eta, phi " << Pt1 << " " << eta1 << " " << ph1 << endreq;
  } else return 0;

  eta2 = muon2->eta();
  Pt2  = fabs(muon2->pt())*1000.;
  ph2  = muon2->phi();
  msg() << MSG::DEBUG << "in invariant mass, muon 2 pT, eta, phi " << Pt2 << " " << eta2 << " " << ph2 << endreq;

  double the1 = 2*atan2((double)exp(-eta1),1.);
  double the2 = 2*atan2((double)exp(-eta2),1.);
  double P1   = fabs(Pt1)/sin(the1);
  double P2   = fabs(Pt2)/sin(the2);
  double m1   = 105.6;
  double m2   = 105.6;

  double p1[3],p2[3];
  p1[0]  = Pt1*cos(ph1);
  p1[1]  = Pt1*sin(ph1);
  p1[2]  = P1*cos(the1);
  p2[0]  = Pt2*cos(ph2);
  p2[1]  = Pt2*sin(ph2);
  p2[2]  = P2*cos(the2);

  // evaluate mass
  double Ptot1 = sqrt(std::pow(p1[0],2)+std::pow(p1[1],2)+std::pow(p1[2],2));
  double Ptot2 = sqrt(std::pow(p2[0],2)+std::pow(p2[1],2)+std::pow(p2[2],2));
  double e1 = sqrt(Ptot1*Ptot1+m1*m1);
  double e2 = sqrt(Ptot2*Ptot2+m2*m2);
  double mass = sqrt(m1*m1 + m2*m2 + 2*e1*e2 - 2*p1[0]*p2[0] - 2*p1[1]*p2[1] - 2*p1[2]*p2[2]);

  // std::cout<<" Itogo: Mass= "<<mass<<std::endl;
  msg() << MSG::DEBUG << "in invariant mass, mass: " << mass << endreq;

  return mass;
}



/*----------------------------------------------------------------------------------*/
bool TrigL2BMuMuFex::isUnique(const  TrigInDetTrack* id1, const  TrigInDetTrack* id2)
/*----------------------------------------------------------------------------------*/
{

 float dEta = fabs( id1->param()->eta() - id2->param()->eta() );
 float dPhi = id1->param()->phi0() - id2->param()->phi0();
 while  (dPhi >  M_PI) dPhi -= 2*M_PI;
 while  (dPhi < -M_PI) dPhi += 2*M_PI;

 if(dEta < 0.02 && fabs(dPhi) < 0.02 ) return false;
 else return true;
}

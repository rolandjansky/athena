/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// *******************************************************************
//
// NAME:     TrigL2TrkMassFex.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBphysHypo
// AUTHOR:   Julie Kirk
//
// *******************************************************************

#include <math.h>

#include "GaudiKernel/StatusCode.h"

#include "TrigL2TrkMassFex.h"

//#include "TrigMuonEvent/CombinedMuonFeature.h"

#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigInDetTrackFitPar.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"

#include "TrigInDetToolInterfaces/ITrigVertexingTool.h"
#include "TrigInDetEvent/TrigL2Vertex.h"

#include "TrigParticle/TrigL2Bphys.h"

#include "TrigBphysHypo/Constants.h"
#include "BtrigUtils.h"

// additions of xAOD objects
#include "TrigBphysHelperUtilsTool.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTrigMuon/L2StandAloneMuon.h"
#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "xAODTrigMuon/L2CombinedMuon.h"
#include "xAODTrigMuon/L2CombinedMuonContainer.h"

using namespace std;

/*------------------------------------------------------------------------------------*/
TrigL2TrkMassFex::TrigL2TrkMassFex(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::FexAlgo(name, pSvcLocator),
  m_bphysHelperTool("TrigBphysHelperUtilsTool"),
  m_vertFitter("TrigVertexFitter",this),
  m_L2vertFitter("TrigL2VertexFitter",this),
  m_vertexingTool("TrigVertexingTool",this),
  m_TotTimer(0),
  m_VtxFitTimer(0)
/*------------------------------------------------------------------------------------*/
{
    
  // Read properties - boolean switches
  declareProperty("AcceptAll"     , m_acceptAll      = false);
  declareProperty("OppositeCharge", m_oppositeCharge = true );
  declareProperty("doVertexFit"   , m_doVertexFit    = true );

    // Input list of tracks for second leg
    declareProperty("TrackCollection",m_input_trackCollectionKey="InDetTrigTrackingxAODCnv_Bphysics_FTF");
    
  // Read properties - cuts
  declareProperty("MatchL1"     , m_matchL1    = false );
  declareProperty("MuonPTthr"   , m_muonPtthr    = 4000 );
  declareProperty("TrackPTthr"   , m_trackPtthr    = 1400 );
  declareProperty("dEtaTrackRoI" , m_dEta_cut      = 0.1);
  declareProperty("dPhiTrackRoI" , m_dPhi_cut      = 0.1);
  declareProperty("DaughterMass" , m_daughterMass  = 0. );
  declareProperty("Mass_low_cut" , m_mass_low_cut  = 0. );
  declareProperty("Mass_high_cut", m_mass_high_cut = 0. );

  // Read properties - vertexing tools
  declareProperty("TrigVertexFitter"  , m_vertFitter);
  declareProperty("TrigL2VertexFitter", m_L2vertFitter);
  declareProperty("TrigVertexingTool" , m_vertexingTool, "TrigVertexingTool");

  // Variables for monitoring histograms
  declareMonitoredStdContainer("Errors"                     , m_mon_Errors                        , AutoClear);
  declareMonitoredStdContainer("Acceptance"                 , m_mon_Acceptance                    , AutoClear);
  declareMonitoredStdContainer("ROIEta"                     , m_mon_ROIEta                        , AutoClear);
  declareMonitoredStdContainer("ROIPhi"                     , m_mon_ROIPhi                        , AutoClear);
  declareMonitoredVariable(    "nTracks"                    , m_mon_nTracks );
  declareMonitoredStdContainer("TrkPt"                      , m_mon_TrkPt                         , AutoClear);
  declareMonitoredStdContainer("TrkPt_wideRange"            , m_mon_TrkPt_wideRange               , AutoClear);
  declareMonitoredStdContainer("TrkEta"                     , m_mon_TrkEta                        , AutoClear);
  declareMonitoredStdContainer("TrkPhi"                     , m_mon_TrkPhi                        , AutoClear);
  declareMonitoredStdContainer("TrkROIdEta"                 , m_mon_TrkROIdEta                    , AutoClear);
  declareMonitoredStdContainer("TrkROIdPhi"                 , m_mon_TrkROIdPhi                    , AutoClear);
  declareMonitoredStdContainer("TrkROIdR"                   , m_mon_TrkROIdR                      , AutoClear);
  declareMonitoredStdContainer("InvMassNoTrkPtCut"          , m_mon_InvMassNoTrkPtCut             , AutoClear);
  declareMonitoredStdContainer("InvMassNoTrkPtCut_wideRange", m_mon_InvMassNoTrkPtCut_wideRange   , AutoClear);
  declareMonitoredStdContainer("InvMass"                    , m_mon_InvMass                       , AutoClear);
  declareMonitoredStdContainer("InvMass_wideRange"          , m_mon_InvMass_wideRange             , AutoClear);
  declareMonitoredStdContainer("Trk1Pt"                     , m_mon_Trk1Pt                        , AutoClear);
  declareMonitoredStdContainer("Trk2Pt"                     , m_mon_Trk2Pt                        , AutoClear);
  declareMonitoredStdContainer("Trk1Eta"                    , m_mon_Trk1Eta                       , AutoClear);
  declareMonitoredStdContainer("Trk2Eta"                    , m_mon_Trk2Eta                       , AutoClear);
  declareMonitoredStdContainer("Trk1Phi"                    , m_mon_Trk1Phi                       , AutoClear);
  declareMonitoredStdContainer("Trk2Phi"                    , m_mon_Trk2Phi                       , AutoClear);
  declareMonitoredStdContainer("Trk1Trk2dEta"               , m_mon_Trk1Trk2dEta                  , AutoClear);
  declareMonitoredStdContainer("Trk1Trk2dPhi"               , m_mon_Trk1Trk2dPhi                  , AutoClear);
  declareMonitoredStdContainer("Trk1Trk2dR"                 , m_mon_Trk1Trk2dR                    , AutoClear);
  declareMonitoredStdContainer("SumPtTrk12"                 , m_mon_SumPtTrk12                    , AutoClear);
  declareMonitoredStdContainer("FitMass"                    , m_mon_FitMass                       , AutoClear);
  declareMonitoredStdContainer("FitMass_wideRange"          , m_mon_FitMass                       , AutoClear);
  declareMonitoredStdContainer("InvMass_okFit"              , m_mon_InvMass_okFit                 , AutoClear);
  declareMonitoredStdContainer("Chi2toNDoF"                 , m_mon_Chi2toNDoF                    , AutoClear);
  declareMonitoredStdContainer("Chi2toNDoFProb"             , m_mon_Chi2toNDoFProb                , AutoClear);
  declareMonitoredStdContainer("FitTotalPt"                 , m_mon_FitTotalPt                    , AutoClear);
  declareMonitoredStdContainer("SumPtTrk12_okFit"           , m_mon_SumPtTrk12_okFit              , AutoClear);
  declareMonitoredStdContainer("FitVtxR"                    , m_mon_FitVtxR                       , AutoClear);
  declareMonitoredStdContainer("FitVtxZ"                    , m_mon_FitVtxZ                       , AutoClear);
  declareMonitoredVariable(    "nBphys"                     , m_mon_nBphys );
  declareMonitoredVariable(    "TotalRunTime"               , m_mon_TotalRunTime );
  declareMonitoredVariable(    "VertexingTime"              , m_mon_VertexingTime );

  // Zero the counters
  m_lastEvent           = 999;
  m_lastEventPassed     = 999;
  m_countTotalEvents    = 0;
  m_countTotalRoI       = 0;
  m_countPassedEvents   = 0;
  m_countPassedRoIs     = 0;
  m_countPassedRoIMatch = 0;
  m_countPassedMass     = 0;

  // Initialize the collections
  m_trigBphysColl = NULL;
  m_VertexColl    = NULL;
}

/*----------------------------------*/
TrigL2TrkMassFex::~TrigL2TrkMassFex()
/*----------------------------------*/
{
  // Delete the collections
  delete m_trigBphysColl;
  delete m_VertexColl;
}

/*---------------------------------------------*/
HLT::ErrorCode TrigL2TrkMassFex::hltInitialize()
/*---------------------------------------------*/
{

  // Print out properties, cuts
  msg() << MSG::INFO << "AcceptAll               = " <<
           (m_acceptAll==true ? "True" : "False") << endmsg;
  msg() << MSG::INFO << "Apply opposite sign cut = " <<
           (m_oppositeCharge==true ? "True" : "False") << endmsg;
  msg() << MSG::INFO << "Muon pT cut        : " << m_muonPtthr << " GeV" << endmsg;
  msg() << MSG::INFO << "Track pT cut        : " << m_trackPtthr << " GeV" << endmsg;
  msg() << MSG::INFO << "Track-RoI match cuts: dEta < " << m_dEta_cut << " dPhi < " << m_dPhi_cut << endmsg;
  msg() << MSG::INFO << "Mass cuts           : " << m_mass_low_cut << " to " << m_mass_high_cut << " GeV" << endmsg;
  msg() << MSG::INFO << "Daughter mass       : " << m_daughterMass << " GeV" << endmsg;

  msg() << MSG::INFO
        << "Initialization completed successfully"
        << endmsg;

  // Retrieve the vertexing tools
  StatusCode sc = m_vertFitter.retrieve();
  if ( sc.isFailure() ) {
    msg() << MSG::FATAL << "Unable to locate TrigVertexFitter tool" << endmsg;
    return HLT::BAD_JOB_SETUP;
  } else {
    msg() << MSG::INFO << "TrigVertexFitter retrieved" << endmsg;
  }

  sc = m_L2vertFitter.retrieve();
  if ( sc.isFailure() ) {
    msg() << MSG::FATAL << "Unable to locate TrigL2VertexFitter tool" << endmsg;
    return HLT::BAD_JOB_SETUP;
  }
  else {
    msg() << MSG::INFO << "TrigL2VertexFitter retrieved" << endmsg;
  }

  sc = m_vertexingTool.retrieve();
  if ( sc.isFailure() ) {
    msg() << MSG::FATAL << "Unable to locate TrigVertexingTool tool" << endmsg;
    return HLT::BAD_JOB_SETUP;
  } else {
    msg() << MSG::INFO << "TrigVertexingTool retrieved" << endmsg;
  }
    if (m_bphysHelperTool.retrieve().isFailure()) {
        msg() << MSG::ERROR << "Can't find TrigBphysHelperUtilsTool" << endmsg;
        return HLT::BAD_JOB_SETUP;
    } else {
        if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "TrigBphysHelperUtilsTool found" << endmsg;
    }

  // Add the timers
  if ( timerSvc() ) {
    m_TotTimer    = addTimer("L2TrkMassTot");
    m_VtxFitTimer = addTimer("L2TrkMassVFit");
  }

  return HLT::OK;
}


/*-------------------------------------------*/
HLT::ErrorCode TrigL2TrkMassFex::hltFinalize()
/*-------------------------------------------*/
{

  msg() << MSG::INFO << "in finalize()" << endmsg;
  msg() << MSG::INFO << "Run on events/RoIs " << m_countTotalEvents << "/" << m_countTotalRoI << endmsg;
  msg() << MSG::INFO << "Passed events/RoIs " << m_countPassedEvents << "/" << m_countPassedRoIs << endmsg;
  msg() << MSG::INFO << "Passed RoI match " << m_countPassedRoIMatch << endmsg;
  msg() << MSG::INFO << "Passed mass cuts " << m_countPassedMass << endmsg;

  return HLT::OK;
}

// Define the bins for error-monitoring histogram  (//#define ERROR_Empty_TrackColl        3)
#define ERROR_No_EventInfo           0
#define ERROR_No_RoI                 1
#define ERROR_No_TrackColl           2
#define ERROR_No_CombMuon            3
#define ERROR_More_TrackColl         4
#define ERROR_AddTrack_Fails         5
#define ERROR_Unique_AddTrack_Fails  6
#define ERROR_CalcInvMass_Fails      7
#define ERROR_CalcMother_Fails       8
#define ERROR_CalcMassPull_Fails     9
#define ERROR_BphysColl_Fails        10

// Define the bins for acceptance-monitoring histogram
#define ACCEPT_Input                 0
#define ACCEPT_AcceptAll             1
#define ACCEPT_Got_RoI               2
#define ACCEPT_Got_TrackColl         3
#define ACCEPT_Full_TrackColl        4
#define ACCEPT_Single_TrackColl      5
#define ACCEPT_Full_IDTracks         6
#define ACCEPT_Mu1_Chi2_Cut          7
#define ACCEPT_Mu1_pT_Cut            8
#define ACCEPT_Mu1_Chi2Pt_Cuts       9
#define ACCEPT_Mu1_dEta_Cut         10
#define ACCEPT_Mu1_dPhi_Cut         11
#define ACCEPT_Mu1_dR_Cut           12
#define ACCEPT_Mu2_Chi2_Cut         13
#define ACCEPT_Mu2_pT_Cut           14
#define ACCEPT_Mu2_Chi2Pt_Cuts      15
#define ACCEPT_Opp_Charge           16
#define ACCEPT_InvMass_Cut          17
#define ACCEPT_AddTrack_Test        18
#define ACCEPT_Vertexing            19
#define ACCEPT_CalcInvMass          20
#define ACCEPT_CalcMother           21
#define ACCEPT_Vertex_not_NULL      22
// Separator                        23
#define ACCEPT_BphysColl_not_Empty  24
// Separator                        25
#define ACCEPT_Each_Mu1_Chi2_Cut    26
#define ACCEPT_Each_Mu1_pT_Cut      27
#define ACCEPT_Each_Mu1_Chi2Pt_Cuts 28
#define ACCEPT_Each_Mu1_dEta_Cut    29
#define ACCEPT_Each_Mu1_dPhi_Cut    30
#define ACCEPT_Each_Mu1_dR_Cut      31
// Separator                        32
#define ACCEPT_Each_Mu2_Chi2_Cut    33
#define ACCEPT_Each_Mu2_pT_Cut      34
#define ACCEPT_Each_Mu2_Chi2Pt_Cuts 35
#define ACCEPT_Each_Opp_Charge      36
#define ACCEPT_Each_InvMass_Cut     37
#define ACCEPT_Each_AddTrack_Test   38
#define ACCEPT_Each_Vertexing       39
#define ACCEPT_Each_CalcInvMass     40
#define ACCEPT_Each_CalcMother      41
#define ACCEPT_Each_Vertex_not_NULL 42

// /*-----------------------------------------------------------------------------------------*/
// HLT::ErrorCode TrigL2TrkMassFex::acceptInput(const HLT::TriggerElement* inputTE, bool& pass)
// /*-----------------------------------------------------------------------------------------*/
// {
// }

/*---------------------------------------------------------------------------------------------------*/
HLT::ErrorCode TrigL2TrkMassFex::hltExecute(const HLT::TriggerElement*, HLT::TriggerElement* outputTE)
/*---------------------------------------------------------------------------------------------------*/
{

  // Global flags
  bool result         = false;
  bool PassedRoIMatch = false;
  bool PassedMass     = false;

  // Processing timers
  if ( timerSvc() ) m_TotTimer->start();

  // Initialize the monitoring variables
  m_mon_nBphys        = 0;
  m_mon_TotalRunTime  = 0;
  m_mon_VertexingTime = 0;

  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " In B tag and probe FEX acceptInput" << endmsg;
  m_mon_Acceptance.push_back( ACCEPT_Input );

    // Retrieve event info
    int IdRun   = 0;
    int IdEvent = 0;
    // event info
    uint32_t runNumber(0), evtNumber(0), lbBlock(0);
    if (m_bphysHelperTool->getRunEvtLb( runNumber, evtNumber, lbBlock).isFailure()) {
        msg() << MSG::ERROR << "Error retriving EventInfo" << endmsg;
    }
    IdRun = runNumber;
    IdEvent = evtNumber;

  // Accept-All mode: temporary patch; should be done with force-accept
  if (m_acceptAll) {
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "AcceptAll property is set: taking all events" << endmsg;
    // pass = true;
    m_mon_Acceptance.push_back( ACCEPT_AcceptAll );
  }
  else {
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "AcceptAll property not set: applying selection" << endmsg;
    // pass = false;
  }

  // Get RoI descriptor
  const TrigRoiDescriptor* roiDescriptor = 0;
  if (getFeature(outputTE, roiDescriptor, "initialRoI") != HLT::OK) roiDescriptor = 0;

  if ( !roiDescriptor ) {
    if ( msgLvl() <= MSG::WARNING) msg() <<  MSG::WARNING << "No RoI for this Trigger Element! " << endmsg;
    if ( timerSvc() ) {
      m_TotTimer->stop();
      m_mon_TotalRunTime = m_TotTimer->elapsed();
    }
    m_mon_Errors.push_back( ERROR_No_RoI );
    return HLT::NAV_ERROR;
  }
  m_mon_Acceptance.push_back( ACCEPT_Got_RoI );

  // Print RoI debugging information
  if ( msgLvl() <= MSG::DEBUG ) {
    msg() << MSG::DEBUG << "Using TE(" << outputTE << ")->getId(): " << outputTE->getId()
          << "; RoI ID = " << roiDescriptor->roiId()
          << ": Eta = "    << roiDescriptor->eta()
          << ", Phi = "    << roiDescriptor->phi() << endmsg;
  }

  // RoI monitoring
  m_mon_ROIEta.push_back( roiDescriptor->eta() );
  m_mon_ROIPhi.push_back( roiDescriptor->phi() );

  // Create vector for TrigL2Bphys particles
    //  delete m_trigBphysColl;
    //  delete m_VertexColl;
    m_trigBphysColl = new xAOD::TrigBphysContainer();
    xAOD::TrigBphysAuxContainer trigBphysAuxColl;
    m_trigBphysColl->setStore(&trigBphysAuxColl);
    m_VertexColl    = new TrigVertexCollection();


  ///////////////// Get vector of tracks /////////////////

    std::vector<const xAOD::TrackParticleContainer*> vectorOfTrackCollections;

    //    typedef  ElementLinkVector<xAOD::TrackParticleContainer> ELVTrackParticles;
    //    ELVTrackParticles elvtps;
    //
    //    if(getFeaturesLinks<xAOD::TrackParticleContainer,xAOD::TrackParticleContainer>(outputTE, elvtps,m_input_trackCollectionKey)!=HLT::OK ) {
    //        if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Failed to get TrackParticleContainers feature, exiting" << endmsg;
    //        if ( timerSvc() ) m_TotTimer->stop();
    //        m_mon_Errors.push_back( ERROR_No_TrackColl );
    //        return HLT::MISSING_FEATURE; // was HLT::OK
    //    }
    //
    //    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Found TrackParticleContainer, size: " << elvtps.size() << endmsg;
    //    if(msgLvl() <= MSG::DEBUG) { // print debug
    //        for ( const auto eltp: elvtps) {
    //            if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "ELLink: "
    //                << " index: "  << eltp.index()
    //                << " sgkey: "  << eltp.dataID()
    //                << " hashkey: "<< eltp.key()
    //                << " valid: "  << eltp.isValid()
    //                << " ptr: "    << (eltp.isValid() ? *eltp : nullptr)
    //                << endmsg;
    //        }
    //        
    //    } // if debug

    
    
  HLT::ErrorCode status = getFeatures(outputTE, vectorOfTrackCollections,m_input_trackCollectionKey);

  // Were the track-collections read out ?
  if ( status != HLT::OK ) {
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Failed to get InDetTrackCollections, exiting " << endmsg;
    if ( timerSvc() ) {
      m_TotTimer->stop();
      m_mon_TotalRunTime = m_TotTimer->elapsed();
    }
    m_mon_Errors.push_back( ERROR_No_TrackColl );
    return HLT::OK;
  }
  m_mon_Acceptance.push_back( ACCEPT_Got_TrackColl );

  // Is there at least one collection ?
  if (vectorOfTrackCollections.size() == 0) {
    if ( timerSvc() ) {
      m_TotTimer->stop();
      m_mon_TotalRunTime = m_TotTimer->elapsed();
    }
//    m_mon_Errors.push_back( ERROR_Empty_TrackColl );
    return HLT::OK;
  }
  m_mon_Acceptance.push_back( ACCEPT_Full_TrackColl );

  // Is there only one collection ?
  if (vectorOfTrackCollections.size() != 1) {
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Got " << vectorOfTrackCollections.size() << " InDetTrackCollections, expected one unless this is FullScan" << endmsg;
    if ( timerSvc() ) {
      m_TotTimer->stop();
      m_mon_TotalRunTime = m_TotTimer->elapsed();
    }
    //    m_mon_Errors.push_back( ERROR_More_TrackColl );
    //return HLT::OK;
  }

  m_mon_Acceptance.push_back( ACCEPT_Single_TrackColl );

  std::vector<const xAOD::TrackParticleContainer*>::iterator pTrackColl    = vectorOfTrackCollections.begin();
  std::vector<const xAOD::TrackParticleContainer*>::iterator lastTrackColl = vectorOfTrackCollections.end();

  // JK If more than 1 track collection then this is FullScan instance. Find collection with most tarcks
  if (vectorOfTrackCollections.size() > 1) {
    std::vector<const xAOD::TrackParticleContainer*>::iterator findMaxTrackColl    = vectorOfTrackCollections.begin();
    unsigned int Ntracks=0;
    for (;findMaxTrackColl != lastTrackColl; ++ findMaxTrackColl) {
      if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "  InDetTrackCollections size, " << (*findMaxTrackColl)->size() << endmsg;
      if ((*findMaxTrackColl)->size() > Ntracks) {
          if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Largest collection so far, assume it is FullScan " << endmsg;
          Ntracks= (*findMaxTrackColl)->size();
          pTrackColl= findMaxTrackColl;
      }
    }
  }

  ////////////////////////////////////////////////////////

  // Check if we are still in the same event
  if ( IdEvent != (int) m_lastEvent ) {
    m_countTotalEvents++;
    m_lastEvent = IdEvent;
  }
  m_countTotalRoI++;

  // Get the number of tracks
  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << " Got InDetTrackCollections with " << (*pTrackColl)->size() << " tracks" << endmsg;

  // Number of tracks monitoring
  if ( (*pTrackColl)->size() > 0 ) m_mon_Acceptance.push_back( ACCEPT_Full_IDTracks );
  m_mon_nTracks = (*pTrackColl)->size();

  // Boolean flags indicating what stages were already reached (for monitoring purposes)
  std::vector<bool> flag_stages( 43, false );

  // Prepare the vertexing timer
  if ( timerSvc() ) {
    m_VtxFitTimer->start();
    m_VtxFitTimer->pause(); // resume it only for the vertexing periods
  }


  ///////////////// Get muons /////////////////
    //  const CombinedMuonFeature *muon;
    const xAOD::L2CombinedMuon *muon(nullptr);
    //ElementLink<xAOD::L2CombinedMuonContainer> muonEL;
    typedef  ElementLinkVector<xAOD::L2CombinedMuonContainer>  ELVMuons;
    ELVMuons muonEL;

  if (!m_matchL1) {
      //HLT::ErrorCode status = getFeature(outputTE, muon);
      //HLT::ErrorCode status = getFeatureLink<xAOD::L2CombinedMuonContainer,xAOD::L2CombinedMuonContainer>(outputTE,muonEL);
      HLT::ErrorCode status = getFeaturesLinks<xAOD::L2CombinedMuonContainer,xAOD::L2CombinedMuonContainer>(outputTE,muonEL);

   if ( status != HLT::OK || !muonEL.size()) {
     if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Failed to get CombinedMuonFeature, exiting " << endmsg;
     m_mon_Errors.push_back( ERROR_No_CombMuon );
       return HLT::OK;
   }
      if (muonEL.size() > 1) {
          msg() << MSG::WARNING << "Unexpected number of containers for comb feature: " << endmsg;
      }
      if (!muonEL[0].isValid()) {
          msg() << MSG::WARNING << "Invalid comb muon: " << endmsg;
          m_mon_Errors.push_back( ERROR_No_CombMuon );
          return HLT::OK;
      }

      muon = *(muonEL.at(0));
      
   if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Got CombinedMuonFeature addr=" << muon
            << " pt=" << muon->charge() *muon->charge() << " trkAddr=" << muon->idTrack()
            << " trackParamAddr=" << muon->idTrack()->track()
            << endmsg;
  }



  //////////// Loop over track collections ///////////////

  for ( ; pTrackColl != lastTrackColl;  pTrackColl++ ) {

    // Monitor the tracks that failed to be added into the vertexing procedure
    std::vector<bool> addTrackFails ( (*pTrackColl)->size(), false );

    // loop over tracks and look for match to RoI
    xAOD::TrackParticleContainer::const_iterator track1     = (*pTrackColl)->begin();
    xAOD::TrackParticleContainer::const_iterator lastTrack1 = (*pTrackColl)->end();

    ///////////// Find RoI matching track ////////////////

    for (int itrk = 0; track1 != lastTrack1; ++itrk, track1++) {

      if ( msgLvl() <= MSG::DEBUG)
        msg() << MSG::DEBUG << "track1 : " << itrk
              << " pT = "   << (*track1)->pt()
              << " phi = "  << (*track1)->phi()
              << " eta = "  << (*track1)->eta()
              << " chi2 = " << (*track1)->chiSquared() << endmsg;

      // Tracks monitoring
      m_mon_TrkPt_wideRange.push_back( fabs((*track1)->pt()) / CLHEP::GeV );
      m_mon_TrkPt .push_back( fabs((*track1)->pt()) / CLHEP::GeV );
      m_mon_TrkEta.push_back( (*track1)->eta() );
      m_mon_TrkPhi.push_back( (*track1)->phi() );

      // Get the pT and chi2 of the tracks
      float trackPt   = fabs( (*track1)->pt() );
      float trackChi2 = (*track1)->chiSquared();

      // Check pT and chi2 (monitoring only)
      if ( trackChi2 <= 1e7 ) {
        if ( !flag_stages[ ACCEPT_Mu1_Chi2_Cut ] ) {
          m_mon_Acceptance.push_back( ACCEPT_Mu1_Chi2_Cut );
          flag_stages[ ACCEPT_Mu1_Chi2_Cut ] = true;
        }
        m_mon_Acceptance.push_back( ACCEPT_Each_Mu1_Chi2_Cut );
      }

      //JK 4/5/11 changed this to use muonPtthr i.e. different pT cuts for muon and tarck
      if ( trackPt >= m_muonPtthr ) {
        if ( !flag_stages[ ACCEPT_Mu1_pT_Cut ] ){
          m_mon_Acceptance.push_back( ACCEPT_Mu1_pT_Cut );
          flag_stages[ ACCEPT_Mu1_pT_Cut ] = true;
        }
        m_mon_Acceptance.push_back( ACCEPT_Each_Mu1_pT_Cut );
      }

      // Check pT and chi2
      if ( trackPt < m_muonPtthr || trackChi2 > 1e7 ) continue;

      // Check pT+chi2 (monitoring only)
      if ( !flag_stages[ ACCEPT_Mu1_Chi2Pt_Cuts ] ) {
        m_mon_Acceptance.push_back( ACCEPT_Mu1_Chi2Pt_Cuts );
        flag_stages[ ACCEPT_Mu1_Chi2Pt_Cuts ] = true;
      }
      m_mon_Acceptance.push_back( ACCEPT_Each_Mu1_Chi2Pt_Cuts );

      // Match to RoI
      // JK 25/7/11 Add option to match to l2 muon
      double deta;
      double dphi;
      if (m_matchL1) {
        deta = (*track1)->eta()  - roiDescriptor->eta();
        dphi = (*track1)->phi() - roiDescriptor->phi();
      } else {
        deta = (*track1)->eta()  - muon->idTrack()->eta();
        dphi = (*track1)->phi() - muon->idTrack()->phi();
      }
      double absdphi = fabs(dphi);
      if ( 2.* M_PI - absdphi < absdphi ) {
        if ( dphi > 0 ) {
          dphi = dphi - 2.* M_PI;
        } else {
          dphi = 2* M_PI - absdphi;
        }
      }

      // Monitoring of the RoI matching
      m_mon_TrkROIdEta.push_back( fabs(deta) );
      m_mon_TrkROIdPhi.push_back( fabs(dphi) );
      m_mon_TrkROIdR  .push_back( sqrt(dphi*dphi+deta*deta) );

      // Check the deta and dphi cuts (monitoring only)
      if ( fabs(deta) < m_dEta_cut ) {
        if ( !flag_stages[ ACCEPT_Mu1_dEta_Cut ] ) {
          m_mon_Acceptance.push_back( ACCEPT_Mu1_dEta_Cut );
          flag_stages[ ACCEPT_Mu1_dEta_Cut ] = true;
        }
        m_mon_Acceptance.push_back( ACCEPT_Each_Mu1_dEta_Cut );
      }
      if ( fabs(dphi) < m_dPhi_cut ) {
        if ( !flag_stages[ ACCEPT_Mu1_dPhi_Cut ] ) {
          m_mon_Acceptance.push_back( ACCEPT_Mu1_dPhi_Cut );
          flag_stages[ ACCEPT_Mu1_dPhi_Cut ] = true;
        }
        m_mon_Acceptance.push_back( ACCEPT_Each_Mu1_dPhi_Cut );
      }

      // Check the deta and dphi cuts for RoI matching of the track
      if ( fabs(deta) < m_dEta_cut && fabs(dphi) < m_dPhi_cut ) {

        // Check deta+dphi cuts (monitoring only)
        if ( !flag_stages[ ACCEPT_Mu1_dR_Cut ] ) {
          m_mon_Acceptance.push_back( ACCEPT_Mu1_dR_Cut );
          flag_stages[ ACCEPT_Mu1_dR_Cut ] = true;
        }
        m_mon_Acceptance.push_back( ACCEPT_Each_Mu1_dR_Cut );

        PassedRoIMatch = true;

        // Match to RoI found
        if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Track matched RoI, now search other tracks for good mass " << endmsg;

        xAOD::TrackParticleContainer::const_iterator track2     = (*pTrackColl)->begin();
        xAOD::TrackParticleContainer::const_iterator lastTrack2 = (*pTrackColl)->end();

        ///////////// Find the second muon ///////////////

        for ( int jtrk = 0; track2 != lastTrack2; ++jtrk, track2++ ) {

          // Skip identical track
          if ( itrk == jtrk ) continue;

          if ( msgLvl() <= MSG::DEBUG)
            msg() << MSG::DEBUG << "track2 : " << jtrk
                  << " pT = "   << (*track2)->pt()
                  << " phi = "  << (*track2)->phi()
                  << " eta = "  << (*track2)->eta()
                  << " chi2 = " << (*track2)->chiSquared() << endmsg;

          // Get the pT and chi2 of the tracks
          float track2Pt   = fabs( (*track2)->pt() );
          float track2Chi2 = (*track2)->chiSquared();

          // Check pT and chi2 (monitoring only)
          if ( track2Chi2 <= 1e7 ) {
            if ( !flag_stages[ ACCEPT_Mu2_Chi2_Cut ] ) {
              m_mon_Acceptance.push_back( ACCEPT_Mu2_Chi2_Cut );
              flag_stages[ ACCEPT_Mu2_Chi2_Cut ] = true;
            }
            m_mon_Acceptance.push_back( ACCEPT_Each_Mu2_Chi2_Cut );
          }
          if ( track2Pt >= m_trackPtthr ) {
            if ( !flag_stages[ ACCEPT_Mu2_pT_Cut ] ) {
              m_mon_Acceptance.push_back( ACCEPT_Mu2_pT_Cut );
              flag_stages[ ACCEPT_Mu2_pT_Cut ] = true;
            }
            m_mon_Acceptance.push_back( ACCEPT_Each_Mu2_pT_Cut );
          }

          // Prepare the di-muon pair
            //          std::vector<const TrigInDetTrackFitPar*> inputtrks;
            std::vector<const xAOD::TrackParticle*> inputtrks;
          std::vector<double> massHypo ;
          inputtrks.push_back( (*track1) );
          inputtrks.push_back( (*track2) );
          massHypo.push_back( m_daughterMass );
          massHypo.push_back( m_daughterMass );

          // Calculate the invariant mass (hardcoded pT cut 2 GeV for monitoring purposes)
          double Mass = 0;
          if ( track2Chi2 <= 1e7 ) {
            if ( track2Pt > 2000. ) {
                // Mass = InvMass(inputtrks, massHypo);
                Mass = m_bphysHelperTool->invariantMass(inputtrks,massHypo);
              m_mon_InvMassNoTrkPtCut.push_back( Mass / CLHEP::GeV );
              m_mon_InvMassNoTrkPtCut_wideRange.push_back( Mass / CLHEP::GeV );
            } else if ( track2Pt >= m_trackPtthr ) {
                //Mass = InvMass(inputtrks, massHypo);
                Mass = m_bphysHelperTool->invariantMass(inputtrks,massHypo);
            }
          }

          // Check pT and chi2
          if ( track2Pt < m_trackPtthr || track2Chi2 > 1e7 ) continue;

          // Check pT+chi2 (monitoring only)
          if ( !flag_stages[ ACCEPT_Mu2_Chi2Pt_Cuts ] ) {
            m_mon_Acceptance.push_back( ACCEPT_Mu2_Chi2Pt_Cuts );
            flag_stages[ ACCEPT_Mu2_Chi2Pt_Cuts ] = true;
          }
          m_mon_Acceptance.push_back( ACCEPT_Each_Mu2_Chi2Pt_Cuts );

          ///////// Check the di-muon properties /////////

          // Check opposite charge
          if ( m_oppositeCharge ) {
            if (( (*track1)->charge() > 0. && (*track2)->charge() > 0. ) ||
                ( (*track1)->charge() < 0. && (*track2)->charge() < 0. )) continue;
            if ( msgLvl() <= MSG::VERBOSE ) msg() << MSG::VERBOSE << "Tracks " << itrk << " and " << jtrk << " are opposite sign, make mass cuts " << endmsg;
          } else {
            if ( msgLvl() <= MSG::VERBOSE ) msg() << MSG::VERBOSE << "opposite sign cuts not applied, make mass cuts " << endmsg;
          }

          // Check opposite charge (monitoring only)
          if ( !flag_stages[ ACCEPT_Opp_Charge ] ) {
            m_mon_Acceptance.push_back( ACCEPT_Opp_Charge );
            flag_stages[ ACCEPT_Opp_Charge ] = true;
          }
          m_mon_Acceptance.push_back( ACCEPT_Each_Opp_Charge );

          // Check the invariant mass cut
          if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " mass =  " << Mass << endmsg;
          m_mon_InvMass.push_back( Mass / CLHEP::GeV );
          m_mon_InvMass_wideRange.push_back( Mass / CLHEP::GeV );
          if (Mass < m_mass_low_cut || Mass > m_mass_high_cut) continue;

          // Check the invariant mass cut (monitoring only)
          if ( !flag_stages[ ACCEPT_InvMass_Cut ] ) {
            m_mon_Acceptance.push_back( ACCEPT_InvMass_Cut );
            flag_stages[ ACCEPT_InvMass_Cut ] = true;
          }
          m_mon_Acceptance.push_back( ACCEPT_Each_InvMass_Cut );

          PassedMass = true;

          // Monitoring of the di-muon track pairs
          m_mon_Trk1Pt .push_back( fabs((*track1)->pt()) / CLHEP::GeV );
          m_mon_Trk2Pt .push_back( fabs((*track2)->pt()) / CLHEP::GeV );
          m_mon_Trk1Eta.push_back( (*track1)->eta() );
          m_mon_Trk2Eta.push_back( (*track2)->eta() );
          m_mon_Trk1Phi.push_back( (*track1)->phi() );
          m_mon_Trk2Phi.push_back( (*track2)->phi() );
          m_mon_SumPtTrk12.push_back ( (fabs((*track1)->pt()) + fabs((*track2)->pt())) / CLHEP::GeV );

          // Monitoring of the opening between the two tracks
          double dTrkEta = (*track1)->eta()  - (*track2)->eta();
          double dTrkPhi = (*track1)->phi() - (*track2)->phi();
          double absdTrkPhi = fabs( dTrkPhi );
          if ( 2.* M_PI - absdTrkPhi < absdTrkPhi ) {
            if ( dTrkPhi > 0 ) {
              dTrkPhi = dTrkPhi - 2.* M_PI;
            } else {
              dTrkPhi = 2* M_PI - absdTrkPhi;
            }
          }
          m_mon_Trk1Trk2dEta.push_back( fabs(dTrkEta) );
          m_mon_Trk1Trk2dPhi.push_back( fabs(dTrkPhi) );
          m_mon_Trk1Trk2dR  .push_back( sqrt(dTrkPhi*dTrkPhi+dTrkEta*dTrkEta) );

          // Found pair with good mass , now make L2Bphys object
            //          TrigL2Bphys* trigL2Bphys = new TrigL2Bphys((*pTrackColl)->RoI_ID(),
            //                                                     roiDescriptor->eta(),
            //                                                     roiDescriptor->phi(),
            //                                                     TrigL2Bphys::JPSIMUMU,
            //                                                     Mass);
            xAOD::TrigBphys* trigL2Bphys = new xAOD::TrigBphys();
            m_trigBphysColl->push_back(trigL2Bphys);
            trigL2Bphys->initialise(0,//(*pTrackColl)->RoI_ID(), #FIXME JW
                                    roiDescriptor->eta(),
                                    roiDescriptor->phi(),
                                    xAOD::TrigBphys::JPSIMUMU,
                                    Mass,
                                    xAOD::TrigBphys::L2);
            
            
          if ( msgLvl() <= MSG::DEBUG)
            msg() << MSG::DEBUG << "Create Bphys particle with roIId " << trigL2Bphys->roiId() << " mass " << Mass
                  << " phi, eta " << trigL2Bphys->phi() << " " << trigL2Bphys->eta() << " vertex type " << trigL2Bphys->particleType() << endmsg;

          // Store links to the two tracks forming the di-muon
            ElementLink<xAOD::TrackParticleContainer> track1EL(*(*pTrackColl),itrk);
          ElementLink<xAOD::TrackParticleContainer> track2EL(*(*pTrackColl),jtrk);
          trigL2Bphys->addTrackParticleLink(track1EL);
          trigL2Bphys->addTrackParticleLink(track2EL);

          // Set result here if mass cut is passed. Currently no vertex requirement. Need to determine what is needed.
          result = true;

          ///////////// Now do vertexing /////////////////

          if ( m_doVertexFit ) {

            // Vertexing time processing
            if ( timerSvc() ) m_VtxFitTimer->resume();

            // Create the vertexer
            TrigL2Vertex* pL2V     = new TrigL2Vertex();
            TrigVertex*   p_vertex = NULL;

            // Add tracks to the vertexer
            bool addTracks = true;
            StatusCode sc = m_vertexingTool->addTrack( (*track1)->track(), pL2V, Trk::muon );
            if ( sc.isFailure() ) {
              if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Failed to add track 1 to vertexingTool pT, chi2 " << (*track1)->pt() << " " << (*track1)->chiSquared() << endmsg;
              addTracks = false;
              // Monitoring only
              m_mon_Errors.push_back( ERROR_AddTrack_Fails );
              if ( !addTrackFails[itrk] ) {
                addTrackFails[itrk] = true;
                m_mon_Errors.push_back( ERROR_Unique_AddTrack_Fails );
              }
            }
            sc = m_vertexingTool->addTrack( (*track2)->track(), pL2V, Trk::muon );
            if ( sc.isFailure() ) {
              if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Failed to add track 2 to vertexingTool pT, chi2 " << (*track2)->pt() << " " << (*track2)->chiSquared() << endmsg;
              addTracks = false;
              // Monitoring only
              m_mon_Errors.push_back( ERROR_AddTrack_Fails );
              if ( !addTrackFails[jtrk] ) {
                addTrackFails[jtrk] = true;
                m_mon_Errors.push_back( ERROR_Unique_AddTrack_Fails );
              }
            }

            //bool vtx_created = false;

            if ( addTracks ) {

              // Passed Add-tracks test (monitoring only)
              if ( !flag_stages[ ACCEPT_AddTrack_Test ] ) {
                m_mon_Acceptance.push_back( ACCEPT_AddTrack_Test );
                flag_stages[ ACCEPT_AddTrack_Test ] = true;
              }
              m_mon_Acceptance.push_back( ACCEPT_Each_AddTrack_Test );

              // DO THE VERTEX-FIT
              sc = m_L2vertFitter->fit(pL2V);

              // Check the result
              if ( sc.isFailure() ) {
                if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "TrigL2VertexFitter failed" << endmsg;
              } else {
                // Passed vertexing (monitoring only)
                if ( !flag_stages[ ACCEPT_Vertexing ] ) {
                  m_mon_Acceptance.push_back( ACCEPT_Vertexing );
                  flag_stages[ ACCEPT_Vertexing ] = true;
                }
                m_mon_Acceptance.push_back( ACCEPT_Each_Vertexing );
                // Calculate invariant mass
                sc = m_vertexingTool->calculateInvariantMass(pL2V);
                if ( sc.isSuccess() ) {
                  // Calc. Inv. Mass succeeded (monitoring only)
                  if ( !flag_stages[ ACCEPT_CalcInvMass ] ) {
                    m_mon_Acceptance.push_back( ACCEPT_CalcInvMass );
                    flag_stages[ ACCEPT_CalcInvMass ] = true;
                  }
                  m_mon_Acceptance.push_back( ACCEPT_Each_CalcInvMass );
                  // Create mother particle
                  sc = m_vertexingTool->createMotherParticle(pL2V);
                  if ( sc.isSuccess() ) {
                    // Creating mother particle succeeded (monitoring only)
                    if ( !flag_stages[ ACCEPT_CalcMother ] ) {
                      m_mon_Acceptance.push_back( ACCEPT_CalcMother );
                      flag_stages[ ACCEPT_CalcMother ] = true;
                    }
                    m_mon_Acceptance.push_back( ACCEPT_Each_CalcMother );
                    //vtx_created = true;
                    if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "Mother particle created: pT " << pL2V->m_getMotherTrack()->pT() << endmsg;
                  } else {
                    m_mon_Errors.push_back( ERROR_CalcMother_Fails );
                  }
                  p_vertex = m_vertexingTool->createTrigVertex(pL2V);
                } else {
                  m_mon_Errors.push_back( ERROR_CalcInvMass_Fails );
                }
              }
            } // end of if (addTracks)
            delete pL2V;

            // Test whether the vertex-fit was successful
            if ( p_vertex == NULL ) {

              if ( msgLvl() <= MSG::DEBUG)
                msg() << MSG::DEBUG << " vertex fit failed for tracks  " << itrk << " and " << jtrk << endmsg;

              // delete pL2V;
//               delete p_vertex;
              // delete trigL2Bphys;
              // continue;

            } else {

              // Monitoring of p_vertex != NULL
              if ( !flag_stages[ ACCEPT_Vertex_not_NULL ] ) {
                m_mon_Acceptance.push_back( ACCEPT_Vertex_not_NULL );
                flag_stages[ ACCEPT_Vertex_not_NULL ] = true;
              }
              m_mon_Acceptance.push_back( ACCEPT_Each_Vertex_not_NULL );

              // Some debugging information
              if ( msgLvl() <= MSG::VERBOSE)
                msg() << MSG::VERBOSE << "vertex fit successful: x= y= z=" << p_vertex->x() << " " << p_vertex->y() << " " << p_vertex->z() << endmsg;
              if ( msgLvl() <= MSG::DEBUG)
                msg() << MSG::DEBUG << "vertex fit mass = " << p_vertex->mass() << " chi2 " << p_vertex->chi2() << " ndof " << p_vertex->ndof() <<  endmsg;
              if (p_vertex->massVariance() != 0) {
                if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "Mass pull = " << (p_vertex->mass()-JPSIMASS)/p_vertex->massVariance() << endmsg;
              } else {
                if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "Phi mass variance is zero, can't calculate the pull" << endmsg;
                m_mon_Errors.push_back( ERROR_CalcMassPull_Fails );
              }

              double chi2prob = 1.0 - Genfun::CumulativeChiSquare(p_vertex->ndof())(p_vertex->chi2());

              if ( msgLvl() <= MSG::VERBOSE ) {
                msg() << MSG::VERBOSE << "Chi2 probability of the fit = " << chi2prob << endmsg;
                msg() << MSG::VERBOSE << "SigmaX =  SigmaY =  SigmaZ = "
                                      << sqrt(p_vertex->cov()[0]) << " "
                                      << sqrt(p_vertex->cov()[2]) << " "
                                      << sqrt(p_vertex->cov()[5]) << endmsg;
              }

              // Add vertex results to L2Bphys
              trigL2Bphys->setFitmass( p_vertex->mass() );
              trigL2Bphys->setFitchi2( p_vertex->chi2() );
              trigL2Bphys->setFitndof( p_vertex->ndof() );
              trigL2Bphys->setFitx( p_vertex->x() );
              trigL2Bphys->setFity( p_vertex->y() );
              trigL2Bphys->setFitz( p_vertex->z() );

              m_VertexColl->push_back(p_vertex);
              if ( msgLvl() <= MSG::VERBOSE ) msg() << MSG::VERBOSE << "added vertex to vertex collection" << endmsg;

              // Monitoring of the fit results
              m_mon_FitMass_wideRange.push_back( p_vertex->mass() / CLHEP::GeV );
              m_mon_FitMass          .push_back( p_vertex->mass() / CLHEP::GeV );
              m_mon_InvMass_okFit    .push_back( Mass / CLHEP::GeV );
              m_mon_Chi2toNDoF       .push_back( p_vertex->chi2() / p_vertex->ndof() );
              m_mon_Chi2toNDoFProb   .push_back( chi2prob );
              if ( p_vertex->getMotherTrack() != NULL ) {
                m_mon_FitTotalPt.push_back( p_vertex->getMotherTrack()->pT() / CLHEP::GeV );
                m_mon_SumPtTrk12_okFit.push_back( (fabs((*track1)->pt()) + fabs((*track2)->pt())) / CLHEP::GeV );
              }
              m_mon_FitVtxR.push_back( sqrt(p_vertex->x()*p_vertex->x() + p_vertex->y()*p_vertex->y()) / CLHEP::mm );
              m_mon_FitVtxZ.push_back( p_vertex->z() / CLHEP::mm );

            } // end if vertex fit successful

            // Pause the vertexing time monitoring
            if ( timerSvc() ) m_VtxFitTimer->pause();

          } // end if do vertex fit

          // Store L2Bphys to the output collection
          if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Store L2Bphys and move onto next track" << endmsg;
          // moved to next to creation point m_trigBphysColl->push_back(trigL2Bphys);

        } // end loop over track 2, looking for track pair with good mass
      } // if pass cut on deta, dphi match to RoI
    } // end loop over track1 looking for RoI match
  } // end loop over track collections

  // Check the size of the bphys-objects collection
  if ( m_trigBphysColl->size() > 0 ) m_mon_Acceptance.push_back( ACCEPT_BphysColl_not_Empty );
  m_mon_nBphys = m_trigBphysColl->size();

  // Update various counters
  if ( PassedRoIMatch ) m_countPassedRoIMatch++;
  if ( PassedMass     ) m_countPassedMass++;
  if ( result ) {
    m_countPassedRoIs++;
    if (IdEvent!= (int) m_lastEventPassed) {
      m_countPassedEvents++;
      m_lastEventPassed=IdEvent;
    }
  }

  // Some debugging information
  if ( msgLvl() <= MSG::DEBUG) {
    /*if ( m_VertexColl    != NULL )*/ msg() << MSG::DEBUG << " vertex collection size : " << m_VertexColl->size() << " m_VertexColl " << m_VertexColl << endmsg;
    /*if ( m_trigBphysColl != NULL )*/ msg() << MSG::DEBUG << "Bphys collection size : " << m_trigBphysColl->size() << " m_trigBphysColl " << m_trigBphysColl << endmsg;
  }

  // Reach this point successfully
  if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Run: " << IdRun << " Event: " << IdEvent << "  result is " << result << endmsg;

  // if (!m_acceptAll) pass = result;

  // Stop the processing timers
  if ( timerSvc() ) {
    m_TotTimer   ->stop();
    m_VtxFitTimer->resume();
    m_VtxFitTimer->stop();
    // Fill the monitoring variables
    m_mon_TotalRunTime  = m_TotTimer   ->elapsed();
    m_mon_VertexingTime = m_VtxFitTimer->elapsed();
  }

  if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "m_VertexColl " << m_VertexColl << "    m_trigBphysColl " << m_trigBphysColl << endmsg;

  delete m_VertexColl;
  m_VertexColl = 0;

  // Attach the resulting bphys-objects to the output trigger element
  if ( /*( m_trigBphysColl != 0 ) &&*/ ( m_trigBphysColl->size() > 0 ) ) {
    if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST: Store Bphys Collection size: " << m_trigBphysColl->size() << endmsg;
    HLT::ErrorCode sc = attachFeature(outputTE, m_trigBphysColl, "L2TrackMass" );
    if ( sc != HLT::OK ) {
      msg() << MSG::WARNING << "Failed to store trigBphys Collection" << endmsg;
      m_mon_Errors.push_back( ERROR_BphysColl_Fails );
      return sc;
    }
  } else {
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "REGTEST: no bphys collection to store " << endmsg;
    delete m_trigBphysColl;
  }
  m_trigBphysColl = nullptr;

  return HLT::OK;
}


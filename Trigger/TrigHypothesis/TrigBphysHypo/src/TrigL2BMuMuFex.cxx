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

//#include "TrigMuonEvent/CombinedMuonFeature.h"

#include "TrigL2BMuMuFex.h"

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
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTrigMuon/L2StandAloneMuon.h"
#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "TrigBphysHelperUtilsTool.h"

//class ISvcLocator;


/*--------------------------------------------------------------------------------*/
TrigL2BMuMuFex::TrigL2BMuMuFex(const std::string & name, ISvcLocator* pSvcLocator):
HLT::ComboAlgo(name, pSvcLocator),
m_muon1(0),
m_muon2(0),
m_L2vertFitter("TrigL2VertexFitter",this),
m_vertexingTool("TrigVertexingTool",this),
m_BmmHypTot(0),
m_BmmHypVtx(0),
// counters
m_lastEvent(-1),
m_lastEventPassed(-1),
m_countTotalEvents(0),
m_countTotalRoI(0),
m_countPassedEvents(0),
m_countPassedRoIs(0),
m_countPassedmumuPairs(0),
m_countPassedBsMass(0),
m_countPassedVtxFit(0),

m_massMuon(105.6583715)
/*--------------------------------------------------------------------------------*/
{
    
    // Read properties - boolean switches
    declareProperty("AcceptAll"        , m_acceptAll         = true );
    declareProperty("AcceptSameMuon"   , m_acceptSameMuon    = false);
    declareProperty("OppositeSign"     , m_oppositeCharge    = true );
    declareProperty("SameSign"         , m_sameCharge        = false );
    declareProperty("ApplyUpperMassCut", m_ApplyupperMassCut = true );
    declareProperty("doVertexFit"      , m_doVertexFit       = true );
    declareProperty("noId"             , m_noId              = false);
    
    // Read properties - mass widnow cuts
    declareProperty("LowerMassCut", m_lowerMassCut =  2000.0 );
    declareProperty("UpperMassCut", m_upperMassCut = 10000.0 );
    
    // Read properties - vertexing tools
    declareProperty("TrigL2VertexFitter", m_L2vertFitter );
    declareProperty("TrigVertexingTool" , m_vertexingTool, "TrigVertexingTool" );
    
    declareProperty("MassTrack" , m_massMuon, "Mass of the muon" );
    declareProperty("L2CombinedMuonIDTrackKey",m_combinedMuonIDTrackKey="InDetTrigTrackingxAODCnv_Muon_FTF");
    declareProperty("L2CombinedMuonKey",       m_combinedMuonKey       ="MuonL2CBInfo");
    declareProperty("StandAloneMuonKey",       m_standaloneMuonKey     ="MuonL2SAInfo");

    
    // Variables for monitoring histograms
    declareMonitoredStdContainer("Errors"                 , m_mon_Errors                  , AutoClear);
    declareMonitoredStdContainer("Acceptance"             , m_mon_Acceptance              , AutoClear);
    declareMonitoredStdContainer("ROIEta"                 , m_mon_ROIEta                  , AutoClear);
    declareMonitoredStdContainer("ROIPhi"                 , m_mon_ROIPhi                  , AutoClear);
    declareMonitoredStdContainer("Roi1Roi2dEta"           , m_mon_Roi1Roi2dEta            , AutoClear);
    declareMonitoredStdContainer("Roi1Roi2dPhi"           , m_mon_Roi1Roi2dPhi            , AutoClear);
    declareMonitoredStdContainer("Roi1Roi2dR"             , m_mon_Roi1Roi2dR              , AutoClear);
    // - two combined muons
    declareMonitoredStdContainer("MucombROIdR"            , m_mon_MucombROIdR             , AutoClear);
    declareMonitoredStdContainer("MucombTrkdR"            , m_mon_MucombTrkdR             , AutoClear);
    declareMonitoredStdContainer("MucombPt"               , m_mon_MucombPt                , AutoClear);
    declareMonitoredStdContainer("MutrkPt"                , m_mon_MutrkPt                 , AutoClear);
    declareMonitoredStdContainer("MutrkPt_wideRange"      , m_mon_MutrkPt_wideRange       , AutoClear);
    declareMonitoredStdContainer("MutrkEta"               , m_mon_MutrkEta                , AutoClear);
    declareMonitoredStdContainer("MutrkPhi"               , m_mon_MutrkPhi                , AutoClear);
    declareMonitoredStdContainer("Mutrk1Mutrk2dEta"       , m_mon_Mutrk1Mutrk2dEta        , AutoClear);
    declareMonitoredStdContainer("Mutrk1Mutrk2dPhi"       , m_mon_Mutrk1Mutrk2dPhi        , AutoClear);
    declareMonitoredStdContainer("Mutrk1Mutrk2dR"         , m_mon_Mutrk1Mutrk2dR          , AutoClear);
    declareMonitoredStdContainer("SumPtMutrk12"           , m_mon_SumPtMutrk12            , AutoClear);
    declareMonitoredStdContainer("InvMass_comb"           , m_mon_InvMass_comb            , AutoClear);
    declareMonitoredStdContainer("InvMass_comb_wideRange" , m_mon_InvMass_comb_wideRange  , AutoClear);
    declareMonitoredStdContainer("FitMass"                , m_mon_FitMass                 , AutoClear);
    declareMonitoredStdContainer("FitMass_wideRange"      , m_mon_FitMass_wideRange       , AutoClear);
    declareMonitoredStdContainer("InvMass_comb_okFit"     , m_mon_InvMass_comb_okFit      , AutoClear);
    declareMonitoredStdContainer("Chi2toNDoF"             , m_mon_Chi2toNDoF              , AutoClear);
    //  declareMonitoredStdContainer("Chi2toNDoFProb"         , m_mon_Chi2toNDoFProb          , AutoClear);
    declareMonitoredStdContainer("FitTotalPt"             , m_mon_FitTotalPt              , AutoClear);
    declareMonitoredStdContainer("SumPtMutrk12_okFit"     , m_mon_SumPtMutrk12_okFit      , AutoClear);
    declareMonitoredStdContainer("FitVtxR"                , m_mon_FitVtxR                 , AutoClear);
    declareMonitoredStdContainer("FitVtxZ"                , m_mon_FitVtxZ                 , AutoClear);
    declareMonitoredVariable(    "VertexingTime"          , m_mon_VertexingTime );
    declareMonitoredVariable(    "TotalRunTime"          , m_mon_TotalRunTime );
    //  // - one combined + one standalone muon
    //  declareMonitoredStdContainer("MustandROIdR"           , m_mon_MustandROIdR            , AutoClear);
    //  declareMonitoredStdContainer("MustandPt"              , m_mon_MustandPt               , AutoClear);
    //  declareMonitoredStdContainer("MustandPt_wideRange"    , m_mon_MustandPt_wideRange     , AutoClear);
    //  declareMonitoredStdContainer("MustandEta"             , m_mon_MustandEta              , AutoClear);
    //  declareMonitoredStdContainer("MustandPhi"             , m_mon_MustandPhi              , AutoClear);
    //  declareMonitoredStdContainer("MutrkMustanddEta"       , m_mon_MutrkMustanddEta        , AutoClear);
    //  declareMonitoredStdContainer("MutrkMustanddPhi"       , m_mon_MutrkMustanddPhi        , AutoClear);
    //  declareMonitoredStdContainer("MutrkMustanddR"         , m_mon_MutrkMustanddR          , AutoClear);
    //  declareMonitoredStdContainer("SumPtMutrkMustand"      , m_mon_SumPtMutrkMustand       , AutoClear);
    //  declareMonitoredStdContainer("InvMass_stand"          , m_mon_InvMass_stand           , AutoClear);
    //  declareMonitoredStdContainer("InvMass_stand_wideRange", m_mon_InvMass_stand_wideRange , AutoClear);
    
    // Initialize the collections
    //  m_trigBphysColl = NULL;
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
        msg() << MSG::DEBUG << "Initialization ..." << endmsg;
        msg() << MSG::DEBUG << "AcceptAll          = "
        << (m_acceptAll==true ? "True" : "False") << endmsg;
        msg() << MSG::DEBUG << "noId          = "
        << (m_noId==true ? "True" : "False") << endmsg;
        msg() << MSG::DEBUG << "OppositeCharge     = "
        << (m_oppositeCharge==true ? "True" : "False") << endmsg;
        msg() << MSG::DEBUG << "SameCharge     = "
        << (m_sameCharge==true ? "True" : "False") << endmsg;
        msg() << MSG::DEBUG << "LowerMassCut       = " << m_lowerMassCut << endmsg;
        msg() << MSG::DEBUG << "UpperMassCut       = " << m_upperMassCut << endmsg;
        msg() << MSG::DEBUG << "ApplyUpperMassCut  = " << m_ApplyupperMassCut << endmsg;
    }
    
    // Consistency check of charge requirements
    if( m_oppositeCharge && m_sameCharge ) {
      msg() << MSG::ERROR << "Bad configuration: OppositeCharge and SameCharge are required together" << endmsg;
      return HLT::BAD_JOB_SETUP;
    }
    
    // Retrieving the vertex fitting tool
    // StatusCode sc = toolSvc()->retrieveTool("TrigVertexFitter","TrigVertexFitter",m_vertFitter);
    // if ( sc.isFailure() ) {
    //   msg() << MSG::FATAL << "Unable to locate TrigVertexFitter tool " << endmsg;
    //   return HLT::BAD_JOB_SETUP;
    // }
    
    StatusCode sc = m_L2vertFitter.retrieve();
    if ( sc.isFailure() ) {
        msg() << MSG::FATAL << "Unable to locate TrigL2VertexFitter tool " << endmsg;
        return HLT::BAD_JOB_SETUP;
    }
    else {
        msg() << MSG::INFO << "TrigL2VertexFitter retrieved"<< endmsg;
    }
    
    sc = m_vertexingTool.retrieve();
    if ( sc.isFailure() ) {
        msg() << MSG::FATAL << "Unable to locate TrigVertexingTool tool " << endmsg;
        return HLT::BAD_JOB_SETUP;
    } else {
        msg() << MSG::INFO << "TrigVertexingTool retrieved"<< endmsg;
    }
    
    if (m_bphysHelperTool.retrieve().isFailure()) {
        msg() << MSG::ERROR << "Can't find TrigBphysHelperUtilsTool" << endmsg;
        return HLT::BAD_JOB_SETUP;
    } else {
        if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "TrigBphysHelperUtilsTool found" << endmsg;
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
    
    msg() << MSG::INFO << "in finalize()" << endmsg;
    MsgStream log(msgSvc(), name());
    
    msg() << MSG::INFO << "|----------------------- SUMMARY FROM TrigL2BMuMuFex -------------|" << endmsg;
    msg() << MSG::INFO << "Run on events/2xRoIs " << m_countTotalEvents << "/" << m_countTotalRoI <<  endmsg;
    msg() << MSG::INFO << "Passed events/2xRoIs " << m_countPassedEvents << "/" << m_countPassedRoIs <<  endmsg;
    msg() << MSG::INFO << "RoIs Passed MuMu pairs: "  << m_countPassedmumuPairs << endmsg;
    msg() << MSG::INFO << "RoIs Passed BsMass: "  << m_countPassedBsMass << endmsg;
    msg() << MSG::INFO << "RoIs Passed Vtx Fit: "  << m_countPassedVtxFit << endmsg;
    
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
    if (m_resultHolder.size()) {
        if ( msgLvl() <= MSG::WARNING ) msg() << MSG::DEBUG << "m_resultHolder not empty" << endmsg;
        
    }
    m_resultHolder.clear();
    
    // Initialize the monitoring variables
    m_mon_TotalRunTime = 0;
    m_mon_VertexingTime = 0;
    
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Running TrigL2BMuMuFex::acceptInputS" << endmsg;
    m_mon_Acceptance.push_back( ACCEPT_Input );
    
    if ( m_acceptAll ) {
        pass = true;
        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Accept property is set: taking all the events" << endmsg;
        return HLT::OK;
    }
    
    // Processing timers
    if ( timerSvc() ) m_BmmHypTot->start();
    
    // Retrieve event info
    //int IdRun   = 0;
    int IdEvent = 0;
    // event info
    uint32_t runNumber(0), evtNumber(0), lbBlock(0);
    if (m_bphysHelperTool->getRunEvtLb( runNumber, evtNumber, lbBlock).isFailure()) {
        msg() << MSG::ERROR << "Error retriving EventInfo" << endmsg;
    }
    //IdRun = runNumber;
    IdEvent = evtNumber;
    
    
    
    // Check consistency of the number of input Trigger Elements
    if ( inputTE.size() != 2 ) {
        msg() << MSG::ERROR << "Got different than 2 number of input TEs" << endmsg;
        if ( timerSvc() ) {
            m_BmmHypTot->stop();
            m_mon_TotalRunTime = m_BmmHypTot->elapsed();
        }
        m_mon_Errors.push_back( ERROR_Not_2_Input_TE );
        return HLT::BAD_JOB_SETUP;
    }
    
    // Get the RoIs from the navigation
    const TrigRoiDescriptor *roi[2] = { 0, 0 };
    for ( int i=0; i<2; i++ ) {
        if ( getFeature(inputTE.at(i), roi[i]) != HLT::OK ) {
            msg() << MSG::ERROR << "Navigation error while getting RoI descriptor " << i+1 << endmsg;
            if ( timerSvc() ) {
                m_BmmHypTot->stop();
                m_mon_TotalRunTime = m_BmmHypTot->elapsed();
            }
            if ( !i ) m_mon_Errors.push_back( ERROR_No_RoI_1 );
            else      m_mon_Errors.push_back( ERROR_No_RoI_2 );
            return HLT::NAV_ERROR;
        }
        if ( roi[i] == NULL ) {
            msg() << MSG::ERROR << "Navigation error while getting RoI descriptor " << i+1 << " (null pointer)" << endmsg;
            if ( timerSvc() ) {
                m_BmmHypTot->stop();
                m_mon_TotalRunTime = m_BmmHypTot->elapsed();
            }
            if ( !i ) m_mon_Errors.push_back( ERROR_No_RoI_1 );
            else      m_mon_Errors.push_back( ERROR_No_RoI_2 );
            return HLT::NAV_ERROR;
        }
    }
    
    m_mon_Acceptance.push_back( ACCEPT_Got_RoIs );
    
    // Print RoIs debugging information
    if ( msgLvl() <= MSG::DEBUG ) {
        msg() << MSG::DEBUG << "Using inputTEs: " << inputTE.front()->getId() << " AND " << inputTE.back()->getId()
        << "; RoI IDs = "   << roi[0]->roiId() << " AND "    << roi[1]->roiId()
        << ": Eta1 = "      << roi[0]->eta()  << " Eta2 = " << roi[1]->eta()
        << ", Phi1 = "      << roi[0]->phi()  << " Phi2 = " << roi[1]->phi()
        << endmsg;
    }
    
    // Monitoring of the RoIs opening angle
    m_mon_ROIEta.push_back( roi[0]->eta() );
    m_mon_ROIPhi.push_back( roi[0]->phi() );
    m_mon_ROIEta.push_back( roi[1]->eta() );
    m_mon_ROIPhi.push_back( roi[1]->phi() );
    float dEtaRoI = roi[0]->eta() - roi[1]->eta();
    float dPhiRoI = roi[0]->phi() - roi[1]->phi();
    while ( dPhiRoI >  M_PI ) dPhiRoI -= 2*M_PI;
    while ( dPhiRoI < -M_PI ) dPhiRoI += 2*M_PI;
    m_mon_Roi1Roi2dEta.push_back( fabs(dEtaRoI) );
    m_mon_Roi1Roi2dPhi.push_back( fabs(dPhiRoI) );
    m_mon_Roi1Roi2dR  .push_back( sqrt(dEtaRoI*dEtaRoI + dPhiRoI*dPhiRoI) );
    
    // JW test for xAOD l2 combined muons:
    typedef  ElementLinkVector<xAOD::L2CombinedMuonContainer>  ELVMuons;
    typedef  ElementLinkVector<xAOD::L2StandAloneMuonContainer>  ELVMuonsSA;
    
    for (unsigned int mui =0; mui < 2; ++mui) {
        msg() << MSG::DEBUG << "JW attempt: " << mui+1 << endmsg;
        ELVMuons elvmuon;
        if(getFeaturesLinks<xAOD::L2CombinedMuonContainer,xAOD::L2CombinedMuonContainer>(inputTE[mui], elvmuon)!=HLT::OK ) {
            msg() << MSG::DEBUG << "JW: " << mui+1 << " Problem" << endmsg;
            
        } else {
            msg() << MSG::DEBUG << "JW: " << mui+1 << " Ok: size: " << elvmuon.size() << endmsg;
            
        }
        
    } // mui
    
    
    m_muon1 = m_muon2 = nullptr;
    ELVMuonsSA muonFeatureEL;
    const xAOD::L2StandAloneMuon*  pMuonFeature(nullptr);
    ElementLink<xAOD::L2StandAloneMuonContainer> pMuonFeatureEL; // for m_noId will be set to the SA muon
    
    const xAOD::L2CombinedMuon *muon(nullptr);
    ElementLink<xAOD::L2CombinedMuonContainer> muonEL;
    int i_comb = 0; // for noId chains, index of the RoI with combined muon feature (should be always 0, but better check it)
    
    // Get the muons combined/standalone features
    // const CombinedMuonFeature *muonX[2] = { m_muon1, m_muon2 };
    const xAOD::L2CombinedMuon *muonX[2] = { m_muon1, m_muon2 }; // JW FIXME shouldn't this just initialise to nullptr?
    ElementLink<xAOD::L2CombinedMuonContainer> muonXEL[2];
    
    //ElementLink<xAOD::L2CombinedMuonContainer> l2combinedMuonEL[2];
    ELVMuons l2combinedMuonEL[2];
    if ( !m_noId ) {
        for ( int i = 0; i < 2; i++ ) {
            //if ( (getFeature(inputTE.at(i), muonX[i]) != HLT::OK) || !muonX[i] ) {
            //if (getFeatureLink<xAOD::L2CombinedMuonContainer,xAOD::L2CombinedMuonContainer>(inputTE.at(i),l2combinedMuonEL[i]) != HLT::OK) {
            if(getFeaturesLinks<xAOD::L2CombinedMuonContainer,xAOD::L2CombinedMuonContainer>(inputTE.at(i), l2combinedMuonEL[i])!=HLT::OK ) {
                msg() << MSG::ERROR << "Navigation error while getting CombinedMuonFeature " << i+1 << endmsg;
                if ( timerSvc() ) {
                    m_BmmHypTot->stop();
                    m_mon_TotalRunTime = m_BmmHypTot->elapsed();
                }
                if ( i == 0 ) m_mon_Errors.push_back( ERROR_Mu1_not_Combined );
                else          m_mon_Errors.push_back( ERROR_Mu2_not_Combined );
                return HLT::NAV_ERROR;
            } else {
                if (l2combinedMuonEL[i].size()) {
                    muonX[i] = l2combinedMuonEL[i][0].isValid() ? *(l2combinedMuonEL[i][0]): nullptr;
                    muonXEL[i] = l2combinedMuonEL[i][0];
                }
                if (l2combinedMuonEL[i].size() > 1) {
                    msg() << MSG::WARNING << "Unexpected number of containers for comb feature: " << i+1 << endmsg;
                }
                if (!muonX[i]) {
                    msg() << MSG::ERROR << "Nullptr from EL for muon: " << i+1 << endmsg;
                    if ( timerSvc() ) {
                        m_BmmHypTot->stop();
                        m_mon_TotalRunTime = m_BmmHypTot->elapsed();
                    }
                    if ( i == 0 ) m_mon_Errors.push_back( ERROR_Mu1_not_Combined );
                    else          m_mon_Errors.push_back( ERROR_Mu2_not_Combined );
                    return HLT::NAV_ERROR;
                }
                msg() << MSG::DEBUG << "successfully got muon combined feature for RoI " << i+1 << endmsg;
                // Monitoring of the combined muons w.r.t. ROI
                if ( i == 0 ) m_mon_Acceptance.push_back( ACCEPT_Mu1_Combined );
                else          m_mon_Acceptance.push_back( ACCEPT_Mu2_Combined );
                float dEtaCombRoI = ((i==0)?(roi[0]->eta()):(roi[1]->eta())) - muonX[i]->eta();
                float dPhiCombRoI = ((i==0)?(roi[0]->phi()):(roi[1]->phi())) - muonX[i]->phi();
                while ( dPhiCombRoI >  M_PI ) dPhiCombRoI -= 2*M_PI;
                while ( dPhiCombRoI < -M_PI ) dPhiCombRoI += 2*M_PI;
                m_mon_MucombROIdR.push_back( sqrt(dEtaCombRoI*dEtaCombRoI + dPhiCombRoI*dPhiCombRoI) );
            }
        } //end loop over RoIs
        
    } else { // m_noId
        
        for ( i_comb=0; i_comb<2; i_comb++ ) {
            //if ( (getFeature(inputTE.at(i_comb), muonX[i_comb]) == HLT::OK) && muonX[i_comb] ) break;
            //if (getFeatureLink<xAOD::L2CombinedMuonContainer,xAOD::L2CombinedMuonContainer>(inputTE.at(i_comb),l2combinedMuonEL[i_comb]) == HLT::OK) {
            if(getFeaturesLinks<xAOD::L2CombinedMuonContainer,xAOD::L2CombinedMuonContainer>(inputTE.at(i_comb), l2combinedMuonEL[i_comb]) ==HLT::OK ) {
                //muonX[i_comb] = l2combinedMuonEL[i_comb].isValid() ? *l2combinedMuonEL[i_comb]: nullptr;
                if (l2combinedMuonEL[i_comb].size()) {
                    muonX[i_comb] = (l2combinedMuonEL[i_comb][0]).isValid() ? *(l2combinedMuonEL[i_comb][0]): nullptr;
                    muonXEL[i_comb]  = l2combinedMuonEL[i_comb][0];
                }
                if ( muonX[i_comb]) break;
            }
            muonX[i_comb] = NULL;
            
        } // for
        if ( i_comb > 1 ) {
            msg() << MSG::ERROR << "Navigation error while getting CombinedMuonFeatures 1 or 2" << endmsg;
            if ( timerSvc() ) {
                m_BmmHypTot->stop();
                m_mon_TotalRunTime = m_BmmHypTot->elapsed();
            }
            m_mon_Errors.push_back( ERROR_Mu1_not_Combined );
            m_mon_Errors.push_back( ERROR_Mu2_not_Combined );
            return HLT::NAV_ERROR;
        } else {
            msg() << MSG::DEBUG << "successfully got muon combined feature for RoI " << i_comb+1 << endmsg;
            // Monitoring of the combined muons w.r.t. ROI
            if ( !i_comb ) m_mon_Acceptance.push_back( ACCEPT_Mu1_Combined );
            else           m_mon_Acceptance.push_back( ACCEPT_Mu2_Combined );
            float dEtaCombRoI = roi[i_comb]->eta() - muonX[i_comb]->eta();
            float dPhiCombRoI = roi[i_comb]->phi() - muonX[i_comb]->phi();
            while ( dPhiCombRoI >  M_PI ) dPhiCombRoI -= 2*M_PI;
            while ( dPhiCombRoI < -M_PI ) dPhiCombRoI += 2*M_PI;
            m_mon_MucombROIdR.push_back( sqrt(dEtaCombRoI*dEtaCombRoI + dPhiCombRoI*dPhiCombRoI) );
        }
        
        // Look for standalone muon in 2nd RoI
        //JW      msg() << MSG::ERROR << " getFeatureLink fails to get MuonFeature" << endmsg;
        //if ( HLT::OK != getFeatureLink<xAOD::L2StandAloneMuonContainer, xAOD::L2StandAloneMuon>(inputTE.at(1-i_comb), muonFeatureEL) ) {
        //if ( HLT::OK != getFeatureLink<xAOD::L2StandAloneMuonContainer, xAOD::L2StandAloneMuonContainer>(inputTE.at(1-i_comb), muonFeatureEL) ) {
        if ( HLT::OK != getFeaturesLinks<xAOD::L2StandAloneMuonContainer, xAOD::L2StandAloneMuonContainer>(inputTE.at(1-i_comb), muonFeatureEL) ) {
            if ( timerSvc() ) {
                m_BmmHypTot->stop();
                m_mon_TotalRunTime = m_BmmHypTot->elapsed();
            }
            if ( !i_comb ) m_mon_Errors.push_back( ERROR_Mu2_not_Standalone );
            else           m_mon_Errors.push_back( ERROR_Mu1_not_Standalone );
            return HLT::ERROR;
        }
        if ( !muonFeatureEL.size() || !muonFeatureEL[0].isValid() ) {
            msg() << MSG::ERROR << " getFeatureLink finds no TrigMuonFeature (EL invalid)" << endmsg;
            if ( timerSvc() ) {
                m_BmmHypTot->stop();
                m_mon_TotalRunTime = m_BmmHypTot->elapsed();
            }
            if ( !i_comb ) m_mon_Errors.push_back( ERROR_Mu2_Standalone_Invalid );
            else           m_mon_Errors.push_back( ERROR_Mu1_Standalone_Invalid );
            return HLT::NAV_ERROR;
        }
        pMuonFeatureEL = muonFeatureEL[0]; // for output object
        pMuonFeature = *(muonFeatureEL[0]);
        if ( msgLvl() <= MSG::DEBUG )
            msg() << MSG::DEBUG
            << "RoI " << 2-i_comb << ", the MuonFeature is saddress = " << pMuonFeature ->sAddress()
            << " / pt = "       << pMuonFeature->pt()
            << " / radius = "   << pMuonFeature->rMS() // JW radius()
            << " / eta = "      << pMuonFeature->etaMS()//eta()
            << " / phi = "      << pMuonFeature->phiMS()//phi()
            << " / dir_phi = "  << pMuonFeature->dirPhiMS()//dir_phi()
            << " / zeta = "     << pMuonFeature->zMS()//zeta()
            << " / dir_zeta = " << pMuonFeature->dirZMS()//dir_zeta()
            << endmsg;
        if ( !i_comb ) m_mon_Acceptance.push_back( ACCEPT_Mu2_Standalone );
        else           m_mon_Acceptance.push_back( ACCEPT_Mu1_Standalone );
        // Monitoring of the standalone muon w.r.t. ROI
        float dEtaStandRoI = roi[1-i_comb]->eta() - pMuonFeature->eta();
        float dPhiStandRoI = roi[1-i_comb]->phi() - pMuonFeature->phi();
        while ( dPhiStandRoI >  M_PI ) dPhiStandRoI -= 2*M_PI;
        while ( dPhiStandRoI < -M_PI ) dPhiStandRoI += 2*M_PI;
        m_mon_MustandROIdR.push_back( sqrt(dEtaStandRoI*dEtaStandRoI + dPhiStandRoI*dPhiStandRoI) );
    }
    
    m_muon1 = muonX[0];
    m_muon2 = muonX[1];
    
    // Check if we have the requested objects
    if ( !m_noId ) {
        // Need two combined muons
        if ( m_muon1 && m_muon2 && roi[0] && roi[1] ) {
            if ( msgLvl() <= MSG::DEBUG ) {
                msg() << MSG::DEBUG << "Got all necessary objects " << m_muon1 << " " << m_muon2 << endmsg;
                msg() << MSG::DEBUG << "               for TE's " << inputTE.front() << " and " << inputTE.back() << endmsg;
            }
            m_mon_Acceptance.push_back( ACCEPT_MuMu_Both_Combined );
        } else {
            msg() << MSG::ERROR << "Can't get all necessary objects rois: " << ((roi[0] && roi[1])?"OK": "MISSING")
            << " muons: " <<  ((m_muon1 && m_muon2)?"OK": "MISSING")  << endmsg;
            if ( timerSvc() ) {
                m_BmmHypTot->stop();
                m_mon_TotalRunTime = m_BmmHypTot->elapsed();
            }
            m_mon_Errors.push_back( ERROR_MuMu_not_Both_Combined );
            return HLT::BAD_JOB_SETUP;
        }
    } else {
        // The noId flag means one RoI only has a muon feature - no combined track
        if ( muonX[i_comb] && roi[0] && roi[1] && pMuonFeature ) {
            if ( msgLvl() <= MSG::DEBUG ) {
                msg() << MSG::DEBUG << "NoId version, Got all necessary objects " << m_muon1 << " " << m_muon2 << " " << pMuonFeature << endmsg;
                msg() << MSG::DEBUG << "               for TE's " << inputTE.front() << " and " << inputTE.back() << endmsg;
            }
            muon = muonX[i_comb];
            muonEL = muonXEL[i_comb];
            m_mon_Acceptance.push_back( ACCEPT_MuMu_Comb_Standalone );
        } else {
            msg() << MSG::ERROR << "NoId version, Can't get all necessary objects rois: " << ((roi[0] && roi[1])?"OK": "MISSING")
            << " combined muon: " << (m_muon1?"OK": "MISSING") << " standalone muons: " << (pMuonFeature?"OK": "MISSING") << endmsg;
            if ( timerSvc() ) {
                m_BmmHypTot->stop();
                m_mon_TotalRunTime = m_BmmHypTot->elapsed();
            }
            m_mon_Errors.push_back( ERROR_MuMu_not_Comb_Standalone );
            return HLT::BAD_JOB_SETUP;
        }
    }
    
    // Debug dump of the gained information
    if ( msgLvl() <= MSG::DEBUG ) {
        if ( !m_noId ) {
            msg() << MSG::DEBUG << "1st CombinedMuonFeature: addr=" << m_muon1
            << " pt=" << m_muon1->pt()*m_muon1->charge() << " trkAddr=" << m_muon1->idTrack();
            if ( m_muon1->idTrack() ) msg() << MSG::DEBUG << " trackTrackAddr=" << m_muon1->idTrack()->track();
            else                      msg() << MSG::DEBUG << " trackTrackAddr=0";
            msg() << MSG::DEBUG << endmsg;
            msg() << MSG::DEBUG << "2nd CombinedMuonFeature: addr=" << m_muon2
            << " pt=" << m_muon2->pt()*m_muon2->charge() << " trkAddr=" << m_muon2->idTrack();
            if ( m_muon2->idTrack() ) msg() << MSG::DEBUG << " trackTrackAddr=" << m_muon2->idTrack()->track();
            else                      msg() << MSG::DEBUG << " trackTrackAddr=0";
            msg() << MSG::DEBUG << endmsg;
        } else {
            msg() << MSG::DEBUG << "CombinedMuonFeature: addr=" << muon
            << " pt=" << muon->pt()*muon->charge() << " trkAddr=" << muon->idTrack();
            if ( muon->idTrack() ) msg() << MSG::DEBUG << " trackTrackAddr=" << muon->idTrack()->track();
            else                      msg() << MSG::DEBUG << " trackTrackAddr=0";
            msg() << MSG::DEBUG << endmsg;
            msg() << MSG::DEBUG << "StandaloneMuonFeature: addr=" << pMuonFeature
            << " pt=" << pMuonFeature->pt() << " eta=" << pMuonFeature->eta() << " phi=" << pMuonFeature->phi()
            << endmsg;
        }
    } // debug
    
    // Check if we are still in the same event
    if ( IdEvent !=  m_lastEvent ) {
        m_countTotalEvents++;
        m_lastEvent=IdEvent;
    }
    m_countTotalRoI++;
    
    // Get the Inner Detector tracks associated with the muons
    if ( !m_noId ) {
        if ( m_muon1->idTrack() ) m_mon_Acceptance.push_back( ACCEPT_Mu1_IDTrack );
        //      if ( m_muon2->idTrack() && isUnique(m_muon1->idTrack(),m_muon2->idTrack())) m_mon_Acceptance.push_back( ACCEPT_Mu2_IDTrack );
        if ( m_muon2->idTrack() && isUnique(m_muon1->idTrack(),m_muon2->idTrack())) m_mon_Acceptance.push_back( ACCEPT_Mu2_IDTrack );
        
        // ST: option to accept all events if two identical muon tracks are found
        if ( m_acceptSameMuon && m_muon1->idTrack() && m_muon2->idTrack() && !(isUnique(m_muon1->idTrack(),m_muon2->idTrack())) ) {
          pass = true;
          if ( timerSvc() ) {
            m_BmmHypTot->stop();
            m_mon_TotalRunTime = m_BmmHypTot->elapsed();
          }
          if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Accepting two identical muons as AcceptSameMuon is set" << endmsg;
          // return HLT::OK;
        }
        
        if ( !m_acceptSameMuon && !(m_muon1->idTrack() && m_muon2->idTrack() && isUnique(m_muon1->idTrack(),m_muon2->idTrack())) ) {
            if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Missing track(s) from ID or they are the same" << endmsg;
            vtxpass    = false;
            mumuIDpass = false;
            pass = true; // ST: to actually pass the empty collection to L2BMuMuHypo
//             if ( timerSvc() ) {
//               m_BmmHypTot->stop();
//               m_mon_TotalRunTime = m_BmmHypTot->elapsed();
//             }
            return HLT::OK;
        } else {
            m_mon_Acceptance.push_back( ACCEPT_MuMu_Both_IDTracks );
            // Monitoring of the combined muons w.r.t. ID tracks
            for ( int i=0; i<2; i++ ) {
                //JW        float dEtaMucombTrk = muonX[i]->IDTrack()->param()->eta()  - muonX[i]->eta();
                //        float dPhiMucombTrk = muonX[i]->IDTrack()->param()->phi0() - muonX[i]->phi();
                float dEtaMucombTrk = muonX[i]->idTrack()->eta()  - muonX[i]->eta();
                float dPhiMucombTrk = muonX[i]->idTrack()->phi() - muonX[i]->phi();
                while ( dPhiMucombTrk >  M_PI ) dPhiMucombTrk -= 2*M_PI;
                while ( dPhiMucombTrk < -M_PI ) dPhiMucombTrk += 2*M_PI;
                m_mon_MucombTrkdR.push_back( sqrt(dEtaMucombTrk*dEtaMucombTrk + dPhiMucombTrk*dPhiMucombTrk) );
                m_mon_MucombPt.push_back( muonX[i]->pt()*muonX[i]->charge() / CLHEP::GeV );
                // Monitoring of the ID tracks
                //        m_mon_MutrkPt_wideRange.push_back( fabs(muonX[i]->IDTrack()->param()->pT()) / CLHEP::GeV );
                //        m_mon_MutrkPt          .push_back( fabs(muonX[i]->IDTrack()->param()->pT()) / CLHEP::GeV );
                //        m_mon_MutrkEta.push_back( muonX[i]->IDTrack()->param()->eta() );
                //        m_mon_MutrkPhi.push_back( muonX[i]->IDTrack()->param()->phi0() );
                m_mon_MutrkPt_wideRange.push_back( fabs(muonX[i]->idTrack()->pt()) / CLHEP::GeV );
                m_mon_MutrkPt          .push_back( fabs(muonX[i]->idTrack()->pt()) / CLHEP::GeV );
                m_mon_MutrkEta.push_back( muonX[i]->idTrack()->eta() );
                m_mon_MutrkPhi.push_back( muonX[i]->idTrack()->phi() );
            }
            //      float dEtaMuMu = m_muon1->IDTrack()->param()->eta()  - m_muon2->IDTrack()->param()->eta(); // TODO: put opening angle calculation int the ToolBox)
            //JW      float dPhiMuMu = m_muon1->IDTrack()->param()->phi0() - m_muon2->IDTrack()->param()->phi0();
            float dEtaMuMu = m_muon1->idTrack()->eta()  - m_muon2->idTrack()->eta(); // TODO: put opening angle calculation int the ToolBox)
            float dPhiMuMu = m_muon1->idTrack()->phi()  - m_muon2->idTrack()->phi();
            
            while ( dPhiMuMu >  M_PI ) dPhiMuMu -= 2*M_PI;
            while ( dPhiMuMu < -M_PI ) dPhiMuMu += 2*M_PI;
            m_mon_Mutrk1Mutrk2dEta.push_back( fabs(dEtaMuMu) );
            m_mon_Mutrk1Mutrk2dPhi.push_back( fabs(dPhiMuMu) );
            m_mon_Mutrk1Mutrk2dR  .push_back( sqrt(dEtaMuMu*dEtaMuMu + dPhiMuMu*dPhiMuMu) );
            //      m_mon_SumPtMutrk12.push_back( (fabs(m_muon1->IDTrack()->param()->pT()) + fabs(m_muon2->IDTrack()->param()->pT())) / CLHEP::GeV );
            m_mon_SumPtMutrk12.push_back( (fabs(m_muon1->idTrack()->pt()) + fabs(m_muon2->idTrack()->pt())) / CLHEP::GeV );
        }
    } else {
        if( !(muon->idTrack()) ) {
            if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Missing track(s) from ID" << endmsg;
            vtxpass    = false;
            mumuIDpass = false;
            // if ( timerSvc() ) m_BmmHypTot->stop();
            // return HLT::OK;
        } else {
            m_mon_Acceptance.push_back( ACCEPT_CombMu_IDTrack );
            // Monitoring of the combined / standalone muons w.r.t. ID tracks
            float dEtaMucombTrk = muon->idTrack()->eta() - muon->eta();
            float dPhiMucombTrk = muon->idTrack()->phi() - muon->phi();
            while ( dPhiMucombTrk >  M_PI ) dPhiMucombTrk -= 2*M_PI;
            while ( dPhiMucombTrk < -M_PI ) dPhiMucombTrk += 2*M_PI;
            m_mon_MucombTrkdR.push_back( sqrt(dEtaMucombTrk*dEtaMucombTrk + dPhiMucombTrk*dPhiMucombTrk) );
            m_mon_MucombPt.push_back( muon->pt()*muon->charge() / CLHEP::GeV );
            // Monitoring of the ID tracks
            m_mon_MutrkPt_wideRange  .push_back( fabs(muon->idTrack()->pt()) / CLHEP::GeV );
            //m_mon_MustandPt_wideRange.push_back( fabs(pMuonFeature->pt()) / CLHEP::GeV );
            m_mon_MustandPt_wideRange.push_back( fabs(pMuonFeature->pt()) *1000. / CLHEP::GeV );
            m_mon_MutrkPt  .push_back( fabs(muon->idTrack()->pt()) / CLHEP::GeV );
            m_mon_MustandPt.push_back( fabs(pMuonFeature->pt()) *1000. / CLHEP::GeV );
            m_mon_MutrkEta  .push_back( muon->idTrack()->eta() );
            m_mon_MustandEta.push_back( pMuonFeature->eta() );
            m_mon_MutrkPhi  .push_back( muon->idTrack()->phi() );
            m_mon_MustandPhi.push_back( pMuonFeature->phi() );
            float dEtaMuMu = muon->idTrack()->eta()  - pMuonFeature->eta();
            float dPhiMuMu = muon->idTrack()->phi() - pMuonFeature->phi();
            while ( dPhiMuMu >  M_PI ) dPhiMuMu -= 2*M_PI;
            while ( dPhiMuMu < -M_PI ) dPhiMuMu += 2*M_PI;
            m_mon_MutrkMustanddEta.push_back( fabs(dEtaMuMu) );
            m_mon_MutrkMustanddPhi.push_back( fabs(dPhiMuMu) );
            m_mon_MutrkMustanddR  .push_back( sqrt(dEtaMuMu*dEtaMuMu + dPhiMuMu*dPhiMuMu) );
            m_mon_SumPtMutrkMustand.push_back( (fabs(muon->idTrack()->pt()) + fabs(pMuonFeature->pt()*1000.)) / CLHEP::GeV );
        }
    }
    
    m_mon_Acceptance.push_back( ACCEPT_DiMuon_Pair );
    if ( vtxpass    ) m_mon_Acceptance.push_back( ACCEPT_vtxpass_is_TRUE );
    if ( mumuIDpass ) m_mon_Acceptance.push_back( ACCEPT_mumuIDpass_is_TRUE );
    
    msg() << MSG::DEBUG << "JWW: charge check - !noId" << endmsg;
    if ( !m_noId ) {
        double mu1ptq, mu2ptq;
        if ( m_muon1->idTrack())  {
            mu1ptq = m_muon1->idTrack()->pt() * m_muon1->idTrack()->charge();
            msg() << MSG::DEBUG << "JWW: mu1ptq from IDTrack()->param()->pT(): " << mu1ptq << endmsg;
            msg() << MSG::DEBUG << "JWW: " << m_muon1->charge() << "  " << m_muon1->pt() << endmsg;
        } else {
            mu1ptq = m_muon1->pt() * m_muon1->charge();
            msg() << MSG::DEBUG << "JWW: mu1ptq from ptq(): " << mu1ptq << endmsg;
            msg() << MSG::DEBUG << "JWW: " << m_muon1->charge() << "  " << m_muon1->pt() << " "   << mu1ptq << endmsg;
            
        }
        if ( m_muon2->idTrack() ){
            mu2ptq = m_muon2->idTrack()->pt() * m_muon2->idTrack()->charge();
            msg() << MSG::DEBUG << "JWW: mu2ptq from IDTrack()->param()->pT(): " << mu2ptq << endmsg;
            msg() << MSG::DEBUG << "JWW: " << m_muon2->charge() << "  " << m_muon2->pt() << " " << endmsg;
            
        } else{
            mu2ptq = m_muon2->pt() * m_muon2->charge();
            msg() << MSG::DEBUG << "JWW: mu2ptq from ptq(): " << mu2ptq << endmsg;
            msg() << MSG::DEBUG << "JWW: " << m_muon2->charge() << "  " << m_muon2->pt() << " "   << mu2ptq << endmsg;
        }
        msg() << MSG::DEBUG << "JWW: muon1 \n" << *m_muon1 << endmsg;
        msg() << MSG::DEBUG << "JWW: muon2 \n" << *m_muon2 << endmsg;
        
        
    } // !noid
    
    // Check opposite/same charge
    double mu1ptq, mu2ptq;
    if ( !m_noId ) {
        if ( m_muon1->idTrack() ) mu1ptq = fabs(m_muon1->idTrack()->pt()) * m_muon1->idTrack()->charge();
        else                      mu1ptq = fabs(m_muon1->pt()) * m_muon1->charge();
        if ( m_muon2->idTrack() ) mu2ptq = fabs(m_muon2->idTrack()->pt()) * m_muon2->idTrack()->charge();
        else                      mu2ptq = fabs(m_muon2->pt()) * m_muon2->charge();
    } else {
        if ( muon->idTrack() ) mu1ptq = fabs(muon->idTrack()->pt()) * muon->idTrack()->charge();
        else                     mu1ptq = fabs(muon->pt()) * muon->charge();
        mu2ptq = pMuonFeature->pt()*1000.; // convert to MeV
    }
    if ( !m_acceptSameMuon && m_oppositeCharge ) {
        if ( mu1ptq * mu2ptq > 0 ) {
            if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Combination discarded by opposite charge check" << endmsg;
            pass = true; // ST: to actually pass the empty collection to L2BMuMuHypo
            if ( timerSvc() ){
                m_BmmHypTot->stop();
                m_mon_TotalRunTime = m_BmmHypTot->elapsed();
            }
            return HLT::OK;
        }
    }
    if ( m_sameCharge ) {
        if ( mu1ptq * mu2ptq < 0 ) {
            if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Combination discarded by same charge check" << endmsg;
            pass = true; // ST: to actually pass the empty collection to L2BMuMuHypo
            if ( timerSvc() ){
                m_BmmHypTot->stop();
                m_mon_TotalRunTime = m_BmmHypTot->elapsed();
            }
            return HLT::OK;
        }
    }
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "opposite/same charge cut passed" << endmsg;
    m_mon_Acceptance.push_back( ACCEPT_Opp_Charge );
    
    // Calculate the invariant mass
    double mass(0.0);
    if ( !m_noId ) {
        //mass = invariantMass(m_muon1, m_muon2);
        //        mass  = m_bphysHelperTool->invariantMass( {m_muon1, m_muon2}, {m_massMuon,m_massMuon} );
        mass  = m_bphysHelperTool->invariantMass( m_muon1->idTrack(), m_muon2->idTrack(), m_massMuon,m_massMuon ); // #FIXME - use the ID track, yes?
        
        m_mon_InvMass_comb          .push_back(mass / CLHEP::GeV);
        m_mon_InvMass_comb_wideRange.push_back(mass / CLHEP::GeV);
    } else {
        //mass = invariantMass(muon, pMuonFeature);
        //        msg() << MSG::DEBUG << "Mass determination of noId" << endmsg;
        //        msg() << MSG::DEBUG << "Muon1 Id Track " << muon->idTrack()->pt() << " " <<  muon->idTrack()->eta() << " " <<  muon->idTrack()->phi() << endmsg;
        //        msg() << MSG::DEBUG << "Muon2 Print:  " << pMuonFeature->pt() << " " << pMuonFeature->eta() << " " << pMuonFeature->phi() << endmsg;
        //        auto p = dynamic_cast<const xAOD::IParticle*>(pMuonFeature);
        //        msg() << MSG::DEBUG << "Muon2 IParticle: " << p->pt() << " " << p->eta() << " " << p->phi() << endmsg;
        //        msg() << MSG::DEBUG << "Muon2 IDtrack " << p->pt() << " " << p->eta() << " " << p->phi() << endmsg;
        //        msg() << MSG::DEBUG << "Muon2 SAtrack: " << p->pt() << " " << p->eta() << " " << p->phi() << endmsg;
        //        msg() << MSG::DEBUG << "Muon2 type: " <<  pMuonFeature->type() << endmsg;
        mass  = m_bphysHelperTool->invariantMass( muon->idTrack(), pMuonFeature, m_massMuon,m_massMuon );
        m_mon_InvMass_stand          .push_back(mass / CLHEP::GeV);
        m_mon_InvMass_stand_wideRange.push_back(mass / CLHEP::GeV);
    }
    msg() << MSG::DEBUG << "Mass: " << mass << endmsg;
    
    // ST: if two identical muons - set mass to -1 to accept in Hypo then
    if( m_acceptSameMuon && !(isUnique(m_muon1->idTrack(),m_muon2->idTrack())) ) {
      mass = -1.;
      if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Mass is set to -1. for identical muons" << endmsg;
    }

    // Apply the cut on the mass
    if ( !m_acceptSameMuon && (mass < m_lowerMassCut || (mass > m_upperMassCut && m_ApplyupperMassCut)) ) {
        if ( msgLvl() <= MSG::DEBUG )
            msg() << MSG::DEBUG << "Combination discarded by mass cut: " << mass << " MeV" << endmsg;
        pass = true; // ST: to actually pass the empty collection to L2BMuMuHypo
        if ( timerSvc() ) {
            m_BmmHypTot->stop();
            m_mon_TotalRunTime = m_BmmHypTot->elapsed();
        }
        return HLT::OK;
    } else {
        m_countPassedBsMass++;
    }
    m_mon_Acceptance.push_back( ACCEPT_InvMass_Cut );
    
    // Start the vertexing timer
    if ( timerSvc() ) m_BmmHypVtx->start();
    
    // Found pair with good mass, now make L2Bphys object
    //  TrigL2Bphys* trigPartBmumu = new TrigL2Bphys( 0, 0., 0., TrigL2Bphys::BMUMU, mass); // TODO: shouldn't there be RoIs parameters ?
    xAOD::TrigBphys* trigPartBmumu = new xAOD::TrigBphys();
    trigPartBmumu->makePrivateStore();
    trigPartBmumu->initialise( 0, 0., 0., xAOD::TrigBphys::BMUMU, mass,xAOD::TrigBphys::L2 );
    
    if ( msgLvl() <= MSG::DEBUG )
        msg() << MSG::DEBUG << " Create Bphys particle with RoIId " << trigPartBmumu->roiId()
        << " phi, eta "    << trigPartBmumu->phi() << " " << trigPartBmumu->eta()
        << " vertex type " << trigPartBmumu->particleType() << endmsg;
    
    if ( !m_noId ) {
        //const ElementLink< xAOD::TrackParticleContainer >& track1EL  = m_muon1->idTrackLink();
        //const ElementLink< xAOD::TrackParticleContainer >& track2EL  = m_muon2->idTrackLink();
        ElementLink< xAOD::TrackParticleContainer > track1EL  = m_muon1->idTrackLink();
        ElementLink< xAOD::TrackParticleContainer > track2EL  = m_muon2->idTrackLink();
        // note - the above tracks have HLT autokey - need to remap to pers. container
        // no garauntee that some slimming won't latter mess this up - better to use TDT in analysis
        ElementLinkVector< xAOD::TrackParticleContainer > newContainer1, newContainer2;
        if (HLT::OK != getFeaturesLinks<xAOD::TrackParticleContainer,xAOD::TrackParticleContainer>(inputTE[0],newContainer1,m_combinedMuonIDTrackKey)) {
            msg() << MSG::DEBUG << "No elv trkcon roi0" << endmsg;

        }
        if (HLT::OK != getFeaturesLinks<xAOD::TrackParticleContainer,xAOD::TrackParticleContainer>(inputTE[1],newContainer2,m_combinedMuonIDTrackKey)) {
            msg() << MSG::DEBUG << "No elv trkcon roi1" << endmsg;
        }
        msg() << MSG::DEBUG << "Before mapping: Valid " << track1EL.isValid() << " "  << track2EL.isValid() << endmsg;
        msg() << MSG::DEBUG << "V1 " << track1EL.dataID() << " "  << track1EL.index() << endmsg;
        msg() << MSG::DEBUG << "V2 " << track2EL.dataID() << " "  << track2EL.index() << endmsg;
        track1EL = remap_container(track1EL,newContainer1);
        track2EL = remap_container(track2EL,newContainer2);
        
        
        
        if ( msgLvl() <= MSG::DEBUG ) {
            msg() << MSG::DEBUG << "Just check track links... " << endmsg;
            msg() << MSG::DEBUG << "Valid " << track1EL.isValid() << " "  << track2EL.isValid() << endmsg; 
            msg() << MSG::DEBUG << "V1 " << track1EL.dataID() << " "  << track1EL.index() << endmsg; 
            msg() << MSG::DEBUG << "V2 " << track2EL.dataID() << " "  << track2EL.index() << endmsg; 
            msg() << MSG::DEBUG << "Track 1 pT " << (*track1EL)->pt()
            << " eta: " << (*track1EL)->eta()
            << " phi: " << (*track1EL)->phi() << endmsg;
            msg() << MSG::DEBUG << "Track 2 pT " << (*track2EL)->pt()
            << " eta: " << (*track2EL)->eta()
            << " phi: " << (*track2EL)->phi() << endmsg;
        }
        trigPartBmumu->addTrackParticleLink(track1EL);
        trigPartBmumu->addTrackParticleLink(track2EL);
        
        msg() << MSG::DEBUG << "JWPtl1: " << muonXEL[0].isValid() << " " << muonXEL[0].dataID() << " " << muonXEL[0].index() << endmsg;
        msg() << MSG::DEBUG << "JWPt12: " << muonXEL[1].isValid() << " " << muonXEL[1].dataID() << " " << muonXEL[1].index() << endmsg;

        ElementLinkVector< xAOD::L2CombinedMuonContainer > muonContainerComb1,muonContainerComb2;
        if (HLT::OK != getFeaturesLinks<xAOD::L2CombinedMuonContainer,xAOD::L2CombinedMuonContainer>(inputTE[0],muonContainerComb1,m_combinedMuonKey)) {
            msg() << MSG::DEBUG << "No comb muon container" << endmsg;
        } else {
            msg() << MSG::DEBUG << "Comb muon container: " << muonContainerComb1.size() << endmsg;
        }
        for (auto el: muonContainerComb1) {
            msg() << MSG::DEBUG << "Comb muon container: " << el.dataID() << " " << el.index() << endmsg;
        }
        if (HLT::OK != getFeaturesLinks<xAOD::L2CombinedMuonContainer,xAOD::L2CombinedMuonContainer>(inputTE[1],muonContainerComb2,m_combinedMuonKey)) {
            msg() << MSG::DEBUG << "No comb muon container" << endmsg;
        } else {
            msg() << MSG::DEBUG << "Comb muon container: " << muonContainerComb2.size() << endmsg;
        }
        for (auto el: muonContainerComb2) {
            msg() << MSG::DEBUG << "Comb muon container: " << el.dataID() << " " << el.index() << endmsg;
        }
        ElementLink<xAOD::IParticleContainer> ptl1EL = remap_container(muonXEL[0],muonContainerComb1);
        ElementLink<xAOD::IParticleContainer> ptl2EL = remap_container(muonXEL[1],muonContainerComb2);

        
        //ElementLink<xAOD::IParticleContainer> ptl1EL(muonXEL[0].dataID(),muonXEL[0].index());
        //ElementLink<xAOD::IParticleContainer> ptl2EL(muonXEL[1].dataID(),muonXEL[1].index());
        trigPartBmumu->addParticleLink(ptl1EL); //
        trigPartBmumu->addParticleLink(ptl2EL); //

        // set the eta and phi based on simple 4-mom
        // ignore the correction mass of tracks
        xAOD::IParticle::FourMom_t dimuon = (*track1EL)->p4() + (*track2EL)->p4();
        trigPartBmumu->setEta(dimuon.Rapidity());
        trigPartBmumu->setPhi(dimuon.Phi());

        
    } else {
        ElementLink< xAOD::TrackParticleContainer > track1EL  = muon->idTrackLink();
        ElementLinkVector< xAOD::TrackParticleContainer > newContainer1;
        if (HLT::OK != getFeaturesLinks<xAOD::TrackParticleContainer,xAOD::TrackParticleContainer>(inputTE[0],newContainer1,m_combinedMuonIDTrackKey)) {
            msg() << MSG::DEBUG << "No elv trkcon roi0" << endmsg;
            
        }
        msg() << MSG::DEBUG << "V1 " << track1EL.dataID() << " "  << track1EL.index() << endmsg;
        track1EL = remap_container(track1EL,newContainer1);

        if ( msgLvl() <= MSG::VERBOSE ){
            msg() << MSG::VERBOSE << "Just check track links... " << endmsg;
            msg() << MSG::VERBOSE << "Track 1 pT " << (*track1EL)->pt()
            << " eta: " << (*track1EL)->eta()
            << " phi: " << (*track1EL)->phi() << endmsg;
        }
        // set the eta and phi based on simple 4-mom
        // ignore the correction mass of tracks
        xAOD::IParticle::FourMom_t saMuon;
        saMuon.SetPtEtaPhiM(pMuonFeature->pt() * 1000., pMuonFeature->p4().Eta(), pMuonFeature->p4().Phi(),pMuonFeature->p4().M()*1000.);
        xAOD::IParticle::FourMom_t dimuon = (*track1EL)->p4() + saMuon;
        
        trigPartBmumu->setEta(dimuon.Eta());
        trigPartBmumu->setPhi(dimuon.Phi());

        trigPartBmumu->addTrackParticleLink(track1EL);
        msg() << MSG::DEBUG << "JWnoid1: " << muonEL.dataID() << " " << muonEL.index() << endmsg;
        msg() << MSG::DEBUG << "JWnoid2: " << pMuonFeatureEL.dataID() << " " << pMuonFeatureEL.index() << endmsg;
        

        ElementLink<xAOD::IParticleContainer> tEL(muonEL.dataID(),muonEL.index());
        trigPartBmumu->addParticleLink(tEL); // use Iparticle container for standalone muon

        ElementLink<xAOD::IParticleContainer> pEL(pMuonFeatureEL.dataID(),pMuonFeatureEL.index());
        msg() << MSG::DEBUG << "Inseting IParticle EL from L2Muon with:  " << pEL.isValid() << " " << pEL.dataID() << " " << pEL.index()  << endmsg;
        trigPartBmumu->addParticleLink(pEL); // use Iparticle container for standalone muon

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
            // #FIXME - previous code used MuonFeature::IDTrack()::chi2() / are these equivalent ... ?
            if (muonX[i]->idTrack()->chiSquared() < 1e7 ) {
                sc = m_vertexingTool->addTrack(muonX[i]->idTrack()->track(), pL2V, Trk::muon);
                if ( sc.isFailure() ) {
                    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Failed to add track " << i+1 << " to vertexingTool" << endmsg;
                    addTracks = false;
                    m_mon_Errors.push_back( ERROR_AddTrack_Fails );
                }
            } else {
                addTracks = false;
                m_mon_Errors.push_back( ERROR_AddTrack_Fails );
            }
        }
        
        // DO THE VERTEX-FIT
        if ( addTracks ) {
            m_mon_Acceptance.push_back( ACCEPT_AddTrack_Test );
            sc = m_L2vertFitter->fit(pL2V);
        }
        
        // Check the result
        if ( sc.isFailure() || (!addTracks) ) {
            if ( msgLvl() <= MSG::DEBUG )msg() << MSG::DEBUG << "TrigL2VertexFitter failed" << endmsg;
        } else {
            m_mon_Acceptance.push_back( ACCEPT_Vertexing );
            // Calculate invariant mass
            sc = m_vertexingTool->calculateInvariantMass(pL2V);
            if ( sc.isSuccess() ) {
                m_mon_Acceptance.push_back( ACCEPT_CalcInvMass );
                // Create mother particle
                sc = m_vertexingTool->createMotherParticle(pL2V);
                if ( sc.isSuccess() ) {
                    m_mon_Acceptance.push_back( ACCEPT_CalcMother );
                    if ( msgLvl() <= MSG::VERBOSE ) msg() << MSG::VERBOSE << "Bs created: pT "<<pL2V->m_getMotherTrack()->pT() << endmsg;
                } else {
                    m_mon_Errors.push_back( ERROR_CalcMother_Fails );
                }
                p_mumuV = m_vertexingTool->createTrigVertex(pL2V);
            } else {
                m_mon_Errors.push_back( ERROR_CalcInvMass_Fails );
            }
        }
        delete pL2V;
        
        if ( p_mumuV == NULL ) {
            if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " vertex fit failed for tracks" << endmsg;
            // delete pL2V;
            delete p_mumuV;
            // delete trigPartBmumu;
            vtxpass = false;
            // continue;
        } else {
            m_mon_Acceptance.push_back( ACCEPT_Vertex_not_NULL );
            double chi2prob = 0.;
            chi2prob = 1.0 - Genfun::CumulativeChiSquare(p_mumuV->ndof())(p_mumuV->chi2());
            if ( msgLvl() <= MSG::DEBUG ) {
                msg() << MSG::DEBUG << "REGTEST: mumu vertex Fit: x= y= z=" << p_mumuV->x() << " " << p_mumuV->y() << " " << p_mumuV->z() << endmsg;
                msg() << MSG::DEBUG << "REGTEST: Bmumu mass = " << p_mumuV->mass() << endmsg;
                if (p_mumuV->massVariance() !=0) {
                    msg() << MSG::DEBUG << "REGTEST: Bmumu mass pull = " << (p_mumuV->mass()-BSMASS)/p_mumuV->massVariance() << endmsg;
                } else {
                    msg() << MSG::DEBUG << "REGTEST: MuMu mass variance is 0 can't calculate the pull" << endmsg;
                    m_mon_Errors.push_back( ERROR_CalcMassPull_Fails );
                }
                // msg() << MSG::DEBUG << "REGTEST: Chi2 probability of the fit = " << chi2prob << endmsg;
                msg() << MSG::DEBUG << "REGTEST: Chi2 vtx fit = " << p_mumuV->chi2() << endmsg;
                msg() << MSG::DEBUG << "SigmaX =  SigmaY =  SigmaZ = " << sqrt(p_mumuV->cov()[0]) << " " << sqrt(p_mumuV->cov()[2]) << " " << sqrt(p_mumuV->cov()[5]) << endmsg;
            }
            
            m_VertexColl = new TrigVertexCollection();
            m_VertexColl->push_back(p_mumuV);
            
            // JK 28/4/08 changes for ElementLinks
            ElementLink<TrigVertexCollection> BMuMuVertexEL(*m_VertexColl,0);
            // trigPartBmumu->pVertex(p_mumuV);
            // 14-05-08 trigPartBmumu->pVertex(BMuMuVertexEL);
            
            //      trigPartBmumu->fitmass(p_mumuV->mass());
            //      trigPartBmumu->fitchi2(p_mumuV->chi2());
            //      trigPartBmumu->fitndof(p_mumuV->ndof());
            //      trigPartBmumu->fitx(p_mumuV->x());
            //      trigPartBmumu->fity(p_mumuV->y());
            //      trigPartBmumu->fitz(p_mumuV->z());
            trigPartBmumu->setFitmass(p_mumuV->mass());
            trigPartBmumu->setFitchi2(p_mumuV->chi2());
            trigPartBmumu->setFitndof(p_mumuV->ndof());
            trigPartBmumu->setFitx   (p_mumuV->x());
            trigPartBmumu->setFity   (p_mumuV->y());
            trigPartBmumu->setFitz   (p_mumuV->z());
            
            
            // Monitoring of the fit results
            m_mon_FitMass_wideRange .push_back( p_mumuV->mass() / CLHEP::GeV );
            m_mon_FitMass           .push_back( p_mumuV->mass() / CLHEP::GeV );
            m_mon_InvMass_comb_okFit.push_back( mass / CLHEP::GeV );
            m_mon_Chi2toNDoF        .push_back( p_mumuV->chi2() / p_mumuV->ndof() );
            m_mon_Chi2toNDoFProb    .push_back( chi2prob );
            if ( p_mumuV->getMotherTrack() != NULL ) {
                m_mon_FitTotalPt.push_back( p_mumuV->getMotherTrack()->pT() / CLHEP::GeV );
                m_mon_SumPtMutrk12_okFit.push_back( (fabs(m_muon1->idTrack()->pt()) + fabs(m_muon2->idTrack()->pt())) / CLHEP::GeV );
            }
            m_mon_FitVtxR.push_back( sqrt(p_mumuV->x()*p_mumuV->x() + p_mumuV->y()*p_mumuV->y()) / CLHEP::mm );
            m_mon_FitVtxZ.push_back( p_mumuV->z() / CLHEP::mm );
        }
        // Stop the vertexing timer
        if ( timerSvc() ) {
            m_BmmHypVtx->stop();
            m_mon_VertexingTime = m_BmmHypVtx->elapsed();
        }
    }
    
    // Create vector for TrigL2Bphys particles
    // m_trigBphysColl = new TrigL2BphysContainer();
    //m_trigBphysColl = new xAOD::TrigBphysContainer();
    
    
    // Append this combination to the result
    //  m_trigBphysColl->push_back(trigPartBmumu);
    m_resultHolder.push_back(trigPartBmumu);
    
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
        m_mon_TotalRunTime = m_BmmHypTot->elapsed();
    }
    
    msg() << MSG::DEBUG << "End of acceptInput, pass is " << pass << endmsg;
    return HLT::OK;
}



/*----------------------------------------------------------------------------------------*/
HLT::ErrorCode TrigL2BMuMuFex::hltExecute(HLT::TEConstVec& , HLT::TriggerElement* outputTE)
/*----------------------------------------------------------------------------------------*/
{
    
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " In TrigL2BMuMu hltExecute" << endmsg;
    
    //  if ((m_trigBphysColl!=0) && (m_trigBphysColl->size() > 0)) {
    //    m_mon_Acceptance.push_back( ACCEPT_BphysColl_not_Empty );
    //    if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "REGTEST: Store Bphys Collection size: " << m_trigBphysColl->size() << endmsg;
    //    HLT::ErrorCode sc = attachFeature(outputTE, m_trigBphysColl, "L2BMuMuFex" );
    //    if(sc != HLT::OK) {
    //      msg()  << MSG::WARNING << "Failed to store trigBphys Collection" << endmsg;
    //      m_mon_Errors.push_back( ERROR_BphysColl_Fails );
    //      return HLT::ERROR;
    //    }
    //  } else {
    //    if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "REGTEST: no bphys collection to store "  << endmsg;
    //  }
    if (m_resultHolder.size()){
        m_mon_Acceptance.push_back( ACCEPT_BphysColl_not_Empty );
        xAOD::TrigBphysContainer * trigBphysColl = new xAOD::TrigBphysContainer;
        xAOD::TrigBphysAuxContainer trigBphysAuxColl;
        trigBphysColl->setStore(&trigBphysAuxColl);
        
        for (std::vector<xAOD::TrigBphys*>::iterator it = m_resultHolder.begin(); it != m_resultHolder.end();++it) {
            trigBphysColl->push_back(*it);
        }// for
        m_resultHolder.clear();
        if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "REGTEST: Store Bphys Collection size: " << trigBphysColl->size() << endmsg;
        HLT::ErrorCode sc = attachFeature(outputTE, trigBphysColl, "L2BMuMuFex" );
        if(sc != HLT::OK) {
            msg()  << MSG::WARNING << "Failed to store trigBphys Collection" << endmsg;
            m_mon_Errors.push_back( ERROR_BphysColl_Fails );
            return HLT::ERROR;
        }
    } else {
        if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "REGTEST: no bphys collection to store "  << endmsg;
    }
    
    
    return HLT::OK;
}



///*-----------------------------------------------------------------------------------------------------*/
//double TrigL2BMuMuFex::invariantMass(const CombinedMuonFeature* muon1, const CombinedMuonFeature* muon2)
///*-----------------------------------------------------------------------------------------------------*/
//{
//
//  // Check pointers
//  // if(!(muon1->IDTrack()) || !(muon2->IDTrack())) return 0;
//
//  double eta1,eta2,Pt1,Pt2,ph1,ph2;
//  if(muon1->IDTrack()){
//    eta1 = muon1->IDTrack()->param()->eta();
//    Pt1  = fabs(muon1->IDTrack()->param()->pT());
//    ph1  = muon1->IDTrack()->param()->phi0();
//    //       std::cout<<" IDtrack-1  pT,eta,phi= "<<Pt1<<" "<<eta1<<" "<<ph1<<std::endl;
//    //     } else if( muon1->muFastTrack() ) {
//    //       Pt1  = fabs(muon1->pt());
//    //       eta1 = muon1->muFastTrack()->eta();
//    //       ph1  = muon1->muFastTrack()->phi();
//    //       std::cout<<"  muFast-1  pT,eta,phi= "<<Pt1<<" "<<eta1<<" "<<ph1<<std::endl;
//  } else return 0;
//
//  if(muon2->IDTrack()){
//    eta2 = muon2->IDTrack()->param()->eta();
//    Pt2  = fabs(muon2->IDTrack()->param()->pT());
//    ph2  = muon2->IDTrack()->param()->phi0();
//    //       std::cout<<" IDtrack-2  pT,eta,phi= "<<Pt2<<" "<<eta2<<" "<<ph2<<std::endl;
//    //    } else if( muon2->muFastTrack() ) {
//    //       Pt2  = fabs(muon2->pt());
//    //       eta2 = muon1->muFastTrack()->eta();
//    //       ph2  = muon1->muFastTrack()->phi();
//    //       std::cout<<" muFast-2  pT,eta,phi= "<<Pt2<<" "<<eta2<<" "<<ph2<<std::endl;
//  } else return 0;
//
//  // get parameters
//  //double eta1 = muon1->IDTrack()->param()->eta();
//  //double eta2 = muon2->IDTrack()->param()->eta();
//  //double Pt1  = fabs(1/muon1->ptq());
//  //double Pt2  = fabs(1/muon2->ptq());
//  //double Pt1  = fabs(muon1->ptq());
//  //double Pt2  = fabs(muon2->ptq());
//  //double ph1  = muon1->IDTrack()->param()->phi0();
//  //double ph2  = muon2->IDTrack()->param()->phi0();
//
//  double the1 = 2*atan2((double)exp(-eta1),1.);
//  double the2 = 2*atan2((double)exp(-eta2),1.);
//  double P1   = fabs(Pt1)/sin(the1);
//  double P2   = fabs(Pt2)/sin(the2);
//  double m1   = 105.6;
//  double m2   = 105.6;
//
//  double p1[3],p2[3];
//  p1[0]  = Pt1*cos(ph1);
//  p1[1]  = Pt1*sin(ph1);
//  p1[2]  = P1*cos(the1);
//  p2[0]  = Pt2*cos(ph2);
//  p2[1]  = Pt2*sin(ph2);
//  p2[2]  = P2*cos(the2);
//
//  // evaluate mass
//  double Ptot1 = sqrt(std::pow(p1[0],2)+std::pow(p1[1],2)+std::pow(p1[2],2));
//  double Ptot2 = sqrt(std::pow(p2[0],2)+std::pow(p2[1],2)+std::pow(p2[2],2));
//  double e1 = sqrt(Ptot1*Ptot1+m1*m1);
//  double e2 = sqrt(Ptot2*Ptot2+m2*m2);
//  double mass = sqrt(m1*m1 + m2*m2 + 2*e1*e2 - 2*p1[0]*p2[0] - 2*p1[1]*p2[1] - 2*p1[2]*p2[2]);
//
//  // std::cout<<" Itogo: Mass= "<<mass<<std::endl;
//
//  return mass;
//}
//
///*---------------------------------------------------------------------------------------------*/
//double TrigL2BMuMuFex::invariantMass(const CombinedMuonFeature* muon1, const MuonFeature* muon2)
///*---------------------------------------------------------------------------------------------*/
//{
//
//  double eta1,eta2,Pt1,Pt2,ph1,ph2;
//  if(muon1->IDTrack()){
//    eta1 = muon1->IDTrack()->param()->eta();
//    Pt1  = fabs(muon1->IDTrack()->param()->pT());
//    ph1  = muon1->IDTrack()->param()->phi0();
//    msg() << MSG::DEBUG << "in invariant mass, muon 1 pT, eta, phi " << Pt1 << " " << eta1 << " " << ph1 << endmsg;
//  } else return 0;
//
//  eta2 = muon2->eta();
//  Pt2  = fabs(muon2->pt())*1000.;
//  ph2  = muon2->phi();
//  msg() << MSG::DEBUG << "in invariant mass, muon 2 pT, eta, phi " << Pt2 << " " << eta2 << " " << ph2 << endmsg;
//
//  double the1 = 2*atan2((double)exp(-eta1),1.);
//  double the2 = 2*atan2((double)exp(-eta2),1.);
//  double P1   = fabs(Pt1)/sin(the1);
//  double P2   = fabs(Pt2)/sin(the2);
//  double m1   = 105.6;
//  double m2   = 105.6;
//
//  double p1[3],p2[3];
//  p1[0]  = Pt1*cos(ph1);
//  p1[1]  = Pt1*sin(ph1);
//  p1[2]  = P1*cos(the1);
//  p2[0]  = Pt2*cos(ph2);
//  p2[1]  = Pt2*sin(ph2);
//  p2[2]  = P2*cos(the2);
//
//  // evaluate mass
//  double Ptot1 = sqrt(std::pow(p1[0],2)+std::pow(p1[1],2)+std::pow(p1[2],2));
//  double Ptot2 = sqrt(std::pow(p2[0],2)+std::pow(p2[1],2)+std::pow(p2[2],2));
//  double e1 = sqrt(Ptot1*Ptot1+m1*m1);
//  double e2 = sqrt(Ptot2*Ptot2+m2*m2);
//  double mass = sqrt(m1*m1 + m2*m2 + 2*e1*e2 - 2*p1[0]*p2[0] - 2*p1[1]*p2[1] - 2*p1[2]*p2[2]);
//
//  // std::cout<<" Itogo: Mass= "<<mass<<std::endl;
//  msg() << MSG::DEBUG << "in invariant mass, mass: " << mass << endmsg;
//
//  return mass;
//}
//
bool TrigL2BMuMuFex::isUnique(const  xAOD::TrackParticle* id1, const  xAOD::TrackParticle* id2) const {
    if (!id1 || !id2) return false;
    float dEta = fabs( id1->eta() - id2->eta() );
    float dPhi = id1->phi() - id2->phi();
    while  (dPhi >  M_PI) dPhi -= 2*M_PI;
    while  (dPhi < -M_PI) dPhi += 2*M_PI;
    
    if( dEta < 0.02 && fabs(dPhi) < 0.02 && id1->charge() * id2->charge() > 0 ) return false;
    else return true;
    
}


ElementLink<xAOD::TrackParticleContainer> TrigL2BMuMuFex::remap_container(const ElementLink<xAOD::TrackParticleContainer> & oldElink,
                                                                          const ElementLinkVector<xAOD::TrackParticleContainer> &newContainer) const {
    // take a valid element link, and remap it onto already stored objects
    // if original not valid, then no hope in finding new object, return the old
    
    //    msg() << MSG::DEBUG << oldElink.isValid() << " " << (*oldElink)->pt()<< " " << (*oldElink)->phi() << " " << (*oldElink)->eta()  << endmsg;
    //    msg() << MSG::DEBUG << "Container: " << newContainer.size() << endmsg;
    if (!oldElink.isValid()) return oldElink;
    const auto tp  = *oldElink;

    for (auto elink : newContainer) {
        if (!elink.isValid()) continue;
        const auto el  = *elink;

        // do some simple matching:
        if ( fabs(tp->pt()  - el->pt()  ) > 1   ) continue;
        if ( fabs(tp->eta() - el->eta() ) > 0.001) continue;
        double dphi = fabs(tp->phi() - el->phi());
        if (dphi > M_PI) dphi = 2*M_PI - dphi;
        if (fabs(dphi) > 0.001) continue;
        //msg() << MSG::DEBUG << elink.isValid() << " "
        //      << elink.dataID() << " " << elink.index()
        //      << " \n \t" << (*elink)->pt()<< " " << (*elink)->phi() << " " << (*elink)->eta()  << endmsg;

        // if here, then passed the critera and assume is a match
        return elink;
        
    } // loop over container
    msg() << MSG::DEBUG << " No matching link found for remapping" << endmsg;
    
      // if here, then no match was found;
      // best to return the original object
    return oldElink;
} // remap_container

ElementLink<xAOD::IParticleContainer> TrigL2BMuMuFex::remap_container(const ElementLink<xAOD::L2CombinedMuonContainer> & oldElink,
                                                                          const ElementLinkVector<xAOD::L2CombinedMuonContainer> &newContainer) const {
    // take a valid element link, and remap it onto already stored objects
    // if original not valid, then no hope in finding new object, return the old
    ////msg() << MSG::DEBUG << "IParticle mapping" << endmsg;
    //msg() << MSG::DEBUG << oldElink.isValid() << " " << (*oldElink)->pt()<< " " << (*oldElink)->phi() << " " << (*oldElink)->eta()  << endmsg;
    //msg() << MSG::DEBUG << "Container: " << newContainer.size() << endmsg;
    ElementLink<xAOD::IParticleContainer> iptlELold(oldElink.dataID(),oldElink.index());
    //msg() << MSG::DEBUG << " Valid: " << oldElink.isValid() << endmsg;
    if (!oldElink.isValid()) return iptlELold;
    const auto tp  = *oldElink;
    
    for (auto elink : newContainer) {
        if (!elink.isValid()) continue;
        const auto el  = *elink;
        
        // do some simple matching:
        if ( fabs(tp->pt()  - el->pt()  ) > 1   ) continue;
        if ( fabs(tp->eta() - el->eta() ) > 0.001) continue;
        double dphi = fabs(tp->phi() - el->phi());
        if (dphi > M_PI) dphi = 2*M_PI - dphi;
        if (fabs(dphi) > 0.001) continue;
        //msg() << MSG::DEBUG << elink.isValid() << " "
        //      << elink.dataID() << " " << elink.index()
        //      << " \n \t" << (*elink)->pt()<< " " << (*elink)->phi() << " " << (*elink)->eta()  << endmsg;
        
        // if here, then passed the critera and assume is a match
        //return elink;
        ElementLink<xAOD::IParticleContainer> iptlEL(elink.dataID(),elink.index());
        //msg() << MSG::DEBUG << "ELIparticle link: " << iptlEL.isValid() << " "
        //<< iptlEL.dataID() << " " << iptlEL.index() << endmsg;
        //msg() << MSG::DEBUG << " \n \t" << (*iptlEL)->pt()<< " " << (*iptlEL)->phi() << " " << (*iptlEL)->eta()  << endmsg;
        return iptlEL;
    } // loop over container
    msg() << MSG::DEBUG << " No matching link found for remapping" << endmsg;
    
    // if here, then no match was found;
    // best to return the original object
    return iptlELold; // this is the old link to the IParticle
} // remap_container



///*----------------------------------------------------------------------------------*/
//bool TrigL2BMuMuFex::isUnique(const  TrigInDetTrack* id1, const  TrigInDetTrack* id2)
///*----------------------------------------------------------------------------------*/
//{
//
// float dEta = fabs( id1->param()->eta() - id2->param()->eta() );
// float dPhi = id1->param()->phi0() - id2->param()->phi0();
// while  (dPhi >  M_PI) dPhi -= 2*M_PI;
// while  (dPhi < -M_PI) dPhi += 2*M_PI;
//
// if(dEta < 0.02 && fabs(dPhi) < 0.02 ) return false;
// else return true;
//}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigEFTrkMassFex.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBphysHypo
// author :  Julie Kirk
// ********************************************************************

#include "TrigEFTrkMassFex.h"
#include "BtrigUtils.h"
#include "TrigBphysHelperUtilsTool.h"

#include "xAODTrigBphys/TrigBphysAuxContainer.h"
// additions of xAOD objects
#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/MuonContainer.h"


#include <algorithm>
#include <math.h>
//#include "GaudiKernel/MsgStream.h"
//#include "GaudiKernel/IToolSvc.h"
//#include "GaudiKernel/StatusCode.h"
//
//#include "StoreGate/StoreGateSvc.h"
//
//#include "EventInfo/EventInfo.h"
//#include "EventInfo/EventID.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"

//#include "TrigBphysHypo/BtrigUtils.h"
//#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
//
//#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
//
#include "TrigSteeringEvent/TrigRoiDescriptor.h"


#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"


class ISvcLocator;

using namespace std;

/*------------------------------------------------------------------------------------*/
TrigEFTrkMassFex::TrigEFTrkMassFex(const std::string & name, ISvcLocator* pSvcLocator):
HLT::FexAlgo(name, pSvcLocator)
,m_bphysHelperTool("TrigBphysHelperUtilsTool")
,m_TotTimer(0)
,m_VtxFitTimer(0)
/*------------------------------------------------------------------------------------*/
{
    
    // Read cuts
    declareProperty( "AcceptAll", m_acceptAll  = false );
    declareProperty( "OppositeCharge", m_oppositeCharge  = true );
    declareProperty("m_matchL1", m_matchL1 = false);
    declareProperty("MuonPTthr", m_muonPtthr=4.);
    declareProperty("TrackPTthr", m_trackPtthr=1.4);
    declareProperty("dEtaTrackRoI", m_dEta_cut=0.1);
    declareProperty("dPhiTrackRoI", m_dPhi_cut=0.1);
    declareProperty("DaughterMass", m_daughterMass=105.6583715);
    declareProperty("Mass_low_cut", m_mass_low_cut=0.);
    declareProperty("Mass_high_cut", m_mass_high_cut=0.);
    declareProperty("doVertexFit", m_doVertexFit=true);
    
    // Input list of tracks for second leg
    declareProperty("TrackCollection",m_input_trackCollectionKey="InDetTrigTrackingxAODCnv_Bphysics_IDTrig");

    
    // Variables for monitoring histograms
    declareMonitoredStdContainer("Errors"           , mon_Errors                                    , AutoClear);
    declareMonitoredStdContainer("Acceptance"       , mon_Acceptance                                , AutoClear);
    declareMonitoredStdContainer("ROIEta"           , mon_ROIEta                                    , AutoClear);
    declareMonitoredStdContainer("ROIPhi"           , mon_ROIPhi                                    , AutoClear);
    declareMonitoredVariable(    "nTracks"          , mon_nTracks );
    declareMonitoredStdContainer("TrkPt"            , mon_TrkPt                                     , AutoClear);
    declareMonitoredStdContainer("TrkPt_wideRange"  , mon_TrkPt                                     , AutoClear);
    declareMonitoredStdContainer("TrkEta"           , mon_TrkEta                                    , AutoClear);
    declareMonitoredStdContainer("TrkPhi"           , mon_TrkPhi                                    , AutoClear);
    declareMonitoredStdContainer("TrkROIdEta"       , mon_TrkROIdEta                                , AutoClear);
    declareMonitoredStdContainer("TrkROIdPhi"       , mon_TrkROIdPhi                                , AutoClear);
    declareMonitoredStdContainer("TrkROIdR"         , mon_TrkROIdR                                  , AutoClear);
    declareMonitoredStdContainer("InvMassNoTrkPtCut", mon_InvMassNoTrkPtCut                         , AutoClear);
    declareMonitoredStdContainer("InvMassNoTrkPtCut_wideRange", mon_InvMassNoTrkPtCut_wideRange     , AutoClear);
    declareMonitoredStdContainer("InvMass"          , mon_InvMass                                   , AutoClear);
    declareMonitoredStdContainer("InvMass_wideRange", mon_InvMass_wideRange                         , AutoClear);
    declareMonitoredStdContainer("Trk1Pt"           , mon_Trk1Pt                                    , AutoClear);
    declareMonitoredStdContainer("Trk2Pt"           , mon_Trk2Pt                                    , AutoClear);
    declareMonitoredStdContainer("Trk1Eta"          , mon_Trk1Eta                                   , AutoClear);
    declareMonitoredStdContainer("Trk2Eta"          , mon_Trk2Eta                                   , AutoClear);
    declareMonitoredStdContainer("Trk1Phi"          , mon_Trk1Phi                                   , AutoClear);
    declareMonitoredStdContainer("Trk2Phi"          , mon_Trk2Phi                                   , AutoClear);
    declareMonitoredStdContainer("Trk1Trk2dR"       , mon_Trk1Trk2dR                                , AutoClear);
    declareMonitoredStdContainer("Trk1Trk2dEta"     , mon_Trk1Trk2dEta                              , AutoClear);
    declareMonitoredStdContainer("Trk1Trk2dPhi"     , mon_Trk1Trk2dPhi                              , AutoClear);
    declareMonitoredStdContainer("SumPtTrk12"       , mon_SumPtTrk12                                , AutoClear);
    declareMonitoredStdContainer("FitMass"          , mon_FitMass                                   , AutoClear);
    declareMonitoredStdContainer("FitMass_wideRange", mon_FitMass                                   , AutoClear);
    declareMonitoredStdContainer("InvMass_okFit"    , mon_InvMass_okFit                             , AutoClear);
    declareMonitoredStdContainer("Chi2toNDoF"       , mon_Chi2toNDoF                                , AutoClear);
    declareMonitoredStdContainer("Chi2toNDoFProb"   , mon_Chi2toNDoFProb                            , AutoClear);
    declareMonitoredStdContainer("FitTotalPt"       , mon_FitTotalPt                                , AutoClear);
    declareMonitoredStdContainer("SumPtTrk12_okFit" , mon_SumPtTrk12_okFit                          , AutoClear);
    declareMonitoredStdContainer("FitVtxR"          , mon_FitVtxR                                   , AutoClear);
    declareMonitoredStdContainer("FitVtxZ"          , mon_FitVtxZ                                   , AutoClear);
    declareMonitoredVariable(    "nBphys"           , mon_nBphys );
    declareMonitoredVariable(    "TotalRunTime"     , mon_TotalRunTime );
    declareMonitoredVariable(    "VertexingTime"    , mon_VertexingTime );
    
    // zero counters
    m_lastEvent = 999;
    m_lastEventPassed = 999;
    m_countTotalEvents=0;
    m_countTotalRoI=0;
    m_countPassedEvents=0;
    m_countPassedRoIs=0;
    m_countPassedRoIMatch=0;
    m_countPassedMass=0;
    
}

/*-------------------------------------------*/
TrigEFTrkMassFex::~TrigEFTrkMassFex()
/*-------------------------------------------*/
{}

/*-------------------------------------------*/
HLT::ErrorCode TrigEFTrkMassFex::hltInitialize()
/*-------------------------------------------*/
{
    
    if (m_bphysHelperTool.retrieve().isFailure()) {
        msg() << MSG::ERROR << "Can't find TrigBphysHelperUtilsTool" << endreq;
        return HLT::BAD_JOB_SETUP;
    } else {
        if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "TrigBphysHelperUtilsTool found" << endreq;
    }
    
    if (m_matchL1) {
        if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "matchL1 not yet re-implemented." << endreq;
        return HLT::BAD_JOB_SETUP;
    }
    
    
    msg() << MSG::INFO << "AcceptAll            = "
	<< (m_acceptAll==true ? "True" : "False") << endreq;
    msg() << MSG::INFO << "Apply opposite sign cut = " <<
    (m_oppositeCharge==true ? "True" : "False") << endreq;
    msg() << MSG::INFO << "Muon pT cut " << m_muonPtthr << endreq;
    msg() << MSG::INFO << "Track pT cut " << m_trackPtthr << endreq;
    msg() << MSG::INFO << "Track-RoI match cuts: dEta<" << m_dEta_cut << " dPhi<" << m_dPhi_cut << endreq;
    msg() << MSG::INFO << "Mass cuts :" << m_mass_low_cut << " to " << m_mass_high_cut << endreq;
    msg() << MSG::INFO << "Daughter mass:" << m_daughterMass << endreq;
    
    msg() << MSG::INFO
    << "Initialization completed successfully"
    << endreq;
    
    // add timers
    if ( timerSvc() ) {
        m_TotTimer = addTimer("EFTrkMassTot");
        m_VtxFitTimer = addTimer("EFTrkMassVFit");
    }
    return HLT::OK;
}


/*-------------------------------------------*/
HLT::ErrorCode TrigEFTrkMassFex::hltFinalize()
/*-------------------------------------------*/
{
    msg() << MSG::INFO << "|----------------------- SUMMARY FROM TrigEFTrkMassFex -------------|" << endreq;
    msg() << MSG::INFO << "in finalize()" << endreq;
    msg() << MSG::INFO << "Run on events/RoIs " << m_countTotalEvents << "/" << m_countTotalRoI <<  endreq;
    msg() << MSG::INFO << "Passed events/RoIs " << m_countPassedEvents << "/" << m_countPassedRoIs <<  endreq;
    msg() << MSG::INFO << "Passed RoI match " << m_countPassedRoIMatch <<  endreq;
    msg() << MSG::INFO << "Passed mass cuts " << m_countPassedMass <<  endreq;
    
    return HLT::OK;
}

// Define the bins for error-monitoring histogram
#define ERROR_No_EventInfo           0
#define ERROR_No_RoI                 1
#define ERROR_No_TrackColl           2
#define ERROR_No_Muon                3
#define ERROR_Empty_TrackColl        4
#define ERROR_More_TrackColl         5
#define ERROR_AddTrack_Fails         6
#define ERROR_Unique_AddTrack_Fails  7
#define ERROR_CalcInvMass_Fails      8
#define ERROR_CalcMother_Fails       9
#define ERROR_CalcMassPull_Fails    10
#define ERROR_BphysColl_Fails       11

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
#define ACCEPT_Mu1_Chi2Pt_Cut        9
#define ACCEPT_Mu1_dEta_Cut         10
#define ACCEPT_Mu1_dPhi_Cut         11
#define ACCEPT_Mu1_dR_Cut           12
#define ACCEPT_Mu2_Chi2_Cut         13
#define ACCEPT_Mu2_pT_Cut           14
#define ACCEPT_Mu2_Chi2Pt_Cut       15
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
#define ACCEPT_Each_Mu1_Chi2Pt_Cut  28
#define ACCEPT_Each_Mu1_dEta_Cut    29
#define ACCEPT_Each_Mu1_dPhi_Cut    30
#define ACCEPT_Each_Mu1_dR_Cut      31
// Separator                        32
#define ACCEPT_Each_Mu2_Chi2_Cut    33
#define ACCEPT_Each_Mu2_pT_Cut      34
#define ACCEPT_Each_Mu2_Chi2Pt_Cut  35
#define ACCEPT_Each_Opp_Charge      36
#define ACCEPT_Each_InvMass_Cut     37
#define ACCEPT_Each_AddTrack_Test   38
#define ACCEPT_Each_Vertexing       39
#define ACCEPT_Each_CalcInvMass     40
#define ACCEPT_Each_CalcMother      41
#define ACCEPT_Each_Vertex_not_NULL 42

/*--------------------------------------------------------------*/
//HLT::ErrorCode TrigEFTrkMassFex::acceptInput(const HLT::TriggerElement* inputTE, bool& pass)
/*--------------------------------------------------------------*/
//{
//
//  return HLT::OK;
//}


/*--------------------------------------------------------------*/
HLT::ErrorCode TrigEFTrkMassFex::hltExecute(const HLT::TriggerElement*  inputTE , HLT::TriggerElement* outputTE)
/*--------------------------------------------------------------*/
{
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " In EF B tag and probe FEX hltExecute" << endreq;
    
    xAOD::TrigBphysContainer * xAODTrigBphysColl = new xAOD::TrigBphysContainer;
    xAOD::TrigBphysAuxContainer xAODTrigBphysAuxColl;
    xAODTrigBphysColl->setStore(&xAODTrigBphysAuxColl);

    bool result = false;
    //bool PassedRoIMatch=false;
    //bool PassedMass=false;
    // Processing timers
    if ( timerSvc() ) m_TotTimer->start();
    // Initialize the monitoring variables
    mon_nTracks = -1;
    mon_nBphys        = 0;
    mon_TotalRunTime     = 0;
    mon_VertexingTime = 0;

    mon_Acceptance.push_back( ACCEPT_Input );
    
    // event info
    uint32_t runNumber(0), evtNumber(0), lbBlock(0);
    if (m_bphysHelperTool->getRunEvtLb( runNumber, evtNumber, lbBlock).isFailure()) {
        msg() << MSG::ERROR << "Error retriving EventInfo" << endreq;
    }

    // Accept-All mode: temporary patch; should be done with force-accept
    if (m_acceptAll) {
        if ( msgLvl() <= MSG::DEBUG )
            msg() << MSG::DEBUG << "AcceptAll property is set: taking all events" << endreq;
        mon_Acceptance.push_back( ACCEPT_AcceptAll );
        result = true;
    }
    else {
        if ( msgLvl() <= MSG::DEBUG )
            msg() << MSG::DEBUG << "AcceptAll property not set: applying selection" << endreq;
        result=false;
    } // accept all

    // get RoI descriptor
    const TrigRoiDescriptor* roiDescriptor = 0;
    if (getFeature(outputTE, roiDescriptor, "initialRoI") != HLT::OK) roiDescriptor = 0;
    
    if ( !roiDescriptor ) {
        if ( msgLvl() <= MSG::WARNING) {
            msg() <<  MSG::WARNING << "No RoI for this Trigger Element! " << endreq;
        }
        if ( timerSvc() ) m_TotTimer->stop();
        mon_Errors.push_back( ERROR_No_RoI );
        return HLT::NAV_ERROR;
    }
    mon_Acceptance.push_back( ACCEPT_Got_RoI );

    if ( msgLvl() <= MSG::DEBUG ){
        msg() << MSG::DEBUG
        << "Using TE("<< outputTE <<")->getId(): " << outputTE->getId()
        << "; RoI ID = "   << roiDescriptor->roiId()
        << ": Eta = "      << roiDescriptor->eta()
        << ", Phi = "      << roiDescriptor->phi()
        << endreq;
    }
    // RoI monitoring
    mon_ROIEta.push_back( roiDescriptor->eta() );
    mon_ROIPhi.push_back( roiDescriptor->phi() );
    if (evtNumber != m_lastEvent) {
        m_countTotalEvents++;
        m_lastEvent=evtNumber;
    }
    m_countTotalRoI++;

    typedef  ElementLinkVector<xAOD::MuonContainer>  ELVMuons;
    typedef  ElementLinkVector<xAOD::TrackParticleContainer> ELVTrackParticles;

    
    // get the muons
    ELVMuons elvmuon;
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Try to retrieve EFInfo container of muons " << endreq;
    if(getFeaturesLinks<xAOD::MuonContainer,xAOD::MuonContainer>(outputTE, elvmuon)!=HLT::OK ) {
        if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Failed to get EFInfo feature of muon, exiting" << endreq;
        mon_Errors.push_back( ERROR_No_Muon );
        if ( timerSvc() ) m_TotTimer->stop();
        return HLT::MISSING_FEATURE; // was HLT::OK
    }
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Found MuonContainer, Got MuonEF size = " << elvmuon.size() << endreq;
    if(msgLvl() <= MSG::DEBUG) { // print debug
        for ( const auto muel: elvmuon) {
            if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "ELLink: "
                << " index: "  << muel.index()
                << " sgkey: "  << muel.dataID()
                << " hashkey: "<< muel.key()
                << " valid: "  << muel.isValid()
                << " ptr: "    << (muel.isValid() ? *muel : nullptr)
                << endreq;
        }
        for ( const auto muel: elvmuon) {
            if (!muel.isValid()) continue;
            if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Muon:   "
                << " pt: " <<  (*muel)->pt()
                << " eta: " << (*muel)->eta()
                << " phi: " << (*muel)->phi()
                << " q: "   << (*muel)->charge()
                << " mutype: "   << (*muel)->muonType()
                << " author: "   << (*muel)->author()
                << " id/ms/cb: " << (*muel)->trackParticle(xAOD::Muon::InnerDetectorTrackParticle)
                << " "           << (*muel)->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle)
                << " "           << (*muel)->trackParticle(xAOD::Muon::CombinedTrackParticle)
                << endreq;
            
        } // for

    } // if debug
    
    // do some muon collections check to see that the ELVs do the same as the vec collections
    std::vector<const xAOD::MuonContainer*> MuEFTracks;
    if ( getFeatures(inputTE, MuEFTracks) != HLT::OK ) {
        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Failed to get EFInfo feature, exiting" << endreq;
        return HLT::OK;
    }
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Get muon vec collections of size: " << MuEFTracks.size() << endreq;
    for (const auto mucol : MuEFTracks) {
        if (!mucol) continue;
        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "This muon collection has: " << mucol->size() << " muons " << endreq << endreq;
    }
    
    // now get the tracks
    ELVTrackParticles elvtps;
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Try to retrieve TrackParticleContainers " << endreq;
    //if(getFeaturesLinks<xAOD::TrackParticleContainer,xAOD::TrackParticleContainer>(outputTE, elvtps)!=HLT::OK ) {
    if(getFeaturesLinks<xAOD::TrackParticleContainer,xAOD::TrackParticleContainer>(inputTE, elvtps,m_input_trackCollectionKey)!=HLT::OK ) {
        if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Failed to get TrackParticleContainers feature, exiting" << endreq;
        if ( timerSvc() ) m_TotTimer->stop();
        mon_Errors.push_back( ERROR_No_TrackColl );
        return HLT::MISSING_FEATURE; // was HLT::OK
    }

    if(msgLvl() <= MSG::DEBUG) { // print debug
        msg() << MSG::DEBUG << "Found TrackParticleContainer, size: " << elvtps.size() << endreq;
        for ( const auto eltp: elvtps) {
            if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "ELLink: "
                << " index: "  << eltp.index()
                << " sgkey: "  << eltp.dataID()
                << " hashkey: "<< eltp.key()
                << " valid: "  << eltp.isValid()
                << " ptr: "    << (eltp.isValid() ? *eltp : nullptr)
                << endreq;
        }

    } // if debug

    //    // some debug stuff re tracks - previously used vector of track collections
    //    // now want to use ElementLinkVector. Add some extra checks here to compare the two approaches
    //    std::vector<const xAOD::TrackParticleContainer*> vectorOfTrackCollections;
    //    HLT::ErrorCode status = getFeatures(inputTE, vectorOfTrackCollections);
    //    //    HLT::ErrorCode status = getFeatures(outputTE, vectorOfTrackCollections);
    //    if(status != HLT::OK ) {
    //        msg() << MSG::WARNING << "Error when getting vec TrackParticleContainer's from the trigger element" << endreq;
    //        return HLT::MISSING_FEATURE;
    //    } else {
    //        if ( msgLvl() <= MSG::DEBUG )msg() << MSG::DEBUG << " Got vec Track Collections, size " << vectorOfTrackCollections.size() << endreq;
    //    }
    //    for (const auto trkcol : vectorOfTrackCollections) {
    //        if (!trkcol) continue;
    //        if ( msgLvl() <= MSG::DEBUG )msg() << MSG::DEBUG << " This track collection has: " << trkcol->size() << " tracks " << endreq;
    //    } // vec loop
    //
    //    const xAOD::TrackParticleContainer* ptlcont(nullptr);
    //    status = getFeature(inputTE, ptlcont,m_input_trackCollectionKey);
    //    if(status != HLT::OK || !ptlcont ) {
    //        msg() << MSG::WARNING << "Error when getting 1 TrackParticleContainer's from the trigger element" << endreq;
    //        return HLT::MISSING_FEATURE;
    //    } else {
    //        if ( msgLvl() <= MSG::DEBUG )msg() << MSG::DEBUG << " Got 1 Track Collections, size " << ptlcont->size() << endreq;
    //    }

    
    mon_Acceptance.push_back( ACCEPT_Got_TrackColl );
    if (elvtps.size() == 0) {
        if ( timerSvc() ) m_TotTimer->stop();
        mon_Errors.push_back( ERROR_Empty_TrackColl );
        return HLT::OK;
    }
    mon_Acceptance.push_back( ACCEPT_Full_TrackColl );

    
    // Loop over tracks (Particles)
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Now loop over tracks " << endreq;
    if (elvtps.size() > 0 ) mon_Acceptance.push_back( ACCEPT_Full_IDTracks );
    mon_nTracks = elvtps.size();
    
    // Boolean flags indicating what stages were already reached (for monitoring purposes)
    m_flag_stages.resize(43,false); // should set the size the first time, and reinitiallise on all others
                                    // On second and subsequent pases, need to force to false.
    std::fill(m_flag_stages.begin(), m_flag_stages.end(), false);
    
    // Prepare the vertexing timer
    if ( timerSvc() ) {
        m_VtxFitTimer->start();
        m_VtxFitTimer->pause(); // resume it only for the vertexing periods
    }

    
    // combine track from muon with track from id
    //#FIXME - remember to implement the scenario of (tracks matched to roi + tracks)
    std::vector<ElementLink<xAOD::MuonContainer> > muons;
    std::vector<ElementLink<xAOD::TrackParticleContainer> > tracks;
    for (auto muel: elvmuon) {
        if (!muel.isValid()) continue;
        const xAOD::TrackParticle * mutrk = (*muel)->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
        if (!mutrk) continue;
        if (mutrk->definingParametersCovMatrixVec().size() == 0) {
            if ( msgLvl() <= MSG::INFO ) msg() << MSG::INFO << "mu track has no ParametersCovMatrix - will reject" << endreq;
            //#FIXME add monitoring flags here
            continue;
        }

        double pT = mutrk->pt();
        float trackChi2 = mutrk->chiSquared();
        //double eta =  mutrk->eta() ;
        //double phi =  mutrk->phi() ;

        if ( fabs(pT) >= m_muonPtthr ) {
            if ( !m_flag_stages[ ACCEPT_Mu1_pT_Cut ] ) {
                mon_Acceptance.push_back( ACCEPT_Mu1_pT_Cut );
                m_flag_stages[ ACCEPT_Mu1_pT_Cut ] = true;
            }
            mon_Acceptance.push_back( ACCEPT_Each_Mu1_pT_Cut );
        } else {
            if ( msgLvl() <= MSG::VERBOSE ) msg() << MSG::VERBOSE << "Muon fails pT cut " <<mutrk  << " pT = "  << pT << endreq;
        } // threshold
        if ( trackChi2 <= 1e7 ) {
            if ( !m_flag_stages[ ACCEPT_Mu1_Chi2_Cut ] ) {
                mon_Acceptance.push_back( ACCEPT_Mu1_Chi2_Cut );
                m_flag_stages[ACCEPT_Mu1_Chi2_Cut] = true;
            }
            mon_Acceptance.push_back( ACCEPT_Each_Mu1_Chi2_Cut );
        } else {
            if ( msgLvl() <= MSG::VERBOSE ) msg() << MSG::VERBOSE << "Track fails chi2 cut "<<mutrk  << " chi2 = "  << trackChi2 << endreq;
        }
        if ( fabs(pT) <m_muonPtthr || trackChi2 > 1e7 ) continue;
        // Check pT+chi2 (monitoring only)
        if ( !m_flag_stages[ ACCEPT_Mu1_Chi2Pt_Cut ] ) {
            mon_Acceptance.push_back( ACCEPT_Mu1_Chi2Pt_Cut );
            m_flag_stages[ACCEPT_Mu1_Chi2Pt_Cut] = true;
        }
        mon_Acceptance.push_back( ACCEPT_Each_Mu1_Chi2Pt_Cut );

        // these are muon tracks, so the d-cuts are a but redundant atm.
        //        dphi = fabs(dphi);
        //        deta = fabs(deta);
        //
        //        // Monitoring of the RoI matching
        //        mon_TrkROIdEta.push_back( fabs(deta) );
        //        mon_TrkROIdPhi.push_back( fabs(dphi) );
        //        mon_TrkROIdR  .push_back( sqrt(dphi*dphi+deta*deta) );
        //
        //        // Check the deta and dphi cuts (monitoring only)
        //        if ( fabs(deta) < m_dEta_cut ) {
        //            if ( !m_flag_stages[ ACCEPT_Mu1_dEta_Cut ] ) {
        //                mon_Acceptance.push_back( ACCEPT_Mu1_dEta_Cut );
        //                m_flag_stages[ ACCEPT_Mu1_dEta_Cut ] = true;
        //            }
        //            mon_Acceptance.push_back( ACCEPT_Each_Mu1_dEta_Cut );
        //        }
        //        if ( fabs(dphi) < m_dPhi_cut ) {
        //            if ( !m_flag_stages[ ACCEPT_Mu1_dPhi_Cut ] ) {
        //                mon_Acceptance.push_back( ACCEPT_Mu1_dPhi_Cut );
        //                m_flag_stages[ ACCEPT_Mu1_dPhi_Cut ] = true;
        //            }
        //            mon_Acceptance.push_back( ACCEPT_Each_Mu1_dPhi_Cut );
        //        }
        //
        //        if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "check RoI match deta, dphi: " << deta << " " << dphi << endreq;
        //
        //        if( deta < m_dEta_cut && dphi < m_dPhi_cut)
        //        { PassedRoIMatch = true;
        //
        //            // Check deta+dphi cuts (monitoring only)
        //            if ( !m_flag_stages[ ACCEPT_Mu1_dR_Cut ] ) {
        //                mon_Acceptance.push_back( ACCEPT_Mu1_dR_Cut );
        //                m_flag_stages[ ACCEPT_Mu1_dR_Cut ] = true;
        //            }
        //            mon_Acceptance.push_back( ACCEPT_Each_Mu1_dR_Cut );

        
        muons.push_back(muel);
    } // optimize? addUnique?
    for (auto trkel: elvtps)  {
        const xAOD::TrackParticle * trk = *trkel;
        if (!trk) continue;
        if (trk->definingParametersCovMatrixVec().size() == 0) {
            if ( msgLvl() <= MSG::INFO ) msg() << MSG::INFO << "track has no ParametersCovMatrix - will reject" << endreq;
            //#FIXME add monitoring flags here
            continue;
        }


        double pT2 = trk->pt();
        float track2Chi2 = trk->chiSquared();
        double eta2 =  trk->eta() ;
        double phi2 =  trk->phi() ;

        if ( track2Chi2 <= 1e7 ) {
            if ( !m_flag_stages[ ACCEPT_Mu2_Chi2_Cut ] ) {
                mon_Acceptance.push_back( ACCEPT_Mu2_Chi2_Cut );
                m_flag_stages[ ACCEPT_Mu2_Chi2_Cut ] = true;
            }
            mon_Acceptance.push_back( ACCEPT_Each_Mu2_Chi2_Cut );
        }
        if ( pT2 >= m_trackPtthr ) {
            if ( !m_flag_stages[ ACCEPT_Mu2_pT_Cut ] ) {
                mon_Acceptance.push_back( ACCEPT_Mu2_pT_Cut );
                m_flag_stages[ ACCEPT_Mu2_pT_Cut ] = true;
            }
            mon_Acceptance.push_back( ACCEPT_Each_Mu2_pT_Cut );
        }
        
        mon_TrkPt.push_back(pT2*0.001);
        mon_TrkEta.push_back(eta2);
        mon_TrkPhi.push_back(phi2);
        
        mon_TrkROIdEta.push_back( m_bphysHelperTool->absDeltaEta(eta2, roiDescriptor->eta()) );
        mon_TrkROIdPhi.push_back( m_bphysHelperTool->absDeltaPhi(phi2, roiDescriptor->phi()) );
        mon_TrkROIdR.push_back( m_bphysHelperTool->deltaR(eta2,phi2, roiDescriptor->eta(),roiDescriptor->phi()) );
        
        tracks.push_back(trkel);
    }// optimize? addUnique?
    
    buildMuTrkPairs(roiDescriptor,muons,tracks,*xAODTrigBphysColl); // make pairs of objects and add to output

    if (xAODTrigBphysColl->size()) result = true;
    
    // Reach this point successfully
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Run: " << runNumber << " Event: " << evtNumber << "  result is " << result << endreq;
    // Stop the processing timers
    if ( timerSvc() ) {
        m_TotTimer   ->stop();
        m_VtxFitTimer->resume();
        m_VtxFitTimer->stop();
        // Fill the monitoring variables
        mon_TotalRunTime     = m_TotTimer   ->elapsed();
        mon_VertexingTime = m_VtxFitTimer->elapsed();
    }

    if (/*xAODTrigBphysColl &&*/ xAODTrigBphysColl->size()) {
        if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "REGTEST: Store Bphys Collection size: " << xAODTrigBphysColl->size() << endreq;
        
        HLT::ErrorCode sc = attachFeature(outputTE, xAODTrigBphysColl, "EFTrackMass" );
        if(sc != HLT::OK) {
            msg()  << MSG::WARNING << "Failed to store trigBphys Collection" << endreq;
            delete xAODTrigBphysColl; xAODTrigBphysColl = nullptr; // assume deletion responsibility
            return HLT::ERROR;
        }
    } else {
        if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "REGTEST: no bphys collection to store "  << endreq;
        delete xAODTrigBphysColl; xAODTrigBphysColl = nullptr;
    }

    return HLT::OK;
}


void TrigEFTrkMassFex::buildMuTrkPairs(const TrigRoiDescriptor * roi,
                                           const std::vector<ElementLink<xAOD::MuonContainer> > & muons,
                                           const std::vector<ElementLink< xAOD::TrackParticleContainer> > & tracks,
                                           xAOD::TrigBphysContainer & physcontainer) {
    if (!roi) {
        if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "null roi ptr"<< endreq;
        return;
    }
    if (!muons.size()) {
        if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "No Muons in buildTrkPairs"<< endreq;
        return;
    }
    if (!tracks.size()) {
        if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "No tracks in buildTrkPairs"<< endreq;
        return;
    }

    for (auto muel: muons) {
        if (!muel.isValid()) continue;
        const xAOD::Muon * muon = *muel;
        if (!muon) continue;
        const xAOD::TrackParticle * mutrk = muon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
        const ElementLink<xAOD::TrackParticleContainer> & mutrkel = muon->trackParticleLink(xAOD::Muon::InnerDetectorTrackParticle);
        if (!mutrk) continue;
        
        for (auto trkel: tracks) {
            if (!trkel.isValid()) continue;
            const xAOD::TrackParticle * trk = *trkel;
            if (!trk) continue;
            
            if (trk->definingParametersCovMatrixVec().size() == 0) {
                if ( msgLvl() <= MSG::INFO ) msg() << MSG::INFO << "track has no ParametersCovMatrix - will reject" << endreq;
                //#FIXME add monitoring flags here
                continue;
            }

            // compare the two tps, make sure unique
            if (!m_bphysHelperTool->areUnique(mutrk,trk,0.005,0.005,10)) {
                if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Trk and MuTrk match, continue"<< endreq;
            }
            
            double charge1 = mutrk->charge();
            double charge2 = trk  ->charge();

            double pT1 = mutrk->pt();
            double pT2 = trk->pt();
            //float track1Chi2 = mutrk->chiSquared();
            float track2Chi2 = trk->chiSquared();
            double eta1 =  mutrk->eta() ;
            double eta2 =  trk->eta() ;
            double phi1 =  mutrk->phi() ;
            double phi2 =  trk->phi() ;

            

            
            
            std::vector<const xAOD::TrackParticle*> inputtrks;
            std::vector<double> massHypo ;
            inputtrks.push_back(mutrk);
            inputtrks.push_back(trk);
            massHypo.push_back( m_daughterMass );
            massHypo.push_back( m_daughterMass );

            double Mass = m_bphysHelperTool->invariantMass(mutrk,trk,m_daughterMass,m_daughterMass);

            if ( track2Chi2 <= 1e7 ) {
                if ( pT2 > 2e3 ) { // updated the hard coded value to GeV, rather than MeV
                    mon_InvMassNoTrkPtCut.push_back( Mass / CLHEP::GeV );
                    mon_InvMassNoTrkPtCut_wideRange.push_back( Mass / CLHEP::GeV );
                }
            } // If good track
            
            // check Pt and chi2
            if( pT2 < m_trackPtthr || track2Chi2 > 1e7) continue;
            if ( !m_flag_stages[ ACCEPT_Mu2_Chi2Pt_Cut ] ) {
                mon_Acceptance.push_back( ACCEPT_Mu2_Chi2Pt_Cut );
                m_flag_stages[ ACCEPT_Mu2_Chi2Pt_Cut ] = true;
            }
            mon_Acceptance.push_back( ACCEPT_Each_Mu2_Chi2Pt_Cut );

            if (m_oppositeCharge) {
                if ( charge1 * charge2 > 0) {
                    if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Fail opp charge reqirement." << charge1 << " " << charge2<< endreq;
                    continue;
                } else {// same q
                    if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Passes opp charge reqirement." << charge1 << " " << charge2<< endreq;
                }
            } else {// apply opp charge cut
                if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE
                    << "opposite sign cuts not applied, make mass cuts " << endreq;
            }
            if ( !m_flag_stages[ ACCEPT_Opp_Charge ] ) {
                mon_Acceptance.push_back( ACCEPT_Opp_Charge );
                m_flag_stages[ ACCEPT_Opp_Charge ] = true;
            }
            mon_Acceptance.push_back( ACCEPT_Each_Opp_Charge );

            
            // Check the invariant mass cut
            if (Mass < m_mass_low_cut || Mass > m_mass_high_cut){
                if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Fail mass cuts: " << Mass << endreq;
                continue;
            } else {
                if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Passes Mass cut " << Mass << endreq;
            }
            if ( !m_flag_stages[ ACCEPT_InvMass_Cut ] ) {
                mon_Acceptance.push_back( ACCEPT_InvMass_Cut );
                m_flag_stages[ ACCEPT_InvMass_Cut ] = true;
            }
            mon_Acceptance.push_back( ACCEPT_Each_InvMass_Cut );
            mon_InvMass.push_back( Mass / CLHEP::GeV );
            mon_InvMass_wideRange.push_back( Mass / CLHEP::GeV );

            // Monitoring of the di-muon track pairs
            mon_Trk1Pt .push_back( pT1 / CLHEP::GeV );
            mon_Trk2Pt .push_back( pT2 / CLHEP::GeV );
            mon_Trk1Eta.push_back( eta1 );
            mon_Trk2Eta.push_back( eta2 );
            mon_Trk1Phi.push_back( phi1 );
            mon_Trk2Phi.push_back( phi2 );
            mon_SumPtTrk12.push_back ( (pT1 + pT2) / CLHEP::GeV );

            mon_Trk1Trk2dEta.push_back( m_bphysHelperTool->absDeltaEta(eta1,eta2) );
            mon_Trk1Trk2dPhi.push_back( m_bphysHelperTool->absDeltaPhi(phi1,phi2) );
            mon_Trk1Trk2dR  .push_back( m_bphysHelperTool->deltaR(eta1,phi1,eta2,phi2) );

            // got to here - add object to container
            xAOD::TrigBphys* xaodObj = new xAOD::TrigBphys();
            physcontainer.push_back( xaodObj );
            xaodObj->initialise(roi->roiId(), 0.,0.,0.,
                                xAOD::TrigBphys::JPSIMUMU, Mass, xAOD::TrigBphys::EF );
            m_bphysHelperTool->fillTrigObjectKinematics(xaodObj,{mutrk,trk});
            
            // #FIXME- is it really JPSIMUMU flag?
            
            xaodObj->addTrackParticleLink(mutrkel);
            xaodObj->addTrackParticleLink(trkel);
            
            // Find and set the IParticle link to the Muon and Track
            // note it's the muon, and not the muon's ID track here.
            ElementLink<xAOD::IParticleContainer> ptl1EL,ptl2EL;
            ptl1EL.resetWithKeyAndIndex(muel.dataID(),muel.index());
            ptl2EL.resetWithKeyAndIndex(trkel.dataID(),trkel.index());

            xaodObj->addParticleLink(ptl1EL); //
            xaodObj->addParticleLink(ptl2EL); //

            
            
            if (m_doVertexFit) {
                std::vector<ElementLink<xAOD::TrackParticleContainer> > input = {mutrkel, trkel};
                if (m_bphysHelperTool->vertexFit(xaodObj,input,massHypo).isFailure()) {
                    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Problems with vertex fit"  << endreq;
                }
                // Passed vertexing (monitoring only) - well actually, only it's called
                if ( !m_flag_stages[ ACCEPT_Vertexing ] ) {
                    mon_Acceptance.push_back( ACCEPT_Vertexing );
                    m_flag_stages[ ACCEPT_Vertexing ] = true;
                }
                mon_Acceptance.push_back( ACCEPT_Each_Vertexing );
                if ( timerSvc() ) m_VtxFitTimer->pause();
            } // vertex fitting
            
            if (xaodObj->fitmass() < 0) {
              if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Fail to get fitMass from xAOD::TrigBphys" << endreq;
              // mon_Errors.push_back( ERROR_CalcInvMass_Fails );
            }
            else {
                if ( !m_flag_stages[ ACCEPT_CalcInvMass ] ) {
                    mon_Acceptance.push_back( ACCEPT_CalcInvMass );
                    m_flag_stages[ ACCEPT_CalcInvMass ] = true;
                }
                mon_Acceptance.push_back( ACCEPT_Each_CalcInvMass );
            }
            mon_FitMass       .push_back( xaodObj->fitmass() / CLHEP::GeV );
            mon_InvMass_okFit .push_back( xaodObj->mass() / CLHEP::GeV );

            double chisq     = xaodObj->fitchi2();
            unsigned int ndf = xaodObj->fitndof();
            
            if (chisq <=0 || ndf == 0) {
                
                mon_Chi2toNDoF.push_back( -1. );
                mon_Chi2toNDoFProb.push_back(-1.);
            } else {
                mon_Chi2toNDoF.push_back( chisq / ndf );
                double chi2prob = 1.0 - Genfun::CumulativeChiSquare( ndf )(chisq);
                mon_Chi2toNDoFProb.push_back( chi2prob );
            }
            double r2 = xaodObj->fitx()*xaodObj->fitx() + xaodObj->fitz()*xaodObj->fitz();
            mon_FitVtxR.push_back(sqrt(r2));
            mon_FitVtxZ.push_back(xaodObj->fitz());
            
        } // for tracks
    } // for mu
    
    
    
} //buildMuTrkPairs




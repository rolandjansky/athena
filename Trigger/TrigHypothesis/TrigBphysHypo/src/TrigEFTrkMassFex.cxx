/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigEFTrkMassFex.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBphysHypo
// author :  Julie Kirk
// ********************************************************************

#include <math.h>
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"

#include "StoreGate/StoreGateSvc.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "TrigBphysHypo/TrigEFTrkMassFex.h"
#include "TrigBphysHypo/BtrigUtils.h"
#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"

#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"


#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"

// additions of xAOD objects
#include "xAODEventInfo/EventInfo.h"

//#include <map.h>

class ISvcLocator;

using namespace std;

/*------------------------------------------------------------------------------------*/
TrigEFTrkMassFex::TrigEFTrkMassFex(const std::string & name, ISvcLocator* pSvcLocator):
HLT::FexAlgo(name, pSvcLocator),
m_iVKVVertexFitter("Trk::TrkVKalVrtFitter")
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
    declareProperty("DaughterMass", m_daughterMass=0.0);
    declareProperty("Mass_low_cut", m_mass_low_cut=0.);
    declareProperty("Mass_high_cut", m_mass_high_cut=0.);
    declareProperty("doVertexFit", m_doVertexFit=true);
    
    // variables for monitoring histograms
    //declareMonitoredVariable("Ntrack" , m_Ntrack);
    //declareMonitoredVariable("NBphys" , m_NBphys);
    //declareMonitoredStdContainer("trackPt" , m_trackPt);
    //declareMonitoredStdContainer("dEta", m_dEta);
    //declareMonitoredStdContainer("dPhi", m_dPhi);
    //declareMonitoredStdContainer("Mass", m_Mass);
    //declareMonitoredStdContainer("FitMass", m_FitMass);
    //declareMonitoredStdContainer("Chi2", m_Chi2);
    //declareMonitoredStdContainer("Chi2Prob", m_Chi2Prob);
    
    // Variables for monitoring histograms
    declareMonitoredStdContainer("Errors"           , mon_Errors                                    , AutoClear);
    declareMonitoredStdContainer("Acceptance"       , mon_Acceptance                                , AutoClear);
    declareMonitoredStdContainer("ROIEta"           , mon_ROIEta                                    , AutoClear);
    declareMonitoredStdContainer("ROIPhi"           , mon_ROIPhi                                    , AutoClear);
    declareMonitoredVariable(    "nTracks"          , mon_nTracks );
    declareMonitoredStdContainer("TrkPt"            , mon_TrkPt                                     , AutoClear);
    declareMonitoredStdContainer("TrkPt_wideRange"  , mon_TrkPt_wideRange                           , AutoClear);
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
    
    m_trigBphysColl=0;
}

/*-------------------------------------------*/
TrigEFTrkMassFex::~TrigEFTrkMassFex()
/*-------------------------------------------*/
{
    delete m_trigBphysColl;
}

/*-------------------------------------------*/
HLT::ErrorCode TrigEFTrkMassFex::hltInitialize()
/*-------------------------------------------*/
{
    
    // Get the VKalVrt vertex fitting tool from ToolSvc
    if ( m_iVKVVertexFitter.retrieve().isFailure() )
    {
        if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"Fail to retrieve tool "<< m_iVKVVertexFitter<<endreq;
    }
    else {
        if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"Retrieved tool " << m_iVKVVertexFitter<<endreq;
        m_VKVFitter = dynamic_cast<Trk::TrkVKalVrtFitter*>(&(*m_iVKVVertexFitter));
    }
    // print out properties, cuts
    
    
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
    bool result = false;
    bool PassedRoIMatch=false;
    bool PassedMass=false;
    // Processing timers
    if ( timerSvc() ) m_TotTimer->start();
    // Initialize the monitoring variables
    mon_nTracks = -1;
    mon_nBphys        = 0;
    mon_TotalRunTime     = 0;
    mon_VertexingTime = 0;
    
    /*
     m_Ntrack=-99;
     m_NBphys=-99;
     m_trackPt.clear();
     m_dEta.clear();
     m_dPhi.clear();
     m_Mass.clear();
     m_FitMass.clear();
     m_Chi2.clear();
     m_Chi2Prob.clear();
     */
    
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " In EF B tag and probe FEX hltExecute" << endreq;
    mon_Acceptance.push_back( ACCEPT_Input );
    // Retrieve event info
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
        if ( msgLvl() <= MSG::DEBUG )
            msg() << MSG::DEBUG << "AcceptAll property is set: taking all events"
            << endreq;
        mon_Acceptance.push_back( ACCEPT_AcceptAll );
        result = true;
    }
    else {
        if ( msgLvl() <= MSG::DEBUG )
            msg() << MSG::DEBUG << "AcceptAll property not set: applying selection"
            << endreq;
        result=false;
    }
    
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
    
    
    if (IdEvent != (int) m_lastEvent) {
        m_countTotalEvents++;
        m_lastEvent=IdEvent;
    }
    m_countTotalRoI++;
    
    //  create vector for TrigEFBphys particles
    delete m_trigBphysColl;
    m_trigBphysColl = new TrigEFBphysContainer();
    //  m_VertexColl = new TrigVertexCollection;
    // TrigEFBphys* trigPart (NULL);
    
    ///////////////// Get vector of tracks /////////////////
    const Rec::TrackParticleContainer* TrkParticleCont=NULL;
    
    std::vector<const Rec::TrackParticleContainer*> vectorOfTrackCollections;
    
    //  HLT::ErrorCode status = getFeature(outputTE, TrkParticleCont);
    HLT::ErrorCode status = getFeatures(outputTE, vectorOfTrackCollections);
    if(status != HLT::OK ) {
        msg() << MSG::WARNING << "Error when getting TrackParticleContainer's from the trigger element" << endreq;
        if ( timerSvc() ) m_TotTimer->stop();
        mon_Errors.push_back( ERROR_No_TrackColl );
        return HLT::MISSING_FEATURE;
        // }
        //else if (TrkParticleCont==NULL) {
        //   msg() << MSG::DEBUG << "No TrackParticle container" << endreq;
        //   if ( timerSvc() ) m_TotTimer->stop();
        //   mon_Errors.push_back( ERROR_No_TrackColl );
        //   return HLT::MISSING_FEATURE;
    } else {
        //    if ( msgLvl() <= MSG::DEBUG )msg() << MSG::DEBUG << " Got Track Collections, size " << TrkParticleCont->size() << endreq;
        if ( msgLvl() <= MSG::DEBUG )msg() << MSG::DEBUG << " Got Track Collections, size " << vectorOfTrackCollections.size() << endreq;
    }
    mon_Acceptance.push_back( ACCEPT_Got_TrackColl );
    //    if (TrkParticleCont->size() == 0) {
    if (vectorOfTrackCollections.size() == 0) {
        if ( timerSvc() ) m_TotTimer->stop();
        mon_Errors.push_back( ERROR_Empty_TrackColl );
        
        //    if (TrkParticleCont->size() !=1) {
        //  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Got " << TrkParticleCont->size()
        //	  << " InDetTrackCollections, expected one - is there a problem?" << endreq;
        return HLT::OK;
    }
    mon_Acceptance.push_back( ACCEPT_Full_TrackColl );
    
    std::vector<const Rec::TrackParticleContainer*>::iterator pTrackColl    = vectorOfTrackCollections.begin();
    std::vector<const Rec::TrackParticleContainer*>::iterator lastTrackColl = vectorOfTrackCollections.end();
    
    // JK If more than 1 track collection then this is FullScan instance. Find collection with most tarcks
    
    unsigned int Ntracks=0;
    for (;pTrackColl != lastTrackColl; ++ pTrackColl) {
        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "  InDetTrackCollections size, " << (*pTrackColl)->size() << endreq;
        if ((*pTrackColl)->size() > Ntracks) {
            if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Largest collection so far, assume it is FullScan " << endreq;
            Ntracks= (*pTrackColl)->size();
            TrkParticleCont = *pTrackColl;
        }
    }
    
    
    // Retrieve the MuonFInfo container
    std::vector<const TrigMuonEFInfoContainer*> MuEFTracks;
    if (!m_matchL1) {
        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Try to retrieve EFInfo container" << endreq;
        status = getFeatures(inputTE, MuEFTracks);
        if ( status != HLT::OK ) {
            if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Failed to get EFInfo feature, exiting" << endreq;
            mon_Errors.push_back( ERROR_No_Muon );
            return HLT::OK;
        }
        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Got MuonEF Feature, size = " << MuEFTracks.size() << endreq;
        for ( unsigned int i_mu=0; i_mu<MuEFTracks.size(); i_mu++ ) {
            std::vector<const Trk::Track*> idTrks;
            HLT::ErrorCode status = GetTrigMuonEFInfoTracks(MuEFTracks[i_mu], idTrks, msg());
            if ( status != HLT::OK ) return status;
            for ( unsigned int i_trk=0; i_trk<idTrks.size(); i_trk++ ) {
                const Trk::Perigee* perigee = idTrks[i_trk]->perigeeParameters();
                if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG
                    << " Comb muon pt/eta/phi " << perigee->pT()
                    << " / " << perigee->eta() << " / " << perigee->parameters()[Trk::phi]
                    << endreq;
            }
        }
    }
    
    
    ////////////////////////////////////////////////////////////
    if(IdEvent != (int) m_lastEvent) {
        m_countTotalEvents++;
        m_lastEvent = IdEvent;
    }
    m_countTotalRoI++;
    
    
    // Loop over tracks (Particles)
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Now loop over tracks " << endreq;
    
    Rec::TrackParticleContainer::const_iterator trkIt =  TrkParticleCont->begin();
    Rec::TrackParticleContainer::const_iterator lastTrkIt = TrkParticleCont->end();
    if ( msgLvl() <= MSG::DEBUG )msg() << MSG::DEBUG << "Found tracks, ntrack= " << TrkParticleCont->size() << endreq;
    // Number of tracks monitoring
    if (TrkParticleCont->size() > 0 ) mon_Acceptance.push_back( ACCEPT_Full_IDTracks );
    mon_nTracks = TrkParticleCont->size();
    
    // Boolean flags indicating what stages were already reached (for monitoring purposes)
    std::vector<bool> flag_stages( 43, false );
    
    // Prepare the vertexing timer
    if ( timerSvc() ) {
        m_VtxFitTimer->start();
        m_VtxFitTimer->pause(); // resume it only for the vertexing periods
    }
    for (int itrk=0 ; trkIt != lastTrkIt; itrk++, trkIt++) {
        
        // JW EDM const Trk::MeasuredPerigee* trckPerigee =  (*trkIt)->measuredPerigee();
        const Trk::Perigee* trckPerigee =  (*trkIt)->measuredPerigee();
        
        //      double d0 = trckPerigee->parameters()[Trk::d0];
        double px = trckPerigee->momentum()[Trk::px];
        double py = trckPerigee->momentum()[Trk::py];
        double ptSquare=std::pow(px,2) + std::pow(py,2);
        double pT = sqrt(ptSquare) * trckPerigee->parameters()[Trk::qOverP]/fabs(trckPerigee->parameters()[Trk::qOverP]);
        //m_trackPt.push_back(fabs(pT)/CLHEP::GeV);
        
        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Track: " << itrk << " pT = "  <<  pT << " phi = " << trckPerigee->parameters()[Trk::phi] << endreq;
        double phi = trckPerigee->parameters()[Trk::phi];
        //      double theta = trckPerigee->parameters()[Trk::theta];
        double eta = trckPerigee->eta();
        // Tracks monitoring
        mon_TrkPt_wideRange.push_back( pT / CLHEP::GeV );
        mon_TrkPt .push_back( pT / CLHEP::GeV );
        mon_TrkEta.push_back( eta );
        mon_TrkPhi.push_back( phi );
        float trackChi2 = (*trkIt)->fitQuality()->chiSquared();
        //filter tracks pT, quality?
        // JK 4/5/11 change to muonPtthr i.e. different pT cuts for muon and track
        if ( fabs(pT) >= m_muonPtthr ) {
            if ( !flag_stages[ ACCEPT_Mu1_pT_Cut ] ) {
                mon_Acceptance.push_back( ACCEPT_Mu1_pT_Cut );
                flag_stages[ ACCEPT_Mu1_pT_Cut ] = true;
            }
            mon_Acceptance.push_back( ACCEPT_Each_Mu1_pT_Cut );
        } else {
            if ( msgLvl() <= MSG::VERBOSE ) msg() << MSG::VERBOSE << "Track fails pT cut " << itrk << " pT = "  << pT << endreq;
        }
        if ( trackChi2 <= 1e7 ) {
            if ( !flag_stages[ ACCEPT_Mu1_Chi2_Cut ] ) {
                mon_Acceptance.push_back( ACCEPT_Mu1_Chi2_Cut );
                flag_stages[ACCEPT_Mu1_Chi2_Cut] = true;
            }
            mon_Acceptance.push_back( ACCEPT_Each_Mu1_Chi2_Cut );
        } else {
            if ( msgLvl() <= MSG::VERBOSE ) msg() << MSG::VERBOSE << "Track fails chi2 cut " << itrk << " chi2 = "  << trackChi2 << endreq;
        }
        if ( fabs(pT) <m_muonPtthr || trackChi2 > 1e7 ) continue;
        // Check pT+chi2 (monitoring only)
        if ( !flag_stages[ ACCEPT_Mu1_Chi2Pt_Cut ] ) {
            mon_Acceptance.push_back( ACCEPT_Mu1_Chi2Pt_Cut );
            flag_stages[ACCEPT_Mu1_Chi2Pt_Cut] = true;
        }
        mon_Acceptance.push_back( ACCEPT_Each_Mu1_Chi2Pt_Cut );
        
        //match to RoI?
        double deta=-99.;
        double dphi=-99.;
        if (m_matchL1) {
            deta = eta - roiDescriptor->eta(); //at which surface? perigee?
            dphi = phi - roiDescriptor->phi();
            while (dphi > M_PI ) dphi -= 2*M_PI;
            while (dphi < -M_PI) dphi += 2*M_PI;
        } else {
            // check match to muon track
            for ( unsigned int i_mu=0; i_mu<MuEFTracks.size(); i_mu++ ) {
                std::vector<const Trk::Track*> idTrks;
                HLT::ErrorCode status = GetTrigMuonEFInfoTracks(MuEFTracks[i_mu], idTrks, msg());
                if ( status != HLT::OK ) return status;
                for ( unsigned int i_trk=0; i_trk<idTrks.size(); i_trk++ ) {
                    const Trk::Perigee* perigee = idTrks[i_trk]->perigeeParameters();
                    double dphi_temp = phi - perigee->parameters()[Trk::phi];
                    while (dphi_temp > M_PI ) dphi_temp -= 2*M_PI;
                    while (dphi_temp < -M_PI) dphi_temp += 2*M_PI;
                    double deta_temp = eta - perigee->eta();
                    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Muon: deta, dphi = "  <<  deta_temp << "  " << dphi_temp << endreq;
                    if ( fabs(deta_temp) < fabs(deta) || fabs(dphi_temp) < fabs(dphi)) {
                        // work out which is smallest dR
                        double dR = sqrt(deta*deta + dphi*dphi);
                        double dR_temp = sqrt(deta_temp * deta_temp + dphi_temp*dphi_temp);
                        if (dR_temp < dR) {
                            dphi = dphi_temp;
                            deta = deta_temp;
                        }
                    }
                }
            }
        }
        dphi = fabs(dphi);
        deta = fabs(deta);
        
        // Monitoring of the RoI matching
        mon_TrkROIdEta.push_back( fabs(deta) );
        mon_TrkROIdPhi.push_back( fabs(dphi) );
        mon_TrkROIdR  .push_back( sqrt(dphi*dphi+deta*deta) );
        
        // Check the deta and dphi cuts (monitoring only)
        if ( fabs(deta) < m_dEta_cut ) {
            if ( !flag_stages[ ACCEPT_Mu1_dEta_Cut ] ) {
                mon_Acceptance.push_back( ACCEPT_Mu1_dEta_Cut );
                flag_stages[ ACCEPT_Mu1_dEta_Cut ] = true;
            }
            mon_Acceptance.push_back( ACCEPT_Each_Mu1_dEta_Cut );
        }
        if ( fabs(dphi) < m_dPhi_cut ) {
            if ( !flag_stages[ ACCEPT_Mu1_dPhi_Cut ] ) {
                mon_Acceptance.push_back( ACCEPT_Mu1_dPhi_Cut );
                flag_stages[ ACCEPT_Mu1_dPhi_Cut ] = true;
            }
            mon_Acceptance.push_back( ACCEPT_Each_Mu1_dPhi_Cut );
        }
        
        if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "check RoI match deta, dphi: " << deta << " " << dphi << endreq;
        
        if( deta < m_dEta_cut && dphi < m_dPhi_cut)
        { PassedRoIMatch = true;
            
            // Check deta+dphi cuts (monitoring only)
            if ( !flag_stages[ ACCEPT_Mu1_dR_Cut ] ) {
                mon_Acceptance.push_back( ACCEPT_Mu1_dR_Cut );
                flag_stages[ ACCEPT_Mu1_dR_Cut ] = true;
            }
            mon_Acceptance.push_back( ACCEPT_Each_Mu1_dR_Cut );
            
            
            if ( msgLvl() <= MSG::DEBUG ) msg()<<MSG::DEBUG<<"Track matched RoI found, looking another track"<<endreq;
            Rec::TrackParticleContainer::const_iterator trkIt2 =  TrkParticleCont->begin();
            for (int jtrk=0; trkIt2 != lastTrkIt; ++jtrk, trkIt2++)
            {
                if(itrk==jtrk) continue;
                //JW EDM const Trk::MeasuredPerigee* trckPerigee2 =  (*trkIt2)->measuredPerigee();
                const Trk::Perigee* trckPerigee2 =  (*trkIt2)->measuredPerigee();
                
                double track2phi = trckPerigee2->parameters()[Trk::phi];
                double track2eta = trckPerigee2->eta();
                
                if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Track: " << jtrk
                    <<" pT = "<<sin(trckPerigee2->parameters()[Trk::theta])/trckPerigee2->parameters()[Trk::qOverP]
                    << " phi = " << trckPerigee2->parameters()[Trk::phi]
                    <<" eta= " << trckPerigee2->eta() << endreq;
                
                float track2Pt = fabs( sin(trckPerigee2->parameters()[Trk::theta])/trckPerigee2->parameters()[Trk::qOverP]);
                float track2Chi2 = (*trkIt2)->fitQuality()->chiSquared();
                // Check pT and chi2 (monitoring only)
                if ( track2Chi2 <= 1e7 ) {
                    if ( !flag_stages[ ACCEPT_Mu2_Chi2_Cut ] ) {
                        mon_Acceptance.push_back( ACCEPT_Mu2_Chi2_Cut );
                        flag_stages[ ACCEPT_Mu2_Chi2_Cut ] = true;
                    }
                    mon_Acceptance.push_back( ACCEPT_Each_Mu2_Chi2_Cut );
                }
                if ( track2Pt >= m_trackPtthr ) {
                    if ( !flag_stages[ ACCEPT_Mu2_pT_Cut ] ) {
                        mon_Acceptance.push_back( ACCEPT_Mu2_pT_Cut );
                        flag_stages[ ACCEPT_Mu2_pT_Cut ] = true;
                    }
                    mon_Acceptance.push_back( ACCEPT_Each_Mu2_pT_Cut );
                }
                
                // m_trackPt.push_back(track2Pt/CLHEP::GeV);      // ????????
                // JW EDM std::vector<const Trk::MeasuredPerigee*> inputtrks;
                std::vector<const Trk::Perigee*> inputtrks;
                std::vector<double> massHypo ;
                inputtrks.push_back(trckPerigee);
                inputtrks.push_back(trckPerigee2);
                massHypo.push_back( m_daughterMass );
                massHypo.push_back( m_daughterMass );
                
                //double Mass=InvMass(inputtrks, massHypo);
                //m_Mass.push_back(Mass);
                // Calculate the invariant mass (hardcoded pT cut 2 GeV for monitoring purposes)
                double Mass = 0;
                if ( track2Chi2 <= 1e7 ) {
                    if ( track2Pt > 2. ) {
                        Mass = InvMass(inputtrks, massHypo);
                        mon_InvMassNoTrkPtCut.push_back( Mass / CLHEP::GeV );
                        mon_InvMassNoTrkPtCut_wideRange.push_back( Mass / CLHEP::GeV );
                    } else if ( track2Pt >= m_trackPtthr ) {
                        Mass = InvMass(inputtrks, massHypo);
                    }
                }
                // check Pt and chi2
                if( track2Pt < m_trackPtthr || track2Chi2 > 1e7) continue;
                // Check pT+chi2 (monitoring only)
                if ( !flag_stages[ ACCEPT_Mu2_Chi2Pt_Cut ] ) {
                    mon_Acceptance.push_back( ACCEPT_Mu2_Chi2Pt_Cut );
                    flag_stages[ ACCEPT_Mu2_Chi2Pt_Cut ] = true;
                }
                mon_Acceptance.push_back( ACCEPT_Each_Mu2_Chi2Pt_Cut );
                
                if (m_oppositeCharge) {
                    if ((trckPerigee->parameters()[Trk::qOverP]>0. && trckPerigee2->parameters()[Trk::qOverP]>0. ) ||
                        (trckPerigee->parameters()[Trk::qOverP]<0. && trckPerigee2->parameters()[Trk::qOverP]<0.))  continue;
                    
                    if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE
                        << "Tracks " << itrk << " and " << jtrk << " are opposite sign, make mass cuts " << endreq;
                } else {
                    if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE
                        << "opposite sign cuts not applied, make mass cuts " << endreq;
                }
                if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " mass =  " << Mass << endreq;
                
                // Check opposite charge (monitoring only)
                if ( !flag_stages[ ACCEPT_Opp_Charge ] ) {
                    mon_Acceptance.push_back( ACCEPT_Opp_Charge );
                    flag_stages[ ACCEPT_Opp_Charge ] = true;
                }
                mon_Acceptance.push_back( ACCEPT_Each_Opp_Charge );
                
                // Check the invariant mass cut
                if (Mass < m_mass_low_cut || Mass > m_mass_high_cut) continue;
                // Check the invariant mass cut (monitoring only)
                if ( !flag_stages[ ACCEPT_InvMass_Cut ] ) {
                    mon_Acceptance.push_back( ACCEPT_InvMass_Cut );
                    flag_stages[ ACCEPT_InvMass_Cut ] = true;
                }
                mon_Acceptance.push_back( ACCEPT_Each_InvMass_Cut );
                mon_InvMass.push_back( Mass / CLHEP::GeV );
                mon_InvMass_wideRange.push_back( Mass / CLHEP::GeV );
                
                PassedMass=true;
                
                // Monitoring of the di-muon track pairs
                mon_Trk1Pt .push_back( pT / CLHEP::GeV );
                mon_Trk2Pt .push_back( track2Pt / CLHEP::GeV );
                mon_Trk1Eta.push_back( eta );
                mon_Trk2Eta.push_back( track2eta );
                mon_Trk1Phi.push_back( phi );
                mon_Trk2Phi.push_back( track2phi );
                mon_SumPtTrk12.push_back ( (pT + track2Pt) / CLHEP::GeV );
                // Monitoring of the opening between the two tracks
                double dTrkEta = eta  - track2eta;
                double dTrkPhi = phi - track2phi;
                double absdTrkPhi = fabs( dTrkPhi );
                if ( 2.* M_PI - absdTrkPhi < absdTrkPhi ) {
                    if ( dTrkPhi > 0 ) {
                        dTrkPhi = dTrkPhi - 2.* M_PI;
                    } else {
                        dTrkPhi = 2* M_PI - absdTrkPhi;
                    }
                }
                mon_Trk1Trk2dEta.push_back( fabs(dTrkEta) );
                mon_Trk1Trk2dPhi.push_back( fabs(dTrkPhi) );
                mon_Trk1Trk2dR  .push_back( sqrt(dTrkPhi*dTrkPhi+dTrkEta*dTrkEta) );
                
                // found pair with good mass , now make EFBphys object
                // ROI_Id - ???
                TrigEFBphys* trigEFBphys = new TrigEFBphys(roiDescriptor->roiId(), roiDescriptor->eta(), roiDescriptor->phi(), TrigEFBphys::JPSIMUMU, Mass);
                if ( msgLvl() <= MSG::DEBUG) msg()  << MSG::DEBUG << "Create Bphys particle with roIId " << trigEFBphys->roiId()
                    <<" mass " << Mass << " phi, eta " << trigEFBphys->phi() << " "
                    << trigEFBphys->eta() << " vertex type " << trigEFBphys->particleType() << endreq;
                ElementLink<Rec::TrackParticleContainer> track1EL(*TrkParticleCont,itrk);
                ElementLink<Rec::TrackParticleContainer> track2EL(*TrkParticleCont,jtrk);
                trigEFBphys->addTrack(track1EL);
                trigEFBphys->addTrack(track2EL);
                //m_Mass.push_back(Mass);
                // set result here if mass cut is passed.
                result=true;
                
                // now do vertexing
                if (m_doVertexFit) {
                    
                    Trk::VxCandidate * myVxCandidate ( 0 );
                    std::vector<const Trk::TrackParticleBase*> pair;
                    // CLHEP::Hep3Vector vtx_CLHEP ( 0.,0.,0. );
                    Amg::Vector3D vtx(0.,0.,0.);
                    Trk::Vertex vertex ( vtx );
                    
                    // Vertexing time processing
                    if ( timerSvc() ) m_VtxFitTimer->resume();
                    
                    pair.push_back(*trkIt);
                    pair.push_back(*trkIt2);
                    myVxCandidate = m_iVKVVertexFitter->fit(pair,vertex);
                    if(myVxCandidate) {
                        if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"Vertex fit ok"<<endreq;
                        
                        // Passed vertexing (monitoring only)
                        if ( !flag_stages[ ACCEPT_Vertexing ] ) {
                            mon_Acceptance.push_back( ACCEPT_Vertexing );
                            flag_stages[ ACCEPT_Vertexing ] = true;
                        }
                        mon_Acceptance.push_back( ACCEPT_Each_Vertexing );
                        std::vector<int> trkIndices;
                        double invariantMass=0, invariantMassError=0;
                        for (int i=0;i<(int)pair.size();++i) {trkIndices.push_back(1);}
                        if (!(m_VKVFitter->VKalGetMassError(trkIndices,invariantMass,invariantMassError).isSuccess())) {
                            mon_Errors.push_back( ERROR_CalcInvMass_Fails );
                            if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"Warning from VKaVrt - cannot calculate uncertainties!"<<endreq;
                        } else {
                            // Calc. Inv. Mass succeeded (monitoring only)
                            if ( !flag_stages[ ACCEPT_CalcInvMass ] ) {
                                mon_Acceptance.push_back( ACCEPT_CalcInvMass );
                                flag_stages[ ACCEPT_CalcInvMass ] = true;
                            }
                            mon_Acceptance.push_back( ACCEPT_Each_CalcInvMass );
                        }
                        trigEFBphys->fitmass((float)invariantMass);
                        trigEFBphys->fitchi2(((myVxCandidate->recVertex().fitQuality().chiSquared())));
                        trigEFBphys->fitndof(2*(pair.size())-3);
                        trigEFBphys->fitx(myVxCandidate->recVertex().position() [0]);
                        trigEFBphys->fity(myVxCandidate->recVertex().position() [1]);
                        trigEFBphys->fitz(myVxCandidate->recVertex().position() [2]);
                        // Monitoring of the fit results
                        mon_FitMass       .push_back( invariantMass / CLHEP::GeV );
                        mon_InvMass_okFit .push_back( Mass / CLHEP::GeV );
                        double chisq = myVxCandidate->recVertex().fitQuality().chiSquared();
                        unsigned int ndf = (unsigned int) myVxCandidate->recVertex().fitQuality().numberDoF();
                        if ( ndf==0 )  ndf = 1; 
                        
                        mon_Chi2toNDoF.push_back( chisq / ndf );
                        
                        double chi2prob = 1.0 - Genfun::CumulativeChiSquare( ndf )(chisq);
                        mon_Chi2toNDoFProb.push_back( chi2prob );
                        //if ( p_vertex->getMotherTrack() != NULL ) {
                        //  mon_FitTotalPt.push_back( p_vertex->getMotherTrack()->pT() / CLHEP::GeV );
                        //  mon_SumPtTrk12_okFit.push_back( (fabs((*track)->param()->pT()) + fabs((*track2)->param()->pT())) / CLHEP::GeV );
                        // }
                        mon_FitVtxR.push_back( myVxCandidate->recVertex().position().x()*myVxCandidate->recVertex().position().x() + myVxCandidate->recVertex().position().y()*myVxCandidate->recVertex().position().y()) ;
                        mon_FitVtxZ.push_back( myVxCandidate->recVertex().position().z() );
                        
                        delete myVxCandidate;
                        myVxCandidate = 0;
                        
                    } else {             // fit successful?
                        if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "TrigL2VertexFitter failed" << endreq;
                    }
                    // Pause the vertexing time monitoring
                    if ( timerSvc() ) m_VtxFitTimer->pause();
                } // if doVertexFit
                if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Store EFBphys and move onto next track" << endreq;
                m_trigBphysColl->push_back(trigEFBphys);
            } //2d track
        } else {               //passed RoI match
            if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "RoI match failed deta, dphi: " << deta << " " << dphi << endreq;
            
        }
    } // end loop over tracks
    
    //m_NBphys = m_trigBphysColl->size();
    // Check the size of the bphys-objects collection
    if ( m_trigBphysColl->size() > 0 ) mon_Acceptance.push_back( ACCEPT_BphysColl_not_Empty );
    mon_nBphys = m_trigBphysColl->size();
    
    if (PassedRoIMatch) m_countPassedRoIMatch++;
    if (PassedMass) m_countPassedMass++;
    
    if (result) {
        m_countPassedRoIs++;
        if (IdEvent!= (int) m_lastEventPassed) {
            m_countPassedEvents++;
            m_lastEventPassed=IdEvent;
        }
    }              
    if ( msgLvl() <= MSG::DEBUG) {
        if (m_trigBphysColl != 0) msg() << MSG::DEBUG << "Bphys collection size : " << m_trigBphysColl->size() << " m_trigBphysColl " << m_trigBphysColl << endreq;
    }
    // Reach this point successfully
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Run: " << IdRun << " Event: " << IdEvent << "  result is " << result << endreq;
    // Stop the processing timers
    if ( timerSvc() ) {
        m_TotTimer   ->stop();
        m_VtxFitTimer->resume();
        m_VtxFitTimer->stop();
        // Fill the monitoring variables
        mon_TotalRunTime     = m_TotTimer   ->elapsed();
        mon_VertexingTime = m_VtxFitTimer->elapsed();
    }
    
    //  if (!m_acceptAll) pass = result; 
    if ( msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "    m_trigBphysColl " << m_trigBphysColl << endreq;
    
    if ((m_trigBphysColl!=0) && (m_trigBphysColl->size() > 0)) {
        if ( msgLvl() <= MSG::DEBUG) msg()  << MSG::DEBUG << "REGTEST: Store Bphys Collection size: " << m_trigBphysColl->size() << endreq;
        
        HLT::ErrorCode sc = attachFeature(outputTE, m_trigBphysColl, "EFTrackMass" );
        if(sc != HLT::OK) {
            msg()  << MSG::WARNING << "Failed to store trigBphys Collection" << endreq;
            mon_Errors.push_back( ERROR_BphysColl_Fails );
            return sc;
        }
    } else {
        if ( msgLvl() <= MSG::DEBUG) msg()  << MSG::DEBUG << "REGTEST: no bphys collection to store "  << endreq;
        delete m_trigBphysColl;
    }
    m_trigBphysColl=0;
    
    
    
    
    return HLT::OK;
}


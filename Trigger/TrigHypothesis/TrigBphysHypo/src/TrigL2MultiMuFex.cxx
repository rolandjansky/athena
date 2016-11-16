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

//#include "TrigMuonEvent/CombinedMuonFeature.h"

#include "TrigL2MultiMuFex.h"

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

#include "TrigBphysHelperUtilsTool.h"

//class ISvcLocator;

/*--------------------------------------------------------------------------------*/
TrigL2MultiMuFex::TrigL2MultiMuFex(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::ComboAlgo(name, pSvcLocator),
  m_bphysHelperTool("TrigBphysHelperUtilsTool"),
  m_checkNinputTE(true),
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

  m_passInvMass(false),
  m_massMuon(105.6583715)
/*--------------------------------------------------------------------------------*/
{

  // Read properties - boolean switches
  declareProperty("AcceptAll"        , m_acceptAll         = true );
  declareProperty("NInputMuon"     , m_NInputMuon    = 3 );
  declareProperty("OppositeSign"     , m_oppositeCharge    = true );
  declareProperty("NMassMuon"     , m_NMassMuon    = 2 );
  declareProperty("doVertexFit"      , m_doVertexFit       = true );
  declareProperty("CheckNinputTE", m_checkNinputTE=true);

  // Read properties - mass widnow cuts
  declareProperty("LowerMassCut", m_lowerMassCut =  2000.0 );
  declareProperty("UpperMassCut", m_upperMassCut = 10000.0 );

  // Read properties - vertexing tools
  declareProperty("TrigL2VertexFitter", m_L2vertFitter );
  declareProperty("TrigVertexingTool" , m_vertexingTool, "TrigVertexingTool" );

    declareProperty("MassTrack" , m_massMuon, "Mass of the muon" );

    
  // Variables for monitoring histograms
  declareMonitoredStdContainer("Errors"                 , m_mon_Errors                  , AutoClear);
  declareMonitoredStdContainer("Acceptance"             , m_mon_Acceptance              , AutoClear);
  //  declareMonitoredStdContainer("ROIEta"                 , m_mon_ROIEta                  , AutoClear);
  // declareMonitoredStdContainer("ROIPhi"                 , m_mon_ROIPhi                  , AutoClear);
  // - two combined muons
  declareMonitoredStdContainer("MucombPt"               , m_mon_MucombPt                , AutoClear);
  declareMonitoredStdContainer("MutrkPt"                , m_mon_MutrkPt                 , AutoClear);
  //  declareMonitoredStdContainer("MutrkPt_wideRange"      , m_mon_MutrkPt_wideRange       , AutoClear);
  declareMonitoredStdContainer("MutrkEta"               , m_mon_MutrkEta                , AutoClear);
  declareMonitoredStdContainer("MutrkPhi"               , m_mon_MutrkPhi                , AutoClear);
  //  declareMonitoredStdContainer("Mutrk1Mutrk2dEta"       , m_mon_Mutrk1Mutrk2dEta        , AutoClear);
  //declareMonitoredStdContainer("Mutrk1Mutrk2dPhi"       , m_mon_Mutrk1Mutrk2dPhi        , AutoClear);
  //declareMonitoredStdContainer("Mutrk1Mutrk2dR"         , m_mon_Mutrk1Mutrk2dR          , AutoClear);
  //declareMonitoredStdContainer("SumPtMutrk12"           , m_mon_SumPtMutrk12            , AutoClear);
  declareMonitoredStdContainer("InvMass_comb"           , m_mon_InvMass_comb            , AutoClear);
  declareMonitoredStdContainer("InvMass_comb_wideRange" , m_mon_InvMass_comb            , AutoClear);
  declareMonitoredStdContainer("FitMass"                , m_mon_FitMass                 , AutoClear);
  declareMonitoredStdContainer("FitMass_wideRange"      , m_mon_FitMass                 , AutoClear);
  declareMonitoredStdContainer("InvMass_comb_okFit"     , m_mon_InvMass_comb_okFit      , AutoClear);
  declareMonitoredStdContainer("Chi2toNDoF"             , m_mon_Chi2toNDoF              , AutoClear);
  declareMonitoredStdContainer("FitTotalPt"             , m_mon_FitTotalPt              , AutoClear);
  declareMonitoredStdContainer("SumPtMutrk12_okFit"     , m_mon_SumPtMutrk12_okFit      , AutoClear);
  declareMonitoredStdContainer("FitVtxR"                , m_mon_FitVtxR                 , AutoClear);
  declareMonitoredStdContainer("FitVtxZ"                , m_mon_FitVtxZ                 , AutoClear);

  // Initialize the collections
  m_trigBphysColl = NULL;
  //m_VertexColl    = NULL;
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
    msg() << MSG::DEBUG << "Initialization ..." << endmsg;
    msg() << MSG::DEBUG << "AcceptAll          = "
          << (m_acceptAll==true ? "True" : "False") << endmsg;
    msg() << MSG::DEBUG << "Number of input muons expected   = " << m_NInputMuon << endmsg;
    msg() << MSG::DEBUG << "Number of muons for mass   = " << m_NMassMuon << endmsg;
    msg() << MSG::DEBUG << "OppositeCharge     = "
          << (m_oppositeCharge==true ? "True" : "False") << endmsg;
    msg() << MSG::DEBUG << "LowerMassCut       = " << m_lowerMassCut << endmsg;
    msg() << MSG::DEBUG << "UpperMassCut       = " << m_upperMassCut << endmsg;
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
HLT::ErrorCode TrigL2MultiMuFex::hltFinalize()
/*-----------------------------------------*/
{

  msg() << MSG::INFO << "in finalize()" << endmsg;
  MsgStream log(msgSvc(), name());

  msg() << MSG::INFO << "|----------------------- SUMMARY FROM TrigL2MultiMuFex -------------|" << endmsg;
  msg() << MSG::INFO << "Run on events/2xRoIs " << m_countTotalEvents << "/" << m_countTotalRoI <<  endmsg;
  msg() << MSG::INFO << "Passed events/2xRoIs " << m_countPassedEvents << "/" << m_countPassedRoIs <<  endmsg;
  msg() << MSG::INFO << "RoIs Passed MuMu pairs: "  << m_countPassedmumuPairs << endmsg;
  msg() << MSG::INFO << "RoIs Passed BsMass: "  << m_countPassedBsMass << endmsg;
  msg() << MSG::INFO << "RoIs Passed Vtx Fit: "  << m_countPassedVtxFit << endmsg;

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

  
  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Running TrigL2MultiMuFex::acceptInputS" << endmsg;
  m_mon_Acceptance.push_back( ACCEPT_Input );

  if ( m_acceptAll ) {
    pass = true;
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Accept property is set: taking all the events" << endmsg;
    return HLT::OK;
  }

  // Processing timers
  if ( timerSvc() ) m_BmmHypTot->start();

    // Retrieve event info
    //    int IdRun   = 0;
    //    int IdEvent = 0;
    //    // event info
    //    uint32_t runNumber(0), evtNumber(0), lbBlock(0);
    //    if (m_bphysHelperTool->getRunEvtLb( runNumber, evtNumber, lbBlock).isFailure()) {
    //        msg() << MSG::ERROR << "Error retriving EventInfo" << endmsg;
    //    }
    //    IdRun = runNumber;
    //    IdEvent = evtNumber;


    
  // Check consistency of the number of input Trigger Elements
  if (m_checkNinputTE && inputTE.size() != m_NInputMuon ) {
    msg() << MSG::ERROR << "Got wrong number of input TEs, expect " << m_NInputMuon << " got " << inputTE.size() << endmsg;
    if ( timerSvc() ) {
      m_BmmHypTot->stop();
    }
    m_mon_Errors.push_back( ERROR_WrongNum_Input_TE );
    return HLT::BAD_JOB_SETUP;
  }

  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Number of input TEs " << inputTE.size() << endmsg;
 
  pass = true;
  return HLT::OK;
}



/*----------------------------------------------------------------------------------------*/
HLT::ErrorCode TrigL2MultiMuFex::hltExecute(HLT::TEConstVec& inputTE, HLT::TriggerElement* outputTE)
/*----------------------------------------------------------------------------------------*/
{
   if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " In hltExecute " << endmsg;

    //  m_trigBphysColl = new TrigL2BphysContainer();
    m_trigBphysColl = new xAOD::TrigBphysContainer;
    xAOD::TrigBphysAuxContainer xAODTrigBphysAuxColl;
    m_trigBphysColl->setStore(&xAODTrigBphysAuxColl);

  if (m_NInputMuon==3) {
   if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Call processTriMuon " << endmsg;

    processTriMuon(inputTE);
  }

      if ((m_trigBphysColl!=0) && (m_trigBphysColl->size() > 0)) {
	//  m_mon_Acceptance.push_back( ACCEPT_BphysColl_not_Empty );
    if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "REGTEST: Store Bphys Collection size: " << m_trigBphysColl->size() << endmsg;

    HLT::ErrorCode sc = attachFeature(outputTE, m_trigBphysColl, "L2MultiMuFex" );
    if(sc != HLT::OK) {
      msg()  << MSG::WARNING << "Failed to store trigBphys Collection" << endmsg;
      m_mon_Errors.push_back( ERROR_BphysColl_Fails );
        m_trigBphysColl = nullptr; // delete it?
        return HLT::ERROR;
    }
  } else {
    if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "REGTEST: no bphys collection to store "  << endmsg;
    delete m_trigBphysColl;
      m_trigBphysColl = nullptr;
  }

      
  return HLT::OK;
}


void TrigL2MultiMuFex::processTriMuon(HLT::TEConstVec& inputTE)
{
   if ( msgLvl() <= MSG::VERBOSE ) msg() << MSG::VERBOSE << " In processTriMuon " << endmsg;

   const xAOD::L2CombinedMuon *muon1(nullptr), *muon2(nullptr), *muon3(nullptr);
   ElementLinkVector<xAOD::L2CombinedMuonContainer>  l2combinedMuonEL[3];
    
   if( m_checkNinputTE ){

      const HLT::TriggerElement* te1 = inputTE[0];
      const HLT::TriggerElement* te2 = inputTE[1];
      const HLT::TriggerElement* te3 = inputTE[2];

    //      const CombinedMuonFeature *muon1;
    //      const CombinedMuonFeature *muon2;
    //      const CombinedMuonFeature *muon3;

    
    //if(getFeature(te1,muon1)!= HLT::OK) {
    if(getFeaturesLinks<xAOD::L2CombinedMuonContainer,xAOD::L2CombinedMuonContainer>(te1,l2combinedMuonEL[0]) != HLT::OK
       || !l2combinedMuonEL[0].size()) {
        msg() <<MSG::DEBUG << "Failed to get muon Feature for TE 1" << endmsg;
        m_mon_Errors.push_back( ERROR_GetMuonFailed );
        return;
    }
    muon1 = l2combinedMuonEL[0][0].isValid() ? *(l2combinedMuonEL[0][0]) : nullptr;
    if ( muon1 == NULL ) {
        msg() <<MSG::DEBUG << "NULL pointer of muon Feature for TE 1" << endmsg;
        m_mon_Errors.push_back( ERROR_GetMuonFailed );
        return;
    }
    //if(getFeature(te2,muon2)!= HLT::OK) {
    if(getFeaturesLinks<xAOD::L2CombinedMuonContainer,xAOD::L2CombinedMuonContainer>(te2,l2combinedMuonEL[1]) != HLT::OK
       || !l2combinedMuonEL[1].size()) {
        msg() <<MSG::DEBUG << "Failed to get muon Feature for TE 2" << endmsg;
        m_mon_Errors.push_back( ERROR_GetMuonFailed );
        return;
    }
    muon2 = l2combinedMuonEL[1][0].isValid() ? *(l2combinedMuonEL[1][0]) : nullptr;
    if ( muon2 == NULL ) {
        msg() <<MSG::DEBUG << "NULL pointer of muon Feature for TE 2" << endmsg;
        m_mon_Errors.push_back( ERROR_GetMuonFailed );
        return;
    }
    if(getFeaturesLinks<xAOD::L2CombinedMuonContainer,xAOD::L2CombinedMuonContainer>(te3,l2combinedMuonEL[2]) != HLT::OK
       || !l2combinedMuonEL[2].size()) {
        //if(getFeature(te3,muon3)!= HLT::OK) {
        msg() <<MSG::DEBUG << "Failed to get muon Feature for TE 3" << endmsg;
        m_mon_Errors.push_back( ERROR_GetMuonFailed );
        return;
    }
    muon3 = l2combinedMuonEL[2][0].isValid() ? *(l2combinedMuonEL[2][0]) : nullptr;
    if ( muon3 == NULL ) {
        msg() <<MSG::DEBUG << "NULL pointer of muon Feature for TE 3" << endmsg;
        m_mon_Errors.push_back( ERROR_GetMuonFailed );
        return;
    }

   }else{
     std::vector<ElementLinkVector<xAOD::L2CombinedMuonContainer> >vec_elv_muons ;
     unsigned int nTEs = 3;
     if( ! m_checkNinputTE ) nTEs = inputTE.size();


     for ( unsigned int i=0; i < nTEs; ++i) {
        ElementLinkVector<xAOD::L2CombinedMuonContainer> elvmuon;
        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Try to retrieve L2CombinedMuon  " << i << endmsg;
        if(getFeaturesLinks<xAOD::L2CombinedMuonContainer,xAOD::L2CombinedMuonContainer>(inputTE[i], elvmuon)!=HLT::OK ) {
            if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Failed to get L2CombinedMuon " << i << ", exiting" << endmsg;
            m_mon_Errors.push_back( ERROR_GetMuonFailed );
            return;
        }
        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Found L2CombinedMuon " << i << " Feature, size = " << elvmuon.size() << endmsg;
        vec_elv_muons.push_back(elvmuon);
    } // loop over each roi

     if( vec_elv_muons.size() > 3 ){
        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "More than 3 muons found, use first 3 out of  " << vec_elv_muons.size() << endmsg;
     }else if( vec_elv_muons.size() < 3 ){
       if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Less than 3 muons found   " << vec_elv_muons.size() << " , exiting"<< endmsg;
       m_mon_Errors.push_back( ERROR_GetMuonFailed );
       return;
     }
     muon1 = vec_elv_muons[0][0].isValid() ? *(vec_elv_muons[0][0]) : nullptr;
     if ( muon1 == NULL ) {
       msg() <<MSG::DEBUG << "NULL pointer of muon Feature for TE 1" << endmsg;
       m_mon_Errors.push_back( ERROR_GetMuonFailed );
       return;
     }
     muon2 = vec_elv_muons[1][0].isValid() ? *(vec_elv_muons[1][0]) : nullptr;
     if ( muon2 == NULL ) {
       msg() <<MSG::DEBUG << "NULL pointer of muon Feature for TE 2" << endmsg;
       m_mon_Errors.push_back( ERROR_GetMuonFailed );
       return;
     }
     muon3 = vec_elv_muons[2][0].isValid() ? *(vec_elv_muons[2][0]) : nullptr;
     if ( muon3 == NULL ) {
       msg() <<MSG::DEBUG << "NULL pointer of muon Feature for TE 3" << endmsg;
       m_mon_Errors.push_back( ERROR_GetMuonFailed );
       return;
     }

   }

    m_mon_Acceptance.push_back( ACCEPT_GotMuons );


    const ElementLink< xAOD::TrackParticleContainer >& ELidtrack1  = muon1->idTrackLink();
    const ElementLink< xAOD::TrackParticleContainer >& ELidtrack2  = muon2->idTrackLink();
    const ElementLink< xAOD::TrackParticleContainer >& ELidtrack3  = muon3->idTrackLink();
    if (!ELidtrack1.isValid()) {
        msg() <<MSG::DEBUG << "Failed to get muon Feature for TE 3" << endmsg;
        m_mon_Errors.push_back( ERROR_GetMuonFailed );
        return;
    }
    if (!ELidtrack2.isValid()) {
        msg() <<MSG::DEBUG << "Failed to get muon Feature for TE 3" << endmsg;
        m_mon_Errors.push_back( ERROR_GetMuonFailed );
        return;
    }
    if (!ELidtrack3.isValid()) {
        msg() <<MSG::DEBUG << "Failed to get muon Feature for TE 3" << endmsg;
        m_mon_Errors.push_back( ERROR_GetMuonFailed );
        return;
    }

        
      //monitoring
      m_mon_MucombPt.push_back( muon1->pt() * muon1->charge() / CLHEP::GeV );
      m_mon_MucombPt.push_back( muon2->pt() * muon2->charge() / CLHEP::GeV );
      m_mon_MucombPt.push_back( muon3->pt() * muon3->charge() / CLHEP::GeV );
      m_mon_MutrkPt.push_back( (*ELidtrack1)->pt() / CLHEP::GeV );
      m_mon_MutrkPt.push_back( (*ELidtrack2)->pt() / CLHEP::GeV );
      m_mon_MutrkPt.push_back( (*ELidtrack3)->pt() / CLHEP::GeV );
      m_mon_MutrkEta.push_back( (*ELidtrack1)->eta() );
      m_mon_MutrkEta.push_back( (*ELidtrack2)->eta() );
      m_mon_MutrkEta.push_back( (*ELidtrack3)->eta() );
      m_mon_MutrkPhi.push_back( (*ELidtrack1)->phi0() );
      m_mon_MutrkPhi.push_back( (*ELidtrack2)->phi0() );
      m_mon_MutrkPhi.push_back( (*ELidtrack3)->phi0() );

    // simple lambda to help print out information, without tedious typing
    /*
    auto dumpinfo = [this] (const xAOD::L2CombinedMuon* muon) {
        this->msg() << "pt=" << muon->pt()*muon->charge() << " trkAddr=" << muon->idTrack() << " Trk:Track=" << muon->idTrack()->track()
    };
    if (msgLvl() <= MSG::DEBUG ) {
        msg() << MSG::DEBUG << "1st CombinedMuonFeature " << dumpinfo(muon1) << endmsg;
        msg() << MSG::DEBUG << "2st CombinedMuonFeature " << dumpinfo(muon2) << endmsg;
        msg() << MSG::DEBUG << "3st CombinedMuonFeature " << dumpinfo(muon3) << endmsg;
    } // if debug
    */
 

    if (m_NMassMuon ==2) {   // check mass of opposite sign muon pairs
        
        m_passInvMass=false;
        double Mass(0.);
        if (muon1->charge() * muon2->charge() < 0) {
            Mass  = m_bphysHelperTool->invariantMass( {*ELidtrack1,*ELidtrack2}, {m_massMuon,m_massMuon} );
            m_mon_InvMass_comb.push_back(Mass/1000.);
        }
        checkInvMass2(muon1, muon2, Mass);
        
        
        Mass = 0.;
        if (muon2->charge() * muon3->charge() < 0) {
            Mass  = m_bphysHelperTool->invariantMass( {*ELidtrack2,*ELidtrack3}, {m_massMuon,m_massMuon} );
            m_mon_InvMass_comb.push_back(Mass/1000.);
        }
        checkInvMass2(muon2, muon3, Mass);

        
        Mass = 0.;
        if (muon1->charge() * muon3->charge() < 0) {
            Mass  = m_bphysHelperTool->invariantMass( {*ELidtrack1,*ELidtrack3}, {m_massMuon,m_massMuon} );
            m_mon_InvMass_comb.push_back(Mass/1000.);
        }
        checkInvMass2(muon1, muon3, Mass);

        if (m_passInvMass) {
            m_mon_Acceptance.push_back( ACCEPT_InvMass_Cut );
        }

    } // 2 muon
    
    if (m_NMassMuon == 3) {   // check mass of all 3 muons
        m_passInvMass=false;
        double Mass(0.);
        Mass  = m_bphysHelperTool->invariantMass( {*ELidtrack1,*ELidtrack2,*ELidtrack3}, {m_massMuon,m_massMuon,m_massMuon} );
        checkInvMass3(muon1, muon2,muon3, Mass);
        
        if (m_passInvMass) {
            m_mon_Acceptance.push_back( ACCEPT_InvMass_Cut );
        }
    } // 3 muon

    return;
}


//double TrigL2MultiMuFex::getInvMass2(const CombinedMuonFeature *muon1,const CombinedMuonFeature *muon2)
//{
//  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " In getInvMass2 muon pT: " << muon1->IDTrack()->param()->pT() << "  " <<  muon2->IDTrack()->param()->pT() << endmsg;
//   double Mass;
//     std::vector<const TrigInDetTrackFitPar*> inputtrks;
//      std::vector<double> massHypo ;
//      massHypo.push_back( MUMASS ); 
//      massHypo.push_back( MUMASS );
//   // check pair of opposite sign muons
//   if (muon1->IDTrack()->param()->pT() * muon2->IDTrack()->param()->pT() < 0.) {
//     if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Muons 1 and 2 opp. charge will calculate mass " << endmsg;
//      inputtrks.push_back(muon1->IDTrack()->param());
//      inputtrks.push_back(muon2->IDTrack()->param());
//      Mass=InvMass(inputtrks, massHypo);
//      if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Muons 1 and 2 mass =  " << Mass << endmsg;
//      m_mon_InvMass_comb.push_back(Mass/1000.);    
//
//   }
//   return Mass;
//}
//
//double TrigL2MultiMuFex::getInvMass3(const CombinedMuonFeature *muon1,const CombinedMuonFeature *muon2, const CombinedMuonFeature *muon3)
//{
//   if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " In getInvMass3 " << endmsg;
//   double Mass;
//     std::vector<const TrigInDetTrackFitPar*> inputtrks;
//      std::vector<double> massHypo ;
//      massHypo.push_back( MUMASS );
//      massHypo.push_back( MUMASS );
//      massHypo.push_back( MUMASS );
//   // check pair of opposite sign muons
//      double muon1_pT=muon1->IDTrack()->param()->pT();
//      double muon2_pT=muon2->IDTrack()->param()->pT();
//      double muon3_pT=muon3->IDTrack()->param()->pT();
//
//      if ((muon1_pT * muon2_pT < 0.) || (muon1_pT * muon3_pT < 0.) || (muon3_pT * muon2_pT < 0.)) {
//     if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Muons opp. charge will calculate mass " << endmsg;
//      inputtrks.push_back(muon1->IDTrack()->param());
//      inputtrks.push_back(muon2->IDTrack()->param());
//      inputtrks.push_back(muon3->IDTrack()->param());
//      Mass=InvMass(inputtrks, massHypo);
//      if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Muons mass =  " << Mass << endmsg;
//      m_mon_InvMass_comb.push_back(Mass/1000.);    
//
//   }
//   return Mass;
//}

xAOD::TrigBphys* TrigL2MultiMuFex::checkInvMass2(const xAOD::L2CombinedMuon *muon1,const xAOD::L2CombinedMuon *muon2, double Mass) {
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " In checkInvMass2, cuts " << m_lowerMassCut << " " << m_upperMassCut <<endmsg;
    if ( Mass < m_lowerMassCut || Mass > m_upperMassCut  ) {
        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Mass : " << Mass << " cut failed  "  << endmsg;
        return nullptr;
    }
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Mass cut passed, creat TrigL2Bphys  "  << endmsg;
    m_passInvMass=true;

    xAOD::TrigBphys *trigPart = new xAOD::TrigBphys();
    m_trigBphysColl->push_back(trigPart);
    trigPart->initialise(0, 0., 0., xAOD::TrigBphys::MULTIMU, Mass, xAOD::TrigBphys::L2);
    
    trigPart->addTrackParticleLink(muon1->idTrackLink());
    trigPart->addTrackParticleLink(muon2->idTrackLink());

    
    TrigVertex* trigVx = fitToVertex({muon1,muon2});
    if (trigVx) {
        trigPart->setFitmass(trigVx->mass());
        trigPart->setFitchi2(trigVx->chi2());
        trigPart->setFitndof(trigVx->ndof());
        trigPart->setFitx(trigVx->x());
        trigPart->setFity(trigVx->y());
        trigPart->setFitz(trigVx->z());
        delete trigVx;
        trigVx = nullptr;
    } // if fitted
    return trigPart;
} //checkInvMass2 (xAOD)




xAOD::TrigBphys* TrigL2MultiMuFex::checkInvMass3(const xAOD::L2CombinedMuon *muon1,const xAOD::L2CombinedMuon *muon2, const xAOD::L2CombinedMuon *muon3,double Mass) {
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " In checkInvMass3, cuts " << m_lowerMassCut << " " << m_upperMassCut <<endmsg;

    if ( Mass < m_lowerMassCut || Mass > m_upperMassCut  ) {
        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Mass : " << Mass << " cut failed  "  << endmsg;
        return NULL;
    }
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Mass cut passed, creat TrigL2Bphys  "  << endmsg;
    m_passInvMass=true;
    
    xAOD::TrigBphys *trigPart = new xAOD::TrigBphys();
    m_trigBphysColl->push_back(trigPart);
    trigPart->initialise(0, 0., 0., xAOD::TrigBphys::MULTIMU, Mass, xAOD::TrigBphys::L2);

    trigPart->addTrackParticleLink(muon1->idTrackLink());
    trigPart->addTrackParticleLink(muon2->idTrackLink());
    trigPart->addTrackParticleLink(muon3->idTrackLink());

    TrigVertex* trigVx = fitToVertex({muon1,muon2,muon3});
    if (trigVx) {
        trigPart->setFitmass(trigVx->mass());
        trigPart->setFitchi2(trigVx->chi2());
        trigPart->setFitndof(trigVx->ndof());
        trigPart->setFitx(trigVx->x());
        trigPart->setFity(trigVx->y());
        trigPart->setFitz(trigVx->z());
        delete trigVx;
        trigVx = nullptr;
    } // if fitted
    return trigPart;

    
} //checkInvMass3 (xAOD)

//TrigL2Bphys* TrigL2MultiMuFex::checkInvMass3(const CombinedMuonFeature *muon1,const CombinedMuonFeature *muon2, const CombinedMuonFeature *muon3, double Mass)
//{
//  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " In checkInvMass3, cuts " << m_lowerMassCut << " " << m_upperMassCut <<endmsg;
//
//   if ( Mass < m_lowerMassCut || Mass > m_upperMassCut  ) {
//     if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Mass : " << Mass << " cut failed  "  << endmsg;
//      return NULL;
//   }
//   if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Mass cut passed, creat TrigL2Bphys  "  << endmsg;
//   m_passInvMass=true;
//
//   TrigL2Bphys* trigPart = new TrigL2Bphys( 0, 0., 0., TrigL2Bphys::MULTIMU, Mass); //should really be type MULTIMU - needs changes to TrigPartcile
//
//
//  //  Add element-links to tracks
//  ElementLink<TrigInDetTrackCollection> track1EL=muon1->IDTrackLink();
//  ElementLink<TrigInDetTrackCollection> track2EL=muon2->IDTrackLink();
//  ElementLink<TrigInDetTrackCollection> track3EL=muon3->IDTrackLink();
//    if ( msgLvl() <= MSG::DEBUG ) {
//      msg() << MSG::DEBUG << "Just check track links... " << endmsg;
//      msg() << MSG::DEBUG << "Track 1 pT " << (*track1EL)->param()->pT()
//            << " eta: " << (*track1EL)->param()->eta()
//            << " phi: " << (*track1EL)->param()->phi0() << endmsg;
//      msg() << MSG::DEBUG << "Track 2 pT " << (*track2EL)->param()->pT()
//            << " eta: " << (*track2EL)->param()->eta()
//            << " phi: " << (*track2EL)->param()->phi0() << endmsg;
//      msg() << MSG::DEBUG << "Track 3 pT " << (*track3EL)->param()->pT()
//            << " eta: " << (*track3EL)->param()->eta()
//            << " phi: " << (*track3EL)->param()->phi0() << endmsg;
//    }
//    trigPart->addTrack(track1EL);
//    trigPart->addTrack(track2EL);
//    trigPart->addTrack(track3EL);
//
//    //vertexing
//    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Now do vertexing" << endmsg;
//
//    TrigL2Vertex* pL2V    = new TrigL2Vertex();
//    TrigVertex*   p_mumuV = NULL;
//    StatusCode sc;
//    // Add tracks to the vertexer
//    bool addTracks = true;
//    if (muon1->IDTrack()->chi2() < 1e7 ) {
//       sc = m_vertexingTool->addTrack(muon1->IDTrack(),pL2V,Trk::muon);
//        if(sc.isFailure()){
//          if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Failed to add track 1 to vertexingTool" << endmsg;
//          m_mon_Errors.push_back( ERROR_AddTrack_Fails );
//          addTracks = false;
//        }
//    } else {
//     addTracks = false;
//    }    
//    
//    if (muon2->IDTrack()->chi2() < 1e7 ) {
//      sc = m_vertexingTool->addTrack(muon2->IDTrack(),pL2V,Trk::muon);
//      if(sc.isFailure()){
//        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Failed to add track 2 to vertexingTool" << endmsg;
//        addTracks = false;
//        m_mon_Errors.push_back( ERROR_AddTrack_Fails );
//      }
//    } else {
//        addTracks = false;
//    }
//
//    if (muon3->IDTrack()->chi2() < 1e7 ) {
//      sc = m_vertexingTool->addTrack(muon3->IDTrack(),pL2V,Trk::muon);
//      if(sc.isFailure()){
//        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Failed to add track 3 to vertexingTool" << endmsg;
//        addTracks = false;
//        m_mon_Errors.push_back( ERROR_AddTrack_Fails );
//      }
//    } else {
//        addTracks = false;
//    }
//
//    // DO THE VERTEX-FIT
//    if ( addTracks) {
//      sc = m_L2vertFitter->fit(pL2V);
//    }
//
//    // Check the result
//    if ( sc.isFailure() || (!addTracks) ) {
//      if ( msgLvl() <= MSG::DEBUG )msg() << MSG::DEBUG << "TrigL2VertexFitter failed" << endmsg;
//    } else {
//      // Calculate invariant mass
//      sc = m_vertexingTool->calculateInvariantMass(pL2V);
//      if ( sc.isSuccess() ) {
//        // Create mother particle
//	//        sc = m_vertexingTool->createMotherParticle(pL2V);
//        //if ( sc.isSuccess() ) {
//        //  if ( msgLvl() <= MSG::VERBOSE ) msg() << MSG::VERBOSE << "Bs created: pT "<<pL2V->m_getMotherTrack()->pT() << endmsg;
//        //} else {
//        //  m_mon_Errors.push_back( ERROR_CalcMother_Fails );
//	// }
//        p_mumuV = m_vertexingTool->createTrigVertex(pL2V);
//      } else {
//        m_mon_Errors.push_back( ERROR_CalcInvMass_Fails );
//      }
//    }
//    delete pL2V;
//
//    if ( p_mumuV == NULL ) {
//      if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " vertex fit failed for tracks  " << endmsg;
//      delete p_mumuV;
//    } else {
//      double chi2prob = 0.;
//      chi2prob = 1.0 - Genfun::CumulativeChiSquare(p_mumuV->ndof())(p_mumuV->chi2());
//      if ( msgLvl() <= MSG::DEBUG ) {
//        msg() << MSG::DEBUG << "REGTEST: mumu vertex Fit: x= y= z=" << p_mumuV->x() << " " << p_mumuV->y() << " " << p_mumuV->z() << endmsg;
//        msg() << MSG::DEBUG << "REGTEST: mumu mass = " << p_mumuV->mass() << endmsg;
//        if (p_mumuV->massVariance() !=0) {
//          msg() << MSG::DEBUG << "REGTEST: mumu mass pull = " << (p_mumuV->mass()-BSMASS)/p_mumuV->massVariance() << endmsg;
//        } else {
//          msg() << MSG::DEBUG << "REGTEST: MuMu mass variance is 0 can't calculate the pull " << endmsg;
//        }
//
//        msg() << MSG::DEBUG << "REGTEST: Chi2 vtx fit = " << p_mumuV->chi2() << ", prob. " << chi2prob << endmsg;
//        msg() << MSG::DEBUG << "SigmaX =  SigmaY =  SigmaZ = " << sqrt(p_mumuV->cov()[0]) << " " << sqrt(p_mumuV->cov()[2]) << " " << sqrt(p_mumuV->cov()[5]) << endmsg;
//
//      }
//      trigPart->fitmass(p_mumuV->mass());
//      trigPart->fitchi2(p_mumuV->chi2());
//      trigPart->fitndof(p_mumuV->ndof());
//      trigPart->fitx(p_mumuV->x());
//      trigPart->fity(p_mumuV->y());
//      trigPart->fitz(p_mumuV->z());
//      
//      m_mon_FitMass.push_back(p_mumuV->mass()/1000.);      
//      m_mon_Chi2toNDoF.push_back(p_mumuV->chi2()/ p_mumuV->ndof());      
//
//      delete p_mumuV;
//    }
//
//
//    return trigPart;
//}

TrigVertex* TrigL2MultiMuFex::fitToVertex(const std::vector<const xAOD::L2CombinedMuon*> & muons) {
    // note, on sucessful return of the function, calling method owns the returned pointer; please delete it.
    int counter(0);
    std::vector<const xAOD::L2CombinedMuon*>::const_iterator itmu     = muons.begin();
    std::vector<const xAOD::L2CombinedMuon*>::const_iterator itmu_end = muons.end();
    
    if ( msgLvl() <= MSG::DEBUG ) {
        msg() << MSG::DEBUG << "Just check track links... " << endmsg;
        for (;itmu != itmu_end; ++itmu,++counter) {
            msg() << MSG::DEBUG << "Track " << counter << " pT " << (*itmu)->idTrack()->pt()
            << " eta " << (*itmu)->idTrack()->eta() << " phi " << (*itmu)->idTrack()->phi()
            << " q "   << (*itmu)->idTrack()->charge() << endmsg;
            
        } // for loop over muons
        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Now do vertexing" << endmsg;
    }
    
    bool addTracks = true;
    itmu  = muons.begin(); // reset the iterator to the start
    counter =0;

    TrigL2Vertex* pL2V    = new TrigL2Vertex();
    for (;itmu != itmu_end; ++itmu,++counter) {
        // #FIXME - previous code used MuonFeature::IDTrack()::chi2() / are these equivalent ... ?
        if ((*itmu)->idTrack()->chiSquared() > 1e7) { //#FIME JW don't hardcode
            addTracks = false;
            if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Failed to add track " << counter << "  to vertexingTool" << endmsg;
            m_mon_Errors.push_back( ERROR_AddTrack_Fails );
            // break; // may be useful to record all track outputs, so comment out for now
        } // if bad chi2
        
        if (m_vertexingTool->addTrack((*itmu)->idTrack()->track(),pL2V,Trk::muon).isFailure()) {
            if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Failed to add track " << counter << " to vertexingTool" << endmsg;
            addTracks = false;
            m_mon_Errors.push_back( ERROR_AddTrack_Fails );
        } // if addTrack
        
    } // loop over muons
    
    if (!addTracks) {
        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Bad tracks in fit input; return before fitter" << endmsg;
        delete pL2V;
        pL2V = nullptr;
        return nullptr;
    }
    
    
    if (m_L2vertFitter->fit(pL2V).isFailure()) {
        if ( msgLvl() <= MSG::DEBUG )msg() << MSG::DEBUG << "TrigL2VertexFitter failed" << endmsg;
        delete pL2V;
        pL2V = nullptr;
        return nullptr;
    } // if failed fit
    
    if (m_vertexingTool->calculateInvariantMass(pL2V).isFailure()) {
        m_mon_Errors.push_back( ERROR_CalcInvMass_Fails );
        if ( msgLvl() <= MSG::DEBUG )msg() << MSG::DEBUG << "calculateInvariantMass failed" << endmsg;
        delete pL2V;
        pL2V = nullptr;
        return nullptr;
    } // failed inv mass #FIXME - assumes some mass hypothesis??
    
    TrigVertex* p_mumuV = m_vertexingTool->createTrigVertex(pL2V);
    
    delete pL2V;
    pL2V = nullptr;
    
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Sucessful fit"  << endmsg;
    double chi2prob = 0.;
    chi2prob = 1.0 - Genfun::CumulativeChiSquare(p_mumuV->ndof())(p_mumuV->chi2());
    
    if ( msgLvl() <= MSG::DEBUG ) {
        msg() << MSG::DEBUG << "REGTEST: mumu vertex Fit: x= y= z=" << p_mumuV->x() << " " << p_mumuV->y() << " " << p_mumuV->z() << endmsg;
        msg() << MSG::DEBUG << "REGTEST: mumu mass = " << p_mumuV->mass() << endmsg;
        if (p_mumuV->massVariance() !=0) {
            msg() << MSG::DEBUG << "REGTEST: mumu mass pull = " << (p_mumuV->mass()-BSMASS)/p_mumuV->massVariance() << endmsg;
        } else {
            msg() << MSG::DEBUG << "REGTEST: MuMu mass variance is 0 can't calculate the pull " << endmsg;
        }
        
        msg() << MSG::DEBUG << "REGTEST: Chi2 vtx fit = " << p_mumuV->chi2() << ", prob. " << chi2prob << endmsg;
        msg() << MSG::DEBUG << "SigmaX =  SigmaY =  SigmaZ = " << sqrt(p_mumuV->cov()[0]) << " " << sqrt(p_mumuV->cov()[2]) << " " << sqrt(p_mumuV->cov()[5]) << endmsg;
        
    }
    m_mon_FitMass.push_back(p_mumuV->mass()/1000.);
    m_mon_Chi2toNDoF.push_back(p_mumuV->chi2()/ p_mumuV->ndof());
//     m_mon_FitTotalPt.push_back(p_mumuV->getMotherTrack()->pT()); // crashes -- no mother particle is created?
    m_mon_FitVtxR.push_back( pow(p_mumuV->x(),2) + pow(p_mumuV->y(),2) );
    m_mon_FitVtxZ.push_back( p_mumuV->z() );
    
    return p_mumuV;
} //fitToVertex

//TrigL2Bphys* TrigL2MultiMuFex::checkInvMass2(const CombinedMuonFeature *muon1,const CombinedMuonFeature *muon2, double Mass)
//{
//    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " In checkInvMass2, cuts " << m_lowerMassCut << " " << m_upperMassCut <<endmsg;
//    
//    if ( Mass < m_lowerMassCut || Mass > m_upperMassCut  ) {
//        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Mass : " << Mass << " cut failed  "  << endmsg;
//        return NULL;
//    }
//    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Mass cut passed, creat TrigL2Bphys  "  << endmsg;
//    m_passInvMass=true;
//    TrigL2Bphys* trigPart = new TrigL2Bphys( 0, 0., 0., TrigL2Bphys::MULTIMU, Mass); //should really be type MULTIMU - needs changes to TrigPartcile
//    
//    
//    //  Add element-links to tracks
//    ElementLink<TrigInDetTrackCollection> track1EL=muon1->IDTrackLink();
//    ElementLink<TrigInDetTrackCollection> track2EL=muon2->IDTrackLink();
//    if ( msgLvl() <= MSG::DEBUG ) {
//        msg() << MSG::DEBUG << "Just check track links... " << endmsg;
//        msg() << MSG::DEBUG << "Track 1 pT " << (*track1EL)->param()->pT()
//        << " eta: " << (*track1EL)->param()->eta()
//        << " phi: " << (*track1EL)->param()->phi0() << endmsg;
//        msg() << MSG::DEBUG << "Track 2 pT " << (*track2EL)->param()->pT()
//        << " eta: " << (*track2EL)->param()->eta()
//        << " phi: " << (*track2EL)->param()->phi0() << endmsg;
//    }
//    trigPart->addTrack(track1EL);
//    trigPart->addTrack(track2EL);
//    
//    //vertexing
//    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Now do vertexing" << endmsg;
//    
//    TrigL2Vertex* pL2V    = new TrigL2Vertex();
//    TrigVertex*   p_mumuV = NULL;
//    StatusCode sc;
//    // Add tracks to the vertexer
//    bool addTracks = true;
//    if (muon1->IDTrack()->chi2() < 1e7 ) {
//        sc = m_vertexingTool->addTrack(muon1->IDTrack(),pL2V,Trk::muon);
//        if(sc.isFailure()){
//            if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Failed to add track 1 to vertexingTool" << endmsg;
//            addTracks = false;
//            m_mon_Errors.push_back( ERROR_AddTrack_Fails );
//        }
//    } else {
//        addTracks = false;
//    }
//    
//    if (muon2->IDTrack()->chi2() < 1e7 ) {
//        sc = m_vertexingTool->addTrack(muon2->IDTrack(),pL2V,Trk::muon);
//        if(sc.isFailure()){
//            if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Failed to add track 2 to vertexingTool" << endmsg;
//            addTracks = false;
//            m_mon_Errors.push_back( ERROR_AddTrack_Fails );
//        }
//    } else {
//        addTracks = false;
//    }
//    // DO THE VERTEX-FIT
//    if ( addTracks) {
//        sc = m_L2vertFitter->fit(pL2V);
//    }
//    
//    // Check the result
//    if ( sc.isFailure() || (!addTracks) ) {
//        if ( msgLvl() <= MSG::DEBUG )msg() << MSG::DEBUG << "TrigL2VertexFitter failed" << endmsg;
//    } else {
//        // Calculate invariant mass
//        sc = m_vertexingTool->calculateInvariantMass(pL2V);
//        if ( sc.isSuccess() ) {
//            // Create mother particle
//            //        sc = m_vertexingTool->createMotherParticle(pL2V);
//            //if ( sc.isSuccess() ) {
//            //  if ( msgLvl() <= MSG::VERBOSE ) msg() << MSG::VERBOSE << "Bs created: pT "<<pL2V->m_getMotherTrack()->pT() << endmsg;
//            //} else {
//            //  m_mon_Errors.push_back( ERROR_CalcMother_Fails );
//            // }
//            p_mumuV = m_vertexingTool->createTrigVertex(pL2V);
//        } else {
//            m_mon_Errors.push_back( ERROR_CalcInvMass_Fails );
//        }
//    }
//    delete pL2V;
//    
//    if ( p_mumuV == NULL ) {
//        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " vertex fit failed for tracks  " << endmsg;
//        delete p_mumuV;
//    } else {
//        double chi2prob = 0.;
//        chi2prob = 1.0 - Genfun::CumulativeChiSquare(p_mumuV->ndof())(p_mumuV->chi2());
//        if ( msgLvl() <= MSG::DEBUG ) {
//            msg() << MSG::DEBUG << "REGTEST: mumu vertex Fit: x= y= z=" << p_mumuV->x() << " " << p_mumuV->y() << " " << p_mumuV->z() << endmsg;
//            msg() << MSG::DEBUG << "REGTEST: mumu mass = " << p_mumuV->mass() << endmsg;
//            if (p_mumuV->massVariance() !=0) {
//                msg() << MSG::DEBUG << "REGTEST: mumu mass pull = " << (p_mumuV->mass()-BSMASS)/p_mumuV->massVariance() << endmsg;
//            } else {
//                msg() << MSG::DEBUG << "REGTEST: MuMu mass variance is 0 can't calculate the pull " << endmsg;
//            }
//            
//            msg() << MSG::DEBUG << "REGTEST: Chi2 vtx fit = " << p_mumuV->chi2() << ", prob. " << chi2prob << endmsg;
//            msg() << MSG::DEBUG << "SigmaX =  SigmaY =  SigmaZ = " << sqrt(p_mumuV->cov()[0]) << " " << sqrt(p_mumuV->cov()[2]) << " " << sqrt(p_mumuV->cov()[5]) << endmsg;
//            
//        }
//        trigPart->fitmass(p_mumuV->mass());
//        trigPart->fitchi2(p_mumuV->chi2());
//        trigPart->fitndof(p_mumuV->ndof());
//        trigPart->fitx(p_mumuV->x());
//        trigPart->fity(p_mumuV->y());
//        trigPart->fitz(p_mumuV->z());
//        m_mon_FitMass.push_back(p_mumuV->mass()/1000.);      
//        m_mon_Chi2toNDoF.push_back(p_mumuV->chi2()/ p_mumuV->ndof());      
//        delete p_mumuV;
//    }
//    
//    
//    return trigPart;
//}



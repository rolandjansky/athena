/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// vim: tabstop=2:shiftwidth=2:expandtab
// -*- C++ -*-//
/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBPhysHypo/TrigEFBEEXFex.cxx
 **
 **   Description: EF hypothesis algorithms for
 **                B^0_{s,d} \to X e^+ e^-
 **
 **
 **   Authors: O.Igonkina and H.Russell
 **
 **   Created:   25.09.2017
 **
 **
 ***************************************************************************/


#include "TrigEFBEEXFex.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"
#include <math.h>
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "AthenaKernel/Timeout.h"

#include "TrkTrack/TrackCollection.h"
#include "TrkParameters/TrackParameters.h"
//#include "Particle/TrackParticleContainer.h"
#include "VxVertex/VxCandidate.h"

// additions of xAOD objects
#include "xAODEventInfo/EventInfo.h"
#include <memory>

/*----------------------------------------------------------------------------*/
TrigEFBEEXFex::TrigEFBEEXFex(const std::string & name, ISvcLocator* pSvcLocator):
HLT::ComboAlgo(name, pSvcLocator)
,m_fitterSvc("Trk::TrkVKalVrtFitter/VertexFitterTool",this)
,m_bphysHelperTool("TrigBphysHelperUtilsTool")
,m_TrigBphysColl_b(NULL)
,m_TrigBphysColl_X(NULL)

,m_TotTimer(0)
,m_VtxFitTimer(0)
,m_VKVFitter(0)

// counters
,m_lastEvent(-1)
,m_lastEventPassed(-1)
,m_countTotalEvents(0)
,m_countTotalRoI(0)
,m_countPassedEvents(0)
,m_countPassedRoIs(0)

,m_lastEventPassedBplus(-1)
,m_lastEventPassedBd(-1)
,m_lastEventPassedBs(-1)
,m_countPassedEventsBplus(0)
,m_countPassedEventsBs(0)
,m_countPassedEventsBd(0)

,m_countPassedEEID(0)
,m_countPassedEEOS(0)
,m_countPassedEEMass(0)
,m_countPassedEEVtx(0)
,m_countPassedEEVtxChi2(0)

,m_countPassedBplusMass(0)
,m_countPassedBplusVtx(0)
,m_countPassedBplusVtxChi2(0)
,m_countBpToStore(0)

,m_countPassedKstarMass(0)
,m_countPassedBdMass(0)
,m_countPassedKstarVtx(0)
,m_countPassedKstarVtxChi2(0)
,m_countPassedBdVtx(0)
,m_countPassedBdVtxChi2(0)
,m_countBdToStore(0)

,m_countPassedPhi1020Mass(0)
,m_countPassedBsMass(0)
,m_countPassedPhi1020Vtx(0)
,m_countPassedPhi1020VtxChi2(0)
,m_countPassedBsVtx(0)
,m_countPassedBsVtxChi2(0)
,m_countBsToStore(0)


{
    declareProperty("TrigBphysHelperTool", m_bphysHelperTool);
    declareProperty("VertexFitterTool", m_fitterSvc);
    
    declareProperty("bphysCollectionKey", m_bphysCollectionKey  = "EFBEEXFex" );
    declareProperty("AcceptAll",    m_acceptAll=true); // Should we just accept all events
    
    declareProperty("TrackCollection",m_input_trackCollectionKey="InDetTrigTrackingxAODCnv_Bphysics_IDTrig");
    
    // sign & mass cuts
    declareProperty("OppositeSign", m_oppositeCharge = true);                            // deafult = true
    declareProperty("LowerEEMassCut", m_lowerEEMassCut = 100.);                      // default = 100.0 MeV
    declareProperty("UpperEEMassCut", m_upperEEMassCut = 5500.);                      // default = 5500.0 MeV
    declareProperty("VtxChi2Cut", m_VtxChi2Cut = 40.);                              // default = 40.0
    
    // Maximum number of decay candidates (2 mu + 1 or 2 tracks) to test - protection against TimeOuts
    declareProperty("MaxNcombinations", m_maxNcombinations = 50000);
    
    // B+->K+EE cuts
    declareProperty("DoB_KEEDecay", m_doB_KEEDecay = true);                          //Proceed B->KEE part of algorithm
    declareProperty("LowerKEEMassCut", m_lowerKEEMassCut = 4500.);                    // default = 4500.0 MeV
    declareProperty("UpperKEEMassCut", m_upperKEEMassCut = 5900.);                    // default = 5900.0 MeV
    declareProperty("DoB_KEEVertexing", m_doB_KEEVertexing = true);
    declareProperty("BVtxChi2Cut", m_bVtxChi2Cut = 300.);                                // default = 300.0
    declareProperty("MaxBpToStore", m_maxBpToStore = -1);
    
    // Bd->K*EE cuts
    declareProperty("DoBd_KstarEEDecay", m_doBd_KstarEEDecay = true);                //Proceed Bd->K*EE part of algorithm
    declareProperty("LowerKstar_KaonMassCut", m_lowerKstar_KaonMassCut = 600.);          // default = 600.0 MeV
    declareProperty("UpperKstar_KaonMassCut", m_upperKstar_KaonMassCut = 1500.);          // default = 1500.0 MeV
    declareProperty("LowerBd_KstarEEMassCut", m_lowerBd_KstarEEMassCut= 4300.);      // default = 4600.0 MeV
    declareProperty("UpperBd_KstarEEMassCut", m_upperBd_KstarEEMassCut =6300.);      // default = 6300.0 MeV
    declareProperty("DoKstar_KPiVertexing", m_doKstar_KPiVertexing = true);
    declareProperty("DoBd_KstarEEVertexing", m_doBd_KstarEEVertexing = true);
    declareProperty("KstarVtxChi2Cut", m_kStarVtxChi2Cut = 60.);                        // default = 60.0
    declareProperty("BdVtxChi2Cut", m_bDVtxChi2Cut = 60.);                              // default = 60.0
    declareProperty("MaxBdToStore", m_maxBdToStore = -1);
    
    // Bs->PhiEE cuts
    declareProperty("DoBs_Phi1020EEDecay", m_doBs_Phi1020EEDecay = true);            //Proceed Bs->PhiEE part of algorithm
    declareProperty("LowerPhi1020_KaonMassCut", m_lowerPhi1020_KaonMassCut  = 990.);      // default = 990.0 MeV
    declareProperty("UpperPhi1020_KaonMassCut", m_upperPhi1020_KaonMassCut = 1050.);      // default = 1050.0 MeV
    declareProperty("LowerBs_Phi1020EEMassCut", m_lowerBs_Phi1020EEMassCut = 5000.);  // default = 5000.0 MeV
    declareProperty("UpperBs_Phi1020EEMassCut", m_upperBs_Phi1020EEMassCut = 5800.);  // default = 5800.0 MeV
    declareProperty("DoPhi1020_KKVertexing", m_doPhi1020_KKVertexing = true);
    declareProperty("DoBs_Phi1020EEVertexing", m_doBs_Phi1020EEVertexing = true);
    declareProperty("Phi1020VtxChi2Cut", m_phi1020VtxChi2Cut = 60.);                    // default = 60.0
    declareProperty("BsVtxChi2Cut", m_bSVtxChi2Cut = 60.);                              // default = 60.0
    declareProperty("MaxBsToStore", m_maxBsToStore = -1);
    

    // FTK Flag
    declareProperty("DoFTK",    m_FTK=false); // Are we using FTK??

    
    // Monitoring variables
    //   General
    declareMonitoredStdContainer("Errors",     m_mon_Errors,     AutoClear);
    declareMonitoredStdContainer("Acceptance", m_mon_Acceptance, AutoClear);
    declareMonitoredVariable("nTriedCombinations", m_mon_nTriedCombinations );
    //   Timing
    declareMonitoredVariable("TotalRunTime",  m_mon_TotalRunTime);
    declareMonitoredVariable("VertexingTime", m_mon_VertexingTime);
    //   RoIs
    declareMonitoredStdContainer("RoI_RoI1Eta", m_mon_RoI_RoI1Eta, AutoClear);
    declareMonitoredStdContainer("RoI_RoI2Eta", m_mon_RoI_RoI2Eta, AutoClear);
    declareMonitoredStdContainer("RoI_RoI1Phi", m_mon_RoI_RoI1Phi, AutoClear);
    declareMonitoredStdContainer("RoI_RoI2Phi", m_mon_RoI_RoI2Phi, AutoClear);
    declareMonitoredStdContainer("RoI_dEtaRoI", m_mon_RoI_dEtaRoI, AutoClear);
    declareMonitoredStdContainer("RoI_dPhiRoI", m_mon_RoI_dPhiRoI, AutoClear);
    //   DiEon
    declareMonitoredVariable("DiE_n", m_mon_DiE_n);
    declareMonitoredStdContainer("DiE_Pt_Lep1",       m_mon_DiE_Pt_Lep1,       AutoClear);
    declareMonitoredStdContainer("DiE_Pt_Lep2",       m_mon_DiE_Pt_Lep2,       AutoClear);
    declareMonitoredStdContainer("DiE_Eta_Lep1",      m_mon_DiE_Eta_Lep1,      AutoClear);
    declareMonitoredStdContainer("DiE_Eta_Lep2",      m_mon_DiE_Eta_Lep2,      AutoClear);
    declareMonitoredStdContainer("DiE_Phi_Lep1",      m_mon_DiE_Phi_Lep1,      AutoClear);
    declareMonitoredStdContainer("DiE_Phi_Lep2",      m_mon_DiE_Phi_Lep2,      AutoClear);
    declareMonitoredStdContainer("DiE_dEtaEE",     m_mon_DiE_dEtaEE,     AutoClear);
    declareMonitoredStdContainer("DiE_dPhiEE",     m_mon_DiE_dPhiEE,     AutoClear);
    declareMonitoredStdContainer("DiE_pTsumEE",    m_mon_DiE_pTsumEE,    AutoClear);
    declareMonitoredStdContainer("DiE_InvMassEE",  m_mon_DiE_InvMassEE, AutoClear);
    declareMonitoredStdContainer("DiE_VtxMassEE",  m_mon_DiE_VtxMassEE, AutoClear);
    declareMonitoredStdContainer("DiE_Chi2EE",     m_mon_DiE_Chi2EE,    AutoClear);
    //   Tracks
    declareMonitoredVariable("Tracks_n", m_mon_Tracks_n);
    declareMonitoredStdContainer("Tracks_Eta", m_mon_Tracks_Eta, AutoClear);
    declareMonitoredStdContainer("Tracks_Pt",  m_mon_Tracks_Pt,  AutoClear);
    declareMonitoredStdContainer("Tracks_Phi", m_mon_Tracks_Phi, AutoClear);
    //   B+
    declareMonitoredVariable("BEEK_n", m_mon_BEEK_n);
    declareMonitoredStdContainer("BEEK_Pt_K",      m_mon_BEEK_Pt_K,      AutoClear);
    declareMonitoredStdContainer("BEEK_Eta_K",     m_mon_BEEK_Eta_K,     AutoClear);
    declareMonitoredStdContainer("BEEK_Phi_K",     m_mon_BEEK_Phi_K,     AutoClear);
    declareMonitoredStdContainer("BEEK_InvMass_B", m_mon_BEEK_InvMass_B, AutoClear);
    declareMonitoredStdContainer("BEEK_VtxMass_B", m_mon_BEEK_VtxMass_B, AutoClear);
    declareMonitoredStdContainer("BEEK_Chi2_B",    m_mon_BEEK_Chi2_B,    AutoClear);
    //   Bd
    declareMonitoredVariable("BdEEKs_n", m_mon_BdEEKs_n);
    declareMonitoredStdContainer("BdEEKs_Pt_K",          m_mon_BdEEKs_Pt_K,          AutoClear);
    declareMonitoredStdContainer("BdEEKs_Eta_K",         m_mon_BdEEKs_Eta_K,         AutoClear);
    declareMonitoredStdContainer("BdEEKs_Phi_K",         m_mon_BdEEKs_Phi_K,         AutoClear);
    declareMonitoredStdContainer("BdEEKs_Pt_Pi",         m_mon_BdEEKs_Pt_Pi,         AutoClear);
    declareMonitoredStdContainer("BdEEKs_Eta_Pi",        m_mon_BdEEKs_Eta_Pi,        AutoClear);
    declareMonitoredStdContainer("BdEEKs_Phi_Pi",        m_mon_BdEEKs_Phi_Pi,        AutoClear);
    declareMonitoredStdContainer("BdEEKs_InvMass_Kstar", m_mon_BdEEKs_InvMass_Kstar, AutoClear);
    declareMonitoredStdContainer("BdEEKs_VtxMass_Kstar", m_mon_BdEEKs_VtxMass_Kstar, AutoClear);
    declareMonitoredStdContainer("BdEEKs_Chi2_Kstar",    m_mon_BdEEKs_Chi2_Kstar,    AutoClear);
    declareMonitoredStdContainer("BdEEKs_InvMass_Bd",    m_mon_BdEEKs_InvMass_Bd,    AutoClear);
    declareMonitoredStdContainer("BdEEKs_VtxMass_Bd",    m_mon_BdEEKs_VtxMass_Bd,    AutoClear);
    declareMonitoredStdContainer("BdEEKs_Chi2_Bd",       m_mon_BdEEKs_Chi2_Bd,       AutoClear);
    //   Bs
    declareMonitoredVariable("BsEEPhi_n", m_mon_BsEEPhi_n);
    declareMonitoredStdContainer("BsEEPhi_Pt_K1",           m_mon_BsEEPhi_Pt_K1,           AutoClear);
    declareMonitoredStdContainer("BsEEPhi_Eta_K1",          m_mon_BsEEPhi_Eta_K1,          AutoClear);
    declareMonitoredStdContainer("BsEEPhi_Phi_K1",          m_mon_BsEEPhi_Phi_K1,          AutoClear);
    declareMonitoredStdContainer("BsEEPhi_Pt_K2",           m_mon_BsEEPhi_Pt_K2,           AutoClear);
    declareMonitoredStdContainer("BsEEPhi_Eta_K2",          m_mon_BsEEPhi_Eta_K2,          AutoClear);
    declareMonitoredStdContainer("BsEEPhi_Phi_K2",          m_mon_BsEEPhi_Phi_K2,          AutoClear);
    declareMonitoredStdContainer("BsEEPhi_InvMass_Phi1020", m_mon_BsEEPhi_InvMass_Phi1020, AutoClear);
    declareMonitoredStdContainer("BsEEPhi_VtxMass_Phi1020", m_mon_BsEEPhi_VtxMass_Phi1020, AutoClear);
    declareMonitoredStdContainer("BsEEPhi_Chi2_Phi1020",    m_mon_BsEEPhi_Chi2_Phi1020,    AutoClear);
    declareMonitoredStdContainer("BsEEPhi_InvMass_Bs",      m_mon_BsEEPhi_InvMass_Bs,      AutoClear);
    declareMonitoredStdContainer("BsEEPhi_VtxMass_Bs",      m_mon_BsEEPhi_VtxMass_Bs,      AutoClear);
    declareMonitoredStdContainer("BsEEPhi_Chi2_Bs",         m_mon_BsEEPhi_Chi2_Bs,         AutoClear);
    
    
}


/*----------------------------------------------------------------------------*/
TrigEFBEEXFex::~TrigEFBEEXFex()
{ }


/*----------------------------------------------------------------------------*/
HLT::ErrorCode TrigEFBEEXFex::hltInitialize()
{
    msg() << MSG::INFO << "Running TrigEFBEEXFex::hltInitialize" << endmsg;
    
    if(msgLvl() <= MSG::DEBUG) {
        msg() << MSG::DEBUG << "Initialization completed successfully:" << endmsg;
        msg() << MSG::DEBUG << "AcceptAll          = "
        << (m_acceptAll==true ? "True" : "False") << endmsg;
        msg() << MSG::DEBUG << "DoFTK         = "
        << (m_FTK==true ? "True" : "False") << endmsg;
        msg() << MSG::INFO << "MaxNcombinations            = " << m_maxNcombinations << endmsg;
        
        msg() << MSG::DEBUG << "Activated decays:" << endmsg;
        msg() << MSG::DEBUG << "    B+ -> e e K+         : " << (m_doB_KEEDecay==true ? "True" : "False") << endmsg;
        msg() << MSG::DEBUG << "    Bd -> e e K*(K+ Pi-) : " << (m_doBd_KstarEEDecay==true ? "True" : "False") << endmsg;
        msg() << MSG::DEBUG << "    Bs -> e e Phi(K+ K-) : " << (m_doBs_Phi1020EEDecay==true ? "True" : "False") << endmsg;
        
        msg() << MSG::DEBUG << "OppositeCharge     = "        << (m_oppositeCharge==true ? "True" : "False") << endmsg;
        
    }
    
    // setting up timers
    if ( timerSvc() ) {
        m_TotTimer    = addTimer("EFBEEXFexTot");
        m_VtxFitTimer = addTimer("EFBEEXFexVtxFit");
    }
    
    // retrieving the vertex fitting tool
    if (m_fitterSvc.retrieve().isFailure()) {
        msg() << MSG::ERROR << "Can't find Trk::TrkVKalVrtFitter" << endmsg;
        return StatusCode::SUCCESS;
    } else {
        if (msgLvl() <= MSG::DEBUG) {
            msg() << MSG::DEBUG << "Trk::TrkVKalVrtFitter found" << endmsg;
        }
        m_VKVFitter = dynamic_cast< Trk::TrkVKalVrtFitter* > (&(*m_fitterSvc));
    }
    
    // retrieving BphysHelperUtilsTool
    if (m_bphysHelperTool.retrieve().isFailure()) {
        msg() << MSG::ERROR << "Can't find TrigBphysHelperUtilsTool" << endmsg;
        return StatusCode::SUCCESS;
    } else {
            if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "TrigBphysHelperUtilsTool found" << endmsg;
    }
    
    // set counters
    m_lastEvent=-1;
    m_lastEventPassed=-1;
    m_countTotalEvents=0;
    m_countTotalRoI=0;
    m_countPassedEvents=0;
    m_countPassedRoIs=0;
    
    m_lastEventPassedBplus=-1;
    m_lastEventPassedBd=-1;
    m_lastEventPassedBs=-1;
    m_lastEventPassedLb=-1;
    
    m_countPassedEventsBplus=0;
    m_countPassedEventsBs=0;
    m_countPassedEventsBd=0;
    m_countPassedEventsLb=0;
    m_countPassedEventsBc=0;
    
    m_countPassedEEID=0;
    m_countPassedEEOS=0;
    m_countPassedEEMass=0;
    m_countPassedEEVtx=0;
    m_countPassedEEVtxChi2=0;
    
    m_countPassedBplusMass=0;
    m_countPassedBplusVtx=0;
    m_countPassedBplusVtxChi2=0;
    
    m_countPassedKstarMass=0;
    m_countPassedBdMass=0;
    m_countPassedKstarVtx=0;
    m_countPassedKstarVtxChi2=0;
    m_countPassedBdVtx=0;
    m_countPassedBdVtxChi2=0;
    
    m_countPassedPhi1020Mass=0;
    m_countPassedBsMass=0;
    m_countPassedPhi1020Vtx=0;
    m_countPassedPhi1020VtxChi2=0;
    m_countPassedBsVtx=0;
    m_countPassedBsVtxChi2=0;
    

    
    return HLT::OK;
}


/*----------------------------------------------------------------------------*/
HLT::ErrorCode TrigEFBEEXFex::hltFinalize()
{
    
    msg() << MSG::INFO << "Running TrigEFBEEXFex::hltFinalize" << endmsg;
    
    msg() << MSG::INFO << "|----------------------- SUMMARY FROM TrigEFBEEXFex -------------|" << endmsg;
    msg() << MSG::INFO << "Run on events/RoIs      " << m_countTotalEvents << "/" << m_countTotalRoI <<  endmsg;
    msg() << MSG::INFO << "Passed events/RoIs      " << m_countPassedEvents << "/" << m_countPassedRoIs <<  endmsg;
    msg() << MSG::INFO << "Evts Passed B+:         " << m_countPassedEventsBplus << endmsg;
    msg() << MSG::INFO << "Evts Passed Bd:         " << m_countPassedEventsBd << endmsg;
    msg() << MSG::INFO << "Evts Passed Bs:         " << m_countPassedEventsBs << endmsg;
    msg() << MSG::INFO << std::endl << endmsg;
    msg() << MSG::INFO << "PassedEEID:           " << m_countPassedEEID << endmsg;
    msg() << MSG::INFO << "PassedEEOS:           " << m_countPassedEEOS << endmsg;
    msg() << MSG::INFO << "PassedEEMass:         " << m_countPassedEEMass << endmsg;
    msg() << MSG::INFO << "PassedEEVtx:          " << m_countPassedEEVtx << endmsg;
    msg() << MSG::INFO << "PassedEEVtxChi2:      " << m_countPassedEEVtxChi2 << endmsg;
    msg() << MSG::INFO << "PassedBplusMass:        " << m_countPassedBplusMass << endmsg;
    msg() << MSG::INFO << "PassedBplusVtx:         " << m_countPassedBplusVtx << endmsg;
    msg() << MSG::INFO << "PassedBplusVtxChi2:     " << m_countPassedBplusVtxChi2 << endmsg;
    msg() << MSG::INFO << "PassedKstarMass:        " << m_countPassedKstarMass << endmsg;
    msg() << MSG::INFO << "PassedKstarVtx:         " << m_countPassedKstarVtx << endmsg;
    msg() << MSG::INFO << "PassedKstarVtxChi2:     " << m_countPassedKstarVtxChi2 << endmsg;
    msg() << MSG::INFO << "PassedBdMass:           " << m_countPassedBdMass << endmsg;
    msg() << MSG::INFO << "PassedBdVtx:            " << m_countPassedBdVtx << endmsg;
    msg() << MSG::INFO << "PassedBdVtxChi2:        " << m_countPassedBdVtxChi2 << endmsg;
    msg() << MSG::INFO << "PassedPhi1020Mass:      " << m_countPassedPhi1020Mass << endmsg;
    msg() << MSG::INFO << "PassedPhi1020Vtx:       " << m_countPassedPhi1020Vtx << endmsg;
    msg() << MSG::INFO << "PassedPhi1020VtxChi2:   " << m_countPassedPhi1020VtxChi2 << endmsg;
    msg() << MSG::INFO << "PassedBsMass:           " << m_countPassedBsMass << endmsg;
    msg() << MSG::INFO << "PassedBsVtx:            " << m_countPassedBsVtx << endmsg;
    msg() << MSG::INFO << "PassedBsVtxChi2:        " << m_countPassedBsVtxChi2 << endmsg;
    
    return HLT::OK;
}


/*----------------------------------------------------------------------------*/
HLT::ErrorCode TrigEFBEEXFex::acceptInputs(HLT::TEConstVec& , bool& pass)
{
    if ( msgLvl() <= MSG::DEBUG )
        msg() << MSG::DEBUG << "Running TrigEFBEEXFex::acceptInputs" << endmsg;
    
    pass = true;
    
    return HLT::OK;
}


/*----------------------------------------------------------------------------*/
HLT::ErrorCode TrigEFBEEXFex::hltExecute(HLT::TEConstVec& inputTE, HLT::TriggerElement*  outputTE )
{
    if ( msgLvl() <= MSG::DEBUG )
        msg() << MSG::DEBUG << "Running TrigEFBEEXFex::hltExecute" << endmsg;
    
    if ( timerSvc() ) m_TotTimer->start();
    
    if ( timerSvc() ) {
        m_VtxFitTimer->start();
        m_VtxFitTimer->pause();
    }
    
    bool result(false);
    
    // Set monitoring counters to zeroes
    m_mon_nTriedCombinations = 0;
    m_mon_DiE_n         = 0;
    m_mon_Tracks_n       = 0;
    m_mon_BEEK_n       = 0;
    m_mon_BdEEKs_n     = 0;
    m_mon_BsEEPhi_n    = 0;
    m_mon_TotalRunTime   = 0.;
    m_mon_VertexingTime  = 0.;
    
    if(msgLvl() <= MSG::VERBOSE) {
        if (m_acceptAll) {
            msg() << MSG::VERBOSE << "AcceptAll property is set: taking all events" << endmsg;
            result = true;
        } else {
            msg() << MSG::VERBOSE << "AcceptAll property not set: applying selection" << endmsg;
        }
    }
    
    // Retrieve event info
    int IdRun   = 0;
    int IdEvent = 0;
    
    // JW - Try to get the xAOD event info
    const EventInfo* pEventInfo(0);
    const xAOD::EventInfo *evtInfo(0);
    if ( store()->retrieve(evtInfo).isFailure() ) {
        if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Failed to get xAOD::EventInfo " << endmsg;
        // now try the old event ifo
        if ( store()->retrieve(pEventInfo).isFailure() ) {
            if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Failed to get EventInfo " << endmsg;
            m_mon_Errors.push_back( ERROR_No_EventInfo );
        } else {
            IdRun   = pEventInfo->event_ID()->run_number();
            IdEvent = pEventInfo->event_ID()->event_number();
            if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Run " << IdRun << " Event " << IdEvent << " using algo " << "m_lepAlgo"<<  endmsg;
        }// found old event info
    }else { // found the xAOD event info
        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Run " << evtInfo->runNumber()
            << " Event " << evtInfo->eventNumber() << " using algo m_lepAlgo" << endmsg;
        IdRun   = evtInfo->runNumber();
        IdEvent = evtInfo->eventNumber();
    } // get event info
    
    
    //Check that we got 2 input TEs
    int lep1_TE=-1;
    int lep2_TE=-1;
    if (!m_FTK) {
      if ( inputTE.size() != 2 ) {
        msg() << MSG::DEBUG << "Got different than 2 number of input TEs: " << inputTE.size() << endmsg;
          //m_mon_Errors.push_back(ERROR_Not_2_InputTEs);
          //if ( timerSvc() ) m_TotTimer->stop();
          //return HLT::BAD_JOB_SETUP;
        lep1_TE=0;
        lep2_TE=0;
      }else{
        lep1_TE=0;
        lep2_TE=1;
      }
    } else {
      if ( inputTE.size() != 3 ) {
        msg() << MSG::ERROR << "FTK mode expect 3 input TEs, got : " << inputTE.size() << endmsg;
        m_mon_Errors.push_back(ERROR_Not_2_InputTEs);
        if ( timerSvc() ) m_TotTimer->stop();
        return HLT::BAD_JOB_SETUP;
      }
      lep1_TE=1;
      lep2_TE=2;
    }


    if(IdEvent!=m_lastEvent) {
        m_countTotalEvents++;
        m_lastEvent = IdEvent;
        m_countBpToStore = 0;
        m_countBdToStore = 0;
        m_countBsToStore = 0;
    }
    m_countTotalRoI++;
    
    
    // JK DEBUG input TEs

    for (unsigned int iTE=0; iTE<inputTE.size(); ++iTE) {
      msg() << MSG::DEBUG << "Input TE " << iTE << "  " << inputTE[iTE] << " ID " << inputTE[iTE]->getId() << endmsg;
    }


    //Retrieve ROIs
    const TrigRoiDescriptor *roiDescriptor1(0);
    const TrigRoiDescriptor *roiDescriptor2(0);
    const TrigRoiDescriptor *roiDescriptorTrk(0); // for FTK chain
    
    // get them from the navigation

     if ( getFeature(inputTE[lep1_TE], roiDescriptor1) != HLT::OK ) {
        msg() << MSG::ERROR << "Navigation error while getting RoI descriptor 1" << endmsg;
        m_mon_Errors.push_back(ERROR_No_RoIs);
        if ( timerSvc() ) m_TotTimer->stop();
        return HLT::NAV_ERROR;
     }
     if ( !roiDescriptor1 ) {
       msg() << MSG::ERROR << "roiDescriptor1 is NULL" << endmsg;
       return HLT::NAV_ERROR;
     }
    
     if ( getFeature(inputTE[lep2_TE], roiDescriptor2) != HLT::OK ) {
        msg() << MSG::ERROR << "Navigation error while getting RoI descriptor 2" << endmsg;
        m_mon_Errors.push_back(ERROR_No_RoIs);
        if ( timerSvc() ) m_TotTimer->stop();
        return HLT::NAV_ERROR;
     }
     if ( !roiDescriptor2 ) {
       msg() << MSG::ERROR << "roiDescriptor2 is NULL" << endmsg;
       return HLT::NAV_ERROR;
     }

     if (m_FTK) {
      if ( getFeature(inputTE[0], roiDescriptorTrk) != HLT::OK ) {
        msg() << MSG::ERROR << "Navigation error while getting RoI descriptor Trk" << endmsg;
        m_mon_Errors.push_back(ERROR_No_RoIs);
        if ( timerSvc() ) m_TotTimer->stop();
        return HLT::NAV_ERROR;
      }
     }


    if ( msgLvl() <= MSG::DEBUG ){
        msg() << MSG::DEBUG
        << "Using inputTEs: "<< inputTE[lep1_TE] <<  " and "  << inputTE[lep2_TE] << " with Ids " << inputTE[lep1_TE]->getId()<< " AND "<< inputTE[lep2_TE]->getId() << std::endl
        << "; RoI IDs = "   << roiDescriptor1->roiId()<< " AND   " <<roiDescriptor2->roiId() << std::endl
        << ": Eta1 =    "   << roiDescriptor1->eta() << " Eta2= " <<roiDescriptor2->eta() << std::endl
        << ", Phi1 =    "   << roiDescriptor1->phi() << " Phi2= " <<roiDescriptor2->phi()
        << endmsg;
	if (m_FTK) {
	  msg() << MSG::DEBUG << "Using inputTE for tracks: "<< inputTE[0] << " " << inputTE[0]->getId()<< std::endl
        << "; RoI IDs = "   << roiDescriptorTrk->roiId() << std::endl
        << ": EtaTrk =    "   << roiDescriptorTrk->eta() << std::endl
        << ", PhiTrk =    "   << roiDescriptorTrk->phi() 
        << endmsg;
	}
    }
    
    // Fill RoIs monitoring containers
    m_mon_RoI_RoI1Eta.push_back(roiDescriptor1->eta());
    m_mon_RoI_RoI1Phi.push_back(roiDescriptor1->phi());
    m_mon_RoI_RoI2Eta.push_back(roiDescriptor2->eta());
    m_mon_RoI_RoI2Phi.push_back(roiDescriptor2->phi());
    m_mon_RoI_dEtaRoI.push_back( fabs(roiDescriptor1->eta() - roiDescriptor2->eta()) );
    float tmp_RoI_dPhiRoI = roiDescriptor1->phi() - roiDescriptor2->phi();
    while (tmp_RoI_dPhiRoI >  M_PI) tmp_RoI_dPhiRoI -= 2*M_PI;
    while (tmp_RoI_dPhiRoI < -M_PI) tmp_RoI_dPhiRoI += 2*M_PI;
    m_mon_RoI_dPhiRoI.push_back( fabs(tmp_RoI_dPhiRoI) );
    
    // Retrieve leps
    std::vector<ElementLink<xAOD::TrackParticleContainer> > lepTPELtracks1;
    std::vector<ElementLink<xAOD::TrackParticleContainer> > lepTPELtracks2;
    std::vector<ElementLink<xAOD::TrackParticleContainer> > lepTPELtracksMerged; // combined unique tracks from roi 1 and 2

    // Get the lepton container from the outputTE
    ElementLinkVector<xAOD::ElectronContainer> lepContainerEF1;
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Try to retrieve EFInfo container of lep 1" << endmsg;
    if(getFeaturesLinks<xAOD::ElectronContainer,xAOD::ElectronContainer>(inputTE[lep1_TE], lepContainerEF1)!=HLT::OK ) {
        if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Failed to get EFInfo feature of lep 1, exiting" << endmsg;
        m_mon_Errors.push_back(ERROR_No_ElectronContainer);
        if ( timerSvc() ) m_TotTimer->stop();
        return HLT::OK; 
    }
    if(msgLvl() <= MSG::DEBUG) ATH_MSG_DEBUG("Found ElectronContainer, Got LepEF (1) Feature, size = " << lepContainerEF1.size());


    // OI : here we probably should check that electron object has passed identification at the previous step
    for ( const auto muel : lepContainerEF1 ) {
   
      //if ( (*muel)->lepType() != xAOD::Lep::Combined && (*muel)->lepType() != xAOD::Lep::SegmentTagged) {
      //  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Lep from roi1 is neither Combined or SegmentTagged - reject" << endmsg;
      //  continue;
      //}
      const xAOD::Electron* lep = *muel;
      if(!lep) continue;
      if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "electron! " << lep->pt() << ", " << lep->eta() << ", " << lep->phi() << endmsg;
      if ( lep->pt() < 5000.) continue; 

      const xAOD::TrackParticle* idtp(0);
        const ElementLink<xAOD::TrackParticleContainer> & idtpEl = lep->trackParticleLink();
      if( idtpEl.isValid() ) idtp = *idtpEl;
      if (!idtp) {
          if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "No innerdetector lep1 TrackParticle found" << endmsg;
          continue;
      }
        addUnique(lepTPELtracks1,idtpEl);
        addUnique(lepTPELtracksMerged,idtpEl);
        
        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG
            << " Comb lep 1 pt/eta/phi " << idtp->pt()
            << " / " << idtp->eta() << " / " << idtp->phi()
            << endmsg;

        

    } // for
 
    // second
    
    ElementLinkVector<xAOD::ElectronContainer> lepContainerEF2;
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Try to retrieve EFInfo container of lep 2" << endmsg;
    if(getFeaturesLinks<xAOD::ElectronContainer,xAOD::ElectronContainer>(inputTE[lep2_TE], lepContainerEF2)!=HLT::OK ) {
        if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Failed to get EFInfo feature of lep 2, exiting" << endmsg;
        m_mon_Errors.push_back(ERROR_No_ElectronContainer);
        if ( timerSvc() ) m_TotTimer->stop();
        return HLT::OK; 
    }
    if(msgLvl() <= MSG::DEBUG) ATH_MSG_DEBUG("Found LepContainer, Got LepEF (2) Feature, size = " << lepContainerEF2.size());
    
    for ( const auto muel : lepContainerEF2 ) {
      //if ( (*muel)->lepType() != xAOD::Electron::Combined && (*muel)->lepType() != xAOD::Electron::SegmentTagged) {
      //  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Lep from roi2 is neither Combined or SegmentTagged - reject" << endmsg;
      //  continue;
      //}
      const xAOD::Electron* lep = *muel;
      if(!lep) continue;
      const xAOD::TrackParticle* idtp(0);
        const ElementLink<xAOD::TrackParticleContainer> & idtpEl = lep->trackParticleLink();
        if( idtpEl.isValid() ) idtp = *idtpEl;
      if (!idtp) {
          if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "No innerdetector lep2 TrackParticle found" << endmsg;
          continue;
      }
        addUnique(lepTPELtracks2,idtpEl);
        addUnique(lepTPELtracksMerged,idtpEl);
        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG
            << " Comb lep 1 pt/eta/phi " << idtp->pt()
            << " / " << idtp->eta() << " / " << idtp->phi()
            << endmsg;


    } // for loop over leps
    
    // build a map of the tracks and corresponding leps
    std::map<const xAOD::TrackParticle*, ElementLink<xAOD::ElectronContainer> > mapTrkToLeps;
    for (const auto mu : lepContainerEF1) {
            auto idtp  = (*mu)->trackParticleLink();
            if (!idtp.isValid()) continue;
            if (!*idtp) continue;
            //if (!(*idtp)->track()) continue;
            mapTrkToLeps[(*idtp)] = mu;
    } // lepContainerEF1
    for (const auto mu : lepContainerEF2) {
            auto idtp  = (*mu)->trackParticleLink();
            if (!idtp.isValid()) continue;
            if (!*idtp) continue;
            //if (!(*idtp)->track()) continue;
            mapTrkToLeps[(*idtp)] = mu;
    } // muContlepContainerEF2
   
    if(msgLvl() <= MSG::DEBUG) {
        msg() << MSG::DEBUG << "lepContainerEF1.size()= " << lepContainerEF1.size()<<endmsg;
        msg() << MSG::DEBUG << "lepContainerEF2.size()= " << lepContainerEF2.size()<<endmsg;
        msg() << MSG::DEBUG << "lepTPELtracks1.size()= "  << lepTPELtracks1.size() <<endmsg;
        msg() << MSG::DEBUG << "lepTPELtracks2.size()= "  << lepTPELtracks2.size() <<endmsg;
        msg() << MSG::DEBUG << "lepTPELtracksMerged.size()= "  << lepTPELtracksMerged.size() <<endmsg;
        
        for (auto muel: lepTPELtracks1) {
            msg() << MSG::DEBUG << "lepTPELtracks1: " << *muel << " " <<
            (*muel)->pt() << " , " << (*muel)->eta() << " , " << (*muel)->phi() << " , " << (*muel)->charge() << endmsg;
        }
        for (auto muel: lepTPELtracks2) {
            msg() << MSG::DEBUG << "lepTPELtracks2: " << *muel << " " <<
            (*muel)->pt() << " , " << (*muel)->eta() << " , " << (*muel)->phi() << " , " << (*muel)->charge() << endmsg;
        }
        for (auto muel: lepTPELtracksMerged) {
            msg() << MSG::DEBUG << "lepTPELtracksMerged: " << *muel << " " <<
            (*muel)->pt() << " , " << (*muel)->eta() << " , " << (*muel)->phi() << " , " << (*muel)->charge() << endmsg;
        }
    } // if debug
    
    Amg::Vector3D vtx ( 0.,0.,0. );
    
    //   TrigEFBphys* trigPartBEEX (NULL);
    // FIXME - remove these 'new's
    //m_trigBphysColl_b = new TrigEFBphysContainer();
    //m_trigBphysColl_X = new TrigEFBphysContainer();
    
    m_TrigBphysColl_b = new xAOD::TrigBphysContainer();
    xAOD::TrigBphysAuxContainer xAODTrigBphysAuxColl_b;
    m_TrigBphysColl_b->setStore(&xAODTrigBphysAuxColl_b);
    
    m_TrigBphysColl_X = new xAOD::TrigBphysContainer();
    xAOD::TrigBphysAuxContainer xAODTrigBphysAuxColl_X;
    m_TrigBphysColl_X->setStore(&xAODTrigBphysAuxColl_X);
    
    // Tried combinations counter - protection against timeouts
    int nTriedCombinations(0);
    
    if( !(lepTPELtracks1.size()> 0 && lepTPELtracks2.size() > 0) )
    {
        if ( msgLvl() <= MSG::DEBUG )
            msg() << MSG::DEBUG << "No lep candidate found for one or both TEs" << endmsg;
        m_mon_Errors.push_back(ERROR_No_ElectronCandidate);
        delete m_TrigBphysColl_b;
        delete m_TrigBphysColl_X;
        m_TrigBphysColl_b = 0;
        m_TrigBphysColl_X = 0;
        if ( timerSvc() ) m_TotTimer->stop();
        return HLT::OK;
    } else {
        
        // Loop over leps
        std::vector<ElementLink<xAOD::TrackParticleContainer> >::iterator pElItr=lepTPELtracksMerged.begin();
        std::vector<ElementLink<xAOD::TrackParticleContainer> >::iterator mElItr=lepTPELtracksMerged.begin();

        for(pElItr=lepTPELtracksMerged.begin(); pElItr != lepTPELtracksMerged.end(); ++pElItr) {
            for(mElItr=pElItr+1; mElItr != lepTPELtracksMerged.end(); ++mElItr) {
                auto pTp = **pElItr;
                auto mTp = **mElItr;
                ATH_MSG(DEBUG) << "Try to build lep pair from lep1 " << *(*pElItr) << ", lep2 " << *(*mElItr) << endmsg;
                
                // check that we have two different lep tracks
                if (pTp==mTp) {
                    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Lep tracks are same" << endmsg;
                    m_mon_Errors.push_back(ERROR_SameLep);
                    continue;
                }
                m_countPassedEEID++;
                
                // check if leps have opposite signs
                if( m_oppositeCharge && (mTp->charge())*(pTp->charge()) > 0) {
                    ATH_MSG(DEBUG) << "Lep pair rejected by opposite change check: lep1 " << pTp->charge() << ", lep2 " << mTp->charge() << endmsg;
                    continue;
                }
                m_countPassedEEOS++;
                
                // check if dilep mass within the range
                //float massEE = XMass(*pItr,*mItr,di_to_leps);
                float massEE = XMass(pTp,mTp,di_to_electrons);
                if( massEE < m_lowerEEMassCut || massEE > m_upperEEMassCut ) {
                    ATH_MSG(DEBUG) << "Lep pair rejected by mass cut: m = " << massEE << endmsg;
                    continue;
                }
                m_countPassedEEMass++;
		ATH_MSG(DEBUG) << "Lep pair accepted by mass cut: m = " << massEE << endmsg;
                
                if ( timerSvc() ) m_VtxFitTimer->resume();
                //xAOD::TrigBphys * trigEEObj = new xAOD::TrigBphys;
                std::unique_ptr<xAOD::TrigBphys> trigEEUniquePtr(new xAOD::TrigBphys);
                trigEEUniquePtr->makePrivateStore();
                trigEEUniquePtr->initialise(0,0.,0.,0.,
                                              xAOD::TrigBphys::BMUMUX, massEE, xAOD::TrigBphys::EF);
                std::vector<ElementLink<xAOD::TrackParticleContainer> > vec_mumu_tracks = {*pElItr,*mElItr};
                std::vector<double> masses_mumu = {EMASS,EMASS};
                if (!m_bphysHelperTool->vertexFit(trigEEUniquePtr.get(),
                                                  vec_mumu_tracks,masses_mumu).isSuccess()) {
                    ATH_MSG(DEBUG) << " Vertex fitting failed." << endmsg;
                    if ( timerSvc() ) m_VtxFitTimer->pause();
                    continue;
                }
                if ( timerSvc() ) m_VtxFitTimer->pause();
                
                
                m_countPassedEEVtx++;
                
                float chi2EE = trigEEUniquePtr->fitchi2();
                if(chi2EE > m_VtxChi2Cut || chi2EE < 0) {
                    ATH_MSG(DEBUG) << "Lep pair rejected by chi2 cut: chi2 = " << chi2EE << endmsg;
                    continue;
                }
                m_countPassedEEVtxChi2++;
                
                
                // Fill leps monitoring containers
                m_mon_DiE_Pt_Lep1.push_back (pTp->pt()/1000.);
                m_mon_DiE_Pt_Lep2.push_back (mTp->pt()/1000.);
                m_mon_DiE_Eta_Lep1.push_back(pTp->eta());
                m_mon_DiE_Eta_Lep2.push_back(mTp->eta());
                m_mon_DiE_Phi_Lep1.push_back(pTp->phi());
                m_mon_DiE_Phi_Lep2.push_back(mTp->phi());
                
                m_mon_DiE_dEtaEE.push_back( fabs(pTp->eta() - mTp->eta()) );
                float tmp_DiE_dPhiEE = roiDescriptor1->phi() - roiDescriptor2->phi();
                while (tmp_DiE_dPhiEE >  M_PI) tmp_DiE_dPhiEE -= 2*M_PI;
                while (tmp_DiE_dPhiEE < -M_PI) tmp_DiE_dPhiEE += 2*M_PI;
                m_mon_DiE_dPhiEE.push_back( fabs(tmp_DiE_dPhiEE) );
                m_mon_DiE_pTsumEE.push_back( (pTp->pt() + mTp->pt())/1000. );
                m_mon_DiE_InvMassEE.push_back(massEE/1000.);
                m_mon_DiE_VtxMassEE.push_back(trigEEUniquePtr->fitmass()/1000.);
                m_mon_DiE_Chi2EE.push_back(chi2EE);
                
                m_mon_DiE_n++;
                
                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Dilep Sec Vtx at ("
                    << trigEEUniquePtr->fitx() << " , "
                    << trigEEUniquePtr->fity() << " , "
                    << trigEEUniquePtr->fitz()  <<  ") with chi2 = "
                    << trigEEUniquePtr->fitchi2() << " ("
                    << trigEEUniquePtr->fitndof() << "  dof)"
                    << " Mass= "<< massEE << endmsg;

                // JK look for tracks in TrackParticle containers and add to TrigEFBphys
                //
                // TODO
                // Doesn't work now: track pointers *pItr, *mItr are not the same as in vectorTrkParticleCollections elements
                //
                bool Found1Track=false;
                bool Found2Track=false;
                
                // test for new method of track retrieval
                //const xAOD::TrackParticleContainer *tracksRoiI1(nullptr);
                //const xAOD::TrackParticleContainer *tracksRoiI2(nullptr);
                // Sergey S.: Retrieve tracks as ELs
                ElementLinkVector<xAOD::TrackParticleContainer> tracksRoiI1;
                ElementLinkVector<xAOD::TrackParticleContainer> tracksRoiI2;
                //get the tracks for each roi
                //HLT::ErrorCode status = getFeature(inputTE.front(), tracksRoiI1); // Need to insert the correct label
                HLT::ErrorCode status = getFeaturesLinks< xAOD::TrackParticleContainer, xAOD::TrackParticleContainer > (inputTE.front(), tracksRoiI1, m_input_trackCollectionKey);
                if(status != HLT::OK) {
                    msg() << MSG::WARNING << "Failed to get xAOD::TrackParticleCollection from the 1st trigger element" << endmsg;
                    m_mon_Errors.push_back(ERROR_No_TrackColl);
                    delete m_TrigBphysColl_b;
                    delete m_TrigBphysColl_X;
                    m_TrigBphysColl_b = NULL;
                    m_TrigBphysColl_X = NULL;
                    return HLT::MISSING_FEATURE;
                }
                //status = getFeature(inputTE.back() , tracksRoiI2); // Need to insert the correct label
                status = getFeaturesLinks< xAOD::TrackParticleContainer, xAOD::TrackParticleContainer > (inputTE.back(), tracksRoiI2, m_input_trackCollectionKey);
                if(status != HLT::OK) {
                    msg() << MSG::WARNING << "Failed to get xAOD::TrackParticleCollection from the 2nd trigger element" << endmsg;
                    m_mon_Errors.push_back(ERROR_No_TrackColl);
                    delete m_TrigBphysColl_b;
                    delete m_TrigBphysColl_X;
                    m_TrigBphysColl_b = NULL;
                    m_TrigBphysColl_X = NULL;
                    if ( timerSvc() ) m_TotTimer->stop();
                    return HLT::MISSING_FEATURE;
                }
                ATH_MSG(DEBUG) << "Ntracks RoI1: " << ( tracksRoiI1.empty() ? -1 : tracksRoiI1.size() ) << endmsg;
                ATH_MSG(DEBUG) << "Ntracks RoI2: " << ( tracksRoiI2.empty() ? -1 : tracksRoiI2.size() ) << endmsg;
                // JW if containers are empty - it's not necessarily an issue; right?
                
                std::vector<const xAOD::TrackParticle*> merged_tracks;
                merged_tracks.reserve(tracksRoiI1.size() + tracksRoiI2.size());
                
                // in order to use the element links, create a map of the particle pointer, and the corresponding container, and index
                struct ElIndex{
                    int roi;
                    int index;
                    ElementLink<xAOD::TrackParticleContainer> elLink;
                };
                std::map<const xAOD::TrackParticle*, ElIndex> mapTrackToIndex;
                std::map<const xAOD::TrackParticle*, ElementLink<xAOD::TrackParticleContainer> > mapTrackToEL;
                
                int idCounter(0);
                for (auto trk: tracksRoiI1) {
                    // merged_tracks.push_back(trk);
                    addUnique(merged_tracks,*trk);
                    ElIndex tmp;
                    tmp.roi = 1;
                    tmp.index = idCounter;
                    tmp.elLink = trk;
                    mapTrackToIndex.insert( std::make_pair(*trk,tmp));
                    ++idCounter;
                }
                idCounter = 0;
                for (auto trk: tracksRoiI2) {
                    // merged_tracks.push_back(trk);
                    addUnique(merged_tracks,*trk);
                    ElIndex tmp;
                    tmp.roi = 2;
                    tmp.index = idCounter;
                    tmp.elLink = trk;
                    mapTrackToIndex.insert(std::make_pair(*trk,tmp));
                    ++idCounter;
                }
                
                if(msgLvl() <= MSG::DEBUG) {
                    msg() << MSG::DEBUG << "Merged Counts: " << merged_tracks.size() << "With merged tracks:\n";
                    for (auto trk: merged_tracks) {
                        msg() << MSG::DEBUG << trk << " TrackParticle: pt/eta/phi/q/d0/z0: "
                        << trk->pt() << " , " << trk->eta() << " , " << trk->phi() << " , " << trk->charge() << " , "
                        << trk->d0() << " , " << trk->z0()<<  "\n";
                        if (trk->track()) {
                          if(trk->track()->perigeeParameters()) {
                            int qsign = (trk->track()->perigeeParameters()->parameters()[Trk::qOverP] > 0 ? +1 : -1);
                            msg() << MSG::DEBUG << trk->track() << " and Trk::Track: pt/eta/phi/q/d0/z0: "
                            << trk->track()->perigeeParameters()->pT()  << " , "
                            << trk->track()->perigeeParameters()->eta() << " , "
                            << trk->track()->perigeeParameters()->parameters()[Trk::phi0] << " , "
                            << qsign << " , "
                            << trk->track()->perigeeParameters()->parameters()[Trk::d0] << " , "
                            << trk->track()->perigeeParameters()->parameters()[Trk::z0] << "\n";
                          } else {
                            msg() << MSG::DEBUG << "Trk::Track with no perigeeParameters\n";
                          }
                        } else {
                            msg() << MSG::DEBUG <<  "   no Trk::Track\n" ;
                        }
                    }
                    msg() << MSG::DEBUG<< endmsg;
                } // if debug
                
                
                // loop to find the lep associated withthe trk::track. can be optimised
                //                     int piTrk(-1),miTrk(-1);
                // JW just make a reference to the El-link, for simplicity of code updation.
                ElementLink<xAOD::TrackParticleContainer> &trackELlep1 = *pElItr;
                ElementLink<xAOD::TrackParticleContainer> &trackELlep2 = *mElItr;
                ElementLink<xAOD::IParticleContainer> ELlep1;
                ElementLink<xAOD::IParticleContainer> ELlep2;
                auto trkmuit = mapTrkToLeps.find(*trackELlep1);
                if (trkmuit != mapTrkToLeps.end()) {
                    //trackELlep1 = (*(trkmuit->second))->inDetTrackParticleLink();
                    ELlep1.resetWithKeyAndIndex(trkmuit->second.dataID(),trkmuit->second.index());
                    Found1Track = true;
                } // if
                trkmuit = mapTrkToLeps.find(*trackELlep2);
                if (trkmuit != mapTrkToLeps.end()) {
                    //trackELlep2 = (*(trkmuit->second))->inDetTrackParticleLink();
                    ELlep2.resetWithKeyAndIndex(trkmuit->second.dataID(),trkmuit->second.index());
                    Found2Track = true;
                } // if
                  //                ELlep1.resetWithKeyAndIndex(pTp->dataID(),pTp->index());
                  //                Found1Track = true;
                  //                ELlep2.resetWithKeyAndIndex(mTp->dataID(),mTp->index());
                  //                Found2Track = true;
                
                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Matching summary: " << Found1Track<<Found2Track << endmsg;
                
                
                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Now loop over TrackParticles to find tracks " << endmsg;
                //                    xAOD::TrackParticleContainer::const_iterator trkIt =  merged_tracks.begin();
                //                    xAOD::TrackParticleContainer::const_iterator lastTrkIt = merged_tracks.end();
                //std::vector<const xAOD::TrackParticle*>::const_iterator trkIt     = merged_tracks.begin();
                std::vector<const xAOD::TrackParticle*>::const_iterator lastTrkIt = merged_tracks.end();
                
                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Found tracks, ntrack= " << merged_tracks.size() << endmsg;
                
                
                // Main outer loop over tracks
                //xAOD::TrackParticleContainer::const_iterator trkIt1 =  merged_tracks.begin();
                std::vector<const xAOD::TrackParticle*>::const_iterator trkIt1     = merged_tracks.begin();
                
                for (int itrk1=0 ; trkIt1 != lastTrkIt; itrk1++, trkIt1++)
                {
                    //const Trk::Track* track1 = (*trkIt1)->track();
                    const xAOD::TrackParticle* track1 = (*trkIt1);
                    
//                    ElementLink<xAOD::TrackParticleContainer> trackEL3( mapTrackToIndex[*trkIt1].roi == 1 ? tracksRoiI1[0].dataID() : tracksRoiI2[0].dataID(), mapTrackToIndex[*trkIt1].index);
//                    ElementLink<xAOD::IParticleContainer> ItrackEL3;
//                    ItrackEL3.resetWithKeyAndIndex(mapTrackToIndex[*trkIt1].roi == 1 ? tracksRoiI1[0].dataID() : tracksRoiI2[0].dataID(), mapTrackToIndex[*trkIt1].index);
                    
                    ElementLink<xAOD::TrackParticleContainer> & trackEL3 = mapTrackToIndex[*trkIt1].elLink;
                    ElementLink<xAOD::IParticleContainer> ItrackEL3;
                    ItrackEL3.resetWithKeyAndIndex(trackEL3.dataID(), trackEL3.index());

                    if (*trackEL3 != track1) {
                        ATH_MSG(FATAL) << "Track 1 doesn't match dereferenced elementlink: " << track1 << " " << *trackEL3 << endmsg;
                        if ( timerSvc() ) m_TotTimer->stop();
                        return HLT::MISSING_FEATURE;
                    }
                    
                    // Check that it is not lep track
                    //                         if(itrk1==piTrk || itrk1==miTrk) {
                    //                             ATH_MSG(DEBUG) << "Track " << track1 << " was matched to a lep, skip it" << endmsg;
                    //                             continue;
                    //                         }
                    
                    // ST: EL comparison does not work -- they appear to be always different
                    //                         if(trackEL3 == trackELlep1 || trackEL3 == trackELlep2 ) {
                    //                             ATH_MSG(DEBUG) << "Track " << track1 << " was matched to a lep, skip it" << endmsg;
                    //                             continue;
                    //                         }
                    if( !(Found1Track && isUnique(*trkIt1,*trackELlep1)) || !(Found2Track &&isUnique(*trkIt1,*trackELlep2)) ) {
                        ATH_MSG(DEBUG) << "Track " << track1 << " was matched to a lep, skip it" << endmsg;
                        continue;
                    }
                    
                    // Fill tracks monitoring containers
                    m_mon_Tracks_Pt.push_back (track1->pt()/1000.);
                    m_mon_Tracks_Eta.push_back(track1->eta());
                    m_mon_Tracks_Phi.push_back(track1->phi());
                    
                    m_mon_Tracks_n++;
                    
                    //
                    // B+ -> mu mu K+ part
                    //
                    if(m_doB_KEEDecay) {
                        xAOD::TrigBphys* trigPartBplusEEKplus = checkBplusEEKplus(trackEL3,trackELlep1,trackELlep2);
                        ++nTriedCombinations;
                        if (trigPartBplusEEKplus) {
                            m_bphysHelperTool->setBeamlineDisplacement(trigPartBplusEEKplus,{*trackEL3,*trackELlep1,*trackELlep2});
                            if(m_maxBpToStore >= 0 && m_countBpToStore >= m_maxBpToStore) {
                              if(m_countBpToStore == m_maxBpToStore) {
                                ATH_MSG(WARNING) << "Reached maximum number of B+ candidates to store " << m_maxBpToStore << "; following candidates won't be written out" << endmsg;
                                m_mon_Errors.push_back(ERROR_MaxNumBpReached);
                              }
                              else
                                ATH_MSG(DEBUG) << "Do not write out " << m_countBpToStore+1 << "th B+ candidate" << endmsg;
                              delete trigPartBplusEEKplus;
                            }
                            else {
                              m_TrigBphysColl_b->push_back(trigPartBplusEEKplus);
                              // trackParticleLinks are set by the vertexing method
                              trigPartBplusEEKplus->addParticleLink(ItrackEL3);				// Sergey S.
                              if(Found1Track) trigPartBplusEEKplus->addParticleLink(ELlep1);
                              if(Found2Track) trigPartBplusEEKplus->addParticleLink(ELlep2);
                            }
                            result=true;
                            m_mon_BEEK_n++;
                            m_countBpToStore++;
                            if(IdEvent!=m_lastEventPassedBplus) {
                                m_countPassedEventsBplus++;
                                m_lastEventPassedBplus = IdEvent;
                            }
                            
                        }
                    } //  end if(m_doB_KEEDecay)
                    
                    
                    // Protection
                    if( nTriedCombinations > m_maxNcombinations ) {
                        ATH_MSG(DEBUG) << "Too many track combinations: " << endmsg;
                        ATH_MSG(DEBUG) << "  nTriedCombinations = " << nTriedCombinations
                        << ", while MaxNcombinations = " << m_maxNcombinations << endmsg;
                        ATH_MSG(DEBUG) << "Terminate the loop" << endmsg;
                        ATH_MSG(DEBUG) << "  Bphys Collection size is " << m_TrigBphysColl_b->size() << endmsg;
                        if(m_TrigBphysColl_b->size() > 0)
                            m_mon_Errors.push_back(ERROR_TooManyComb_Acc);
                        else
                            m_mon_Errors.push_back(ERROR_TooManyComb_Rej);
                        break;
                    }
                    
                    // Inner loop over tracks
                    //xAOD::TrackParticleContainer::const_iterator trkIt2 = trkIt1 + 1;
                    std::vector<const xAOD::TrackParticle*>::const_iterator trkIt2 = trkIt1 + 1;
                    for (int itrk2=itrk1+1 ; trkIt2 != lastTrkIt; itrk2++, trkIt2++)
                    {
                        //If timeout has happened in spite of the protection
                        if ( Athena::Timeout::instance().reached() ) {
                            return HLT::ErrorCode(HLT::Action::ABORT_EVENT, HLT::Reason::TIMEOUT);
                        }
                        
			if(m_doBd_KstarEEDecay || m_doBs_Phi1020EEDecay ) {
                                //const Trk::Track* track2 = (*trkIt2)->track();
                                const xAOD::TrackParticle* track2 = (*trkIt2);

                            // Sergey S.
                            //                                ElementLink<xAOD::TrackParticleContainer> trackEL4( mapTrackToIndex[*trkIt2].roi == 1 ? tracksRoiI1[0].dataID() : tracksRoiI2[0].dataID(), mapTrackToIndex[*trkIt2].index);
                            //                                ElementLink<xAOD::IParticleContainer> ItrackEL4;
                            //                                ItrackEL4.resetWithKeyAndIndex(mapTrackToIndex[*trkIt2].roi == 1 ? tracksRoiI1[0].dataID() : tracksRoiI2[0].dataID(), mapTrackToIndex[*trkIt2].index);
                            
                            ElementLink<xAOD::TrackParticleContainer> & trackEL4 = mapTrackToIndex[*trkIt2].elLink;
                            ElementLink<xAOD::IParticleContainer> ItrackEL4;
                            ItrackEL4.resetWithKeyAndIndex(trackEL4.dataID(), trackEL4.index());

                            
                            if (*trackEL4 != track2) {
                                ATH_MSG(FATAL) << "Track 2 doesn't match dereferenced elementlink: " << track2 << " " << *trackEL4 << endmsg;
                                if ( timerSvc() ) m_TotTimer->stop();
                                return HLT::MISSING_FEATURE;
                            }

                            
                                // Check that it is not lep track
//                                 if(itrk2==piTrk || itrk2==miTrk) {
//                                     ATH_MSG(DEBUG) << "Track " << track2 << " was matched to a lep, skip it" << endmsg;
//                                     continue;
//                                 }
                                if( !(Found1Track && isUnique(*trkIt2,*trackELlep1)) || !(Found2Track &&isUnique(*trkIt2,*trackELlep2)) ) {
                                    ATH_MSG(DEBUG) << "Track " << track2 << " was matched to a lep, skip it" << endmsg;
                                    continue;
                                }
                                
                                // check that track1 and track2 have opposite charges
				//                                if( (track1->charge())*(track2->charge()) > 0) {
				//                                    ATH_MSG(DEBUG) << "Track pair rejected by opposite charge check: "
				//                                    << track1 << " = " << track1->charge() << ",  "
				//                                    << track2 << " = " << track2->charge() << endmsg;
				//                                    continue;
				//                                }
                                
                                //
                                // Bd -> mu mu K0* (K+ pi-) part
                                //
                                if(m_doBd_KstarEEDecay) {

				  // check that track1 and track2 have opposite charges
				  if( (track1->charge())*(track2->charge()) > 0) {      
				     ATH_MSG(DEBUG) << "Track pair rejected by opposie charge check: "
                                     << track1 << " = " << track1->charge() << ",  "   
                                     << track2 << " = " << track2->charge() << endmsg;
				  } else {

                                    // Mass hypothesis 1: track1 = kaon, track2 = pion
                                    xAOD::TrigBphys* xaod_trigPartKstar(nullptr);
                                    xAOD::TrigBphys* xaod_trigPartBdEEKstar = checkBEE2X(trackEL3,trackEL4,trackELlep1,trackELlep2,
                                                                                             bD_to_Kstar,xaod_trigPartKstar);
                                    ++nTriedCombinations;
                                    if (xaod_trigPartBdEEKstar) {
                                        m_bphysHelperTool->setBeamlineDisplacement(xaod_trigPartBdEEKstar,{*trackEL3,*trackEL4,*trackELlep1,*trackELlep2});
                                        m_bphysHelperTool->setBeamlineDisplacement(xaod_trigPartKstar,      {*trackEL3,*trackEL4});
                                        
                                        if(m_maxBdToStore >= 0 && m_countBdToStore >= m_maxBdToStore) {
                                          if(m_countBdToStore == m_maxBdToStore) {
                                            ATH_MSG(WARNING) << "Reached maximum number of Bd candidates to store " << m_maxBdToStore << "; following candidates won't be written out" << endmsg;
                                            m_mon_Errors.push_back(ERROR_MaxNumBdReached);
                                          }
                                          else
                                            ATH_MSG(DEBUG) << "Do not write out " << m_countBdToStore+1 << "th Bd candidate" << endmsg;
                                          delete xaod_trigPartBdEEKstar;
                                          delete xaod_trigPartKstar;
                                        }
                                        else {
                                          m_TrigBphysColl_b->push_back(xaod_trigPartBdEEKstar);
                                          m_TrigBphysColl_X->push_back(xaod_trigPartKstar );
                                          // trackParticleLinks are set by the vertexing method
                                          xaod_trigPartBdEEKstar->addParticleLink(ItrackEL3);				// Sergey S.
                                          xaod_trigPartBdEEKstar->addParticleLink(ItrackEL4);
                                          if(Found1Track) xaod_trigPartBdEEKstar->addParticleLink(ELlep1);
                                          if(Found2Track) xaod_trigPartBdEEKstar->addParticleLink(ELlep2);
                                          
                                          xaod_trigPartKstar->addParticleLink(ItrackEL3);				// Sergey S.
                                          xaod_trigPartKstar->addParticleLink(ItrackEL4);

                                          // set the secondary link; note, does not set correctly for persistified data.
                                          // see code further down for the re-setting of these links
                                          int iKstar = m_TrigBphysColl_X->size() - 1;
                                          ElementLink<xAOD::TrigBphysContainer> trigPartXEL(*m_TrigBphysColl_X,iKstar);
                                          xaod_trigPartBdEEKstar->setSecondaryDecayLink(trigPartXEL);
                                        }
                                        result=true;
                                        m_mon_BdEEKs_n++;
                                        m_countBdToStore++;
                                        if(IdEvent!=m_lastEventPassedBplus) {
                                            m_countPassedEventsBplus++;
                                            m_lastEventPassedBplus = IdEvent;
                                        }

                                    } // if hypo1
                                    xaod_trigPartKstar = nullptr; //hypo 1 added to SG(if exists), so reuse the pointer
                                    
                                    // Mass hypothesis 2: track1 = pion, track2 = kaon
                                    xaod_trigPartBdEEKstar = checkBEE2X(trackEL4,trackEL3,trackELlep1,trackELlep2,
                                                                            bD_to_Kstar,xaod_trigPartKstar);
                                    ++nTriedCombinations;
                                    if (xaod_trigPartBdEEKstar) {
                                        m_bphysHelperTool->setBeamlineDisplacement(xaod_trigPartBdEEKstar,{*trackEL4,*trackEL3,*trackELlep1,*trackELlep2});
                                        m_bphysHelperTool->setBeamlineDisplacement(xaod_trigPartKstar,      {*trackEL4,*trackEL3});
                                        if(m_maxBdToStore >= 0 && m_countBdToStore >= m_maxBdToStore) {
                                          if(m_countBdToStore == m_maxBdToStore) {
                                            ATH_MSG(WARNING) << "Reached maximum number of Bd candidates to store " << m_maxBdToStore << "; following candidates won't be written out" << endmsg;
                                            m_mon_Errors.push_back(ERROR_MaxNumBdReached);
                                          }
                                          else
                                            ATH_MSG(DEBUG) << "Do not write out " << m_countBdToStore+1 << "th Bd candidate" << endmsg;
                                          delete xaod_trigPartBdEEKstar;
                                          delete xaod_trigPartKstar;
                                        }
                                        else {
                                          m_TrigBphysColl_b->push_back(xaod_trigPartBdEEKstar);
                                          m_TrigBphysColl_X->push_back(xaod_trigPartKstar );
                                          // trackParticleLinks are set by the vertexing method
                                          xaod_trigPartBdEEKstar->addParticleLink(ItrackEL4);				// Sergey S.
                                          xaod_trigPartBdEEKstar->addParticleLink(ItrackEL3);
                                          if(Found1Track) xaod_trigPartBdEEKstar->addParticleLink(ELlep1);
                                          if(Found2Track) xaod_trigPartBdEEKstar->addParticleLink(ELlep2);
                                          
                                          xaod_trigPartKstar->addParticleLink(ItrackEL4);				// Sergey S.
                                          xaod_trigPartKstar->addParticleLink(ItrackEL3);
                                          
                                          // set the secondary link; note, does not set correctly for persistified data.
                                          // see code further down for the re-setting of these links
                                          int iKstar = m_TrigBphysColl_X->size() - 1;
                                          ElementLink<xAOD::TrigBphysContainer> trigPartXEL(*m_TrigBphysColl_X,iKstar);
                                          xaod_trigPartBdEEKstar->setSecondaryDecayLink(trigPartXEL);
                                        }
                                        result=true;
                                        m_mon_BdEEKs_n++;
                                        m_countBdToStore++;
                                        if(IdEvent!=m_lastEventPassedBplus) {
                                            m_countPassedEventsBplus++;
                                            m_lastEventPassedBplus = IdEvent;
                                        }
                                        
                                    } // if hypo2
                                   } // end check that track1 and track2 have opposite charges
                                } // if m_doBd_KstarEEDecay
                                
                                
                                //
                                // Bs -> mu mu Phi (K K) part
                                //
                                if(m_doBs_Phi1020EEDecay) {

				  // check that track1 and track2 have opposite charges
				  if( (track1->charge())*(track2->charge()) > 0) {      
				     ATH_MSG(DEBUG) << "Track pair rejected by opposie charge check: "
                                     << track1 << " = " << track1->charge() << ",  "   
                                     << track2 << " = " << track2->charge() << endmsg;
				  } else {

                                    xAOD::TrigBphys* xaod_trigPartPhi(nullptr);
                                    xAOD::TrigBphys* xaod_trigPartBsEEPhi = checkBEE2X(trackEL3,trackEL4,trackELlep1,trackELlep2,
                                                                                           bS_to_Phi,xaod_trigPartPhi);

                                    ++nTriedCombinations;
                                    if (xaod_trigPartBsEEPhi) {
                                        m_bphysHelperTool->setBeamlineDisplacement(xaod_trigPartBsEEPhi,{*trackEL3,*trackEL4,*trackELlep1,*trackELlep2});
                                        m_bphysHelperTool->setBeamlineDisplacement(xaod_trigPartPhi,        {*trackEL3,*trackEL4});
                                        
                                        if(m_maxBsToStore >= 0 && m_countBsToStore >= m_maxBsToStore) {
                                          if(m_countBsToStore == m_maxBsToStore) {
                                            ATH_MSG(WARNING) << "Reached maximum number of Bs candidates to store " << m_maxBsToStore << "; following candidates won't be written out" << endmsg;
                                            m_mon_Errors.push_back(ERROR_MaxNumBsReached);
                                          }
                                          else
                                            ATH_MSG(DEBUG) << "Do not write out " << m_countBsToStore+1 << "th Bs candidate" << endmsg;
                                          delete xaod_trigPartBsEEPhi;
                                          delete xaod_trigPartPhi;
                                        }
                                        else {
                                          m_TrigBphysColl_b->push_back(xaod_trigPartBsEEPhi);
                                          m_TrigBphysColl_X->push_back(xaod_trigPartPhi );
                                          // trackParticleLinks are set by the vertexing method
                                          xaod_trigPartBsEEPhi->addParticleLink(ItrackEL3);				// Sergey S.
                                          xaod_trigPartBsEEPhi->addParticleLink(ItrackEL4);
                                          if(Found1Track) xaod_trigPartBsEEPhi->addParticleLink(ELlep1);
                                          if(Found2Track) xaod_trigPartBsEEPhi->addParticleLink(ELlep2);
                                          
                                          xaod_trigPartPhi->addParticleLink(ItrackEL3);				// Sergey S.
                                          xaod_trigPartPhi->addParticleLink(ItrackEL4);
                                          
                                          // set the secondary link; note, does not set correctly for persistified data.
                                          // see code further down for the re-setting of these links
                                          int iPhi = m_TrigBphysColl_X->size() - 1;
                                          ElementLink<xAOD::TrigBphysContainer> trigPartXEL(*m_TrigBphysColl_X,iPhi);
                                          xaod_trigPartBsEEPhi->setSecondaryDecayLink(trigPartXEL);
                                        }
                                        result=true;
                                        m_mon_BsEEPhi_n++;
                                        m_countBsToStore++;
                                        if(IdEvent!=m_lastEventPassedBplus) {
                                            m_countPassedEventsBplus++;
                                            m_lastEventPassedBplus = IdEvent;
                                        }
                                        
                                        
                                    }
                                   } // end check that track1 and track2 have opposite charges
                                } // m_doBs_Phi1020EEDecay
                                

                            } // end if(m_doBd_KstarEEDecay || m_doBs_Phi1020EEDecay || m_doLb_LambdaEEDecay)
                    } // end of inner loop over tracks 2
                    
                } // end of outer loop over tracks 1
            } // end of inner lep loop
        } // end of outer lep loop
    } // end if muidIDtracks1,2 sizes >= 0
    
    if (msgLvl() <= MSG::DEBUG) msg()  << MSG::DEBUG << "Totally tried to build " << nTriedCombinations << " tracks cobinations" << endmsg;
    m_mon_nTriedCombinations = nTriedCombinations;
    
//     std::string  KEY; //sivokl
    
    if(m_TrigBphysColl_b!=0 && m_TrigBphysColl_b->size()>0) {
        if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "REGTEST: Store Bphys Collection size: " << m_TrigBphysColl_b->size() << endmsg;
        HLT::ErrorCode sc = attachFeature(outputTE, m_TrigBphysColl_X, "EFBEEXFex_X" );
        if(sc != HLT::OK) {
            msg() << MSG::WARNING << "Failed to store trigBphys_X Collection in outputTE" << endmsg;
            m_mon_Errors.push_back(ERROR_BphysCollStore_Fails);
            delete m_TrigBphysColl_b;
            delete m_TrigBphysColl_X;
            if ( timerSvc() ) m_TotTimer->stop();
            return HLT::ERROR;
        }
        // Sergey S.: get persistified TrigBphysContainer of X decays
        ElementLinkVector<xAOD::TrigBphysContainer> ELvecTBPh;
        sc = getFeaturesLinks< xAOD::TrigBphysContainer, xAOD::TrigBphysContainer > (outputTE, ELvecTBPh, "EFBEEXFex_X");
//         const xAOD::TrigBphysContainer * TrigBphysColl_X_persist(0);
//         sc = getFeature(outputTE, TrigBphysColl_X_persist, "EFBEEXFex_X");
        if(sc != HLT::OK) {
        msg() << MSG::WARNING << "Failed to getFeaturesLinks trigBphys_X Collection in outputTE" << endmsg;
        } else {
            for ( const auto eltp: ELvecTBPh) {
              if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "  ===== TrigBphys Container ElementLinks : " 
              << " index: "  << eltp.index()
              << " sgkey: "  << eltp.dataID()
              << " hashkey: "<< eltp.key()
              << " valid: "  << eltp.isValid()
              << " ptr: "    << (eltp.isValid() ? *eltp : nullptr)
              << endmsg;
//               KEY = eltp.dataID();
            }
        }
        // Sergey S.: reset the secondary decay links to persistified container
        for( xAOD::TrigBphysContainer::iterator BPobj = m_TrigBphysColl_b->begin(); BPobj != m_TrigBphysColl_b->end(); ++BPobj) {
            if ((*BPobj)->secondaryDecayLink().isValid() ) {
              ElementLink<xAOD::TrigBphysContainer> secEL;
//               secEL.resetWithKeyAndIndex(KEY,(*BPobj)->secondaryDecayLink().index());
              // match transient secondary decay ELs with those from persistified container
              for(auto persistentSecEL : ELvecTBPh ) {
                if(*persistentSecEL == *(*BPobj)->secondaryDecayLink())
                  secEL = persistentSecEL;
              }
              if(!secEL.isValid())
                msg() << MSG::WARNING << "Failed to find persistified secondary decay EL" << endmsg;
              else
                (*BPobj)->setSecondaryDecayLink(secEL);
              
              if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "  ===== TrigBphys secondaryDecay ElementLinks AFTER reset: "
              << "S link isValid = " << (*BPobj)->secondaryDecayLink().isValid() << std::endl
              << "S link  dataID = " << (*BPobj)->secondaryDecayLink().dataID() << std::endl
              << "S link   index = " << (*BPobj)->secondaryDecayLink().index() << std::endl
              << "S link     key = " << (*BPobj)->secondaryDecayLink().key() << std::endl
              << "S link    cptr = " << (*BPobj)->secondaryDecayLink().cptr() << std::endl
              << endmsg;
            }
        }



	        
        sc = attachFeature(outputTE, m_TrigBphysColl_b, m_bphysCollectionKey );
        if(sc != HLT::OK) {
            msg() << MSG::WARNING << "Failed to store trigBphys Collection in outputTE" << endmsg;
            m_mon_Errors.push_back(ERROR_BphysCollStore_Fails);
            delete m_TrigBphysColl_b;
            delete m_TrigBphysColl_X;
            if ( timerSvc() ) m_TotTimer->stop();
            return HLT::ERROR;
        }
    } else {
        if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "REGTEST: no bphys collection to store "  << endmsg;
        delete m_TrigBphysColl_b;
        delete m_TrigBphysColl_X;
    }
    
    if(result) {
        if(IdEvent!=m_lastEventPassed) {
            m_countPassedEvents++;
            m_lastEventPassed = IdEvent;
        }
        m_countPassedRoIs++;
    }
    
    if ( timerSvc() )  {
        m_TotTimer->stop();
        m_VtxFitTimer->resume();
        m_VtxFitTimer->stop();
        m_mon_TotalRunTime   = m_TotTimer->elapsed();
        m_mon_VertexingTime  = m_VtxFitTimer->elapsed();
    }
    
    if ( timerSvc() ) {
        ATH_MSG(DEBUG) << "m_TotTimer->elapsed()    = " << m_TotTimer->elapsed() << endmsg;
        ATH_MSG(DEBUG) << "m_VtxFitTimer->elapsed() = " << m_VtxFitTimer->elapsed() << endmsg;
    }
    
    return HLT::OK;
    
}


xAOD::TrigBphys* TrigEFBEEXFex::checkBplusEEKplus(const ElementLink<xAOD::TrackParticleContainer> & eltrack1,
                                                      const ElementLink<xAOD::TrackParticleContainer> & ellep1,
                                                      const ElementLink<xAOD::TrackParticleContainer> & ellep2) {
    
    if(msgLvl() <= MSG::DEBUG) {
        ATH_MSG(DEBUG) << "Try to build B+ -> mu mu K+ with track " << *eltrack1 << endmsg;
    }
    float massKEE = KEEMass(*ellep1,*ellep2,*eltrack1);
    
    static std::vector<double> masses = {KPLUSMASS,EMASS,EMASS};
    
    if( !(massKEE > m_lowerKEEMassCut && massKEE < m_upperKEEMassCut) ) {
        ATH_MSG(DEBUG) << " B+ -> mu mu K+ candidate rejected by mass cut: m = " << massKEE << endmsg;
        return nullptr;
    }
    ++m_countPassedBplusMass;
    auto track1 = *eltrack1;
    
    // Fill BEEK monitoring containers
    m_mon_BEEK_Pt_K.push_back(track1->pt()*0.001);
    m_mon_BEEK_Eta_K.push_back(track1->eta());
    m_mon_BEEK_Phi_K.push_back(track1->phi());
    m_mon_BEEK_InvMass_B.push_back(massKEE*0.001);
    
    // create initial object
    xAOD::TrigBphys * result = new xAOD::TrigBphys;
    result->makePrivateStore();
    result->initialise(0,0.,0.,0.,
                       xAOD::TrigBphys::BKMUMU, massKEE, xAOD::TrigBphys::EF);
    
    std::vector<ElementLink<xAOD::TrackParticleContainer> > vec_tracks = {eltrack1,ellep1, ellep2};
    
    if (m_doB_KEEVertexing) {
        
        if ( timerSvc() ) m_VtxFitTimer->resume();
        if (!m_bphysHelperTool->vertexFit(result,vec_tracks,masses).isSuccess()) {
            ATH_MSG(DEBUG) << " Vertex fitting failed." << endmsg;
            if ( timerSvc() ) m_VtxFitTimer->pause();
            delete result;
            return nullptr;
        }
        m_countPassedBplusVtx++;
        
        double chi2val(result->fitchi2());
        
        if (chi2val > m_bVtxChi2Cut || chi2val < 0) {
            ATH_MSG(DEBUG) << " B+ -> mu mu K+ candidate rejected by chi2 cut: chi2 = " << chi2val << endmsg;
            delete result;
            return nullptr;
        }
        m_mon_BEEK_Chi2_B.push_back(chi2val);
        ++m_countPassedBplusVtxChi2;
        
        
        if(msgLvl() <= MSG::DEBUG) {
            msg() << MSG::DEBUG << " Good B+ -> mu mu K+ found!" << std::endl
            << "  m = " << result->mass() << ", "
            << "chi2 = " << result->fitchi2() << ", vertex (" << result->fitx() << ", "
            << result->fity() << ", " << result->fitz() << ")" << endmsg;
        } // if debug
        
        
    } // if m_doB_KEEVertexing
    else {
      // if vertexting is not done - just attach trackParticle links
      for(auto tpel : vec_tracks) {
        result->addTrackParticleLink(tpel);
      }
    }
    
    return result;
} // checkBplusEEKplus

xAOD::TrigBphys* TrigEFBEEXFex::checkBEE2X(const ElementLink<xAOD::TrackParticleContainer> & eltrack1,
                                               const ElementLink<xAOD::TrackParticleContainer> & eltrack2,
                                               const ElementLink<xAOD::TrackParticleContainer> & ellep1,
                                               const ElementLink<xAOD::TrackParticleContainer> & ellep2,
                                               int decay, xAOD::TrigBphys* & trigPartX) {
    trigPartX = nullptr;
    xAOD::TrigBphys * fitVtx_X(nullptr);
    xAOD::TrigBphys * fitVtx(nullptr);
    
    std::vector<double> massHypoX;
    std::vector<double> massHypo;
    double lowerXMassCut, upperXMassCut;
    double lowerXEEMassCut, upperXEEMassCut;
    double chi2XCut;
    double chi2XEECut;
    std::string decayName;
    xAOD::TrigBphys::pType decayType(xAOD::TrigBphys::UNKNOWNPTYPE);
    unsigned int *countPassedXMass;
    unsigned int *countPassedXEEMass;
    unsigned int *countPassedXVtx;
    unsigned int *countPassedXVtxChi2;
    unsigned int *countPassedXEEVtx;
    unsigned int *countPassedXEEVtxChi2;
    
    bool do2XVertexing;
    bool doBEE2XVertexing;
    
    std::vector<float> *pMon_BEE2X_Pt_X1;
    std::vector<float> *pMon_BEE2X_Eta_X1;
    std::vector<float> *pMon_BEE2X_Phi_X1;
    std::vector<float> *pMon_BEE2X_Pt_X2;
    std::vector<float> *pMon_BEE2X_Eta_X2;
    std::vector<float> *pMon_BEE2X_Phi_X2;
    std::vector<float> *pMon_BEE2X_InvMass_2X;
    std::vector<float> *pMon_BEE2X_VtxMass_2X;
    std::vector<float> *pMon_BEE2X_Chi2_2X;
    std::vector<float> *pMon_BEE2X_InvMass_B;
    std::vector<float> *pMon_BEE2X_VtxMass_B;
    std::vector<float> *pMon_BEE2X_Chi2_B;
    
    // Fix silly warning messages
    //    (void)pMon_BEE2X_VtxMass_2X;
    //    (void)pMon_BEE2X_VtxMass_B;
    
    if(decay == bD_to_Kstar) {
        massHypoX = {KPLUSMASS,PIMASS};
        massHypo  = {KPLUSMASS,PIMASS,EMASS,EMASS};
        lowerXMassCut = m_lowerKstar_KaonMassCut;
        upperXMassCut = m_upperKstar_KaonMassCut;
        chi2XCut = m_kStarVtxChi2Cut;
        lowerXEEMassCut = m_lowerBd_KstarEEMassCut;
        upperXEEMassCut = m_upperBd_KstarEEMassCut;
        chi2XEECut = m_bDVtxChi2Cut;
        decayName = "Bd -> mu mu K*";
        decayType = xAOD::TrigBphys::BDKSTMUMU;
        countPassedXMass        = &m_countPassedKstarMass;
        countPassedXEEMass    = &m_countPassedBdMass;
        countPassedXVtx         = &m_countPassedKstarVtx;
        countPassedXVtxChi2     = &m_countPassedKstarVtxChi2;
        countPassedXEEVtx     = &m_countPassedBdVtx;
        countPassedXEEVtxChi2 = &m_countPassedBdVtxChi2;
        do2XVertexing      = m_doKstar_KPiVertexing;
        doBEE2XVertexing = m_doBd_KstarEEVertexing;
        pMon_BEE2X_Pt_X1      = &m_mon_BdEEKs_Pt_K;
        pMon_BEE2X_Eta_X1     = &m_mon_BdEEKs_Eta_K;
        pMon_BEE2X_Phi_X1     = &m_mon_BdEEKs_Phi_K;
        pMon_BEE2X_Pt_X2      = &m_mon_BdEEKs_Pt_Pi;
        pMon_BEE2X_Eta_X2     = &m_mon_BdEEKs_Eta_Pi;
        pMon_BEE2X_Phi_X2     = &m_mon_BdEEKs_Phi_Pi;
        pMon_BEE2X_InvMass_2X = &m_mon_BdEEKs_InvMass_Kstar;
        pMon_BEE2X_VtxMass_2X = &m_mon_BdEEKs_VtxMass_Kstar;
        pMon_BEE2X_Chi2_2X    = &m_mon_BdEEKs_Chi2_Kstar;
        pMon_BEE2X_InvMass_B  = &m_mon_BdEEKs_InvMass_Bd;
        pMon_BEE2X_VtxMass_B  = &m_mon_BdEEKs_VtxMass_Bd;
        pMon_BEE2X_Chi2_B     = &m_mon_BdEEKs_Chi2_Bd;
    } else if(decay == bS_to_Phi) {
        massHypoX = {KPLUSMASS,KPLUSMASS};
        massHypo  = {KPLUSMASS,KPLUSMASS,EMASS,EMASS};
        lowerXMassCut = m_lowerPhi1020_KaonMassCut;
        upperXMassCut = m_upperPhi1020_KaonMassCut;
        chi2XCut = m_phi1020VtxChi2Cut;
        lowerXEEMassCut = m_lowerBs_Phi1020EEMassCut;
        upperXEEMassCut = m_upperBs_Phi1020EEMassCut;
        chi2XEECut = m_bSVtxChi2Cut;
        decayName = "Bs -> mu mu phi";
        decayType = xAOD::TrigBphys::BSPHIMUMU;
        countPassedXMass        = &m_countPassedPhi1020Mass;
        countPassedXEEMass    = &m_countPassedBsMass;
        countPassedXVtx         = &m_countPassedPhi1020Vtx;
        countPassedXVtxChi2     = &m_countPassedPhi1020VtxChi2;
        countPassedXEEVtx     = &m_countPassedBsVtx;
        countPassedXEEVtxChi2 = &m_countPassedBsVtxChi2;
        do2XVertexing      = m_doPhi1020_KKVertexing;
        doBEE2XVertexing = m_doBs_Phi1020EEVertexing;
        pMon_BEE2X_Pt_X1      = &m_mon_BsEEPhi_Pt_K1;
        pMon_BEE2X_Eta_X1     = &m_mon_BsEEPhi_Eta_K1;
        pMon_BEE2X_Phi_X1     = &m_mon_BsEEPhi_Phi_K1;
        pMon_BEE2X_Pt_X2      = &m_mon_BsEEPhi_Pt_K2;
        pMon_BEE2X_Eta_X2     = &m_mon_BsEEPhi_Eta_K2;
        pMon_BEE2X_Phi_X2     = &m_mon_BsEEPhi_Phi_K2;
        pMon_BEE2X_InvMass_2X = &m_mon_BsEEPhi_InvMass_Phi1020;
        pMon_BEE2X_VtxMass_2X = &m_mon_BsEEPhi_VtxMass_Phi1020;
        pMon_BEE2X_Chi2_2X    = &m_mon_BsEEPhi_Chi2_Phi1020;
        pMon_BEE2X_InvMass_B  = &m_mon_BsEEPhi_InvMass_Bs;
        pMon_BEE2X_VtxMass_B  = &m_mon_BsEEPhi_VtxMass_Bs;
        pMon_BEE2X_Chi2_B     = &m_mon_BsEEPhi_Chi2_Bs;
    } else {
        ATH_MSG(DEBUG) << "Wrong decay identifier passed to checkBEE2X: decay = " << decay << endmsg;
        m_mon_Errors.push_back(ERROR_WrongDecayID);
        return nullptr;
    }
     
   ATH_MSG(DEBUG) << "Try to build " << decayName << " with tracks " << *eltrack1 << ", " << *eltrack2 << endmsg;
    
    float massX = XMass(*eltrack1, *eltrack2, decay);
    if( !(massX > lowerXMassCut && massX < upperXMassCut) ) {
        ATH_MSG(DEBUG) << " " << decayName << " candidate rejected by X mass cut: m = " << massX << endmsg;
        trigPartX = nullptr;
        return nullptr;
    }
    
    ++(*countPassedXMass);
    
    float massXEE = XEEMass(*ellep1, *ellep2, *eltrack1, *eltrack2, decay);
    if( !(massXEE > lowerXEEMassCut && massXEE < upperXEEMassCut) ) {
        ATH_MSG(DEBUG) << " " << decayName << " candidate rejected by XEE mass cut: m = " << massXEE << endmsg;
        trigPartX = nullptr;
        return nullptr;
    }
    
    ++(*countPassedXEEMass);
    
    // create initial object
    fitVtx_X = new xAOD::TrigBphys;
    fitVtx_X->makePrivateStore();
    fitVtx_X->initialise(0,0.,0.,0.,
                         decayType, massX, xAOD::TrigBphys::EF);
    fitVtx = new xAOD::TrigBphys;
    fitVtx->makePrivateStore();
    fitVtx->initialise(0,0.,0.,0.,
                       decayType, massXEE, xAOD::TrigBphys::EF);
    
    
    std::vector<ElementLink<xAOD::TrackParticleContainer> > vec_tracksX = {eltrack1,eltrack2};
    std::vector<ElementLink<xAOD::TrackParticleContainer> > vec_tracks  = {eltrack1,eltrack2,ellep1,ellep2};
    
    
    pMon_BEE2X_Pt_X1->push_back ((*eltrack1)->pt()*0.001);
    pMon_BEE2X_Eta_X1->push_back((*eltrack1)->eta());
    pMon_BEE2X_Phi_X1->push_back((*eltrack1)->phi());
    pMon_BEE2X_Pt_X2->push_back ((*eltrack2)->pt()*0.001);
    pMon_BEE2X_Eta_X2->push_back((*eltrack2)->eta());
    pMon_BEE2X_Phi_X2->push_back((*eltrack2)->phi());
    pMon_BEE2X_InvMass_2X->push_back(massX*0.001);
    pMon_BEE2X_InvMass_B->push_back(massXEE/1000.);
    
    
    
    if(do2XVertexing) {
        if ( timerSvc() ) m_VtxFitTimer->resume();
        if (!m_bphysHelperTool->vertexFit(fitVtx_X,vec_tracksX,massHypoX).isSuccess()) {
            ATH_MSG(DEBUG) << " Vertex fitting of X failed." << decayName << endmsg;
        }
        if ( timerSvc() ) m_VtxFitTimer->pause();
        
        ++(*countPassedXVtx);
        double chi2X(fitVtx_X->fitchi2());
        if( chi2X > chi2XCut || chi2X < 0 ) {
            ATH_MSG(DEBUG) << " " << decayName << " candidate rejected by X vertex chi2 cut: chi2 = " << chi2X << endmsg;
            delete fitVtx_X;
            delete fitVtx;
            return nullptr;
        }
        (*countPassedXVtxChi2)++;
        pMon_BEE2X_VtxMass_2X->push_back(fitVtx_X->fitmass()*0.001);
        pMon_BEE2X_Chi2_2X->push_back(chi2X);
    } // do2XVertexing
    else {
      // if vertexting is not done - just attach trackParticle links
      for(auto tpel : vec_tracksX) {
        fitVtx_X->addTrackParticleLink(tpel);
      }
    }
    
    if(doBEE2XVertexing) {
        if ( timerSvc() ) m_VtxFitTimer->resume();
        if (!m_bphysHelperTool->vertexFit(fitVtx,vec_tracks,massHypo).isSuccess()) {
            ATH_MSG(DEBUG) << " Vertex fitting of EEX failed for " << decayName << endmsg;
        }
        if ( timerSvc() ) m_VtxFitTimer->pause();
        (*countPassedXEEVtx)++;
        double chi2EEX(fitVtx->fitchi2());
        if( chi2EEX > chi2XEECut || chi2EEX < 0 ) {
            ATH_MSG(DEBUG) << " " << decayName << " candidate rejected by XEE vertex chi2 cut: chi2 = " << chi2EEX << endmsg;
            delete fitVtx_X;
            delete fitVtx;
            return nullptr;
        }
        (*countPassedXEEVtxChi2)++;
        pMon_BEE2X_VtxMass_B->push_back(fitVtx->fitmass()*0.001);
        pMon_BEE2X_Chi2_B->push_back(chi2EEX);
        
    } // doBEE2XVertexing
    else {
      // if vertexting is not done - just attach trackParticle links
      for(auto tpel : vec_tracks) {
        fitVtx->addTrackParticleLink(tpel);
      }
    }
    
    trigPartX = fitVtx_X;
    return fitVtx;
    
} // checkBEE2X



/*----------------------------------------------------------------------------*/
void TrigEFBEEXFex::addUnique(std::vector<const Trk::Track*>& tracks, const Trk::Track* trkIn)
{
    //  std::cout<<" in addUnique : trkIn pT= "<<trkIn->perigeeParameters()->pT()<<std::endl;
    std::vector<const Trk::Track*>::iterator tItr;
    for( tItr = tracks.begin(); tItr != tracks.end(); tItr++)
    {
        double dPhi=fabs((*tItr)->perigeeParameters()->parameters()[Trk::phi] -
                         trkIn->perigeeParameters()->parameters()[Trk::phi]);
        if (dPhi > M_PI) dPhi = 2.*M_PI - dPhi;
        
        if( fabs(dPhi) < 0.02 &&
           fabs((*tItr)->perigeeParameters()->eta() -
                trkIn->perigeeParameters()->eta()) < 0.02 ) 
        { //std::cout<<" TrigEFBEEFex addUnique: the SAME tracks! pT= "<<
            //trkIn->perigeeParameters()->pT()<<" and "<<
            //(*tItr)->perigeeParameters()->pT()<<std::endl;
            return;
        }
    } 
    tracks.push_back(trkIn);       
}

/*----------------------------------------------------------------------------*/
void TrigEFBEEXFex::addUnique(std::vector<const xAOD::TrackParticle*>& tps, const xAOD::TrackParticle* tpIn)
{
    //  std::cout<<" in addUnique : tpIn pT= "<<tpIn->pt()<<std::endl;
    std::vector<const xAOD::TrackParticle*>::iterator tpItr;
    for( tpItr = tps.begin(); tpItr != tps.end(); tpItr++)
    {
        double dPhi=fabs((*tpItr)->phi() -
                         tpIn->phi());
        if (dPhi > M_PI) dPhi = 2.*M_PI - dPhi;
        
        if( fabs(dPhi) < 0.02 &&
           fabs((*tpItr)->eta() -
                tpIn->eta()) < 0.02  &&
                    tpIn->charge() * (*tpItr)->charge() > 0) 
        { //std::cout<<" TrigEFBEEFex addUnique: the SAME tps! pT= "<<
            //tpIn->pt()<<" and "<<
            //(*tpItr)->pt()<<std::endl;
            return;
        }
    } 
    tps.push_back(tpIn);       
}

void TrigEFBEEXFex::addUnique(std::vector<ElementLink<xAOD::TrackParticleContainer> >& tps, const ElementLink<xAOD::TrackParticleContainer>&tpIn) {
    if (!tpIn.isValid()) {
        return;
    }
    for (auto itr = tps.begin(); itr != tps.end(); ++itr) {
        auto tpel = *itr;
        double dPhi = fabs( (*tpel)->phi() - (*tpIn)->phi() );
        if (dPhi > M_PI) dPhi = 2.*M_PI - dPhi;
 
        if( fabs(dPhi) < 0.02 &&
           fabs((*tpel)->eta() -
                (*tpIn)->eta()) < 0.02  &&
           (*tpIn)->charge() * (*tpel)->charge() > 0) {
            // same parameters
            return;
        }
    } // for
    
    // if here then its unique
    tps.push_back(tpIn);
} // addUnique


/*----------------------------------------------------------------------------*/
bool TrigEFBEEXFex::isUnique(const  xAOD::TrackParticle* id1, const  xAOD::TrackParticle* id2) const {
    if (!id1 || !id2) return false;
    float dEta = fabs( id1->eta() - id2->eta() );
    float dPhi = id1->phi() - id2->phi();
    while  (dPhi >  M_PI) dPhi -= 2*M_PI;
    while  (dPhi < -M_PI) dPhi += 2*M_PI;
    
    if( dEta < 0.02 && fabs(dPhi) < 0.02 && id1->charge() * id2->charge() > 0 ) return false;
    else return true;
    
}






double TrigEFBEEXFex::XMass(const xAOD::TrackParticle* particle1, const xAOD::TrackParticle* particle2, int decay) { /// checking the mass
    
    std::vector<double> massHypo;
    massHypo.clear();
    if(decay == di_to_electrons){
        massHypo.push_back(EMASS);
        massHypo.push_back(EMASS);
    }
    if(decay == bD_to_Kstar){
        massHypo.push_back(KPLUSMASS);
        massHypo.push_back(PIMASS);
    }
    if(decay == bS_to_Phi){
        massHypo.push_back(KPLUSMASS);
        massHypo.push_back(KPLUSMASS);
    }
    if(decay == bC_to_PiPi){  
        massHypo.push_back(PIMASS);
        massHypo.push_back(PIMASS);    
    }
    if(decay == lB_to_L){
        massHypo.push_back(PROTONMASS);
        massHypo.push_back(PIMASS);
    }
    std::vector<const xAOD::TrackParticle*> bTracks;
    bTracks.clear();
    bTracks.push_back(particle1);
    bTracks.push_back(particle2);
    return m_bphysHelperTool->invariantMass(bTracks, massHypo);
    
}

double TrigEFBEEXFex::X3Mass(const xAOD::TrackParticle* particle1, const xAOD::TrackParticle* particle2, const xAOD::TrackParticle* particle3 ) {
    
    std::vector<double> massHypo;
    massHypo.clear();
    massHypo.push_back(KPLUSMASS);
    massHypo.push_back(KPLUSMASS);
    massHypo.push_back(PIMASS);
    std::vector<const xAOD::TrackParticle*> bTracks;
    bTracks.clear();
    bTracks.push_back(particle1);
    bTracks.push_back(particle2);
    bTracks.push_back(particle3);
    return m_bphysHelperTool->invariantMass(bTracks, massHypo);
    
}

double TrigEFBEEXFex::XKPiPiMass(const xAOD::TrackParticle* particle1, const xAOD::TrackParticle* particle2, const xAOD::TrackParticle* particle3 ) {
    
    std::vector<double> massHypo;
    massHypo.clear();
    massHypo.push_back(KPLUSMASS);
    massHypo.push_back(PIMASS);
    massHypo.push_back(PIMASS);
    std::vector<const xAOD::TrackParticle*> bTracks;
    bTracks.clear();
    bTracks.push_back(particle1);
    bTracks.push_back(particle2);
    bTracks.push_back(particle3);
    return m_bphysHelperTool->invariantMass(bTracks, massHypo);
    
}

double TrigEFBEEXFex::KEEMass( const xAOD::TrackParticle* lep1, const xAOD::TrackParticle* lep2, const xAOD::TrackParticle* kaon) {
    std::vector<double> massHypo;
    massHypo.clear();
    massHypo.push_back(EMASS);
    massHypo.push_back(EMASS);
    massHypo.push_back(KPLUSMASS);  //K
    std::vector<const xAOD::TrackParticle*> bTracks;
    bTracks.clear();
    bTracks.push_back(lep1);
    bTracks.push_back(lep2);
    bTracks.push_back(kaon);
    return m_bphysHelperTool->invariantMass(bTracks, massHypo);
}

double TrigEFBEEXFex::XEEMass(const xAOD::TrackParticle* lep1, const xAOD::TrackParticle* lep2, const xAOD::TrackParticle* particle1,
                                  const xAOD::TrackParticle* particle2, int decay){
    std::vector<double> massHypo;
    massHypo.clear();
    massHypo.push_back(EMASS);
    massHypo.push_back(EMASS);
    if(decay == bD_to_Kstar){
        massHypo.push_back(KPLUSMASS);
        massHypo.push_back(PIMASS);
    }
    if(decay == bS_to_Phi){
        massHypo.push_back(KPLUSMASS);
        massHypo.push_back(KPLUSMASS);
    }
    if(decay == bC_to_PiPi){  
        massHypo.push_back(PIMASS);
        massHypo.push_back(PIMASS);    
    }
    if(decay == lB_to_L){
        massHypo.push_back(PROTONMASS);
        massHypo.push_back(PIMASS);
    }
    std::vector<const xAOD::TrackParticle*> bTracks;
    bTracks.clear();
    bTracks.push_back(lep1);
    bTracks.push_back(lep2);
    bTracks.push_back(particle1);
    bTracks.push_back(particle2);
    return m_bphysHelperTool->invariantMass(bTracks, massHypo);
}

double TrigEFBEEXFex::X3EEMass(const xAOD::TrackParticle* lep1, const xAOD::TrackParticle* lep2, const xAOD::TrackParticle* particle1,
                                   const xAOD::TrackParticle* particle2, const xAOD::TrackParticle* particle3 ) {
    std::vector<double> massHypo;
    massHypo.clear();
    massHypo.push_back(EMASS);
    massHypo.push_back(EMASS);
    massHypo.push_back(KPLUSMASS);
    massHypo.push_back(KPLUSMASS);
    massHypo.push_back(PIMASS);
    std::vector<const xAOD::TrackParticle*> bTracks;
    bTracks.clear();
    bTracks.push_back(lep1);
    bTracks.push_back(lep2);
    bTracks.push_back(particle1);
    bTracks.push_back(particle2);
    bTracks.push_back(particle3);
    return m_bphysHelperTool->invariantMass(bTracks, massHypo);
    
}

double TrigEFBEEXFex::XKPiPiEEMass(const xAOD::TrackParticle* lep1, const xAOD::TrackParticle* lep2, const xAOD::TrackParticle* particle1,
                                   const xAOD::TrackParticle* particle2, const xAOD::TrackParticle* particle3 ) {
    std::vector<double> massHypo;
    massHypo.clear();
    massHypo.push_back(EMASS);
    massHypo.push_back(EMASS);
    massHypo.push_back(KPLUSMASS);
    massHypo.push_back(PIMASS);
    massHypo.push_back(PIMASS);
    std::vector<const xAOD::TrackParticle*> bTracks;
    bTracks.clear();
    bTracks.push_back(lep1);
    bTracks.push_back(lep2);
    bTracks.push_back(particle1);
    bTracks.push_back(particle2);
    bTracks.push_back(particle3);
    return m_bphysHelperTool->invariantMass(bTracks, massHypo);
    
}



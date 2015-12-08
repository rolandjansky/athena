// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBPhysHypo/TrigEFBMuMuFex.cxx
 **
 **   Description: EF hypothesis algorithms for B0_s -> mu+ mu-  
 **             
 **************************************************************************/ 
 
#include "TrigEFBMuMuFex.h"
#include "BtrigUtils.h"
#include "TrigBphysHelperUtilsTool.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"

#include <iostream>
//class ISvcLocator;

// additions of xAOD objects
#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

#include "TrkTrack/TrackCollection.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"


TrigEFBMuMuFex::TrigEFBMuMuFex(const std::string & name, ISvcLocator* pSvcLocator):
HLT::ComboAlgo(name, pSvcLocator)
//  m_fitterSvc("Trk::TrkVKalVrtFitter/VertexFitterTool",this),
,m_bphysHelperTool("TrigBphysHelperUtilsTool")
//m_trigBphysColl(NULL),
//m_xAODTrigBphysColl(NULL),
//m_xAODTrigBphysAuxColl(NULL)
,m_BmmHypTot(0),m_BmmHypVtx(0)
,m_expectNumberOfInputTE(2)
,m_massMuon(105.6583715)
,m_muonParticleType(xAOD::Muon::InnerDetectorTrackParticle)
,m_oppositeCharge(true)
,m_sameCharge(false)
,m_lowerMassCut(1000)
,m_upperMassCut(1e6)
,m_ApplyupperMassCut(false)
,m_muonAlgo("TrigMuSuperEF")
,m_SAMuonLabel("MuonEFInfo_ExtrapTrackParticles")
,m_acceptAll(false)
,m_noId(false)
,m_useCombinedTP(false)
,m_considerSameRoiCombinations(true)
//Counters
,m_lastEvent(0)
,m_lastEventPassed(0)
,m_vtxpass(false)
,m_PassedBsMass(false)
,m_countTotalEvents(0)
,m_countTotalRoI(0)
,m_countPassedEvents(0)
,m_countPassedRoIs(0)
,m_countPassedmumuPairsEv(0)
,m_countPassedBsMassEv(0)
,m_countPassedVtxFitEv(0)
,m_countPassedmumuPairs2R(0)
,m_countPassedBsMass2R(0)
,m_countPassedVtxFit2R(0)
{
    
    // Read cuts
    declareProperty("NumberOfInputs",m_expectNumberOfInputTE = 2); // expect two muons by default
    declareProperty("UseRoIs",m_useRoIs = true); // expect two muons by default

    declareProperty("AcceptAll",    m_acceptAll=true);
    declareProperty("OppositeSign", m_oppositeCharge=true);
    declareProperty("SameSign",     m_sameCharge=false);
    declareProperty("LowerMassCut", m_lowerMassCut=2000.0);
    declareProperty("UpperMassCut", m_upperMassCut=10000.0);
    declareProperty("ApplyUpperMassCut", m_ApplyupperMassCut=true);
    //declareProperty("VertexFitterTool", m_fitterSvc);
    declareProperty("MuonAlgo"    , m_muonAlgo="TrigMuSuperEF");
    declareProperty("ExtrapolatedTrackParticleContName", m_SAMuonLabel="MuonEFInfo_ExtrapTrackParticles");
    declareProperty("noId"        , m_noId              = false);
    declareProperty("UseCombinedMuonParameters", m_useCombinedTP     = false);
    declareProperty("ConsiderSameRoiCombinations", m_considerSameRoiCombinations = true);
    //
    declareMonitoredStdContainer("Errors",     mon_Errors,     AutoClear);
    declareMonitoredStdContainer("Acceptance", mon_Acceptance, AutoClear);
    declareMonitoredVariable("DeltaEtaRoIs", mon_dEtaRoI    );
    declareMonitoredVariable("DeltaPhiRoIs", mon_dPhiRoI    );
    declareMonitoredVariable("DeltaEtaMuMu", mon_dEtaMuMu   );
    declareMonitoredVariable("DeltaPhiMuMu", mon_dPhiMuMu   );
    declareMonitoredVariable("pTsumMuMu",    mon_pTsum      );
    declareMonitoredVariable("pTMu1",        mon_mu1pT      );
    declareMonitoredVariable("pTMu2",        mon_mu2pT      );
    declareMonitoredVariable("etaMu1",       mon_mu1eta     );
    declareMonitoredVariable("etaMu2",       mon_mu2eta     );
    declareMonitoredVariable("phiMu1",       mon_mu1phi     );
    declareMonitoredVariable("phiMu2",       mon_mu2phi     );
    declareMonitoredVariable("MuMumass",     mon_MuMumass   );
    declareMonitoredVariable("BmassFit",     mon_BmassFit   );
    declareMonitoredVariable("VtxChi2",      mon_Chi2       );
 
}

TrigEFBMuMuFex::~TrigEFBMuMuFex()
{ }

HLT::ErrorCode TrigEFBMuMuFex::hltInitialize()
{    
    if (msgLvl() <= MSG::DEBUG) {
        msg() << MSG::DEBUG << "Initialization ..." << endreq;
        msg() << MSG::DEBUG << "AcceptAll            = "
        << (m_acceptAll==true ? "True" : "False") << endreq;
        msg() << MSG::DEBUG << "noId          = "
        << (m_noId==true ? "True" : "False") << endreq;
        msg() << MSG::DEBUG << "OppositeCharge       = "
        << (m_oppositeCharge==true ? "True" : "False") << endreq;
        msg() << MSG::DEBUG << "SameCharge       = "
        << (m_sameCharge==true ? "True" : "False") << endreq;
        msg() << MSG::DEBUG << "LowerMassCut         = " << m_lowerMassCut << endreq;
        msg() << MSG::DEBUG << "UpperMassCut         = " << m_upperMassCut << endreq;
        msg() << MSG::DEBUG << "ApplyUpperMassCut         = " << m_ApplyupperMassCut << endreq;
    } // DEBUG
    if ( timerSvc() ) {
        m_BmmHypTot = addTimer("EFBmmHypTot");
        m_BmmHypVtx = addTimer("EFBmmHypVtxFit");
    }
    
    // Consistency check of charge requirements
    if( m_oppositeCharge && m_sameCharge ) {
      msg() << MSG::ERROR << "Bad configuration: OppositeCharge and SameCharge are required together" << endreq;
      return HLT::BAD_JOB_SETUP;
    }
    
    if (m_muonAlgo != "TrigMuSuperEF" ) {
        if ( msgLvl() <= MSG::INFO ) msg() << MSG::INFO <<" Expected algorithm name:  TrigMuSuperEF, but got: " << m_muonAlgo <<endreq;
        return HLT::BAD_JOB_SETUP;
    }

    // which muon TP information do we want
    m_muonParticleType = (m_useCombinedTP ?  xAOD::Muon::CombinedTrackParticle : xAOD::Muon::InnerDetectorTrackParticle);
    if ( msgLvl() <= MSG::INFO ) msg() << MSG::INFO <<" Using Muon TrackParameter information from: "
        << (m_useCombinedTP ? "CombinedTrackParticle" : "InnerDetectorTrackParticle") <<endreq;

    
    if (m_bphysHelperTool.retrieve().isFailure()) {
        msg() << MSG::ERROR << "Can't find TrigBphysHelperUtilsTool" << endreq;
        return HLT::BAD_JOB_SETUP;
    } else {
        if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "TrigBphysHelperUtilsTool found" << endreq;
    }

    
    mon_dEtaRoI = -10.;
    mon_dPhiRoI = -10.;
    mon_dEtaMuMu = -10.;
    mon_dPhiMuMu = -10.;
    mon_pTsum = -10.;
    mon_mu1pT = -10.;
    mon_mu2pT = -10.;
    mon_mu1eta = -10.;
    mon_mu2eta = -10.;
    mon_mu1phi = -10.;
    mon_mu2phi = -10.;
    mon_MuMumass = -1.;
    mon_BmassFit = -1;
    mon_Chi2 = -1.;

    // counters
    m_lastEvent=-1;
    m_lastEventPassed=-1;
    m_countTotalEvents=0;
    m_countTotalRoI=0;
    m_countPassedEvents=0;
    m_countPassedRoIs=0;
    m_countPassedmumuPairsEv=0;
    m_countPassedBsMassEv=0;
    m_countPassedVtxFitEv=0;
    m_countPassedmumuPairs2R=0;
    m_countPassedBsMass2R=0;
    m_countPassedVtxFit2R=0;

    
  return HLT::OK;
}

HLT::ErrorCode TrigEFBMuMuFex::hltFinalize()
{
  msg() << MSG::INFO << "in finalize()" << endreq;

    msg() << MSG::INFO << "|----------------------- SUMMARY FROM TrigEFBMuMuFex -------------|" << endreq;
    msg() << MSG::INFO << "Run on events/2xRoIs " << m_countTotalEvents << "/" << m_countTotalRoI <<  endreq;
    msg() << MSG::INFO << "Passed events/2xRoIs " << m_countPassedEvents << "/" << m_countPassedRoIs <<  endreq;
    msg() << MSG::INFO << "Passed MuMu pairs:events/2xRoIs "  << m_countPassedmumuPairsEv<<"/"<<m_countPassedmumuPairs2R << endreq;
    msg() << MSG::INFO << "Passed BsMass: events/2xRoIs "<< m_countPassedBsMassEv<<"/"<<m_countPassedBsMass2R<<endreq;
    msg() << MSG::INFO << "Passed Vtx Fit: events/2xRoIs "<<m_countPassedVtxFitEv<< "/"<<m_countPassedVtxFit2R<< endreq;

    
  return HLT::OK;
}

// Define the bins for error-monitoring histogram
#define ERROR_No_EventInfo               0
#define ERROR_Not_2_Input_TE             1
#define ERROR_No_RoI_1                   2
#define ERROR_No_RoI_2                   3
#define ERROR_MuonFeature_Fails          4
// #define ERROR_Mu1_not_Combined           4
// #define ERROR_Mu1_not_Standalone         5
// #define ERROR_Mu1_Standalone_Invalid     6
// #define ERROR_Mu2_not_Combined           7
// #define ERROR_Mu2_not_Standalone         8
// #define ERROR_Mu2_Standalone_Invalid     9
// #define ERROR_MuMu_not_Both_Combined    10
// #define ERROR_MuMu_not_Comb_Standalone  11
// #define ERROR_AddTrack_Fails            12
// #define ERROR_CalcInvMass_Fails         13
// #define ERROR_CalcMother_Fails          14
// #define ERROR_CalcMassPull_Fails        15
#define ERROR_BphysColl_Fails           5

// Define the bins for acceptance-monitoring histogram
#define ACCEPT_Input                  0
#define ACCEPT_Got_RoIs               1
#define ACCEPT_Mu1                    2
#define ACCEPT_Mu2                    3
#define ACCEPT_MuMu                   4
#define ACCEPT_MuMu_Unique            5
#define ACCEPT_Opp_Charge             6
#define ACCEPT_Dimuon_Built           7
#define ACCEPT_InvMass_Cut            8
// Separator                          9
#define ACCEPT_BphysColl_not_Empty   10


//-------------------------------------------------------------------------------------
HLT::ErrorCode TrigEFBMuMuFex::acceptInputs(HLT::TEConstVec& inputTE, bool& pass) {
    
    if ( msgLvl() <= MSG::DEBUG )  msg() << MSG::DEBUG << "Running TrigEFBMuMuFex::acceptInputs" << endreq;
    if (m_resultsHolder.size()) {
        if ( msgLvl() <= MSG::WARNING )  msg() << MSG::WARNING << "Why is m_resultsHolder not empty!" << endreq;
    }
    m_resultsHolder.clear(); // make sure vector is cleared

    m_vtxpass=false;
    bool mumuIDpass=false;
    m_PassedBsMass=false;
    mon_dEtaRoI = -10.;
    mon_dPhiRoI = -10.;
    mon_dEtaMuMu = -10.;
    mon_dPhiMuMu = -10.;
    mon_pTsum = -10.;
    mon_mu1pT = -10.;
    mon_mu2pT = -10.;
    mon_mu1eta = -10.;
    mon_mu2eta = -10.;
    mon_mu1phi = -10.;
    mon_mu2phi = -10.;
    mon_MuMumass = -1.;
    mon_BmassFit = -1;
    mon_Chi2 = -1.;
    if ( timerSvc() )    m_BmmHypTot->start();
    
    mon_Acceptance.push_back( ACCEPT_Input );

    
    // check the right number of inputTEs
    if (m_expectNumberOfInputTE != inputTE.size()) {
        msg() << MSG::ERROR << "Got different than " << m_expectNumberOfInputTE << " number of input TEs, found " << inputTE.size() << endreq;
        mon_Errors.push_back( ERROR_Not_2_Input_TE );
        if ( timerSvc() )    m_BmmHypTot->stop();
        return HLT::BAD_JOB_SETUP;
    } else {
        if ( msgLvl() <= MSG::DEBUG )  msg() << MSG::DEBUG << "Found Expected " << m_expectNumberOfInputTE << " inputTEs" << endreq;
    }
    // event info
    uint32_t runNumber(0), evtNumber(0), lbBlock(0);
    if (m_bphysHelperTool->getRunEvtLb( runNumber, evtNumber, lbBlock).isFailure()) {
        msg() << MSG::ERROR << "Error retriving EventInfo" << endreq;
    }
    
    // get the TrigRoiDescriptors
    if(m_useRoIs) {
      std::vector<const TrigRoiDescriptor*> vecTrigRoiDescriptor(m_expectNumberOfInputTE);
      for (unsigned int i =0; i < m_expectNumberOfInputTE; ++i)  {
          if ( getFeature(inputTE[i], vecTrigRoiDescriptor[i]) != HLT::OK ) {
              msg() << MSG::ERROR << "Navigation error while getting RoI descriptor " << i << endreq;
              if ( timerSvc() ) m_BmmHypTot->stop();
              if ( !i ) mon_Errors.push_back( ERROR_No_RoI_1 );
              else      mon_Errors.push_back( ERROR_No_RoI_2 );
              return HLT::NAV_ERROR;
          }
          if (vecTrigRoiDescriptor[i] == nullptr) {
              msg() << MSG::ERROR << "Navigation error while getting RoI descriptor " << i << " (retrieved null pointer)" << endreq;
              if ( timerSvc() ) m_BmmHypTot->stop();
              if ( !i ) mon_Errors.push_back( ERROR_No_RoI_1 );
              else      mon_Errors.push_back( ERROR_No_RoI_2 );
              return HLT::NAV_ERROR;
          }
      } // for
      
      mon_Acceptance.push_back( ACCEPT_Got_RoIs );
      
      const TrigRoiDescriptor* roi1(vecTrigRoiDescriptor[0]), *roi2(vecTrigRoiDescriptor[1]);
      // debug for ROI info
      if ( msgLvl() <= MSG::DEBUG ){
          msg() << MSG::DEBUG
          << "Using inputTEs: "<< inputTE.front() <<  " and "  << inputTE.back() << " with Ids " << inputTE.front()->getId()<<" AND "<<inputTE.back()->getId()
          << "; RoI IDs = "   << roi1->roiId()<<" AND "<<roi2->roiId()
          << ": Eta1 = "      << roi1->eta()<<" Eta2= "<<roi2->eta()
          << ", Phi1 = "      << roi1->phi()<<" Phi2= "<<roi2->phi()
          << endreq;
      }
      mon_dEtaRoI = m_bphysHelperTool->absDeltaEta(roi1->eta(), roi2->eta());
      mon_dPhiRoI = m_bphysHelperTool->absDeltaPhi(roi1->phi(), roi2->phi());
    }
    else {
      if ( msgLvl() <= MSG::DEBUG ){
        msg() << MSG::DEBUG << "Not using input TrigRoiDescriptors. For nscan chains." << endreq;
      }
    }
    
    typedef  ElementLinkVector<xAOD::MuonContainer>  ELVMuons;
    typedef  ElementLinkVector<xAOD::TrackParticleContainer> ELVTrackParticles;
    
    std::vector<ELVMuons>          vec_elv_muons; // for muons,
    std::vector<ELVTrackParticles> vec_elv_tps;   // for tp
    
    
    for ( unsigned int i=0; i < m_expectNumberOfInputTE; ++i) {
        ELVMuons elvmuon;
        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Try to retrieve EFInfo container of muon " << i << endreq;
        if(getFeaturesLinks<xAOD::MuonContainer,xAOD::MuonContainer>(inputTE[i], elvmuon)!=HLT::OK ) {
            if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Failed to get EFInfo feature of muon " << i << ", exiting" << endreq;
            mon_Errors.push_back( ERROR_MuonFeature_Fails );
            return HLT::MISSING_FEATURE; // was HLT::OK
        }
        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Found MuonContainer, Got MuonEF " << i << " Feature, size = " << elvmuon.size() << endreq;
        vec_elv_muons.push_back(elvmuon);
    }// for loop over muon rois
    
    // print debug info
    if(msgLvl() <= MSG::DEBUG) {
        int ic(0);
        for ( const auto muelv : vec_elv_muons) {
            // loop over the vector of muon containers
            msg() << MSG::DEBUG <<  "MuonContainer, Got MuonEF " << ic << " Feature, size = " << muelv.size() << endreq;
            int i(0);
            for ( const auto muel: muelv) {
                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "ELLink: " << i++
                    << " index: "  << muel.index()
                    << " sgkey: "  << muel.dataID()
                    << " hashkey: "<< muel.key()
                    << " valid: "  << muel.isValid()
                    << " ptr: "    << (muel.isValid() ? *muel : nullptr)
                    << endreq;
            }
            i=0;
            for ( const auto muel: muelv) {
                if (!muel.isValid()) {++i; continue;}
                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Muon:   " << i++
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
            // http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Event/xAOD/xAODMuon/xAODMuon/versions/Muon_v1.h
            
            ++ic;
        } // for muons
        ic =0;
        for ( const auto tpelv : vec_elv_tps) {
            msg() << MSG::DEBUG <<  "SATrackParticleContainer, Got MUSA " << ic << " Feature, size = " << tpelv.size() << endreq;
            int i(0);
            for ( const auto tpel: tpelv) {
                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "ELLink: " << i
                    << " index: "  << tpel.index()
                    << " sgkey: "  << tpel.dataID()
                    << " hashkey: "<< tpel.key()
                    << " valid: "  << tpel.isValid()
                    << " ptr: "    << (tpel.isValid() ? *tpel : nullptr)
                    << endreq;
            } // fo tps
        } // for
    } // if debug
        
    
    // for each of the two roi's make muons selection (not roi2 has m_noID complexity)
    std::vector<const xAOD::Muon*> muons0, muons1, muonsAll;
    for (const auto muel: vec_elv_muons[0]){
        if ( (*muel)->muonType() != xAOD::Muon::Combined && (*muel)->muonType() != xAOD::Muon::SegmentTagged) {
            if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Muon from roi1 is neither Combined or SegmentTagged - reject" << endreq;
            continue;
        }
        mon_Acceptance.push_back( ACCEPT_Mu1 );
        m_bphysHelperTool->addUnique(*muel, muons0,0.005,0.005,10, m_muonParticleType);
        m_bphysHelperTool->addUnique(*muel, muonsAll,0.005,0.005,10, m_muonParticleType);
    } // roi1
    if(vec_elv_muons.size() > 1) {
      for (const auto muel: vec_elv_muons[1]){
          // special case if noId set
          if (m_noId) {
              // no check needed for the noID
              /*
              if ( (*muel)->muonType() != xAOD::Muon::MuonStandAlone) {
                  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "noID set and Muon from roi2 is not StandAlone - reject" << endreq;
                  continue;
              }
              */
          } else {
              if ( (*muel)->muonType() != xAOD::Muon::Combined && (*muel)->muonType() != xAOD::Muon::SegmentTagged) {
                  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "noID not set and Muon from roi2 is neither Combined or SegmentTagged - reject" << endreq;
                  continue;
              }
          } // if noid
          mon_Acceptance.push_back( ACCEPT_Mu2 );
          m_bphysHelperTool->addUnique(*muel, muons1,0.005,0.005,10, (!m_noId ? m_muonParticleType : xAOD::Muon::MuonSpectrometerTrackParticle));
          m_bphysHelperTool->addUnique(*muel, muonsAll,0.005,0.005,10, (!m_noId ? m_muonParticleType : xAOD::Muon::MuonSpectrometerTrackParticle));
      } // roi2
    }

    
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Muons in roi1: " << muons0.size() << ", roi2: " << muons1.size() << endreq;
    if ( muonsAll.size())         mumuIDpass = true;

    
    if (m_noId || !m_considerSameRoiCombinations) {
        std::vector<const xAOD::Muon*>::const_iterator itmu0     = muons0.begin();
        std::vector<const xAOD::Muon*>::const_iterator itmu0_end = muons0.end();
        std::vector<const xAOD::Muon*>::const_iterator itmu1     = muons1.begin();
        std::vector<const xAOD::Muon*>::const_iterator itmu1_end = muons1.end();
        
        for (; itmu0 != itmu0_end; ++itmu0) {
            const xAOD::Muon* mu0 = *itmu0;
            if (!mu0) continue;
            for (; itmu1 != itmu1_end; ++itmu1) {
                const xAOD::Muon* mu1 = *itmu1;
                if (!mu1) continue;
                if (mu0 == mu1) continue; // avoid same pointers (shouldn't happen?)
                //unsightly hack to add the muon element links:
                auto nresults = m_resultsHolder.size();
                buildCombination(mu0,mu1);                   //  Main method to build the muons
                auto nresults_new = m_resultsHolder.size();
                if (nresults == nresults_new) continue; // no new
                // if here then append muons to the objects
                xAOD::TrigBphys * bphys = m_resultsHolder.back();
                ElementLink<xAOD::IParticleContainer> ptl1EL,ptl2EL;
                bool foundMu0(false), foundMu1(false);
                for ( const auto muel: vec_elv_muons[0] ) {
                    if ( *muel == mu0) {
                        ptl1EL.resetWithKeyAndIndex(muel.dataID(),muel.index());
                        foundMu0 = true;
                    }
                    if ( *muel == mu1) {
                        ptl2EL.resetWithKeyAndIndex(muel.dataID(),muel.index());
                        foundMu1 = true;
                    }
                    if (foundMu0 && foundMu1) break; // found both links
                }
                if(vec_elv_muons.size() > 1) {
                  for ( const auto muel: vec_elv_muons[1] ) {
                      if ( *muel == mu0) {
                          ptl1EL.resetWithKeyAndIndex(muel.dataID(),muel.index());
                          foundMu0 = true;
                      }
                      if ( *muel == mu1) {
                          ptl2EL.resetWithKeyAndIndex(muel.dataID(),muel.index());
                          foundMu1 = true;
                      }
                      if (foundMu0 && foundMu1) break; // found both links
                  }
                }

                bphys->addParticleLink(ptl1EL); //
                bphys->addParticleLink(ptl2EL); //


            } // itmu1
        } // itmu0
    } // noId
    else {
        // use combined muons, and avoid double counting
        std::vector<const xAOD::Muon*>::const_iterator itmu0     = muonsAll.begin();
        std::vector<const xAOD::Muon*>::const_iterator itmu1     = muonsAll.begin();
        std::vector<const xAOD::Muon*>::const_iterator itmu_end  = muonsAll.end();
        for (; itmu0 != itmu_end; ++itmu0) {
            const xAOD::Muon* mu0 = *itmu0;
            if (!mu0) continue;
            for (itmu1 = itmu0+1; itmu1 != itmu_end; ++itmu1) {
                const xAOD::Muon* mu1 = *itmu1;
                if (!mu1) continue;
                if (mu0 == mu1) continue; // avoid same pointers (shouldn't happen?)
                                          //unsightly hack to add the muon element links:
                auto nresults = m_resultsHolder.size();
                buildCombination(mu0,mu1);                   //  Main method to build the muons
                auto nresults_new = m_resultsHolder.size();
                if (nresults == nresults_new) continue; // no new
                                                        // if here then append muons to the objects
                xAOD::TrigBphys * bphys = m_resultsHolder.back();
                ElementLink<xAOD::IParticleContainer> ptl1EL,ptl2EL;
                bool foundMu0(false), foundMu1(false);
                for ( const auto muel: vec_elv_muons[0] ) {
                    if ( *muel == mu0) {
                        ptl1EL.resetWithKeyAndIndex(muel.dataID(),muel.index());
                        foundMu0 = true;
                    }
                    if ( *muel == mu1) {
                        ptl2EL.resetWithKeyAndIndex(muel.dataID(),muel.index());
                        foundMu1 = true;
                    }
                    if (foundMu0 && foundMu1) break; // found both links
                }
                if(vec_elv_muons.size() > 1) {
                  for ( const auto muel: vec_elv_muons[1] ) {
                      if ( *muel == mu0) {
                          ptl1EL.resetWithKeyAndIndex(muel.dataID(),muel.index());
                          foundMu0 = true;
                      }
                      if ( *muel == mu1) {
                          ptl2EL.resetWithKeyAndIndex(muel.dataID(),muel.index());
                          foundMu1 = true;
                      }
                      if (foundMu0 && foundMu1) break; // found both links
                  }
                }
                
                bphys->addParticleLink(ptl1EL); //
                bphys->addParticleLink(ptl2EL); //
            } // mu1
        } //mu0
    } // !m_noId
        
    //    if (m_noId || !m_considerSameRoiCombinations) {
    //        buildCombinations(muons0,muons1); // treat each RoI separately here
    //    } else {
    //        // both good muons, and treat them all together
    //        buildCombinations(muonsAll);
    //
    //    }
    //
    //    // if set, also look at same roi combinations
    //    if (m_considerSameRoiCombinations && (muons0.size() >1 || muons1.size() >1 )) {
    //        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Looking now at Same-ROI combinations"<< endreq;
    //        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "    muons0: muons0"<< endreq;
    //        buildCombinations(muons0,muons0);
    //        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "    muons1: muons1"<< endreq;
    //        buildCombinations(muons1,muons1);
    //        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Finished looking at Same-ROI combinations"<< endreq;
    //    } // if
    
    
    if (evtNumber !=  m_lastEvent) {
        m_countTotalEvents++;
        m_lastEvent=evtNumber;
    }
    m_countTotalRoI++;

    pass = true; // carry on with processing
    
    if(m_vtxpass) m_countPassedVtxFit2R++;
    if(mumuIDpass) m_countPassedmumuPairs2R++;
    if (m_PassedBsMass) m_countPassedBsMass2R++;
    m_countPassedRoIs++;
    if(evtNumber != m_lastEventPassed) {
        m_countPassedEvents++;
        m_lastEventPassed = evtNumber;
        if(m_vtxpass) m_countPassedVtxFitEv++;
        if(mumuIDpass) m_countPassedmumuPairsEv++;
        if (m_PassedBsMass) m_countPassedBsMassEv++;
    }
    if ( timerSvc() )  m_BmmHypTot->stop();
    return HLT::OK;
}

void TrigEFBMuMuFex::buildCombination(const xAOD::Muon *mu0, const xAOD::Muon *mu1) {
    if (!mu0 || !mu1) return;
    if ( mu0 == mu1) {
        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Same Muon pointers" << endreq;
        return;
    }
    const ElementLink< xAOD::TrackParticleContainer > & tpel0 =  mu0->trackParticleLink( m_muonParticleType);
    const ElementLink< xAOD::TrackParticleContainer > & tpel1 =  mu1->trackParticleLink( m_noId ?
                                                                                        xAOD::Muon::MuonSpectrometerTrackParticle :
                                                                                        m_muonParticleType);
    
    mon_Acceptance.push_back( ACCEPT_MuMu );
    if(msgLvl() <= MSG::DEBUG) {
        msg() << MSG::DEBUG << "Track1 from muon: " << m_muonParticleType << " " << tpel0.isValid() << " " << ( tpel0.dataID() == "" ? "<empty string>" : tpel0.dataID()) << " " << tpel0.index() << endreq;
        msg() << MSG::DEBUG << "Track2 from muon: " << (m_noId? "noid" :"id") << " " << tpel1.isValid() << " " << ( tpel1.dataID() == "" ? "<empty string>" : tpel1.dataID()) << " " << tpel1.index() << endreq;
    } // end debug

    const xAOD::TrackParticle* tp0 = mu0->trackParticle(m_muonParticleType);
    const xAOD::TrackParticle* tp1 = mu1->trackParticle(m_noId ? xAOD::Muon::MuonSpectrometerTrackParticle : m_muonParticleType);
    if (!tp0) return;
    if (!tp1) return;
    const Trk::Track * trk0 = tp0->track();
    const Trk::Track * trk1 = tp1->track();
    if (trk1 && (trk1 == trk0)) {
        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Same Trk::Track pointers" << endreq;
        return;
    } // if tracks the same

    if ( !m_bphysHelperTool->areUnique(tp0,tp1, 0.005,0.005,10)) {
        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Reject tracks as failed uniqueness test: " << endreq;
        return;
    } // track uniqueness
    mon_Acceptance.push_back( ACCEPT_MuMu_Unique );
    // opposite charge requirement
    if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "Charges combination: "
        << mu0 << ": " << mu0->charge() << " " << tp0->qOverP() << " " << (trk0 ? trk0->perigeeParameters()->charge() : -99)
        << " , "
        << mu1 << ": " << mu1->charge() << " " << tp1->qOverP() << " " << (trk1 ? trk1->perigeeParameters()->charge() : -99)
        << endreq;
    
    if (m_oppositeCharge && (tp0->charge() * tp1->charge() > 0)) {
        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Reject permutation due to opposite charge requirement: "
            << mu0->charge() << "  " << mu1->charge() << endreq;
        return;
    }// opposite charge
    if (m_sameCharge && (tp0->charge() * tp1->charge() < 0)) {
        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Reject permutation due to same charge requirement: "
            << mu0->charge() << "  " << mu1->charge() << endreq;
        return;
    }// same charge
    mon_Acceptance.push_back( ACCEPT_Opp_Charge );
    
    // Fill monitoring histograms for muons
    mon_mu1eta = mu0->eta(); mon_mu2eta = mu1->eta();
    mon_mu1phi = mu0->phi(); mon_mu2phi = mu1->phi();
    mon_mu1pT = mu0->pt()*0.001; mon_mu2pT = mu1->pt()*0.001;
    mon_dEtaMuMu = m_bphysHelperTool->absDeltaEta(mon_mu1eta, mon_mu2eta);
    mon_dPhiMuMu = m_bphysHelperTool->absDeltaPhi(mon_mu1phi, mon_mu2phi);
    mon_pTsum = mon_mu1pT + mon_mu2pT;
    
    // simple mass
    double tp_mass = m_bphysHelperTool->invariantMass(tp0,tp1,m_massMuon,m_massMuon);
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Mass combination: " << tp_mass << endreq;
    
    xAOD::TrigBphys* result(0);
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Calling fitting code:" << endreq;
    
    if (m_bphysHelperTool->buildDiMu({tpel0,tpel1}, result, xAOD::TrigBphys::BMUMU, xAOD::TrigBphys::EF).isFailure() || !result) {
        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Problem with Fit code" << endreq;
    }
    m_bphysHelperTool->setBeamlineDisplacement(result,{tp0,tp1});
    
    if (result) {
        mon_Acceptance.push_back( ACCEPT_Dimuon_Built );
        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Including result" << endreq;
        m_resultsHolder.push_back(result);
        double massMuMu = result->mass();
        mon_MuMumass = massMuMu * 0.001;
        mon_BmassFit = result->fitmass() * 0.001;
        mon_Chi2 = result->fitchi2();
        m_vtxpass = true; // may not be strictly true now, if vx was null (maybe use chi2 / ndf value as a test?)
        if(m_lowerMassCut < massMuMu && ((massMuMu < m_upperMassCut) || (!m_ApplyupperMassCut) )) {
            mon_Acceptance.push_back( ACCEPT_InvMass_Cut );
            m_PassedBsMass = true;
        }
        
        
    } else {
        // no valid output
        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "No result found" << endreq;
    }

    
}



HLT::ErrorCode TrigEFBMuMuFex::hltExecute(HLT::TEConstVec&, HLT::TriggerElement* outputTE)
{
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " In TrigEFBMuMu hltExecute" << endreq;

    xAOD::TrigBphysContainer * xAODTrigBphysColl = new xAOD::TrigBphysContainer;
    xAOD::TrigBphysAuxContainer xAODTrigBphysAuxColl;
    xAODTrigBphysColl->setStore(&xAODTrigBphysAuxColl);

    // move results across to final area
    for ( auto xaodobj : m_resultsHolder) {
        xAODTrigBphysColl->push_back( xaodobj );
    }
    m_resultsHolder.clear();
    
    
    if (xAODTrigBphysColl && xAODTrigBphysColl->size()) {
        if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "REGTEST: Store Bphys Collection size: " << xAODTrigBphysColl->size() << endreq;
        mon_Acceptance.push_back( ACCEPT_BphysColl_not_Empty );

        HLT::ErrorCode sc = attachFeature(outputTE, xAODTrigBphysColl, "EFBMuMuFex" );
        if(sc != HLT::OK) {
            msg()  << MSG::WARNING << "Failed to store trigBphys Collection" << endreq;
            delete xAODTrigBphysColl; xAODTrigBphysColl = nullptr; // assume deletion responsibility
            mon_Errors.push_back( ERROR_BphysColl_Fails );
            return HLT::ERROR;
        }
    } else {
        if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "REGTEST: no bphys collection to store "  << endreq;
        delete xAODTrigBphysColl; xAODTrigBphysColl = nullptr;
    }
    
  return HLT::OK;
}



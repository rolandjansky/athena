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
    declareProperty("TrigBphysHelperTool", m_bphysHelperTool);
    
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
    declareMonitoredStdContainer("Errors",     m_mon_Errors,     AutoClear);
    declareMonitoredStdContainer("Acceptance", m_mon_Acceptance, AutoClear);
    declareMonitoredVariable("DeltaEtaRoIs", m_mon_dEtaRoI    );
    declareMonitoredVariable("DeltaPhiRoIs", m_mon_dPhiRoI    );
    declareMonitoredVariable("DeltaEtaMuMu", m_mon_dEtaMuMu   );
    declareMonitoredVariable("DeltaPhiMuMu", m_mon_dPhiMuMu   );
    declareMonitoredVariable("pTsumMuMu",    m_mon_pTsum      );
    declareMonitoredVariable("pTMu1",        m_mon_mu1pT      );
    declareMonitoredVariable("pTMu2",        m_mon_mu2pT      );
    declareMonitoredVariable("etaMu1",       m_mon_mu1eta     );
    declareMonitoredVariable("etaMu2",       m_mon_mu2eta     );
    declareMonitoredVariable("phiMu1",       m_mon_mu1phi     );
    declareMonitoredVariable("phiMu2",       m_mon_mu2phi     );
    declareMonitoredVariable("MuMumass",     m_mon_MuMumass   );
    declareMonitoredVariable("BmassFit",     m_mon_BmassFit   );
    declareMonitoredVariable("VtxChi2",      m_mon_Chi2       );
 
}

TrigEFBMuMuFex::~TrigEFBMuMuFex()
{ }

HLT::ErrorCode TrigEFBMuMuFex::hltInitialize()
{    
    ATH_MSG_DEBUG("Initialization ..." );
    ATH_MSG_DEBUG("AcceptAll            = " << (m_acceptAll==true ? "True" : "False") );
    ATH_MSG_DEBUG("noId          = " << (m_noId==true ? "True" : "False") );
    ATH_MSG_DEBUG("OppositeCharge       = " << (m_oppositeCharge==true ? "True" : "False") );
    ATH_MSG_DEBUG("SameCharge       = "     << (m_sameCharge==true ? "True" : "False") );
    ATH_MSG_DEBUG("LowerMassCut         = " << m_lowerMassCut );
    ATH_MSG_DEBUG("UpperMassCut         = " << m_upperMassCut );
    ATH_MSG_DEBUG("ApplyUpperMassCut         = " << m_ApplyupperMassCut );
    
    if ( timerSvc() ) {
        m_BmmHypTot = addTimer("EFBmmHypTot");
        m_BmmHypVtx = addTimer("EFBmmHypVtxFit");
    }
    
    // Consistency check of charge requirements
    if( m_oppositeCharge && m_sameCharge ) {
      ATH_MSG_ERROR("Bad configuration: OppositeCharge and SameCharge are required together" );
      return HLT::BAD_JOB_SETUP;
    }
    
    if (m_muonAlgo != "TrigMuSuperEF" ) {
        ATH_MSG_INFO(" Expected algorithm name:  TrigMuSuperEF, but got: " << m_muonAlgo );
        return HLT::BAD_JOB_SETUP;
    }

    // which muon TP information do we want
    m_muonParticleType = (m_useCombinedTP ?  xAOD::Muon::CombinedTrackParticle : xAOD::Muon::InnerDetectorTrackParticle);
    ATH_MSG_INFO(" Using Muon TrackParameter information from: " << (m_useCombinedTP ? "CombinedTrackParticle" : "InnerDetectorTrackParticle") );

    if (m_bphysHelperTool.retrieve().isFailure()) {
        ATH_MSG_ERROR("Can't find TrigBphysHelperUtilsTool" );
        return HLT::BAD_JOB_SETUP;
    } else {
        ATH_MSG_DEBUG("TrigBphysHelperUtilsTool found" );
    }

    
    m_mon_dEtaRoI = -10.;
    m_mon_dPhiRoI = -10.;
    m_mon_dEtaMuMu = -10.;
    m_mon_dPhiMuMu = -10.;
    m_mon_pTsum = -10.;
    m_mon_mu1pT = -10.;
    m_mon_mu2pT = -10.;
    m_mon_mu1eta = -10.;
    m_mon_mu2eta = -10.;
    m_mon_mu1phi = -10.;
    m_mon_mu2phi = -10.;
    m_mon_MuMumass = -1.;
    m_mon_BmassFit = -1;
    m_mon_Chi2 = -1.;

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
  ATH_MSG_INFO("in finalize()" );
  ATH_MSG_INFO("|----------------------- SUMMARY FROM TrigEFBMuMuFex -------------|" );
  ATH_MSG_INFO("Run on events/2xRoIs " << m_countTotalEvents << "/" << m_countTotalRoI );
  ATH_MSG_INFO("Passed events/2xRoIs " << m_countPassedEvents << "/" << m_countPassedRoIs );
  ATH_MSG_INFO("Passed MuMu pairs:events/2xRoIs "  << m_countPassedmumuPairsEv<<"/"<<m_countPassedmumuPairs2R );
  ATH_MSG_INFO("Passed BsMass: events/2xRoIs "<< m_countPassedBsMassEv<<"/"<<m_countPassedBsMass2R);
  ATH_MSG_INFO("Passed Vtx Fit: events/2xRoIs "<<m_countPassedVtxFitEv<< "/"<<m_countPassedVtxFit2R);
    
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
    
    ATH_MSG_DEBUG("Running TrigEFBMuMuFex::acceptInputs" );
    if (m_resultsHolder.size()) {
        ATH_MSG_WARNING("Why is m_resultsHolder not empty!" );
    }
    m_resultsHolder.clear(); // make sure vector is cleared

    m_vtxpass=false;
    bool mumuIDpass=false;
    m_PassedBsMass=false;
    m_mon_dEtaRoI = -10.;
    m_mon_dPhiRoI = -10.;
    m_mon_dEtaMuMu = -10.;
    m_mon_dPhiMuMu = -10.;
    m_mon_pTsum = -10.;
    m_mon_mu1pT = -10.;
    m_mon_mu2pT = -10.;
    m_mon_mu1eta = -10.;
    m_mon_mu2eta = -10.;
    m_mon_mu1phi = -10.;
    m_mon_mu2phi = -10.;
    m_mon_MuMumass = -1.;
    m_mon_BmassFit = -1;
    m_mon_Chi2 = -1.;
    if ( timerSvc() )    m_BmmHypTot->start();
    
    m_mon_Acceptance.push_back( ACCEPT_Input );

    
    // check the right number of inputTEs
    // if !m_useRoIs -- allow +1 number of TEs, for nscan chains
    if (m_expectNumberOfInputTE != inputTE.size() && (m_useRoIs || m_expectNumberOfInputTE != inputTE.size()-1) ) {
        ATH_MSG_ERROR("Got different than " << m_expectNumberOfInputTE << " number of input TEs, found " << inputTE.size() );
        m_mon_Errors.push_back( ERROR_Not_2_Input_TE );
        if ( timerSvc() )    m_BmmHypTot->stop();
        return HLT::BAD_JOB_SETUP;
    } else {
        ATH_MSG_DEBUG("Found Expected " << m_expectNumberOfInputTE << " inputTEs" );
    }
    // event info
    uint32_t runNumber(0), evtNumber(0), lbBlock(0);
    if (m_bphysHelperTool->getRunEvtLb( runNumber, evtNumber, lbBlock).isFailure()) {
        ATH_MSG_ERROR("Error retriving EventInfo" );
    }
    
    // get the TrigRoiDescriptors
    if(m_useRoIs) {
      std::vector<const TrigRoiDescriptor*> vecTrigRoiDescriptor(m_expectNumberOfInputTE);
      for (unsigned int i =0; i < m_expectNumberOfInputTE; ++i)  {
          if ( getFeature(inputTE[i], vecTrigRoiDescriptor[i]) != HLT::OK ) {
              ATH_MSG_ERROR("Navigation error while getting RoI descriptor " << i );
              if ( timerSvc() ) m_BmmHypTot->stop();
              if ( !i ) m_mon_Errors.push_back( ERROR_No_RoI_1 );
              else      m_mon_Errors.push_back( ERROR_No_RoI_2 );
              return HLT::NAV_ERROR;
          }
          if (vecTrigRoiDescriptor[i] == nullptr) {
              ATH_MSG_ERROR("Navigation error while getting RoI descriptor " << i << " (retrieved null pointer)" );
              if ( timerSvc() ) m_BmmHypTot->stop();
              if ( !i ) m_mon_Errors.push_back( ERROR_No_RoI_1 );
              else      m_mon_Errors.push_back( ERROR_No_RoI_2 );
              return HLT::NAV_ERROR;
          }
      } // for
      
      m_mon_Acceptance.push_back( ACCEPT_Got_RoIs );
      
      const TrigRoiDescriptor* roi1(vecTrigRoiDescriptor[0]), *roi2(vecTrigRoiDescriptor[1]);
      // debug for ROI info
      ATH_MSG_DEBUG("Using inputTEs: "<< inputTE.front() <<  " and "  << inputTE.back() << " with Ids " << inputTE.front()->getId()<<" AND "<<inputTE.back()->getId()
          << "; RoI IDs = "   << roi1->roiId()<<" AND "<<roi2->roiId() << ": Eta1 = "      << roi1->eta()<<" Eta2= "<<roi2->eta()
          << ", Phi1 = "      << roi1->phi()<<" Phi2= "<<roi2->phi());
      m_mon_dEtaRoI = m_bphysHelperTool->absDeltaEta(roi1->eta(), roi2->eta());
      m_mon_dPhiRoI = m_bphysHelperTool->absDeltaPhi(roi1->phi(), roi2->phi());
    }
    else {
     ATH_MSG_DEBUG("Not using input TrigRoiDescriptors. For nscan chains." );
    }
    
    typedef  ElementLinkVector<xAOD::MuonContainer>  ELVMuons;
    typedef  ElementLinkVector<xAOD::TrackParticleContainer> ELVTrackParticles;
    
    std::vector<ELVMuons>          vec_elv_muons; // for muons,
    std::vector<ELVTrackParticles> vec_elv_tps;   // for tp
    
    
    for ( unsigned int i=0; i < m_expectNumberOfInputTE; ++i) {
        ELVMuons elvmuon;
        ATH_MSG_DEBUG("Try to retrieve EFInfo container of muon " << i );
        if(getFeaturesLinks<xAOD::MuonContainer,xAOD::MuonContainer>(inputTE[i], elvmuon)!=HLT::OK ) {
            ATH_MSG_DEBUG("Failed to get EFInfo feature of muon " << i << ", exiting" );
            m_mon_Errors.push_back( ERROR_MuonFeature_Fails );
            return HLT::MISSING_FEATURE; // was HLT::OK
        }
        ATH_MSG_DEBUG("Found MuonContainer, Got MuonEF " << i << " Feature, size = " << elvmuon.size() );
        vec_elv_muons.push_back(elvmuon);
    }// for loop over muon rois
    
    // print debug info
    if(msgLvl() <= MSG::DEBUG) {
        int ic(0);
        for ( const auto muelv : vec_elv_muons) {
            // loop over the vector of muon containers
            msg() << MSG::DEBUG << "MuonContainer, Got MuonEF " << ic << " Feature, size = " << muelv.size() << endmsg;
            int i(0);
            for ( const auto muel: muelv) {
                msg() << MSG::DEBUG << "ELLink: " << i++
                    << " index: "  << muel.index()
                    << " sgkey: "  << muel.dataID()
                    << " hashkey: "<< muel.key()
                    << " valid: "  << muel.isValid()
                    << " ptr: "    << (muel.isValid() ? *muel : nullptr)
                    << endmsg;
                if (!muel.isValid()) {continue;}
                msg() << MSG::DEBUG << "Muon:   " << i
                    << " pt: " <<  (*muel)->pt()
                    << " eta: " << (*muel)->eta()
                    << " phi: " << (*muel)->phi()
                    << " q: "   << (*muel)->charge()
                    << " mutype: "   << (*muel)->muonType()
                    << " author: "   << (*muel)->author()
                    << " id/ms/cb: " << (*muel)->trackParticle(xAOD::Muon::InnerDetectorTrackParticle)
                    << " "           << (*muel)->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle)
                    << " "           << (*muel)->trackParticle(xAOD::Muon::CombinedTrackParticle)
                    << endmsg;
            } // for
            // http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Event/xAOD/xAODMuon/xAODMuon/versions/Muon_v1.h
            ++ic;
        } // for muons
        ic =0;
        for ( const auto tpelv : vec_elv_tps) {
            msg() << MSG::DEBUG <<  "SATrackParticleContainer, Got MUSA " << ic << " Feature, size = " << tpelv.size() << endmsg;
            int i(0);
            for ( const auto tpel: tpelv) {
                msg() << MSG::DEBUG << "ELLink: " << i++
                    << " index: "  << tpel.index()
                    << " sgkey: "  << tpel.dataID()
                    << " hashkey: "<< tpel.key()
                    << " valid: "  << tpel.isValid()
                    << " ptr: "    << (tpel.isValid() ? *tpel : nullptr)
                    << endmsg;
            } // fo tps
        } // for
    } // if debug
        
    
    // for each of the two roi's make muons selection (not roi2 has m_noID complexity)
    std::vector<const xAOD::Muon*> muons0, muons1, muonsAll;
    for (const auto muel: vec_elv_muons[0]){
        if ( (*muel)->muonType() != xAOD::Muon::Combined && (*muel)->muonType() != xAOD::Muon::SegmentTagged) {
            ATH_MSG_DEBUG("Muon from roi1 is neither Combined or SegmentTagged - reject" );
            continue;
        }
        m_mon_Acceptance.push_back( ACCEPT_Mu1 );
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
                  ATH_MSG_DEBUG("noID set and Muon from roi2 is not StandAlone - reject" );
                  continue;
              }
              */
          } else {
              if ( (*muel)->muonType() != xAOD::Muon::Combined && (*muel)->muonType() != xAOD::Muon::SegmentTagged) {
                  ATH_MSG_DEBUG("noID not set and Muon from roi2 is neither Combined or SegmentTagged - reject" );
                  continue;
              }
          } // if noid
          m_mon_Acceptance.push_back( ACCEPT_Mu2 );
          m_bphysHelperTool->addUnique(*muel, muons1,0.005,0.005,10, (!m_noId ? m_muonParticleType : xAOD::Muon::MuonSpectrometerTrackParticle));
          m_bphysHelperTool->addUnique(*muel, muonsAll,0.005,0.005,10, (!m_noId ? m_muonParticleType : xAOD::Muon::MuonSpectrometerTrackParticle));
      } // roi2
    }

    
    ATH_MSG_DEBUG("Muons in roi1: " << muons0.size() << ", roi2: " << muons1.size() );
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
    //        ATH_MSG_DEBUG("Looking now at Same-ROI combinations");
    //        ATH_MSG_DEBUG("    muons0: muons0");
    //        buildCombinations(muons0,muons0);
    //        ATH_MSG_DEBUG("    muons1: muons1");
    //        buildCombinations(muons1,muons1);
    //        ATH_MSG_DEBUG("Finished looking at Same-ROI combinations");
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
        ATH_MSG_DEBUG("Same Muon pointers" );
        return;
    }
    const ElementLink< xAOD::TrackParticleContainer > & tpel0 =  mu0->trackParticleLink( m_muonParticleType);
    const ElementLink< xAOD::TrackParticleContainer > & tpel1 =  mu1->trackParticleLink( m_noId ?
                                                                                        xAOD::Muon::MuonSpectrometerTrackParticle :
                                                                                        m_muonParticleType);
    
    m_mon_Acceptance.push_back( ACCEPT_MuMu );
    if(msgLvl() <= MSG::DEBUG) {
    ATH_MSG_DEBUG("Track1 from muon: " << m_muonParticleType << " " << tpel0.isValid() << " " << ( tpel0.dataID() == "" ? "<empty string>" : tpel0.dataID()) << " " << tpel0.index() );
    ATH_MSG_DEBUG("Track2 from muon: " << (m_noId? "noid" :"id") << " " << tpel1.isValid() << " " << ( tpel1.dataID() == "" ? "<empty string>" : tpel1.dataID()) << " " << tpel1.index() );
    } // end debug

    const xAOD::TrackParticle* tp0 = mu0->trackParticle(m_muonParticleType);
    const xAOD::TrackParticle* tp1 = mu1->trackParticle(m_noId ? xAOD::Muon::MuonSpectrometerTrackParticle : m_muonParticleType);
    if (!tp0) return;
    if (!tp1) return;
    const Trk::Track * trk0 = tp0->track();
    const Trk::Track * trk1 = tp1->track();
    if (trk1 && (trk1 == trk0)) {
        ATH_MSG_DEBUG("Same Trk::Track pointers" );
        return;
    } // if tracks the same

    if ( !m_bphysHelperTool->areUnique(tp0,tp1, 0.005,0.005,10)) {
        ATH_MSG_DEBUG("Reject tracks as failed uniqueness test: " );
        return;
    } // track uniqueness
    m_mon_Acceptance.push_back( ACCEPT_MuMu_Unique );
    // opposite charge requirement
    ATH_MSG_VERBOSE("Charges combination: " << mu0 << ": " << mu0->charge() << " " << tp0->qOverP() << " " << (trk0 ? trk0->perigeeParameters()->charge() : -99)
        << " , " << mu1 << ": " << mu1->charge() << " " << tp1->qOverP() << " " << (trk1 ? trk1->perigeeParameters()->charge() : -99));
    
    if (m_oppositeCharge && (tp0->charge() * tp1->charge() > 0)) {
        ATH_MSG_DEBUG("Reject permutation due to opposite charge requirement: " << mu0->charge() << "  " << mu1->charge() );
        return;
    }// opposite charge
    if (m_sameCharge && (tp0->charge() * tp1->charge() < 0)) {
        ATH_MSG_DEBUG("Reject permutation due to same charge requirement: " << mu0->charge() << "  " << mu1->charge() );
        return;
    }// same charge
    m_mon_Acceptance.push_back( ACCEPT_Opp_Charge );
    
    // Fill monitoring histograms for muons
    m_mon_mu1eta = mu0->eta(); m_mon_mu2eta = mu1->eta();
    m_mon_mu1phi = mu0->phi(); m_mon_mu2phi = mu1->phi();
    m_mon_mu1pT = mu0->pt()*0.001; m_mon_mu2pT = mu1->pt()*0.001;
    m_mon_dEtaMuMu = m_bphysHelperTool->absDeltaEta(m_mon_mu1eta, m_mon_mu2eta);
    m_mon_dPhiMuMu = m_bphysHelperTool->absDeltaPhi(m_mon_mu1phi, m_mon_mu2phi);
    m_mon_pTsum = m_mon_mu1pT + m_mon_mu2pT;
    
    // simple mass
    double tp_mass = m_bphysHelperTool->invariantMass(tp0,tp1,m_massMuon,m_massMuon);
    ATH_MSG_DEBUG("Mass combination: " << tp_mass );
    
    xAOD::TrigBphys* result(0);
    ATH_MSG_DEBUG("Calling fitting code:" );
    
    if (m_bphysHelperTool->buildDiMu({tpel0,tpel1}, result, xAOD::TrigBphys::BMUMU, xAOD::TrigBphys::EF).isFailure() || !result) {
        ATH_MSG_DEBUG("Problem with Fit code" );
    }
    m_bphysHelperTool->setBeamlineDisplacement(result,{tp0,tp1});
    
    if (result) {
        m_mon_Acceptance.push_back( ACCEPT_Dimuon_Built );
        ATH_MSG_DEBUG("Including result" );
        double massMuMu = result->mass();
        
        // apply loosened mass cut here to avoid too large containers
        if(massMuMu < m_lowerMassCut - 5000.) {
          ATH_MSG_DEBUG("Reject due to lower mass cut" );
          delete result;
          return;
        }
        if(m_ApplyupperMassCut && massMuMu > m_upperMassCut + 5000.) {
          ATH_MSG_DEBUG("Reject due to upper mass cut" );
          delete result;
          return;
        }
        
        m_resultsHolder.push_back(result);
        m_mon_MuMumass = massMuMu * 0.001;
        m_mon_BmassFit = result->fitmass() * 0.001;
        m_mon_Chi2 = result->fitchi2();
        m_vtxpass = true; // may not be strictly true now, if vx was null (maybe use chi2 / ndf value as a test?)
        if(m_lowerMassCut < massMuMu && ((massMuMu < m_upperMassCut) || (!m_ApplyupperMassCut) )) {
            m_mon_Acceptance.push_back( ACCEPT_InvMass_Cut );
            m_PassedBsMass = true;
        }
        
        
    } else {
        // no valid output
        ATH_MSG_DEBUG("No result found" );
    }

    
}



HLT::ErrorCode TrigEFBMuMuFex::hltExecute(HLT::TEConstVec&, HLT::TriggerElement* outputTE)
{
    ATH_MSG_DEBUG(" In TrigEFBMuMu hltExecute" );

    xAOD::TrigBphysContainer * xAODTrigBphysColl = new xAOD::TrigBphysContainer;
    xAOD::TrigBphysAuxContainer xAODTrigBphysAuxColl;
    xAODTrigBphysColl->setStore(&xAODTrigBphysAuxColl);

    // move results across to final area
    for ( auto xaodobj : m_resultsHolder) {
        xAODTrigBphysColl->push_back( xaodobj );
    }
    m_resultsHolder.clear();
    
    
    if (xAODTrigBphysColl && xAODTrigBphysColl->size()) {
        ATH_MSG_DEBUG("REGTEST: Store Bphys Collection size: " << xAODTrigBphysColl->size() );
        m_mon_Acceptance.push_back( ACCEPT_BphysColl_not_Empty );

        HLT::ErrorCode sc = attachFeature(outputTE, xAODTrigBphysColl, "EFBMuMuFex" );
        if(sc != HLT::OK) {
            ATH_MSG_WARNING("Failed to store trigBphys Collection" );
            delete xAODTrigBphysColl; xAODTrigBphysColl = nullptr; // assume deletion responsibility
            m_mon_Errors.push_back( ERROR_BphysColl_Fails );
            return HLT::ERROR;
        }
    } else {
        ATH_MSG_DEBUG("REGTEST: no bphys collection to store "  );
        delete xAODTrigBphysColl; xAODTrigBphysColl = nullptr;
    }
    
  return HLT::OK;
}



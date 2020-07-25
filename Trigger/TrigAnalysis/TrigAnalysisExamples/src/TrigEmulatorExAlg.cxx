/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigEmulatorExAlg.cxx 770492 2016-08-28 16:52:40Z rwhite $
// Place holder for TrigEmulator trigger analysis for Trigger Tutorial
//
// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/Units.h"

// Local include(s):
#include "TrigEmulatorExAlg.h"

// EDM includes
#include "xAODTrigger/EmTauRoIContainer.h"
#include "xAODTrigger/EnergySumRoI.h"
#include "xAODTrigger/JetRoIContainer.h"
#include "xAODTrigger/MuonRoIContainer.h"

using Athena::Units::GeV;

TrigEmulatorExAlg::TrigEmulatorExAlg( const std::string& name, ISvcLocator *pSvcLocator )
   : AthAlgorithm( name, pSvcLocator ),
    m_eventNr( 0 ),
    //m_doEg(false),
    m_triggerDecisionTool( "Trig::TrigDecisionTool/TrigDecisionTool" ),
    m_triggerMatchingTool( "Trig::MatchingTool/MatchingTool",this),
    m_triggerAnalysisHelper( "Trig::TriggerAnalysisHelper/TriggerAnalysisHelper",this ),
    m_histSvc( "THistSvc", name ),
    m_h_L1TriggerAccepts (nullptr),
    m_h_L1EmulationAccepts (nullptr),
    m_h_HLTTriggerAccepts (nullptr),
    m_h_HLTEmulationAccepts (nullptr)
{
    // job option configurable properties
    declareProperty( "L1TriggerList", m_l1chainList);
    declareProperty( "HLTTriggerList", m_hltchainList);

    // Settings for tau emulation cuts
    declareProperty("ClusterPt", m_roi_pt_cut = 12000., "cut on the EmTauRoI transverse energy");
    declareProperty("ClusterEta", m_roi_eta_cut = 2.5, "cut on the EmTauRoi |eta|");
    declareProperty("IsolationSlope", m_iso_slope = 0.1, "slope of the pT-dep. isolation cut");
    declareProperty("IsolationOffset", m_iso_offset = 2000., "Offset of the pT-dep. isolation cut");
    declareProperty("IsolationThresh", m_iso_thresh = 60000., "Threshold of the pT-dep. isolation cut");
    declareProperty("IsolationMinimum", m_iso_min = 0., "Minimum Value on pT-dep. isolation cut");
    declareProperty("HadronicLeakageSlope", m_had_leak_slope = 0.1, "slope of the pT-dep. hadronic leakage cut");
    declareProperty("HadronicLeakageOffset", m_had_leak_offset = 2000., "Offset of the pT-dep. hadronic leakage cut");
    declareProperty("HadronicLeakageThresh", m_had_leak_thresh = 60000., "Threshold of the pT-dep. hadronic leakage cut");
    declareProperty("HadronicLeakageMinimum", m_had_leak_min = 1000., "Minimum Value on pT-dep. hadronic leakage cut");
    declareProperty("UseEmClus", m_use_emclus = false, "Switch TAU/EM cluster");
    declareProperty("UseHadCore", m_use_had_core = false, "Hadronic core req. switch");
}

StatusCode TrigEmulatorExAlg::initialize() {

   CHECK( m_triggerDecisionTool.retrieve() );
   CHECK( m_triggerMatchingTool.retrieve() );
   CHECK( m_triggerAnalysisHelper.retrieve() );
   CHECK( m_histSvc.retrieve() );

   m_triggerDecisionTool->ExperimentalAndExpertMethods()->enable();
   
   if ( ! m_l1chainList.empty() ) {
       ATH_MSG_INFO("Considering the following L1 chains:");
       for(const auto& chain : m_l1chainList) {
           ATH_MSG_INFO("\t" << chain);
       }
   }
   
   if ( ! m_hltchainList.empty() ) {
       ATH_MSG_INFO("Considering the following HLT chains:");
       for(const auto& chain : m_hltchainList) {
           ATH_MSG_INFO("\t" << chain);
       }
   }

   // Setup histograms for trigger decision and prescale
   const int nHLTTriggers = static_cast<int>(m_hltchainList.size());
   m_h_HLTTriggerAccepts = new TH1F( "HLTTriggerAccepts", "HLTTriggerAccepts", nHLTTriggers, 0,  nHLTTriggers);
   m_h_HLTEmulationAccepts = new TH1F( "HLTEmulationAccepts", "HLTEmulationAccepts", nHLTTriggers, 0,  nHLTTriggers);
   if ( ! m_hltchainList.empty() ){
       for ( int i = 0; i < std::min( static_cast<int>(m_hltchainList.size()), static_cast<int>(m_h_HLTTriggerAccepts->GetNbinsX()) ); ++i ) {
           int bin = i+1;
           m_h_HLTTriggerAccepts->GetXaxis()->SetBinLabel(bin, m_hltchainList[i].c_str());
           m_h_HLTEmulationAccepts->GetXaxis()->SetBinLabel(bin, m_hltchainList[i].c_str());
           ATH_MSG_INFO("setting label X" <<  m_hltchainList[i] << " for bin " << bin);
       }
   }
   
   const int nL1Triggers = static_cast<int>(m_l1chainList.size());
   m_h_L1TriggerAccepts = new TH1F( "L1TriggerAccepts", "L1TriggerAccepts", nL1Triggers, 0,  nL1Triggers);
   m_h_L1EmulationAccepts = new TH1F( "L1EmulationAccepts", "L1EmulationAccepts", nL1Triggers, 0,  nL1Triggers);
   if ( ! m_l1chainList.empty() ){
       for ( int i = 0; i < std::min( static_cast<int>(m_l1chainList.size()), static_cast<int>(m_h_L1TriggerAccepts->GetNbinsX()) ); ++i ) {
           int bin = i+1;
           m_h_L1TriggerAccepts->GetXaxis()->SetBinLabel(bin, m_l1chainList[i].c_str());
           m_h_L1EmulationAccepts->GetXaxis()->SetBinLabel(bin, m_l1chainList[i].c_str());
           ATH_MSG_INFO("setting label X" <<  m_l1chainList[i] << " for bin " << bin);
       }
   }
   
   CHECK( m_histSvc->regHist( "/Trigger/Emulation/L1TriggerAccepts", m_h_L1TriggerAccepts ) );
   CHECK( m_histSvc->regHist( "/Trigger/Emulation/L1EmulationAccepts", m_h_L1EmulationAccepts ) );
   CHECK( m_histSvc->regHist( "/Trigger/Emulation/HLTTriggerAccepts", m_h_HLTTriggerAccepts ) );
   CHECK( m_histSvc->regHist( "/Trigger/Emulation/HLTEmulationAccepts", m_h_HLTEmulationAccepts ) );
   
   for(const auto& chain : m_l1chainList){
       m_numSelectedEvents[chain] = 0;
       m_numL1PassedEvents[chain] = 0;
       m_numL1EmulatedEvents[chain] = 0;
   }
   
   for(const auto& chain : m_hltchainList){
       m_numSelectedEvents[chain] = 0;
       m_numL1PassedEvents[chain] = 0;
       m_numHLTPassedEvents[chain] = 0;
       m_numL1EmulatedEvents[chain] = 0;
       m_numHLTEmulatedEvents[chain] = 0;
   }

   return StatusCode::SUCCESS;
}

StatusCode TrigEmulatorExAlg::execute() {
   ++m_eventNr;
   ATH_MSG_INFO( "In TrigEmulatorExAlg:: execute"); 

   CHECK(collectTriggerStatistics());
   CHECK(EmulationAnalysis());
   return StatusCode::SUCCESS;
}


StatusCode TrigEmulatorExAlg::finalize() {

   // Here we can print out all the information we've learned
   // We need to be a little clever in how we interpret the information
   // presented here.  In particular, remember that each muon has a chance
   // to act as the tag muon, so for chains with very high efficiency
   // its not unreasonable to get a number of tag muons greater than the
   // number of selected events.
   REPORT_MESSAGE( MSG::DEBUG ) << "TrigEmulatorExAlg::finalize()";

   ATH_MSG_INFO( "" );
   ATH_MSG_INFO( "TrigEmulatorExAlg results" );
   for(const auto& chain : m_l1chainList){
       ATH_MSG_INFO( "Chain " << chain);
       ATH_MSG_INFO( " Number of events L1 fired: " << m_numL1PassedEvents[chain] );
       ATH_MSG_INFO( " Ratio of triggered events to emulated events:" 
               << static_cast<float>(m_numL1PassedEvents[chain])/static_cast<float>(m_numL1EmulatedEvents[chain]) * 100. << "%" );
       ATH_MSG_INFO( "" );
   }
   
   for(const auto& chain : m_hltchainList){
       ATH_MSG_INFO( "Chain " << chain);
       ATH_MSG_INFO( " Number of selected events: " << m_numSelectedEvents[chain] );
       ATH_MSG_INFO( " Number of events HLT fired: " << m_numHLTPassedEvents[chain] );
       ATH_MSG_INFO( " Ratio of triggered events to emulated events:" 
               << static_cast<float>(m_numHLTPassedEvents[chain])/static_cast<float>(m_numHLTEmulatedEvents[chain]) * 100. << "%" );
       ATH_MSG_INFO( "" );
   }

   return StatusCode::SUCCESS;
}

StatusCode TrigEmulatorExAlg::collectTriggerStatistics() {

   // Now we'd like to collect some trigger statistics for the chains specified 
    for(const auto& chain : m_hltchainList){
        if( m_triggerDecisionTool->isPassed( chain ) ){
            m_h_HLTTriggerAccepts->Fill( chain.c_str(), 1 );
            ++m_numHLTPassedEvents[chain];
        }
    }
    
    for(const auto& chain : m_l1chainList){
        auto chain_group = m_triggerDecisionTool->getChainGroup(chain);
        bool cg_passes_event = chain_group->isPassedBits() & TrigDefs::L1_isPassedBeforePrescale;
        bool cg_passes_event_1 = chain_group->isPassedBits() & TrigDefs::L1_isPassedAfterVeto;

        if( cg_passes_event || cg_passes_event_1) {
            m_h_L1TriggerAccepts->Fill( chain.c_str(), 1 );
            ++m_numL1PassedEvents[chain];
        }
    }

   return StatusCode::SUCCESS;
}

/* ******************************************************************************************
 * TriggerAnalysis for TrigEmulator 
 * *******************************************************************************************/
StatusCode TrigEmulatorExAlg::EmulationAnalysis () {
    ATH_MSG_INFO("EmulationAnalysis()");

    const xAOD::EmTauRoIContainer* l1taus = 0;
    ATH_CHECK(evtStore()->retrieve(l1taus, "LVL1EmTauRoIs"));

    const xAOD::JetRoIContainer* l1jets = 0;
    ATH_CHECK(evtStore()->retrieve(l1jets, "LVL1JetRoIs"));

    const xAOD::MuonRoIContainer* l1muons = 0;
    ATH_CHECK(evtStore()->retrieve(l1muons, "LVL1MuonRoIs"));

    const xAOD::EnergySumRoI* l1xe = 0;
    ATH_CHECK(evtStore()->retrieve(l1xe, "LVL1EnergySumRoI"));

    // Now actually do something
    ATH_MSG_INFO("Checking " << l1taus->size() << " taus");
    for(const auto& chain: m_l1chainList) { 
        // Set some properties
        if(chain == "L1_TAU12") { 
            ATH_CHECK(this->setProperty("ClusterPt", 12000.));
            ATH_CHECK(this->setProperty("IsolationThresh", -9999.));
        } else if (chain == "L1_TAU12IM") {
            ATH_CHECK(this->setProperty("ClusterPt", 12000.));
            ATH_CHECK(this->setProperty("IsolationOffset", 2000.));
        } else if(chain == "L1_TAU60") {
            ATH_CHECK(this->setProperty("ClusterPt", 60000.));
            ATH_CHECK(this->setProperty("IsolationThresh", -9999.));
        } 
       
        bool passEvent = false;
        for (const auto l1tau : *l1taus) {
            ATH_MSG_INFO("L1 TAU: pt = " << l1tau->tauClus() << ", eta = " << l1tau->eta()
                    << ", phi = " << l1tau->phi());
      
            const auto pass = this->EmulateTauDecision(*l1tau);
            passEvent = passEvent || pass;
            
            if(pass) {
                ATH_MSG_INFO("\t accepted " << chain); 
            } else {
                ATH_MSG_INFO("\t rejected " << chain); 
            }
        }
        
        if(passEvent) {    
            ++m_numL1EmulatedEvents[chain];
            m_h_L1EmulationAccepts->Fill( chain.c_str(), 1 );
        }
    }
    ATH_MSG_INFO("Checking " << l1jets->size() << " jets ");
    for(const auto l1jet:*l1jets){
        auto names = l1jet->thrNames();
        for(const auto& thr:names){
            ATH_MSG_INFO("L1 jet et8x8 " << l1jet->et8x8() << " passing threshold " << thr);
        }
    }

    return StatusCode::SUCCESS;
}

bool TrigEmulatorExAlg::EmulateTauDecision (const xAOD::EmTauRoI& l1tau) const {
    double roi_pt = m_use_emclus ? l1tau.emClus() : l1tau.tauClus();
    // std::cout << "=> testing L1 cut" << std::endl;
    if (roi_pt <= m_roi_pt_cut) {
        // std::cout << "reject L1: did not pass pT cut (" << roi_pt << " <= " << m_roi_pt_cut <<  ")" << std::endl;
        return false;
    }

    if (!pass_isolation(l1tau)) {
        // std::cout << "reject L1: did not pass isolation cut" << std::endl;
        return false;
    }

    if (!pass_hadronic_leakage(l1tau)) {
        // std::cout << "reject L1: did not pass hadronic leakage" << std::endl;
        return false;
    }

    int roi_type = m_use_emclus ? xAOD::EmTauRoI::EMRoIWord : xAOD::EmTauRoI::TauRoIWord;
    if (l1tau.roiType() != roi_type) {
        // std::cout << "reject L1: did not pass RoI type cut" << std::endl;
        return false;
    }

    // std::cout << "PASSED L1 CUTS" << std::endl;

    return true;
}

/* ******************************************************************************************
 * Helper methods for use in isolated tau chains
 * *******************************************************************************************/
bool TrigEmulatorExAlg::pass_isolation(const xAOD::EmTauRoI& l1tau) const {
    // No isolation applied at high-pT
    double roi_pt = m_use_emclus ? l1tau.emClus() : l1tau.tauClus();
    if (roi_pt >= m_iso_thresh) return true;

    float isolation_cut = m_iso_offset + m_iso_slope * roi_pt;
    float roi_isol = l1tau.emIsol();
    if (isolation_cut < m_iso_min) isolation_cut = m_iso_min;
    if (roi_isol <= isolation_cut)
        return true;
    
    return false;
}

bool TrigEmulatorExAlg::pass_hadronic_leakage(const xAOD::EmTauRoI& l1tau) const {
    // when cut is not used
    if (!m_use_had_core) return true;

    // No isolation applied at high-pT
    double roi_pt = m_use_emclus ? l1tau.emClus() : l1tau.tauClus();
    if (roi_pt >= m_iso_thresh) return true;

    float had_leak_cut = m_had_leak_offset + m_had_leak_slope * roi_pt;
    if (had_leak_cut < m_had_leak_min) had_leak_cut = m_had_leak_min;
    if (l1tau.hadCore() <= had_leak_cut)
        return true;
    
    return false;
}

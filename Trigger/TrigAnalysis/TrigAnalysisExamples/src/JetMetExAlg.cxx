/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetMetExAlg.cxx 770492 2016-08-28 16:52:40Z rwhite $
// Place holder for Jet Met trigger analysis for Trigger Tutorial
//
// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/Units.h"

// Trigger EDM object(s):
#include "xAODJet/JetContainer.h"
#include "xAODBase/IParticle.h"
// Local include(s):
#include "JetMetExAlg.h"

using Athena::Units::GeV;

JetMetExAlg::JetMetExAlg( const std::string& name, ISvcLocator *pSvcLocator )
   : AthAlgorithm( name, pSvcLocator ),
    m_eventNr( 0 ),
    m_trigDec( "Trig::TrigDecisionTool/TrigDecisionTool" ),
    m_matchTool( "Trig::MatchingTool/MatchingTool",this),
    m_tah( "Trig::TriggerAnalysisHelper/TriggerAnalysisHelper",this ),
    m_histSvc( "THistSvc", name ) {

        // job option configurable properties
        declareProperty( "L1TriggerList", m_l1chainList);
        declareProperty( "HLTTriggerList", m_hltchainList);
        declareProperty( "dRMax", m_dRMax=0.1);
        declareProperty( "PtCut", m_ptCut = 20. );
        declareProperty( "EtaMax", m_etaMax = 2.4 );
}

StatusCode JetMetExAlg::initialize() {

   CHECK( m_trigDec.retrieve() );
   CHECK( m_matchTool.retrieve() );
   CHECK( m_tah.retrieve() );
   CHECK( m_histSvc.retrieve() );
   m_trigDec->ExperimentalAndExpertMethods()->enable();
   
   //Setup histograms for trigger decision and prescale
   const int nTrigger = (int) m_hltchainList.size();
   h_triggerAccepts = new TH1F( "TriggerAccepts", "TriggerAccepts", nTrigger, 0,  nTrigger);
   h_emulationAccepts = new TH1F( "EmulationAccepts", "EmulationAccepts", nTrigger, 0,  nTrigger);
   h_triggerAcceptsRaw = new TH1F( "TriggerAcceptsRaw", "TriggerAcceptsRaw", nTrigger, 0, nTrigger );
   h_triggerPrescaled = new TH1F( "TriggerPrescaled", "TriggerPrescaled", nTrigger, 0, nTrigger );
   if ( ! m_hltchainList.empty() ){
       for ( int i = 0; i < std::min( (int)m_hltchainList.size(), (int)h_triggerAccepts->GetNbinsX() ); ++i ) {
           int bin = i+1;
           h_triggerAccepts->GetXaxis()->SetBinLabel(bin, m_hltchainList[i].c_str());
           h_emulationAccepts->GetXaxis()->SetBinLabel(bin, m_hltchainList[i].c_str());
           h_triggerAcceptsRaw->GetXaxis()->SetBinLabel(bin, m_hltchainList[i].c_str());
           h_triggerPrescaled->GetXaxis()->SetBinLabel(bin, m_hltchainList[i].c_str());
           ATH_MSG_INFO("setting label X" <<  m_hltchainList[i] << " for bin " << bin);
       }
   }
   CHECK( m_histSvc->regHist( "/Trigger/JetMet/TriggerAccepts", h_triggerAccepts ) );
   CHECK( m_histSvc->regHist( "/Trigger/JetMet/EmulationAccepts", h_emulationAccepts ) );
   CHECK( m_histSvc->regHist( "/Trigger/JetMet/TriggerAcceptsRaw", h_triggerAcceptsRaw ) );
   CHECK( m_histSvc->regHist( "/Trigger/JetMet/TriggerPrescaled", h_triggerPrescaled ) );
   for(const std::string chain:m_hltchainList){
       m_numSelectedEvents[chain]=0;
       m_numL1PassedEvents[chain]=0;
       m_numHLTPassedEvents[chain]=0;
       std::string histName="Eff_et_"+chain;
       h_eff_et[chain] = new TProfile( histName.c_str(), "#epsilon(Et)", 20, 0., 100. );
       CHECK( m_histSvc->regHist( "/Trigger/JetMet/"+histName, h_eff_et[chain] ) );
       histName="Eff_eta_"+chain;
       h_eff_eta[chain] = new TProfile( histName.c_str(), "#epsilon(Et)", 20, (-1.*m_etaMax), m_etaMax );
       CHECK( m_histSvc->regHist( "/Trigger/JetMet/"+histName, h_eff_eta[chain] ) );
   }
   return StatusCode::SUCCESS;
}

StatusCode JetMetExAlg::execute() {

    ++m_eventNr;
   ATH_MSG_INFO( "In JetMetExAlg:: execute"); 

   CHECK(collectTriggerStatistics());
   const xAOD::JetContainer *jetContainer = 0;
   CHECK( evtStore()->retrieve( jetContainer, "AntiKt2PV0TrackJets" ) );    
   CHECK(TriggerAnalysis(jetContainer));
   CHECK(EmulationAnalysis());
   return StatusCode::SUCCESS;
}


StatusCode JetMetExAlg::finalize() {

   // Here we can print out all the information we've learned
   // We need to be a little clever in how we interpret the information
   // presented here.  In particular, remember that each muon has a chance
   // to act as the tag muon, so for chains with very high efficiency
   // its not unreasonable to get a number of tag muons greater than the
   // number of selected events.
   REPORT_MESSAGE( MSG::DEBUG ) << "JetMetExAlg::finalize()";

   ATH_MSG_INFO( "" );
   ATH_MSG_INFO( "JetMetExAlg results" );
   ATH_MSG_INFO( "L1 Chain name: " << m_l1chainList );
   ATH_MSG_INFO( "HLT Chain name: " << m_hltchainList );
   ATH_MSG_INFO( "Selection cuts: " );
   ATH_MSG_INFO( "jet pT > " << m_ptCut << " GeV" );
   ATH_MSG_INFO( "lepton |eta| < " << m_etaMax );
   for(const std::string chain:m_hltchainList){
       ATH_MSG_INFO( " Chain " << chain);
       ATH_MSG_INFO( "Number of selected events: " << m_numSelectedEvents[chain] );
       ATH_MSG_INFO( "Number of events with a L1 passed probe muon: " << m_numL1PassedEvents[chain] );
       ATH_MSG_INFO( "Number of events with a HLT passed probe muon: " << m_numHLTPassedEvents[chain] );
       ATH_MSG_INFO(" L1 Trigger efficiency (relative to offline): " 
               << (float)m_numL1PassedEvents[chain]/(float)m_numSelectedEvents[chain] * 100. << "%" );
       ATH_MSG_INFO( " HLT trigger efficiency (relative to Offline): " 
               << (float)m_numHLTPassedEvents[chain]/(float)m_numSelectedEvents[chain] * 100. << "%" );
       ATH_MSG_INFO( " HLT trigger efficiency (relative to Level 1): " 
               << (float)m_numHLTPassedEvents[chain]/(float)m_numL1PassedEvents[chain] * 100. << "%" );
       ATH_MSG_INFO( "" );
   }

   

   return StatusCode::SUCCESS;
}

StatusCode JetMetExAlg::collectTriggerStatistics() {

   // Now we'd like to collect some trigger statistics for the chains specified 
    for(const auto chain : m_hltchainList){
        // Get the bits, this tell us more 
        const unsigned int bits = m_trigDec->isPassedBits(chain);
        bool efprescale=bits & TrigDefs::EF_prescaled;
        // bool rerun=bits&TrigDefs::EF_resurrected;
        // bool passThrough=bits&TrigDefs::EF_passThrough;
        bool passedRaw=bits&TrigDefs::EF_passedRaw;

        // What about the L1?
        bool tbp = bits&TrigDefs::L1_isPassedBeforePrescale;
        bool tap = bits&TrigDefs::L1_isPassedAfterPrescale;
        // bool tav = bits&TrigDefs::L1_isPassedAfterVeto;

        // What is the combined prescale result
        bool l1prescale=tbp && !tap;
        bool prescale=efprescale || l1prescale;

        if( m_trigDec->isPassed( chain ) )
            h_triggerAccepts->Fill( chain.c_str(), 1 );
        if(prescale)
            h_triggerPrescaled->Fill( chain.c_str(), 1 );
        if(passedRaw)
            h_triggerAcceptsRaw->Fill( chain.c_str(), 1);
    }

   return StatusCode::SUCCESS;
}

/* ******************************************************************************************
 * TriggerAnalysis for JetMet 
 * *******************************************************************************************/
StatusCode JetMetExAlg::TriggerAnalysis (const xAOD::IParticleContainer *cont){
    bool isTriggered=false; 
    for(const auto chain : m_hltchainList)
        if( m_trigDec->isPassed( chain ) ) isTriggered=true;


    // Now perform trigger analysis
    ATH_MSG_INFO(" Trigger efficiency with " << cont->size() << " candidates " << " decision " << isTriggered);
    float probeEt=0.;
    float probeEta=0.;
    for(const xAOD::IParticle *obj:*cont){
        if(!passObjectSelection(obj)) continue;
        probeEta=obj->eta();
        probeEt=(obj->e()/obj->eta())/GeV;
        ATH_MSG_INFO(" Probe et: " << probeEt << " eta: " << probeEta);
        for(const std::string chain:m_hltchainList){
            ATH_MSG_INFO("Mathcing to chain " << chain);
            m_numSelectedEvents[chain]+=1;
            if(!passHLT(*obj,chain)){
                h_eff_et[chain]->Fill(probeEt,0);
                h_eff_eta[chain]->Fill(probeEta,0);
            } 
            else {
                m_numHLTPassedEvents[chain]+=1;
                h_eff_et[chain]->Fill(probeEt,1);
                h_eff_eta[chain]->Fill(probeEta,1);
            } // Finish trigger analysis
        } // Complete chain analysis
    } // Loop over probes

    return StatusCode::SUCCESS;
}


// Try some simple trigger emulation 
StatusCode JetMetExAlg::EmulationAnalysis(){

    return StatusCode::SUCCESS;
}
// Does the reco'd muon pass L1 of the chain we're interested in?
bool JetMetExAlg::passL1( const xAOD::IParticle &recoObj, const std::string chain ) {
    ATH_MSG_DEBUG("Type " << recoObj.type() << " Chain " << chain);
    //L1 matching, requires custom matching
   return false;  
}

// Does the reco'd object pass HLT of the chain we're interested in?
// And the reco'd object is matched to a triggered object
bool JetMetExAlg::passHLT( const xAOD::IParticle &recoObj, const std::string chain) {
    return m_matchTool->match(recoObj,chain,m_dRMax);
}

// Determines lepton object type and calls appropiate selection (passElectron, passMuon)
bool JetMetExAlg::passObjectSelection(const xAOD::IParticle *obj){
    if(obj==nullptr) return false;
    return true;
}


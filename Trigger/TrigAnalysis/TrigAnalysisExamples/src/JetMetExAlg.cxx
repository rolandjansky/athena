/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetMetExAlg.cxx 770492 2016-08-28 16:52:40Z rwhite $
// Place holder for Jet Met trigger analysis for Trigger Tutorial
//
// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/Units.h"

// Trigger EDM object(s):
#include "xAODJet/JetContainer.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODBase/IParticle.h"
#include "xAODTrigMissingET/TrigMissingETContainer.h"
#include "xAODTrigger/EnergySumRoI.h"
#include "xAODTrigger/JetRoIContainer.h"
#include "TrigT1Interfaces/L1METvalue.h"
// Local include(s):
#include "JetMetExAlg.h"

using Athena::Units::GeV;

JetMetExAlg::JetMetExAlg( const std::string& name, ISvcLocator *pSvcLocator )
   : AthAlgorithm( name, pSvcLocator ),
    m_eventNr( 0 ),
    m_trigDec( "Trig::TrigDecisionTool/TrigDecisionTool" ),
    m_matchTool( "Trig::MatchingTool/MatchingTool",this),
    m_tah( "Trig::TriggerAnalysisHelper/TriggerAnalysisHelper",this ),
    m_histSvc( "THistSvc", name ),
    m_h_triggerAccepts( nullptr ),
    m_h_triggerAcceptsRaw( nullptr ),
    m_h_triggerPrescaled( nullptr ),
    m_h_emulationAccepts( nullptr )
{
        // job option configurable properties
        declareProperty( "L1TriggerList", m_l1chainList);
        declareProperty( "HLTTriggerList", m_hltchainList);
        declareProperty( "HLTJetTriggers", m_hltjetList);
        declareProperty( "HLTMETTriggers", m_hltmetList);
        declareProperty( "dRMax", m_dRMax=0.1);
        declareProperty( "PtCut", m_ptCut = 20. );
        declareProperty( "EtaMax", m_etaMax = 2.4 );
}

StatusCode JetMetExAlg::initialize() {

   ATH_MSG_INFO( "JB: Test" );

   CHECK( m_trigDec.retrieve() );
   CHECK( m_matchTool.retrieve() );
   CHECK( m_tah.retrieve() );
   CHECK( m_histSvc.retrieve() );
   m_trigDec->ExperimentalAndExpertMethods()->enable();
   
   //Setup histograms for trigger decision and prescale
   const int nTrigger = (int) m_hltchainList.size();
   m_h_triggerAccepts = new TH1F( "TriggerAccepts", "TriggerAccepts", nTrigger, 0,  nTrigger);
   m_h_emulationAccepts = new TH1F( "EmulationAccepts", "EmulationAccepts", nTrigger, 0,  nTrigger);
   m_h_triggerAcceptsRaw = new TH1F( "TriggerAcceptsRaw", "TriggerAcceptsRaw", nTrigger, 0, nTrigger );
   m_h_triggerPrescaled = new TH1F( "TriggerPrescaled", "TriggerPrescaled", nTrigger, 0, nTrigger );
   if ( ! m_hltchainList.empty() ){
       for ( int i = 0; i < std::min( (int)m_hltchainList.size(), (int)m_h_triggerAccepts->GetNbinsX() ); ++i ) {
           int bin = i+1;
           m_h_triggerAccepts->GetXaxis()->SetBinLabel(bin, m_hltchainList[i].c_str());
           m_h_emulationAccepts->GetXaxis()->SetBinLabel(bin, m_hltchainList[i].c_str());
           m_h_triggerAcceptsRaw->GetXaxis()->SetBinLabel(bin, m_hltchainList[i].c_str());
           m_h_triggerPrescaled->GetXaxis()->SetBinLabel(bin, m_hltchainList[i].c_str());
           ATH_MSG_INFO("setting label X" <<  m_hltchainList[i] << " for bin " << bin);
       }
   }
   CHECK( m_histSvc->regHist( "/Trigger/JetMet/TriggerAccepts", m_h_triggerAccepts ) );
   CHECK( m_histSvc->regHist( "/Trigger/JetMet/EmulationAccepts", m_h_emulationAccepts ) );
   CHECK( m_histSvc->regHist( "/Trigger/JetMet/TriggerAcceptsRaw", m_h_triggerAcceptsRaw ) );
   CHECK( m_histSvc->regHist( "/Trigger/JetMet/TriggerPrescaled", m_h_triggerPrescaled ) );
   for(const std::string& chain:m_hltchainList){
       ATH_MSG_INFO( chain );
       m_numSelectedEvents[chain]=0;
       m_numL1PassedEvents[chain]=0;
       m_numHLTPassedEvents[chain]=0;
       std::string histName="Eff_et_"+chain;
       m_h_eff_et[chain] = new TProfile( histName.c_str(), "#epsilon(Et)", 20, 0., 100. );
       CHECK( m_histSvc->regHist( "/Trigger/JetMet/"+histName, m_h_eff_et[chain] ) );
       histName="Eff_eta_"+chain;
       m_h_eff_eta[chain] = new TProfile( histName.c_str(), "#epsilon(Et)", 20, (-1.*m_etaMax), m_etaMax );
       CHECK( m_histSvc->regHist( "/Trigger/JetMet/"+histName, m_h_eff_eta[chain] ) );
   }
   ATH_MSG_INFO( "JB: " << m_hltmetList.size() << " met triggers");
   for (const std::string& chain : m_hltmetList) {
     ATH_MSG_INFO( "JB: " << chain );
     // Create TEfficiency objects
     std::string histName = "Eff_xe_"+chain;
     m_h_eff_xe[chain] = new TProfile(histName.c_str(), ";E_{T}^{miss} (offline reference) [GeV]", 400, 0, 400);
     CHECK( m_histSvc->regHist( "/Trigger/JetMet/"+histName, m_h_eff_xe[chain]  ) );
   }
   ATH_MSG_INFO( "JB: " << m_hltjetList.size() << " jet triggers" );
   for (const std::string& chain : m_hltjetList) {
     ATH_MSG_INFO( "JB: " << chain );
     std::string histName = "Eff_jpt_"+chain;
     m_h_eff_jpt[chain] = new TProfile(histName.c_str(), ";Leading jet p_{T} (offline uncalibrated) [GeV]", 400, 0, 400);
     CHECK( m_histSvc->regHist( "/Trigger/JetMet/"+histName, m_h_eff_jpt[chain]  ) );
   }


   return StatusCode::SUCCESS;
}

StatusCode JetMetExAlg::execute() {

    ++m_eventNr;
   ATH_MSG_INFO( "In JetMetExAlg:: execute"); 

   CHECK(collectTriggerStatistics());
   const xAOD::JetContainer *jetContainer = 0;
   CHECK( evtStore()->retrieve( jetContainer, "AntiKt4EMTopoJets") );
   CHECK(TriggerAnalysis(jetContainer));
   CHECK(EmulationAnalysis());
   CHECK(FillEfficiency() );
   ATH_MSG_INFO( "JB : finished execute" );
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
   for(const std::string& chain:m_hltchainList){
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

/*
   // Here we create cumulative efficiency graphs from each of the TEfficiency objects we have at the moment
   // The difference between this and what we have already is subtle - for each point in our TEfficiency object
   // the y-value is the efficiency *in* this bin - i.e. the trigger efficiency for an analysis cut of met == cutValue
   // Of course all analyses will use a cut of the form met > cutValue - to get the turn on curve to reflect this you 
   // need a cumulative numerator and denominator
   for (const auto& effPair : m_h_eff_jpt) {
     std::string histName = "Eff_jpt_cumulative_"+effPair.first;
     TH1* numerator = effPair.second->GetPassedHistogram()->GetCumulative();
     TH1* denominator = effPair.second->GetTotalHistogram()->GetCumulative();
     CHECK( m_histSvc->regHist( "/Trigger/JetMet/"+histName+"_total", denominator) );
     CHECK( m_histSvc->regHist( "/Trigger/JetMet/"+histName+"_passed", numerator) );
     TEfficiency* eff_cumulative = new TEfficiency(*numerator, *denominator);
     CHECK( m_histSvc->regGraph( "/Trigger/JetMet/"+histName, reinterpret_cast<TGraph*>(eff_cumulative) ) );
   }
   for (const auto& effPair : m_h_eff_xe) {
     std::string histName = "Eff_xe_cumulative_"+effPair.first;
     TH1* numerator = effPair.second->GetPassedHistogram()->GetCumulative();
     TH1* denominator = effPair.second->GetTotalHistogram()->GetCumulative();
     CHECK( m_histSvc->regHist( "/Trigger/JetMet/"+histName+"_total", denominator) );
     CHECK( m_histSvc->regHist( "/Trigger/JetMet/"+histName+"_passed", numerator) );
     TEfficiency* eff_cumulative = new TEfficiency(*numerator, *denominator);
     CHECK( m_histSvc->regGraph( "/Trigger/JetMet/"+histName, reinterpret_cast<TGraph*>(eff_cumulative) ) );
   }
*/

   return StatusCode::SUCCESS;
}

StatusCode JetMetExAlg::collectTriggerStatistics() {

   // Now we'd like to collect some trigger statistics for the chains specified 
    for(const auto& chain : m_hltchainList){
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
            m_h_triggerAccepts->Fill( chain.c_str(), 1 );
        if(prescale)
            m_h_triggerPrescaled->Fill( chain.c_str(), 1 );
        if(passedRaw)
            m_h_triggerAcceptsRaw->Fill( chain.c_str(), 1);
    }

   return StatusCode::SUCCESS;
}

/* ******************************************************************************************
 * TriggerAnalysis for JetMet 
 * *******************************************************************************************/
StatusCode JetMetExAlg::TriggerAnalysis (const xAOD::IParticleContainer *cont){
    bool isTriggered=false; 
    for(const auto& chain : m_hltchainList)
        if( m_trigDec->isPassed( chain ) ) isTriggered=true;


    // Now perform trigger analysis
    ATH_MSG_INFO(" Trigger efficiency with " << cont->size() << " candidates " << " decision " << isTriggered);
    float probeEt=0.;
    float probeEta=0.;
    for(const xAOD::IParticle *obj:*cont){
        if(!passObjectSelection(obj)) continue;
        probeEta=obj->eta();
        probeEt=(obj->e()/obj->eta())*0.001; //GeV
        ATH_MSG_INFO(" Probe et: " << probeEt << " eta: " << probeEta);
        for(const std::string& chain:m_hltchainList){
            ATH_MSG_INFO("Matching to chain " << chain);
            m_numSelectedEvents[chain]+=1;
            if(!passHLT(*obj,chain)){
                m_h_eff_et[chain]->Fill(probeEt,0);
                m_h_eff_eta[chain]->Fill(probeEta,0);
            } 
            else {
                m_numHLTPassedEvents[chain]+=1;
                m_h_eff_et[chain]->Fill(probeEt,1);
                m_h_eff_eta[chain]->Fill(probeEta,1);
            } // Finish trigger analysis
        } // Complete chain analysis
    } // Loop over probes

    return StatusCode::SUCCESS;
}


// Try some simple trigger emulation 
StatusCode JetMetExAlg::EmulationAnalysis(){

    float GeV(0.001);

    // MET trigger emulation
    // Instructions provided here: https://twiki.cern.ch/twiki/bin/view/Atlas/METTriggerEmulation
    std::string l1MetName("LVL1EnergySumRoI");
    const xAOD::EnergySumRoI* l1MetCont(0);
    ATH_CHECK( evtStore()->retrieve(l1MetCont, l1MetName) );

    float l1_mex = l1MetCont->exMiss() * GeV;
    float l1_mey = l1MetCont->eyMiss() * GeV;
    float l1_met_approx = sqrt(l1_mex*l1_mex + l1_mey*l1_mey); // The approximate L1 MET

    float l1_met(0.);
    bool overflow(false);
    static LVL1::L1METvalue l1calc;
    l1calc.calcL1MET(l1_mex, l1_mey, l1_met, overflow);

    bool L1_XE50 = overflow || l1_met * GeV > 50; // Either overflow or cut at 50 GeV
 
    ATH_MSG_INFO("L1 emulated MET " << l1_met << " approx. " << l1_met_approx);
    // HLT
    // trigger container names
    std::string cellName = "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET";
    std::string mhtName = cellName + "_mht";
    std::string pufitName = cellName + "_topocl_PUC";

    const xAOD::TrigMissingETContainer* cellCont(0);
    const xAOD::TrigMissingETContainer* mhtCont(0);
    const xAOD::TrigMissingETContainer* pufitCont(0);

    ATH_CHECK( evtStore()->retrieve(cellCont, cellName) );
    ATH_CHECK( evtStore()->retrieve(mhtCont, mhtName) );
    ATH_CHECK( evtStore()->retrieve(pufitCont, pufitName) );

    // Note that the trigger containers only have one entry
    float cell_mex = cellCont->front()->ex() * GeV;
    float cell_mey = cellCont->front()->ey() * GeV;
    float cell_met = sqrt(cell_mex*cell_mex + cell_mey*cell_mey);

    float mht_mex = mhtCont->front()->ex() * GeV;
    float mht_mey = mhtCont->front()->ey() * GeV;
    float mht_met = sqrt(mht_mex*mht_mex + mht_mey*mht_mey);

    float pufit_mex = pufitCont->front()->ex() * GeV;
    float pufit_mey = pufitCont->front()->ey() * GeV;
    float pufit_met = sqrt(pufit_mex*pufit_mex + pufit_mey*pufit_mey);

    // pure mht trigger
    bool HLT_xe110_mht_L1XE50 = L1_XE50 && mht_met > 110;
    ATH_MSG_INFO( "HLT_xe110_mht_L1XE50 " << (HLT_xe110_mht_L1XE50 ? "passed." : "failed." ) 
            << " w/ mht_met " << mht_met);
    
    // pure pufit trigger
    bool HLT_xe110_pufit_L1XE50 = L1_XE50 && pufit_met > 110;
    ATH_MSG_INFO( "HLT_xe110_pufit_L1XE50 " << (HLT_xe110_pufit_L1XE50 ? "passed." : "failed." ) );

    // combined mht+cell trigger
    bool HLT_xe110_mht_L1XE50_AND_HLT_xe70_L1XE50 = L1_XE50 && mht_met > 110 && cell_met > 70;
    ATH_MSG_INFO( "HLT_xe110_mht_L1XE50_AND_HLT_xe70_L1XE50 " << (HLT_xe110_mht_L1XE50_AND_HLT_xe70_L1XE50 ? "passed." : "failed." ) 
            << " w/ mht_met, cell_met" << mht_met << " " << cell_met);


    // Jet trigger emulation
    // Follow instructions from https://twiki.cern.ch/twiki/bin/view/Atlas/JetTriggerEmulation
    // L1
    std::string l1JetName("LVL1JetRoIs");
    const xAOD::JetRoIContainer* l1JetCont(0);
    bool L1_J100(false);
    unsigned int L1_nJ15(0);
    ATH_CHECK( evtStore()->retrieve(l1JetCont, l1JetName) );
    for (const auto l1Jet : *l1JetCont) {
      if (l1Jet->et8x8() * GeV > 100) L1_J100 = true;
      if (l1Jet->et8x8() * GeV > 15) ++L1_nJ15;
    }
    bool L1_4J15 = L1_nJ15 >= 4;

    // HLT
    std::string hltJetName("HLT_xAOD__JetContainer_a4tcemsubjesFS");
    const xAOD::JetContainer* hltJetCont(0);
    ATH_CHECK( evtStore()->retrieve(hltJetCont, hltJetName) );
    unsigned int HLT_nj380(0);
    unsigned int HLT_nj45(0);
    unsigned int HLT_nj45_0eta240(0);
    for (const auto hltJet : *hltJetCont) {
      if (hltJet->pt() * GeV > 380) ++HLT_nj380;
      if (hltJet->pt() * GeV > 45) {
        ++HLT_nj45;
        if (fabs(hltJet->eta() ) < 2.4) ++HLT_nj45_0eta240;
      }
    }
    // single jet trigger
    bool HLT_j380 = L1_J100 && HLT_nj380 >= 1;
    ATH_MSG_INFO( "HLT_j380 " << (HLT_j380 ? "passed." : "failed." ) );

    // multijet trigger
    bool HLT_7j45_L14J15 = L1_4J15 && HLT_nj45 >= 7;
    ATH_MSG_INFO( "HLT_7j45_L14J15 " << (HLT_7j45_L14J15 ? "passed." : "failed." ) );

    // multijet eta restricted trigger
    bool HLT_6j45_0eta240_L14J15 = L1_4J15 && HLT_nj45_0eta240 >= 6;
    ATH_MSG_INFO( "HLT_6j45_0eta240_L14J15 " << (HLT_6j45_0eta240_L14J15 ? "passed." : "failed." ) );

    return StatusCode::SUCCESS;
}
// Does the reco'd muon pass L1 of the chain we're interested in?
bool JetMetExAlg::passL1( const xAOD::IParticle &recoObj, const std::string &chain ) {
    ATH_MSG_DEBUG("Type " << recoObj.type() << " Chain " << chain);
    //L1 matching, requires custom matching
   return false;  
}

// Does the reco'd object pass HLT of the chain we're interested in?
// And the reco'd object is matched to a triggered object
bool JetMetExAlg::passHLT( const xAOD::IParticle &recoObj, const std::string &chain) {
    ATH_MSG_INFO("Object energy " << recoObj.e());
    return m_trigDec->isPassed(chain);
    //return m_matchTool->match(recoObj,chain,m_dRMax);
}

// Determines lepton object type and calls appropiate selection (passElectron, passMuon)
bool JetMetExAlg::passObjectSelection(const xAOD::IParticle *obj){
    if(obj==nullptr) return false;
    return true;
}

StatusCode JetMetExAlg::FillEfficiency() {
  ATH_MSG_INFO("JB: in FillEfficiency");
  // For these examples we use an orthogonal trigger (in this case a single muon trigger) to ensure an unbiased sample for the turn on curve
  if (m_trigDec->isPassed("HLT_mu26_ivarmedium") || m_trigDec->isPassed("HLT_e26_lhtight_nod0_ivarloose")) {
    ATH_MSG_INFO("Mu trigger passed!");
    const xAOD::JetContainer* offlineJets(0);
    // In a real analysis you would have calibrated and selected these - in the interests of time and simplicity we just use the initial offline collection for this tutorial
    ATH_CHECK( evtStore()->retrieve(offlineJets, "AntiKt4EMTopoJets") );
    if (offlineJets->size() > 0) {// Ensure that the event has at least 1 jet
      float leadingJetPt = offlineJets->at(0)->pt() * 0.001; // Convert MeV -> GeV
      for (auto& effPair : m_h_eff_jpt) { //auto -> std::pair<std::string, TEfficiency*>
        ATH_MSG_INFO(effPair.first);
        if(m_trigDec->isPassed(effPair.first))
            effPair.second->Fill(leadingJetPt,1); 
        else 
            effPair.second->Fill(leadingJetPt,0); 
      }
    }
    ATH_MSG_INFO("Filling met hist");
    const xAOD::MissingETContainer* offlineMET(0);
    // In a real analysis you would rebuild your MET with calibrated and selected objects!
    ATH_CHECK( evtStore()->retrieve(offlineMET, "MET_Reference_AntiKt4EMTopo") );
    auto metItr = offlineMET->find("FinalTrk"); // This is the name of the met (with TST) in the reference containers
    if (metItr == offlineMET->end() ) {
      ATH_MSG_WARNING( "FinalTrk not available in MET_Reference_AntiKt4EMTopo container, skipping MET efficiency" );
      return StatusCode::SUCCESS;
    }
    float metValue = (*metItr)->met() * 0.001;
    for (auto& effPair : m_h_eff_xe) { //auto -> std::pair<std::string, TEfficiency*>
      ATH_MSG_INFO(effPair.first);
      //effPair.second->Fill(m_trigDec->isPassed(effPair.first), metValue); // TEfficiency::Fill(bool isPassed, Double_t x)
      if(m_trigDec->isPassed(effPair.first))
          effPair.second->Fill(metValue,1); 
      else 
          effPair.second->Fill(metValue,0); 
    }
  }
  ATH_MSG_INFO( "Trigger not passed!");
  return StatusCode::SUCCESS;
}



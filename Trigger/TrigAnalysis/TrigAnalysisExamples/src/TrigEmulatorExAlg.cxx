/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigEmulatorExAlg.cxx 770492 2016-08-28 16:52:40Z rwhite $
// Place holder for TrigEmulator trigger analysis for Trigger Tutorial
//
// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/Units.h"

// Local include(s):
#include "TrigEmulatorExAlg.h"

using Athena::Units::GeV;

TrigEmulatorExAlg::TrigEmulatorExAlg( const std::string& name, ISvcLocator *pSvcLocator )
   : AthAlgorithm( name, pSvcLocator ),
    m_eventNr( 0 ),
    m_doEg(false),
    m_trigDec( "Trig::TrigDecisionTool/TrigDecisionTool" ),
    m_matchTool( "Trig::MatchingTool/MatchingTool",this),
    m_tah( "Trig::TriggerAnalysisHelper/TriggerAnalysisHelper",this ),
    m_histSvc( "THistSvc", name ) {

        // job option configurable properties
        declareProperty( "L1TriggerList", m_l1chainList);
        declareProperty( "HLTTriggerList", m_hltchainList);
}

StatusCode TrigEmulatorExAlg::initialize() {

   CHECK( m_trigDec.retrieve() );
   CHECK( m_matchTool.retrieve() );
   CHECK( m_tah.retrieve() );
   CHECK( m_histSvc.retrieve() );

   m_trigDec->ExperimentalAndExpertMethods()->enable();
   
   //Setup histograms for trigger decision and prescale
   const int nTrigger = (int) m_hltchainList.size();
   h_triggerAccepts = new TH1F( "TriggerAccepts", "TriggerAccepts", nTrigger, 0,  nTrigger);
   h_emulationAccepts = new TH1F( "EmulationAccepts", "EmulationAccepts", nTrigger, 0,  nTrigger);
   if ( ! m_hltchainList.empty() ){
       for ( int i = 0; i < std::min( (int)m_hltchainList.size(), (int)h_triggerAccepts->GetNbinsX() ); ++i ) {
           int bin = i+1;
           h_triggerAccepts->GetXaxis()->SetBinLabel(bin, m_hltchainList[i].c_str());
           h_emulationAccepts->GetXaxis()->SetBinLabel(bin, m_hltchainList[i].c_str());
           ATH_MSG_INFO("setting label X" <<  m_hltchainList[i] << " for bin " << bin);
       }
   }
   CHECK( m_histSvc->regHist( "/Trigger/Emulation/TriggerAccepts", h_triggerAccepts ) );
   CHECK( m_histSvc->regHist( "/Trigger/Emulation/EmulationAccepts", h_emulationAccepts ) );
   for(const std::string chain:m_hltchainList){
       m_numSelectedEvents[chain]=0;
       m_numL1PassedEvents[chain]=0;
       m_numHLTPassedEvents[chain]=0;
       m_numL1EmulatedEvents[chain]=0;
       m_numHLTEmulatedEvents[chain]=0;
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
   for(const std::string chain:m_hltchainList){
       ATH_MSG_INFO( " Chain " << chain);
       ATH_MSG_INFO( "Number of selected events: " << m_numSelectedEvents[chain] );
       ATH_MSG_INFO( "Number of events with a L1 passed probe muon: " << m_numL1PassedEvents[chain] );
       ATH_MSG_INFO( "Number of events HLT fired: " << m_numHLTPassedEvents[chain] );
       ATH_MSG_INFO( " Ratio of triggered events to emulated events:" 
               << (float)m_numHLTPassedEvents[chain]/(float)m_numHLTEmulatedEvents[chain] * 100. << "%" );
       ATH_MSG_INFO( "" );
   }

   

   return StatusCode::SUCCESS;
}

StatusCode TrigEmulatorExAlg::collectTriggerStatistics() {

   // Now we'd like to collect some trigger statistics for the chains specified 
    for(const auto chain : m_hltchainList){
        if( m_trigDec->isPassed( chain ) ){
            h_triggerAccepts->Fill( chain.c_str(), 1 );
            m_numHLTPassedEvents[chain]+=1;
        }
    }

   return StatusCode::SUCCESS;
}

/* ******************************************************************************************
 * TriggerAnalysis for TrigEmulator 
 * *******************************************************************************************/
StatusCode TrigEmulatorExAlg::EmulationAnalysis (){
    return StatusCode::SUCCESS;
}



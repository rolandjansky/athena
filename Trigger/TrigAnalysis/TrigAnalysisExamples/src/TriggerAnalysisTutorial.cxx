/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerAnalysisTutorial.cxx 778540 2016-10-14 15:39:47Z ssnyder $

// System include(s):
#include <iomanip>

// ROOT include(s):
#include "TTree.h"
#include "TH1F.h"

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/Units.h"


// TrigDecisionTool include(s):
#include "TrigDecisionTool/ChainGroup.h"
#include "TrigDecisionTool/FeatureContainer.h"
#include "TrigDecisionTool/Feature.h"

#include "xAODEventInfo/EventInfo.h"

// Offline EDM include(s):
#include "xAODEgamma/ElectronContainer.h"

// Local include(s):
#include "TriggerAnalysisTutorial.h"

using Athena::Units::GeV;

TriggerAnalysisTutorial::TriggerAnalysisTutorial( const std::string &name,
                                                  ISvcLocator *pSvcLocator)
   : AthAlgorithm( name, pSvcLocator ),
     m_eventNr( 0 ),
     //m_eventsPrinted( 0 ),
     m_trigDec( "Trig::TrigDecisionTool/TrigDecisionTool" ),
     m_matchTool( "Trig::MatchingTool/MatchingTool", this ),
     m_tah( "Trig::TriggerAnalysisHelper/TriggerAnalysisHelper",this ),
     m_bcTool( "Trig::TrigConfBunchCrossingTool/BunchCrossingTool" ),
     m_histSvc( "THistSvc", name ) {
         
         declareProperty( "TriggerList", m_chain_names, "List of triggers to analyze");
}

TriggerAnalysisTutorial::~TriggerAnalysisTutorial()
{
}

StatusCode TriggerAnalysisTutorial::initialize() {

   // retrieve the tools and services:
   CHECK( m_trigDec.retrieve() );
   CHECK( m_matchTool.retrieve() );
   //CHECK( m_bcTool.retrieve() );
   CHECK( m_histSvc.retrieve() );
   m_trigDec->ExperimentalAndExpertMethods()->enable();
   // done
   ATH_MSG_INFO( "Initialization successful" );

   return StatusCode::SUCCESS;
}

StatusCode TriggerAnalysisTutorial::finalize() {

   ATH_MSG_INFO( "STAT Trigger Statistics on " << m_eventNr << " processed events" );


   return StatusCode::SUCCESS;
}

StatusCode TriggerAnalysisTutorial::execute() {

   ++m_eventNr;

   ATH_MSG_INFO( "Event Number " << m_eventNr );
   // Get list of configured chains on first event
   // Check HLT chains are configured and push_back on cfg_chains
   if(m_eventNr==1){
       std::vector<std::string> allHLT = m_trigDec->getChainGroup("HLT_.*")->getListOfTriggers();
       // Create list of configured chains from input list
       for(const std::string chain:allHLT){
           if(std::find(m_chain_names.begin(), m_chain_names.end(), chain) != m_chain_names.end()){ 
               ATH_MSG_INFO("Found corresponding chain in list " << chain); 
               m_cfg_chains.push_back(chain);
           }
       }
   }

   return StatusCode::SUCCESS;
}


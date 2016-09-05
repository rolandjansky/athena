/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerAnalysisTutorial.cxx 771843 2016-09-05 16:56:27Z rwhite $

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
     m_eventsPrinted( 0 ),
     m_trigDec( "Trig::TrigDecisionTool/TrigDecisionTool" ),
     m_matchTool( "Trig::MatchingTool/MatchingTool", this ),
     m_tah( "Trig::TriggerAnalysisHelper/TriggerAnalysisHelper",this ),
     m_bcTool( "Trig::TrigConfBunchCrossingTool/BunchCrossingTool" ),
     m_histSvc( "THistSvc", name ) {

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

   return StatusCode::SUCCESS;
}


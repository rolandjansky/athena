///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthAnalysisAlgorithm.cxx 
// Implementation file for class AthAnalysisAlgorithm
// Exactly like an AthAlgorithm except also has Metadata accessors
// and beginInputFile method
// Author: W.Buttinger<will@cern.ch>
/////////////////////////////////////////////////////////////////// 

#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"

AthAnalysisAlgorithm::AthAnalysisAlgorithm( const std::string& name, 
			    ISvcLocator* pSvcLocator,
			    const std::string& version ) : 
        AthAlgorithm(name,pSvcLocator,version),
        m_inputMetaStore( "StoreGateSvc/InputMetaDataStore", name ),
        m_outputMetaStore( "StoreGateSvc/MetaDataStore", name )
{
  
}

AthAnalysisAlgorithm::~AthAnalysisAlgorithm() { }


ServiceHandle<StoreGateSvc>& AthAnalysisAlgorithm::inputMetaStore() const {
      return m_inputMetaStore;
}

ServiceHandle<StoreGateSvc>& AthAnalysisAlgorithm::outputMetaStore() const {
      return m_outputMetaStore;
}


StatusCode AthAnalysisAlgorithm::sysInitialize() {

      // Connect to the IncidentSvc:
      ServiceHandle< IIncidentSvc > incSvc( "IncidentSvc", name() );
      ATH_CHECK( incSvc.retrieve() );

      // Set up the right callbacks: //but ensure we don't double-register if sysInitialize called twice (appears to be the case)
      incSvc->removeListener( this, IncidentType::BeginInputFile );
      incSvc->addListener( this, IncidentType::BeginInputFile, 0, true );

      // Let the base class do its thing:
      ATH_CHECK( AthAlgorithm::sysInitialize() );

      // Return gracefully:
      return StatusCode::SUCCESS;
}

void AthAnalysisAlgorithm::handle( const Incident& inc ) {

   // Tell the user what's happening:
   ATH_MSG_VERBOSE( "Callback received with incident: " << inc.type() );

   // Call the appropriate member function:
   if( inc.type() == IncidentType::BeginInputFile ) {
      if( beginInputFile().isFailure() ) {
         ATH_MSG_FATAL( "Failed to call beginInputFile()" );
         throw std::runtime_error( "Couldn't call beginInputFile()" );
      }
   } else {
      ATH_MSG_WARNING( "Unknown incident type received: " << inc.type() );
   }

   return;
}

/// Dummy implementation that can be overridden by the derived tool.
///
StatusCode AthAnalysisAlgorithm::beginInputFile() {

   // Return gracefully:
   return StatusCode::SUCCESS;
}



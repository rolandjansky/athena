/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigBjetEtHypo.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetEtHypo
//
// AUTHOR:   Carlo Varni
// EMAIL:    carlo.varni@ge.infn.it
// 
// ************************************************

#include "src/TrigBjetEtHypoTool.h"

TrigBjetEtHypoTool::TrigBjetEtHypoTool( const std::string& type, 
					const std::string& name, 
					const IInterface* parent ) :
  AthAlgTool( type, name, parent ),
  m_id(  HLT::Identifier::fromToolName( name ) ) {}

// -----------------------------------------------------------------------------------------------------------------

TrigBjetEtHypoTool::~TrigBjetEtHypoTool() {}

// -----------------------------------------------------------------------------------------------------------------

StatusCode TrigBjetEtHypoTool::initialize()  {
  ATH_MSG_INFO("Initializing TrigBjetEtHypoTool");
 
   ATH_MSG_DEBUG(  "declareProperty review:"          );
   ATH_MSG_DEBUG(  "    "   <<     m_acceptAll        ); 
   ATH_MSG_DEBUG(  "    "   <<     m_etThreshold      );

  ATH_MSG_DEBUG( "Tool configured for chain/id: " << m_id  );
  return StatusCode::SUCCESS;
}


// ----------------------------------------------------------------------------------------------------------------- 

StatusCode TrigBjetEtHypoTool::decide( const xAOD::JetContainer* jetCollection,bool &pass ) const {
  // Right now only considering single b-jet chains. 
  // Will be revised with higher multeplicity

  ATH_MSG_DEBUG( "Executing "<< name() );
  ATH_MSG_DEBUG( "Evaluating 'decide' on " << jetCollection->size() << " input jets " );

  pass = false;

  // Run on Jet Collection
  for ( const xAOD::Jet* jet : *jetCollection ) {
    ATH_MSG_DEBUG( "EtHypo on Jet " << jet->p4().Et() );
    ATH_MSG_DEBUG( "  Et Threshold "   << m_etThreshold  );

    float et = jet->p4().Et(); 
    
    if ( m_acceptAll ) {
      ATH_MSG_DEBUG( "REGTEST: AcceptAll property is set: taking all events" );
      ATH_MSG_DEBUG( "REGTEST: Trigger decision is 1" );
      pass = true;
    } else {
      ATH_MSG_DEBUG( "REGTEST: AcceptAll property not set: applying the selection" );
      ATH_MSG_DEBUG( "REGTEST: EF jet with et = " << et );
      ATH_MSG_DEBUG( "REGTEST: Requiring EF jets to satisfy 'j' Et > " << m_etThreshold );
      
      if (et >= m_etThreshold)
	pass = true;

      ATH_MSG_DEBUG( "REGTEST: Pass " << pass );
    }
  }

  if ( pass ) {
    ATH_MSG_DEBUG( "Selection cut satisfied, accepting the event" ); 
  } else { 
    ATH_MSG_DEBUG( "Selection cut not satisfied, rejecting the event" ); 
  }
  
  ATH_MSG_DEBUG( "REGTEST: Trigger decision is " << pass );
  return StatusCode::SUCCESS;
}

// ----------------------------------------------------------------------------------------------------------------- 

StatusCode TrigBjetEtHypoTool::finalize()  {
  ATH_MSG_INFO( "Finalizing TrigBjetEtHypoTool" );
  return StatusCode::SUCCESS;
}


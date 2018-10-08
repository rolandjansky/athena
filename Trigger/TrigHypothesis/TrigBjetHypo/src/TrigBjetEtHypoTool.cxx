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
   ATH_MSG_DEBUG(  "    "   <<     m_etCalibration    );
   ATH_MSG_DEBUG(  "    "   <<     m_etThreshold      );
   ATH_MSG_DEBUG(  "    "   <<     m_gscThreshold     );

  ATH_MSG_DEBUG( "Tool configured for chain/id: " << m_id  );
  return StatusCode::SUCCESS;
}


// ----------------------------------------------------------------------------------------------------------------- 

bool TrigBjetEtHypoTool::decide(  const xAOD::Jet* jet ) const {
  ATH_MSG_DEBUG( "Executing TrigBjetEtHypoTool " );
  return true;
  /*
  bool pass = false;

  // AcceptAll declare property setting - no need to save it in TrigPassBit, but instead it will go into decission, where we can save also properties and attach them to objects
  if ( m_acceptAll ) 
    ATH_MSG_DEBUG(  "REGTEST: AcceptAll property is set: taking all events"  );
  else 
    ATH_MSG_DEBUG(  "REGTEST: AcceptAll property not set: applying the selection"  );
  
  ATH_MSG_DEBUG( "EtHypo on Jet " << jet->p4().Et() );
  ATH_MSG_DEBUG( "  Threshold "   << m_etThreshold  );

  float et = jet->p4().Et();

  if ( m_acceptAll ) {
    ATH_MSG_DEBUG( "REGTEST: AcceptAll property is set: taking all events" );
    ATH_MSG_DEBUG( "REGTEST: Trigger decision is 1" );
    pass = true;
  } else {
    ATH_MSG_DEBUG( "REGTEST: EF jet with et = " << et );
    ATH_MSG_DEBUG( "REGTEST: Requiring EF jets to satisfy Et > " << m_etThreshold );
    
    if (et >= m_etThreshold)
      pass = true;

    ATH_MSG_DEBUG( "REGTEST: Pass " << pass );
  }

  if (pass) {
    ATH_MSG_DEBUG( "Selection cut satisfied, accepting the event" );
  } 
  else {
    ATH_MSG_DEBUG( "Selection cut not satisfied, rejecting the event" );
  }
  
  ATH_MSG_DEBUG( "REGTEST: Trigger decision is " << pass );


  return pass;
  */
}


// ----------------------------------------------------------------------------------------------------------------- 


StatusCode TrigBjetEtHypoTool::finalize()  {
  ATH_MSG_INFO( "Finalizing TrigBjetEtHypoTool" );
  return StatusCode::SUCCESS;
}


/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

#include "TrigBjetEtHypoTool.h"

TrigBjetEtHypoTool::TrigBjetEtHypoTool( const std::string& type, 
					const std::string& name, 
					const IInterface* parent ) :
  AthAlgTool( type, name, parent ),
  m_decisionId(  HLT::Identifier::fromToolName( name ) ) {}

// -----------------------------------------------------------------------------------------------------------------

StatusCode TrigBjetEtHypoTool::initialize()  {

  ATH_MSG_DEBUG(  "declareProperty review:"          );
  ATH_MSG_DEBUG(  "    "   <<     m_acceptAll        ); 
  ATH_MSG_DEBUG(  "    "   <<     m_etThreshold      );
  ATH_MSG_DEBUG(  "    "   <<     m_minEtaThreshold  );
  ATH_MSG_DEBUG(  "    "   <<     m_maxEtaThreshold  );

  ATH_MSG_DEBUG( "Tool configured for chain/id: " << m_decisionId  );
  return StatusCode::SUCCESS;
}

// -----------------------------------------------------------------------------------------------------------------

StatusCode TrigBjetEtHypoTool::decide( std::vector< TrigBjetEtHypoToolInfo >& bJetInfos ) const {

  ATH_MSG_DEBUG( "Executing "<< name() );

  for ( TrigBjetEtHypoToolInfo& bJetInfo : bJetInfos ) {

    // Check the HypoTool's chain is active
    if ( not TrigCompositeUtils::passed( getId().numeric(),bJetInfo.previousDecisionIDs ) ) 
      continue;
    
    const xAOD::Jet *jet = *(bJetInfo.jetEL);
    const xAOD::Vertex *vertex = *(bJetInfo.vertexEL);

    ATH_MSG_DEBUG( "Evaluating 'decide' on jet input jets " );
    ATH_MSG_DEBUG( "   ** pt  = " << jet->p4().Et() );
    ATH_MSG_DEBUG( "   ** eta = " << jet->eta() );
    ATH_MSG_DEBUG( "   ** phi = " << jet->phi() );
    
    ATH_MSG_DEBUG( "Event Vertex [x,y,z]=[" 
		   << vertex->x() 
		   << ","<<  vertex->y() 
		   << ","<<  vertex->z() <<  "]" );

    ATH_MSG_DEBUG( "   ** Vertex Type = " << vertex->vertexType() );

    bool pass = true;
    
    if ( vertex->vertexType() != xAOD::VxType::VertexType::PriVtx ) {
      ATH_MSG_DEBUG( "Vertex is not a valid primary vertex!" );
      ATH_MSG_DEBUG( "Trigger decision is FALSE" );
      pass = false;
    } else if ( m_acceptAll ) {
      ATH_MSG_DEBUG( "AcceptAll property is set: taking all events" );
      ATH_MSG_DEBUG( "Trigger decision is TRUE" );
    } else {
      
      ATH_MSG_DEBUG( "AcceptAll property not set: applying the selection" );
      
      // Run on Jet Collection
      float et = jet->p4().Et(); 
      float eta = jet->eta();
      
      ATH_MSG_DEBUG( "EF jet with et = " << et );
      ATH_MSG_DEBUG( "EF jet with eta = " << eta );
      ATH_MSG_DEBUG( "Requiring EF jets to satisfy 'j' Et > " << m_etThreshold );
      ATH_MSG_DEBUG( "Requiring EF jets to satisfy " << m_minEtaThreshold <<" < |Eta| <  " << m_maxEtaThreshold );    
      
      if ( et < m_etThreshold )
	pass = false;
      if ( fabs(eta) < m_minEtaThreshold )
	pass = false;
      if ( fabs(eta) > m_maxEtaThreshold )
	pass = false;
    }

    if ( pass ) {
      ATH_MSG_DEBUG( "Selection cut satisfied, accepting the event" ); 
      TrigCompositeUtils::addDecisionID( getId().numeric(),bJetInfo.decision );
    } else { 
      ATH_MSG_DEBUG( "Selection cut not satisfied, rejecting the event" ); 
    }

    ATH_MSG_DEBUG( "Jet decision is " << (pass?"TRUE":"FALSE") );
    ATH_MSG_DEBUG( "PRINTING DECISION" );
    ATH_MSG_DEBUG( *bJetInfo.decision );  
  }

  return StatusCode::SUCCESS;
}

// ----------------------------------------------------------------------------------------------------------------- 

TrigCompositeUtils::DecisionID TrigBjetEtHypoTool::decisionId() const { return m_decisionId.numeric(); }
const HLT::Identifier TrigBjetEtHypoTool::getId() const { return m_decisionId; }

// ----------------------------------------------------------------------------------------------------------------- 



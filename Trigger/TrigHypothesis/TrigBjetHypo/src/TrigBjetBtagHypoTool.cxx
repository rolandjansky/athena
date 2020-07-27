/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigBjetBtagHypoTool.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypoTool
//
// AUTHOR:   Carlo Varni
// EMAIL:    carlo.varni@cern.ch
// 
// ************************************************

#include "TrigCompositeUtils/HLTIdentifier.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "TrigBjetBtagHypoTool.h"

TrigBjetBtagHypoTool::TrigBjetBtagHypoTool( const std::string& type, 
					      const std::string& name, 
					      const IInterface* parent ) :
  AthAlgTool( type, name, parent ),
  m_decisionId(  HLT::Identifier::fromToolName( name ) ) {}
  
// -----------------------------------------------------------------------------------------------------------------
  
StatusCode TrigBjetBtagHypoTool::initialize()  {

  ATH_MSG_DEBUG(  "declareProperty review:"   );
  ATH_MSG_DEBUG(  "   " << m_acceptAll        ); 
  ATH_MSG_DEBUG(  "   " << m_methodTag        ); 
  ATH_MSG_DEBUG(  "   " << m_bTaggingCut      );
  
  ATH_MSG_DEBUG( "Tool configured for chain/id: " << m_decisionId  );
  return StatusCode::SUCCESS;
}

// ----------------------------------------------------------------------------------------------------------------- 

StatusCode TrigBjetBtagHypoTool::decide( std::vector< TrigBjetBtagHypoToolInfo >& bTagInfos ) const {

  ATH_MSG_DEBUG( "Executing "<< name() );

  for ( TrigBjetBtagHypoToolInfo& bTagInfo : bTagInfos ) {

    // -------------------------------------
    // Check the HypoTool's chain is active
    if ( not TrigCompositeUtils::passed( getId().numeric(),bTagInfo.previousDecisionIDs ) )
      continue;

    // -------------------------------------
    // Check Primary Vertex
    const xAOD::Vertex *vertex = *(bTagInfo.vertexEL);

    ATH_MSG_DEBUG( "Event Vertex [x,y,z]=[" 
		   << vertex->x() 
		   << ","<<  vertex->y() 
		   << ","<<  vertex->z() <<  "]" );

    ATH_MSG_DEBUG( "   ** Vertex Type = " << vertex->vertexType() );

    // -------------------------------------
    // Compute trigger decision
    bool pass = true;

    if ( vertex->vertexType() != xAOD::VxType::VertexType::PriVtx ) {
      ATH_MSG_DEBUG( "Vertex is not a valid primary vertex!" );
      ATH_MSG_DEBUG( "Trigger decision is FALSE" );
      pass = false;
    } else if ( m_acceptAll == true ) {
      ATH_MSG_DEBUG( "AcceptAll property is set: taking all events" );
      ATH_MSG_DEBUG( "Trigger decision is TRUE" );
    } else {
    
      const xAOD::BTagging *btagging = *(bTagInfo.btaggingEL);
      double btaggingWeight = -1000;

      if ( m_methodTag == "MV2c00" or 
	   m_methodTag == "MV2c10" or 
	   m_methodTag == "MV2c20" ) {
	btagging->MVx_discriminant( m_methodTag, btaggingWeight );
      } else {
	ATH_MSG_ERROR( "b-Tagging method has not been recognised: " << m_methodTag.value() );
	return StatusCode::FAILURE;
      }
      
      ATH_MSG_DEBUG( m_methodTag.value() << " = " << btaggingWeight );
      ATH_MSG_DEBUG( "Cutting b-tagging weight at " << m_bTaggingCut.value() );
      
      if ( btaggingWeight < m_bTaggingCut ) 
	pass = false;
    }
    
    // -------------------------------------
    if ( pass == true ) {
      ATH_MSG_DEBUG( "Selection cut satisfied, accepting the event" ); 
      TrigCompositeUtils::addDecisionID( getId().numeric(),bTagInfo.decision );      
    } else {
      ATH_MSG_DEBUG( "Selection cut not satisfied, rejecting the event" ); 
    }

    ATH_MSG_DEBUG( "b-Tagging decision is " << (pass?"TRUE":"FALSE") );
    ATH_MSG_DEBUG( "PRINTING DECISION" );
    ATH_MSG_DEBUG( *bTagInfo.decision );    
  }

  return StatusCode::SUCCESS;
}


// -----------------------------------------------------------------------------------------------------------------

TrigCompositeUtils::DecisionID TrigBjetBtagHypoTool::decisionId() const { return m_decisionId.numeric(); }
const HLT::Identifier TrigBjetBtagHypoTool::getId() const { return m_decisionId; }

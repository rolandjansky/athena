/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigBjetHypoTool.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypoTool
//
// AUTHOR:   Carlo Varni
// EMAIL:    carlo.varni@cern.ch
// 
// ************************************************

#include "DecisionHandling/HLTIdentifier.h"

#include "TrigBjetBtagHypoTool.h"
#include "AthenaMonitoring/Monitored.h"


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
 
  return StatusCode::SUCCESS;
}

StatusCode TrigBjetBtagHypoTool::decide( std::vector< TrigBjetBtagHypoToolInfo >& bTagInfos ) const {

  ATH_MSG_DEBUG( "Executing "<< name() );

  for ( TrigBjetBtagHypoToolInfo& bTagInfo : bTagInfos ) {

    // Check the HypoTool's chain is active
    if ( not TrigCompositeUtils::passed( getId().numeric(),bTagInfo.previousDecisionIDs ) )
      continue;

    // This will disappear!    
    if ( m_acceptAll == true ) {
      ATH_MSG_DEBUG( "Running with 'Accept All' option enabled. Passing cut." );
      TrigCompositeUtils::addDecisionID( getId().numeric(),bTagInfo.decision );

      ATH_MSG_DEBUG("PRINTING DECISION");
      ATH_MSG_DEBUG( *bTagInfo.decision );
      continue;
    }
    
    const xAOD::BTagging *btagging = *(bTagInfo.btaggingEL);

    double btaggingWeight = -1000;
    if ( m_methodTag == "MV2c00" ) 
      btaggingWeight = btagging->auxdata< double >("MV2c00_discriminant");
    else if ( m_methodTag == "MV2c10" ) 
      btaggingWeight = btagging->auxdata< double >("MV2c10_discriminant");
    else if ( m_methodTag == "MV2c20" )
      btaggingWeight = btagging->auxdata< double >("MV2c20_discriminant");
    else {
      ATH_MSG_ERROR( "b-Tagging method has not been recognised: " << m_methodTag.value() );
      return StatusCode::FAILURE;
    }
    
    ATH_MSG_DEBUG( m_methodTag.value() << " = " << btaggingWeight );
    ATH_MSG_DEBUG( "Cutting b-tagging eight at " << m_bTaggingCut.value() );

    if ( btaggingWeight > m_bTaggingCut ) {
      ATH_MSG_DEBUG( "   --> Passed" );
      TrigCompositeUtils::addDecisionID( getId().numeric(),bTagInfo.decision );
    } else {
      ATH_MSG_DEBUG( "   --> Failed" );
    }

    ATH_MSG_DEBUG("PRINTING DECISION");
    ATH_MSG_DEBUG( *bTagInfo.decision );    
  }

  return StatusCode::SUCCESS;
}


// -----------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode TrigBjetBtagHypoTool::retrieveTool( const std::string& toolName,PublicToolHandle< T >& tool) {
  ATH_MSG_DEBUG( "Retrieving " << toolName );
  if ( tool.name().empty() ) {
    ATH_MSG_WARNING( "No " << toolName << " to initialize." );
    return StatusCode::SUCCESS;
  }
  
  if ( tool.retrieve().isFailure() ) {
    ATH_MSG_ERROR( "Failed to Retrieve " << toolName );
    return StatusCode::FAILURE;
  } else
    ATH_MSG_DEBUG( "Retrieved "<< toolName );
  
  return StatusCode::SUCCESS;
}

// -----------------------------------------------------------------------------------------------------------------

TrigCompositeUtils::DecisionID TrigBjetBtagHypoTool::decisionId() const { return m_decisionId.numeric(); }
const HLT::Identifier TrigBjetBtagHypoTool::getId() const { return m_decisionId; }

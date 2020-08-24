/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigCompositeUtils/Combinators.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigEgammaFastCaloHypoToolMult.h"

using namespace TrigCompositeUtils;
TrigEgammaFastCaloHypoToolMult::TrigEgammaFastCaloHypoToolMult( const std::string& type, 
						const std::string& name, 
						const IInterface* parent )
  : base_class( type, name, parent ),
    m_decisionId( HLT::Identifier::fromToolName( name ) ) {}

TrigEgammaFastCaloHypoToolMult::~TrigEgammaFastCaloHypoToolMult() {}

StatusCode TrigEgammaFastCaloHypoToolMult::initialize() {
  if(  m_subTools.size() <= 1 ) {
    ATH_MSG_ERROR( "Number of sub tools " << m_subTools.size() << ", while it has to be > 1, otherwise this is not a multiplicity selection" );
    return StatusCode::FAILURE;
  }
  CHECK( m_subTools.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode TrigEgammaFastCaloHypoToolMult::decide( std::vector<ITrigEgammaFastCaloHypoTool::FastClusterInfo>& input )  const {
  HLT::Index2DVec passingSelection( m_subTools.size() );
  ATH_MSG_DEBUG( "Applying selection of multiplicity " << m_subTools.size() );

  size_t cutIndex{ 0 };
  for ( auto& tool: m_subTools ) {
    size_t clusterIndex{ 0 };
    for ( auto iIter =  input.begin(); iIter != input.end(); ++iIter, ++clusterIndex ) {
      if ( passed( m_decisionId.numeric(), iIter->previousDecisionIDs ) ) {
	if ( tool->decide( *iIter ) ) 
	  passingSelection[ cutIndex ].push_back( clusterIndex );
      }
    }
    
    if ( passingSelection[cutIndex].empty() ) {
      ATH_MSG_DEBUG( "No object passed selection " << cutIndex << " rejecting" );
      return StatusCode::SUCCESS;
    }
    cutIndex++;
  }
  std::set<size_t> passingIndices;
  HLT::elementsInUniqueCombinations( passingSelection, passingIndices );
  
  for ( auto idx: passingIndices ) 
    addDecisionID( m_decisionId.numeric(), input[idx].decision );

  
  return StatusCode::SUCCESS;
}

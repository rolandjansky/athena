/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
// TrigUpgradeTest includes
#include "TestHypoTool.h"


namespace HLTTest {

  TestHypoTool::TestHypoTool( const std::string& type, 
			      const std::string& name, 
			      const IInterface* parent ) : 
    ITestHypoTool( name ),
    AthAlgTool( type, name, parent ) {
    declareProperty( "Threshold", m_threshold );
    declareProperty( "Property", m_property );
  }

  TestHypoTool::~TestHypoTool() {}

  StatusCode TestHypoTool::initialize() {
    ATH_MSG_INFO( "Initializing " << name() << "..." );

    ATH_MSG_DEBUG(m_threshold);
    ATH_MSG_DEBUG(m_property);

    
    CHECK( not m_chainsProperty.empty() );
    
    for ( const std::string& el: m_chainsProperty ) 
      m_chains.insert( HLT::Identifier( el ).numeric() );
    
    for ( const HLT::Identifier& id: m_chains )
      ATH_MSG_DEBUG( "Configured to require chain " << id );

    return StatusCode::SUCCESS;
  }


  
  StatusCode TestHypoTool::decide( DecisionContainer* decisions ) const  {
    ATH_MSG_DEBUG( "Making new decisions " << name() );
    for ( auto d: *decisions )  {
      //get previous decisions
      auto previousDecisions = d->objectLink<DecisionContainer>( "previousDecisions" );    
      TrigCompositeUtils::DecisionIDContainer objDecisions;      
      TrigCompositeUtils::decisionIDs( *previousDecisions, objDecisions );
      
      ATH_MSG_DEBUG("Number of previous decisions for this input: " << objDecisions.size() );
      
      for ( TrigCompositeUtils::DecisionID id : objDecisions ) {
	ATH_MSG_DEBUG( " -- found decision " << HLT::Identifier( id ) );
      }
      
      std::vector<TrigCompositeUtils::DecisionID> intersection;
      std::set_intersection( m_chains.begin(), m_chains.end(),
			   objDecisions.begin(), objDecisions.end(),
			   std::back_inserter( intersection ) );
    
      if ( not intersection.empty() ) {
      
	auto feature = d->objectLink<xAOD::TrigCompositeContainer>( "feature" );
	if ( not feature.isValid() )  {
	  ATH_MSG_ERROR( " Can not find reference to the object from the decision" );
	  return StatusCode::FAILURE;
	}
	float v = (*feature)->getDetail<float>( m_property );
	if ( v > m_threshold ) { // actual cut will be more complex of course
	  ATH_MSG_DEBUG( "  threshold " << m_threshold << " passed by value: " << v );
	  for ( const HLT::Identifier& id: intersection )
	    addDecisionID( id, d );
	}
      }
      else {
	ATH_MSG_DEBUG("No Input decisions requested by active chains");
	for ( const HLT::Identifier& id: m_chains )
	  ATH_MSG_DEBUG( "Configured to require chain " << id );
      }
    }
    
    return StatusCode::SUCCESS;
  }

  
  StatusCode TestHypoTool::finalize() {
    ATH_MSG_INFO( "Finalizing " << name() << "..." );
    return StatusCode::SUCCESS;
  }

} //> end namespace HLTTest

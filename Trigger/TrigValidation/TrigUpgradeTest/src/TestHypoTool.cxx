/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
// TrigUpgradeTest includes
#include "TestHypoTool.h"


namespace HLTTest {

  TestHypoTool::TestHypoTool( const std::string& type, 
			      const std::string& name, 
			      const IInterface* parent ) : 
    AthAlgTool( type, name, parent ),
    m_decisionId( name ) // for now the decision ID will be the hashed name of the tool
  {
    declareProperty( "Threshold", m_threshold );
    declareProperty( "Property", m_property );
  }

  TestHypoTool::~TestHypoTool()
  {}

  StatusCode TestHypoTool::initialize()
  {
    ATH_MSG_INFO( "Initializing " << name() << "..." );
  
    return StatusCode::SUCCESS;
  }

  StatusCode TestHypoTool::decide( DecisionContainer* decisions ) const  {
    ATH_MSG_DEBUG( "Making decisions " << name() );
    for ( auto d: *decisions )  {
      auto feature = d->objectLink<xAOD::TrigCompositeContainer>( "feature" );
      if ( not feature.isValid() )  {
	ATH_MSG_ERROR( " Can not find reference to the object from the decision" );
	return StatusCode::FAILURE;
      }
      float v = (*feature)->getDetail<float>( m_property );
      if ( v > m_threshold ) { // actual cut will be more complex of course
	ATH_MSG_DEBUG( "  threshold " << m_threshold << " passed by value: " << v );
	addDecisionID( m_decisionId.numeric(), d );
      }	
    }
    return StatusCode::SUCCESS;
  }

  
  StatusCode TestHypoTool::finalize()
  {
    ATH_MSG_INFO( "Finalizing " << name() << "..." );

    return StatusCode::SUCCESS;
  }

} //> end namespace HLTTest

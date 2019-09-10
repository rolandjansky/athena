/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
// TrigUpgradeTest includes
#include "TestMHypoTool.h"


namespace HLTTest {

  TestMHypoTool::TestMHypoTool( const std::string& type, 
			      const std::string& name, 
			      const IInterface* parent ) :
    ITestHypoTool( name ),
    AthAlgTool( type, name, parent )
  {
    declareProperty( "Threshold", m_threshold = 0);
    declareProperty( "Multiplicity", m_multiplicity = 0 );
    declareProperty( "Property", m_property );
  }

  TestMHypoTool::~TestMHypoTool()
  {}

  StatusCode TestMHypoTool::initialize()
  {
    ATH_MSG_INFO( "Initializing " << name() << "..." );

    return StatusCode::SUCCESS;
  }

  StatusCode TestMHypoTool::iterate( DecisionContainer* decisions, std::function<void(Decision*)> f) const {
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
	  f(d); // here we inject the functional code
	}	
      }
      return StatusCode::SUCCESS;
    
  }

  
  StatusCode TestMHypoTool::decide( DecisionContainer* decisions ) const  {

    size_t candidates = 0;
       
    CHECK( iterate(decisions,
		   [&](Decision*){ candidates++; } ) );
    if ( candidates >=  m_multiplicity ) 
      CHECK( iterate(decisions,
		     [&](Decision* d) { addDecisionID( decisionId().numeric(), d ); } ) );
    return StatusCode::SUCCESS;
  }

  
  StatusCode TestMHypoTool::finalize()
  {
    ATH_MSG_INFO( "Finalizing " << name() << "..." );

    return StatusCode::SUCCESS;
  }

} //> end namespace HLTTest

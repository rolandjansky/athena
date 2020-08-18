/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "TestHypoTool.h"
#include "TrigCompositeUtils/HLTIdentifier.h"

using namespace TrigCompositeUtils;


namespace HLTTest {

  TestHypoTool::TestHypoTool( const std::string& type, 
    const std::string& name, 
    const IInterface* parent ) : 
  ITestHypoTool( name ),
  AthAlgTool( type, name, parent )
  {
    m_decisionId=HLT::Identifier::fromToolName(name );
  }

  TestHypoTool::~TestHypoTool() {}

  StatusCode TestHypoTool::initialize() {
    ATH_MSG_INFO( "Initializing " << name() << "..." );

    ATH_MSG_DEBUG("Configured to require chain " <<  m_decisionId );
    ATH_MSG_DEBUG("Link name is "<<m_linkName.value());
    ATH_MSG_DEBUG("threshold="<<m_threshold);
    ATH_MSG_DEBUG("property="<<m_property);   
    return StatusCode::SUCCESS;
  }

  StatusCode TestHypoTool::decide( DecisionContainer* decisions ) const  {
    ATH_MSG_DEBUG( "Add decisionIDs to " << decisions->size() <<" output decisions" );
    size_t counter = 0;
    for ( const auto d: *decisions )  {
      //get previous decisions
      ElementLinkVector<DecisionContainer> inputLinks = getLinkToPrevious(d);
      ATH_MSG_DEBUG("Decision "<< counter <<": Got "<<inputLinks.size()<<" input decisions");
      for (const auto previousDecisions: inputLinks){

        TrigCompositeUtils::DecisionIDContainer objDecisions;      
        TrigCompositeUtils::decisionIDs( *previousDecisions, objDecisions );

        ATH_MSG_DEBUG("Number of decision-IDs for input "<< counter <<" is: " << objDecisions.size() );

	if ( TrigCompositeUtils::passed( m_decisionId.numeric(), objDecisions ) ) {
	  const auto feature = d->objectLink<xAOD::TrigCompositeContainer>( "feature" );
          if ( not feature.isValid() )  {
            ATH_MSG_ERROR( " Can not find reference to the object from the decision" );
            return StatusCode::FAILURE;
          }
          const float v = (*feature)->getDetail<float>( m_property );
          if ( v > m_threshold ) { // actual cut will be more complex of course
            ATH_MSG_DEBUG( m_threshold << " passed by value: " << v << ". Adding "<< m_decisionId <<" to output" );  
            addDecisionID(  m_decisionId, d );
          } else {
            ATH_MSG_DEBUG( m_threshold << " not passed by value " << v );
          }
        
        } else { 
          ATH_MSG_DEBUG("No Input decisions matched");        
        }       
        counter++;
      }
    }
    return StatusCode::SUCCESS;
  }


  StatusCode TestHypoTool::finalize() {
    ATH_MSG_INFO( "Finalizing " << name() << "..." );
    return StatusCode::SUCCESS;
  }

} //> end namespace HLTTest

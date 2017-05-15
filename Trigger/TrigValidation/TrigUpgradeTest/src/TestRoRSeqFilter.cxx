/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iterator>
#include <algorithm>

#include "DecisionHandling/HLTIdentifier.h"
#include "TestRoRSeqFilter.h"

#include "GaudiKernel/Property.h"

namespace HLTTest {

TestRoRSeqFilter::TestRoRSeqFilter( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator )
{
  declareProperty( "inputs", m_inputs );
  declareProperty( "chains", m_chainsProperty, "Chains to filter the object on");
}

TestRoRSeqFilter::~TestRoRSeqFilter()
{}

StatusCode TestRoRSeqFilter::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");
  ATH_MSG_DEBUG("Will consume the input data: " << m_inputs );

  
  
  for ( auto& el: m_chainsProperty ) 
    m_chains.insert( HLT::Identifier(el).numeric() );
    
  // minimal validity crosscheck
  if ( m_chainsProperty.size() == 0 ) {
    ATH_MSG_WARNING("This filter will always reject as it has no chains of interest configured");
  }
  
  if ( m_chains.size() != m_chainsProperty.size() ) {
    ATH_MSG_ERROR("Repeated chain requirements (annoyance) or chain to hash conversion clash (major configuration issue therefore can not run)");
    return StatusCode::FAILURE;
  }  
  
  return StatusCode::SUCCESS;
}

StatusCode TestRoRSeqFilter::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode TestRoRSeqFilter::execute() 
{
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  DecisionIDContainer intersection;
  for ( const std::string& input : m_inputs ) {
    ATH_MSG_DEBUG( "Reading input " << input);
    SG::ReadHandle<DecisionContainer> dh( input );
    if ( dh.isValid() ) {
      ATH_MSG_DEBUG( "Valid input: " << input );
      for ( auto obj: *dh.cptr() ) {
	DecisionIDContainer objDecisions;
	decisionIDs( obj, objDecisions ); // this should be replaced in production code by method passingIDs but the later is not printing so in tests we want the IDs
	// here we have to sets of IDs, those we are interested in (via chain property) and those comming from the decision obj

	std::set_intersection( m_chains.begin(), m_chains.end(),
			       objDecisions.begin(), objDecisions.end(),
			       std::inserter( intersection, intersection.begin() ) );
	
	for ( auto positive: intersection ) {
	  ATH_MSG_DEBUG( "A positive decision for object: " << HLT::Identifier( positive ) );
	}
      }
    } else {
      ATH_MSG_DEBUG( "Void input: " << input );
    }	
  }
  
  setFilterPassed( not intersection.empty() );
  return StatusCode::SUCCESS;
}

} //> end namespace HLTTest

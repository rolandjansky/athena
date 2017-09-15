/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DecisionHandling includes
#include "RoRSeqFilter.h"

#include "GaudiKernel/Property.h"



RoRSeqFilter::RoRSeqFilter( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator )
{
  //declareProperty( "Property", m_nProperty );
}

RoRSeqFilter::~RoRSeqFilter()
{}

StatusCode RoRSeqFilter::initialize()
{
  ATH_MSG_INFO ( "Initializing " << name() << "..." );
  CHECK( not m_inputKeys.empty() );
  CHECK( not m_outputKeys.empty() );

  if ( m_mergeInputs ) {
    CHECK( m_inputKeys.size() > 1 );
    CHECK( m_outputKeys.size() == 1 );    
  } else {
    CHECK( m_inputKeys.size() == m_outputKeys.size() );
  }

  CHECK( m_inputKeys.initialize() );
  CHECK( m_outputKeys.initialize() );

  
  CHECK( not m_chainsProperty.empty() );
  
  for ( auto& el: m_chainsProperty ) 
    m_chains.insert( HLT::Identifier( el ).numeric() );

  return StatusCode::SUCCESS;
}

StatusCode RoRSeqFilter::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode RoRSeqFilter::execute() {  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );

  size_t passCounter = 0;    
  if ( m_mergeInputs ) {
    auto output = std::make_unique< ConstDataVector< TrigCompositeUtils::DecisionContainer > > ();
    output->clear( SG::VIEW_ELEMENTS );

    for ( auto inputKey: m_inputKeys ) {
      auto inputHandle = SG::makeHandle( inputKey );      
      passCounter += copyPassing( *inputHandle, *output );
    }

    auto outputHandle = SG::makeHandle( m_outputKeys[ 0 ] );
    CHECK( outputHandle.record( std::move( output ) ) );

  } else {
    size_t outputIndex = 0;
    for ( auto inputKey: m_inputKeys ) {
      auto inputHandle = SG::makeHandle( inputKey );

      auto output = std::make_unique< ConstDataVector< TrigCompositeUtils::DecisionContainer > > ();
      output->clear( SG::VIEW_ELEMENTS );
      
      passCounter += copyPassing( *inputHandle, *output );
      
      auto outputHandle = SG::makeHandle( m_outputKeys[ outputIndex ] );
      CHECK( outputHandle.record( std::move( output ) ) );

      outputIndex++;
    }
  }

  setFilterPassed( passCounter != 0 );  
  return StatusCode::SUCCESS;
}
  
size_t RoRSeqFilter::copyPassing( const TrigCompositeUtils::DecisionContainer& input, 
				  ConstDataVector<TrigCompositeUtils::DecisionContainer>& output ) const {
  size_t passCounter = 0;
  for ( auto i: input ) {
    TrigCompositeUtils::DecisionIDContainer objDecisions;      
    TrigCompositeUtils::passingIDs( i, objDecisions );
    
    std::vector<TrigCompositeUtils::DecisionID> intersection;
    std::set_intersection( m_chains.begin(), m_chains.end(),
			   objDecisions.begin(), objDecisions.end(),
			   std::back_inserter( intersection ) );
    
    if ( not intersection.empty() ) {
      output.push_back( i );
      passCounter ++;
    }
  }
  return passCounter;  
}

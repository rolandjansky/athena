/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DecisionHandling includes
#include "RoRSeqFilter.h"

#include "GaudiKernel/Property.h"

RoRSeqFilter::RoRSeqFilter( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator )
{}

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

  // for ( auto key: m_inputKeys ) {
  //   renounce( key );
  // }
  CHECK( m_inputKeys.initialize() );
  CHECK( m_outputKeys.initialize() );

  renounceArray(m_inputKeys);
  renounceArray(m_outputKeys);

  ATH_MSG_DEBUG("Will consume implicit ReadDH:" );
  for (auto& input: m_inputKeys){  
    ATH_MSG_DEBUG(" - "<<input.key());
  }
  ATH_MSG_DEBUG("Will produce implicit WriteDH: ");
  for (auto& output: m_outputKeys){  
    ATH_MSG_DEBUG(" - "<<output.key());
  }

  
  CHECK( not m_chainsProperty.empty() );
  
  for ( const std::string& el: m_chainsProperty ) 
    m_chains.insert( HLT::Identifier( el ).numeric() );

  for ( const HLT::Identifier& id: m_chains )
    ATH_MSG_DEBUG( "Configured to require chain " << id );
  
  ATH_MSG_DEBUG( "mergeInputs is "<<m_mergeInputs);

  return StatusCode::SUCCESS;
}

StatusCode RoRSeqFilter::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode RoRSeqFilter::execute() {  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );

  auto inputHandles  = m_inputKeys.makeHandles();
  auto outputHandles = m_outputKeys.makeHandles();

  size_t passCounter = 0;    
  if ( m_mergeInputs ) {
    auto output = std::make_unique< ConstDataVector< TrigCompositeUtils::DecisionContainer > > ();
    output->clear( SG::VIEW_ELEMENTS );

    for ( auto inputHandle: inputHandles ) {
      passCounter += copyPassing( *inputHandle, *output );
    }

    ATH_MSG_DEBUG( "Recording " <<  m_outputKeys[ 0 ].key() ); 
    CHECK( outputHandles[0].record( std::move( output ) ) );

  } else {
    size_t outputIndex = 0;
    for ( auto inputKey: m_inputKeys ) {
      auto inputHandle = SG::makeHandle( inputKey );

      auto output = std::make_unique< ConstDataVector< TrigCompositeUtils::DecisionContainer > > ();
      output->clear( SG::VIEW_ELEMENTS );
      
      passCounter += copyPassing( *inputHandle, *output );

      ATH_MSG_DEBUG( "Recording output key " <<  m_outputKeys[ outputIndex ].key() <<" of size "<<output->size()  <<" at index "<< outputIndex);
  
      CHECK( outputHandles[outputIndex].record( std::move( output ) ) );

      outputIndex++;
    }
  }

  ATH_MSG_DEBUG( "Filter " << ( passCounter != 0 ? "passed" : "rejected") );
  setFilterPassed( passCounter != 0 );  
  return StatusCode::SUCCESS;
}
  
size_t RoRSeqFilter::copyPassing( const TrigCompositeUtils::DecisionContainer& input, 
				  ConstDataVector<TrigCompositeUtils::DecisionContainer>& output ) const {
  size_t passCounter = 0;
  ATH_MSG_DEBUG( "Input size " << input.size() );
  for ( const TrigCompositeUtils::Decision* i: input ) {
    
    TrigCompositeUtils::DecisionIDContainer objDecisions;      
    TrigCompositeUtils::decisionIDs( i, objDecisions );

    ATH_MSG_DEBUG("Number of positive decisions " << objDecisions.size() );

    for ( TrigCompositeUtils::DecisionID id : objDecisions ) {
      ATH_MSG_DEBUG( "Positive decision " << HLT::Identifier( id ) );
    }

    std::vector<TrigCompositeUtils::DecisionID> intersection;
    std::set_intersection( m_chains.begin(), m_chains.end(),
			   objDecisions.begin(), objDecisions.end(),
			   std::back_inserter( intersection ) );
    
    if ( not intersection.empty() ) {
      output.push_back( i );
      passCounter ++;
    }
  }
  ATH_MSG_DEBUG( "Output size " << output.size() );
  return passCounter;  
}

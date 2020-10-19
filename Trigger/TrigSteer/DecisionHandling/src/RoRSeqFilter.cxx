/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// DecisionHandling includes
#include "RoRSeqFilter.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "Gaudi/Property.h"

using TrigCompositeUtils::DecisionContainer;
using TrigCompositeUtils::Decision;
using TrigCompositeUtils::DecisionIDContainer;
using TrigCompositeUtils::DecisionID;
using TrigCompositeUtils::decisionIDs;
using TrigCompositeUtils::insertDecisionIDs;
using TrigCompositeUtils::createAndStore;
using TrigCompositeUtils::newDecisionIn;

RoRSeqFilter::RoRSeqFilter( const std::string& name, 
  ISvcLocator* pSvcLocator ) :
  ::AthReentrantAlgorithm( name, pSvcLocator )
{}


StatusCode RoRSeqFilter::initialize()
{
  CHECK( not m_inputKeys.empty() );
  CHECK( not m_outputKeys.empty() );


  CHECK( m_inputKeys.initialize() );
  CHECK( m_outputKeys.initialize() );

  renounceArray(m_inputKeys);

  // default IO mappnig nth input -> nth output
  if ( m_ioMappingProperty.empty()) {
    CHECK( m_inputKeys.size() == m_outputKeys.size() );
    for ( unsigned i = 0; i < m_inputKeys.size(); ++i ) {
      m_ioMapping.push_back( {i} );
    }
    ATH_MSG_DEBUG( "Default I/O collections mapping nth input -> nth output" );
  } else {
    m_ioMapping = m_ioMappingProperty;
    ATH_MSG_DEBUG( "Custom I/O mapping used by the filter" );
  }

  for ( size_t i = 0; i < m_ioMapping.size(); ++i ) {
    for ( unsigned inIndex: m_ioMapping[i] ) {
      ATH_MSG_INFO("Input collection: " << m_inputKeys[inIndex] );
    }
    ATH_MSG_INFO("   Routed to output collection " << m_outputKeys[i] );
  }

  // crosscheck mapping (also the default one)
  CHECK( m_ioMapping.size() == m_outputKeys.size() ); // all outputs will be filled
  std::set<int> inIndices;
  for ( auto i: m_ioMapping ) {
    CHECK( not i.empty() ); // all outputs are connected to at least one input
    inIndices.insert( i.begin(), i.end() );
  }
  CHECK( inIndices.size() == m_inputKeys.size() ); // all inputs are routed
  for ( size_t i = 0; i < m_ioMapping.size(); ++i ) {
    auto iset = std::set( m_ioMapping[i].begin(), m_ioMapping[i].end() );
    for ( size_t j = i+1; j < m_ioMapping.size(); ++j ) {
      for ( auto el: m_ioMapping[j] ) {
        if ( iset.count( el ) != 0 ) {
          ATH_MSG_ERROR( "The input index " << el << " directed to two outputs " << m_ioMapping[i] << " " << m_ioMapping[j] );
          return StatusCode::FAILURE;
        }
      }
    }
  }

  if (msgLvl(MSG::DEBUG)){
    ATH_MSG_DEBUG("Will consume implicit ReadDH:" );
    for (auto& input: m_inputKeys){  
      ATH_MSG_DEBUG(" - "<<input.key());
    }
    ATH_MSG_DEBUG("Will produce WriteDH: ");
    for (auto& output: m_outputKeys){
      ATH_MSG_DEBUG(" - "<<output.key());
    }
  }
  
  CHECK( not m_chainsProperty.empty() );
  
  for ( const std::string& el: m_chainsProperty ) 
    m_chains.insert( HLT::Identifier( el ).numeric() );

  m_chainsPerInput.resize( m_chainsPerInputProperty.size() );
  for ( size_t i = 0; i < m_chainsPerInputProperty.size(); ++i ) {
    for ( const std::string& el: m_chainsPerInputProperty[i] )
        m_chainsPerInput[i].insert( HLT::Identifier( el ).numeric() );
  }

  if (msgLvl(MSG::DEBUG)){
    ATH_MSG_DEBUG( "Configured to require these chains: ");
    for ( const HLT::Identifier& id: m_chains )
      ATH_MSG_DEBUG( " - " << id );
  }
  
  if ( not m_monTool.name().empty() ) {
    ATH_CHECK( m_monTool.retrieve() );
  }
  
  return StatusCode::SUCCESS;
}


StatusCode RoRSeqFilter::execute( const EventContext& ctx ) const {
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  auto inputHandles  = m_inputKeys.makeHandles( ctx );
  auto outputHandles = m_outputKeys.makeHandles( ctx );

  std::vector<std::string> inputNames( {"exec", "anyvalid"} );
  std::vector<bool> inputStats({true, false}); // position 0 for number of execs, always true, bool at position 1 is set later
  inputNames.reserve(inputHandles.size() + 2);
  inputStats.reserve(inputHandles.size() + 2);
  bool validInputs = false;
  for ( auto& inputHandle: inputHandles ) {
    inputNames.push_back( inputHandle.name() );
    if( inputHandle.isValid() ) {// this is because input is implicit
      validInputs = true;
      inputStats.push_back( true );
    } else {
      inputStats.push_back( false );
    }
  }
  inputStats[1] = validInputs; // position 1 for number of execes with any collection valid
  auto inputName = Monitored::Collection<std::vector<std::string>>( "name", inputNames );
  auto inputStat = Monitored::Collection<std::vector<bool>>( "stat", inputStats );
  Monitored::Group( m_monTool, inputStat, inputName );
  
  if (!validInputs) {
    setFilterPassed(false, ctx);
    ATH_MSG_DEBUG ( "No valid inputs found, filter failed. Return...." );
    return StatusCode::SUCCESS;
  }

  ATH_MSG_DEBUG ( "Running on "<< inputHandles.size() <<" input keys" );
  
  size_t passCounter = 0;

  for ( size_t outputIndex = 0; outputIndex < m_ioMapping.size(); ++outputIndex ) {
    bool anyPresent =  false;
    for ( unsigned inputIndex : m_ioMapping[outputIndex] ) {
      if( not inputHandles[inputIndex].isValid() ) {
        ATH_MSG_DEBUG( "InputHandle "<< inputHandles[inputIndex].key() <<" not present" );
      } else if ( inputHandles[inputIndex]->empty() ) {
        ATH_MSG_DEBUG( "InputHandle "<< inputHandles[inputIndex].key() <<" contains all rejected decisions, skipping" );
      } else {
        anyPresent = true;
      }
    }
    if ( anyPresent ) {
      createAndStore(outputHandles[outputIndex]);
      DecisionContainer* output = outputHandles[outputIndex].ptr();
      for ( auto inputIndex : m_ioMapping[outputIndex] ) {
        if ( inputHandles[inputIndex].isValid() and not inputHandles[inputIndex]->empty() ) {
          ATH_MSG_DEBUG( "Checking inputHandle: "<< inputHandles[inputIndex].key() <<" has " << inputHandles[inputIndex]->size() <<" elements");
          if ( not m_chainsPerInput.empty() ) {
            passCounter += copyPassing( *inputHandles[inputIndex], *output, m_chainsPerInput[inputIndex] );
          } else {
            passCounter += copyPassing( *inputHandles[inputIndex], *output, m_chains );
          }
          ATH_MSG_DEBUG( "Recorded output key " <<  m_outputKeys[ outputIndex ].key() <<" of size "<<output->size()  <<" at index "<< outputIndex);
        }
      }
    }
  }

  setFilterPassed( passCounter != 0, ctx );
  ATH_MSG_DEBUG( "Filter " << ( filterPassed(ctx) ? "passed" : "rejected" ) );
  if ( msgLvl( MSG::DEBUG ) ){
    for ( auto& output: outputHandles ) {
      if( output.isValid() ) ATH_MSG_DEBUG( " "<<output.key() );
    }
  }

  
  return StatusCode::SUCCESS;
}
  
size_t RoRSeqFilter::copyPassing( const DecisionContainer& input,
                                  DecisionContainer& output, const std::set<HLT::Identifier>& topass ) const {
  size_t passCounter = 0;
  for (size_t i = 0; i < input.size(); ++i) {
    const Decision* inputDecision = input.at(i);

    DecisionIDContainer objDecisions;      
    decisionIDs( inputDecision, objDecisions );

    ATH_MSG_DEBUG("Number of positive decisions for this input is " << objDecisions.size() <<". Now Filtering...." );

    DecisionIDContainer intersection;
    std::set_intersection( topass.begin(), topass.end(),
         objDecisions.begin(), objDecisions.end(),
         std::inserter(intersection, intersection.end()) );

    if ( not intersection.empty() ) {      
      // This sets up the 'self' link & the 'seed' link (seeds from inputDecision)
      Decision* decisionCopy = newDecisionIn( &output, inputDecision, "F" );

      // Copy accross only the DecisionIDs which have passed through this Filter for this Decision object. 
      // WARNING: Still need to 100% confirm if the correct set to propagate forward is objDecisions or intersection.
      // Tim M changing this from objDecisions (all IDs) -> intersection (only passed IDs) Feb 19
      insertDecisionIDs(intersection, decisionCopy);     
      passCounter ++;
      ATH_MSG_DEBUG("Input satisfied at least one filtering chain. Chain(s) passing:");
      if (msgLvl(MSG::DEBUG)){
      	for ( DecisionID id : intersection ) {
	       ATH_MSG_DEBUG( " -- " << HLT::Identifier( id ) );
	      }
      }
      
    } else {
      ATH_MSG_DEBUG("No Input decisions requested by filtering chains");
    }
  }
  return passCounter;  
}

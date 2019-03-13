/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
// TrigUpgradeTest includes
#include "TestMerger.h"

// STL includes



namespace HLTTest {

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 

  // Constructors
  ////////////////
  TestMerger::TestMerger( const std::string& name, 
    ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator )
  {
    declareProperty( "Inputs", m_inputs );
    declareProperty( "Output", m_outputKey );
  }


  //  TestMerger::~TestMerger()
  //  {}


  StatusCode TestMerger::initialize()
  {
    ATH_MSG_INFO ("Initializing " << name() << "...");
    CHECK( m_outputKey.initialize() );
    return StatusCode::SUCCESS;
  }

  StatusCode TestMerger::finalize()
  {
    ATH_MSG_INFO ("Finalizing " << name() << "...");

    return StatusCode::SUCCESS;
  }

  StatusCode TestMerger::execute()
  {  
    ATH_MSG_DEBUG ("Executing " << name() << "...");

    auto output = std::make_unique<DecisionContainer>();
    auto aux    = std::make_unique<DecisionAuxContainer>();
    output->setStore( aux.get() );

    for (const auto input: m_inputs ) {
      auto iHandle = SG::ReadHandle<DecisionContainer>(input);
      if ( iHandle.isValid() ) {
        size_t counter = 0;
        for ( auto iDecisionIter  = iHandle->begin(); iDecisionIter != iHandle->end(); ++iDecisionIter, ++counter ) {
          auto d = newDecisionIn(output.get());
          linkToPrevious(d, *iDecisionIter );
        }
        ATH_MSG_DEBUG( "Input " << input << " present, linked " << counter << " inputs" );
      } else {
        ATH_MSG_DEBUG( "Input " << input << " absent" );
      }
    }
    
    auto outputHandle = SG::makeHandle(m_outputKey);
    CHECK( outputHandle.record( std::move(output), std::move(aux) ) );
    return StatusCode::SUCCESS;
  }


} //> end namespace HLTTest

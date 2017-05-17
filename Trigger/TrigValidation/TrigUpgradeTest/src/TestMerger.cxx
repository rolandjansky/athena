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
  }


  TestMerger::~TestMerger()
  {}


  StatusCode TestMerger::initialize()
  {
    ATH_MSG_INFO ("Initializing " << name() << "...");

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

    return StatusCode::SUCCESS;
  }


} //> end namespace HLTTest

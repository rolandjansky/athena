/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrigUpgradeTest includes
#include "TestRecoAlg.h"

#include "GaudiKernel/Property.h"

namespace HLTTest {

TestRecoAlg::TestRecoAlg( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator )
{
  //declareProperty( "Property", m_nProperty );
}

TestRecoAlg::~TestRecoAlg()
{}

StatusCode TestRecoAlg::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode TestRecoAlg::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode TestRecoAlg::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  return StatusCode::SUCCESS;
}

} //> end namespace HLTTest

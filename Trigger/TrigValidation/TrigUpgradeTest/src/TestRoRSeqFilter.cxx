/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrigUpgradeTest includes
#include "TestRoRSeqFilter.h"

#include "GaudiKernel/Property.h"

namespace HLTTest {

TestRoRSeqFilter::TestRoRSeqFilter( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthFilterAlgorithm( name, pSvcLocator )
{
  //declareProperty( "Property", m_nProperty );
}

TestRoRSeqFilter::~TestRoRSeqFilter()
{}

StatusCode TestRoRSeqFilter::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode TestRoRSeqFilter::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode TestRoRSeqFilter::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  return StatusCode::SUCCESS;
}

} //> end namespace HLTTest

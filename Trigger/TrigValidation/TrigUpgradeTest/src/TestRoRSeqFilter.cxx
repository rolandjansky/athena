/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrigUpgradeTest includes
#include "TestRoRSeqFilter.h"

#include "GaudiKernel/Property.h"

namespace HLTTest {

TestRoRSeqFilter::TestRoRSeqFilter( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator )
{
  declareProperty( "inputs", m_inputs );
}

TestRoRSeqFilter::~TestRoRSeqFilter()
{}

StatusCode TestRoRSeqFilter::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");
  ATH_MSG_DEBUG("Will consume the input data: " << m_inputs );
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
  bool pass = true;
  for (const std::string& input : m_inputs ) {
    ATH_MSG_DEBUG("Reading input " << input);
    //SG::ReadHandle<>()
  }
  
  setFilterPassed(pass);
  return StatusCode::SUCCESS;
}

} //> end namespace HLTTest

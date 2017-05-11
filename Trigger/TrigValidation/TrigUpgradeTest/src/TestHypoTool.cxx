/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
// TrigUpgradeTest includes
#include "TestHypoTool.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/IToolSvc.h"


namespace HLTTest {

TestHypoTool::TestHypoTool( const std::string& type, 
		      const std::string& name, 
		      const IInterface* parent ) : 
  ::AthAlgTool  ( type, name, parent   )
{
  //declareProperty( "Property", m_nProperty );
}

TestHypoTool::~TestHypoTool()
{}

StatusCode TestHypoTool::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");
  
  return StatusCode::SUCCESS;
}

StatusCode TestHypoTool::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

} //> end namespace HLTTest

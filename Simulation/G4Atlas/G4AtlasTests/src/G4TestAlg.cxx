/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasTests/G4TestAlg.h"
#include "G4AtlasTests/ISimTestTool.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthenaKernel/errorcheck.h"
G4TestAlg::G4TestAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator), m_testTools(this)
{
  declareProperty("SimTestTools", m_testTools, "ISimTestTools to be run for each event");
}


StatusCode G4TestAlg::initialize() 
{
  ATH_MSG_INFO ( "Initializing " << name() << " - package version " << PACKAGE_VERSION );
  //initialize  the digi test tools
  CHECK(m_testTools.retrieve());
  return StatusCode::SUCCESS;
}


StatusCode G4TestAlg::execute() 
{
  ToolHandleArray<ISimTestTool>::iterator simTestTool(m_testTools.begin());
  const ToolHandleArray<ISimTestTool>::iterator endOfSimTestTools(m_testTools.end());
  while (simTestTool != endOfSimTestTools && ((**(simTestTool++)).processEvent()).isSuccess()) ;
  return StatusCode::SUCCESS;
}


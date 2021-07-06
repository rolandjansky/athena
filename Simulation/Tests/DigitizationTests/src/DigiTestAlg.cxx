/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "DigitizationTests/DigiTestAlg.h"
#include "DigitizationTests/IDigiTestTool.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthenaKernel/errorcheck.h"
DigiTestAlg::DigiTestAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
{
  declareProperty("DigiTestTools", m_testTools, "IDigiTestTools to be run for each event");
}


StatusCode DigiTestAlg::initialize() 
{
  ATH_MSG_INFO ( "Initializing" );
  //initialize  the digi test tools
  CHECK(m_testTools.retrieve());
  return StatusCode::SUCCESS;
}


StatusCode DigiTestAlg::execute() 
{
  ToolHandleArray<IDigiTestTool>::iterator digiTestTool(m_testTools.begin());
  const ToolHandleArray<IDigiTestTool>::iterator endOfDigiTestTools(m_testTools.end());
  while (digiTestTool != endOfDigiTestTools && ((**(digiTestTool++)).processEvent()).isSuccess()) ;
  return StatusCode::SUCCESS;
}


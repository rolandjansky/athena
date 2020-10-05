/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MM_Digitization/MM_Digitizer.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthenaKernel/errorcheck.h"
#include "PileUpTools/IPileUpTool.h"

/*******************************************************************************/
MM_Digitizer::MM_Digitizer(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator) {}
/*******************************************************************************/
StatusCode MM_Digitizer::initialize() {
  // intitialize store gate active store
  ATH_CHECK( m_digTool.retrieve() );
  ATH_MSG_DEBUG ("MM_Digitizer::Retrieved MM Digitization Tool.");
  return StatusCode::SUCCESS;
}
/*******************************************************************************/
StatusCode MM_Digitizer::execute() {
  ATH_MSG_DEBUG ("MM_Digitizer::in execute()");
  return m_digTool->processAllSubEvents(Gaudi::Hive::currentContext());
}

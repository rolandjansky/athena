/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "sTGC_Digitization/sTGC_Digitizer.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthenaKernel/errorcheck.h"
#include "PileUpTools/IPileUpTool.h"

/*******************************************************************************/
sTGC_Digitizer::sTGC_Digitizer(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator) {}
/*******************************************************************************/
StatusCode sTGC_Digitizer::initialize() {   
  // intitialize store gate active store
  ATH_CHECK(m_digTool.retrieve());
  ATH_MSG_DEBUG ("Retrieved sTGC Digitization Tool." );
  return StatusCode::SUCCESS; 
}
/*******************************************************************************/
StatusCode sTGC_Digitizer::execute() {   
  ATH_MSG_DEBUG ("sTGC_Digitizer::in execute()"); 
  return m_digTool->processAllSubEvents(Gaudi::Hive::currentContext());
}

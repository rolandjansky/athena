/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "ZDC_SimuDigitization/ZDC_DigiTop.h"
#include "PileUpTools/IPileUpTool.h"

ZDC_DigiTop::ZDC_DigiTop(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator)
{
}

StatusCode ZDC_DigiTop::initialize()
{
  ATH_CHECK(m_digTool.retrieve());
  ATH_MSG_DEBUG ( "Retrieved ZDC_PileUpTool." );
  return StatusCode::SUCCESS;
}

StatusCode ZDC_DigiTop::execute()
{
  ATH_MSG_DEBUG ( "ZDC_DigiTop::execute" );

  return m_digTool->processAllSubEvents(Gaudi::Hive::currentContext());
}

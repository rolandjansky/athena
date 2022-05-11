/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_Digitization/AFP_DigiTop.h"
#include "PileUpTools/IPileUpTool.h"

AFP_DigiTop::AFP_DigiTop(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator)
{
}

StatusCode AFP_DigiTop::initialize()
{
  // intitialize store gate active store
  ATH_CHECK(m_digTool.retrieve());
  ATH_MSG_INFO ( "Retrieved AFP_PileUpTool." );
  return StatusCode::SUCCESS;
}

StatusCode AFP_DigiTop::execute()
{
  ATH_MSG_DEBUG ( "AFP_DigiTop::execute" );
  CHECK ( m_digTool->processAllSubEvents(Gaudi::Hive::currentContext()) );
  return StatusCode::SUCCESS;

}

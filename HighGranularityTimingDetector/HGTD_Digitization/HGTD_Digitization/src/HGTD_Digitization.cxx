/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */

#include "HGTD_Digitization.h"


HGTD_Digitization::HGTD_Digitization(const std::string &name,
                                     ISvcLocator *pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
{
}


StatusCode HGTD_Digitization::initialize()
{
  ATH_MSG_DEBUG("initialize()");
  ATH_CHECK(m_HGTD_DigitizationTool.retrieve());
  ATH_MSG_DEBUG("Successfully retreived HGTD digitization tool.");
  return StatusCode::SUCCESS;
}


StatusCode HGTD_Digitization::execute()
{
  ATH_MSG_DEBUG("execute()");
  return m_HGTD_DigitizationTool->processAllSubEvents(Gaudi::Hive::currentContext());
}

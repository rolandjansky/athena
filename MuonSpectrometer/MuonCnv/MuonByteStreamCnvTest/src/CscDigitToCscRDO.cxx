/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonByteStreamCnvTest/CscDigitToCscRDO.h"

CscDigitToCscRDO::CscDigitToCscRDO(const std::string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name, pSvcLocator),
    m_digTool("CscDigitToCscRDOTool", this )
{
   declareProperty("CscDigitToRDOTool", m_digTool);
}

StatusCode CscDigitToCscRDO::initialize()
{
  ATH_CHECK(m_digTool.retrieve());
  ATH_MSG_DEBUG("Retrieved CSC Digit -> RDO Tool.");
  return StatusCode::SUCCESS;
}

StatusCode CscDigitToCscRDO::execute()
{
  ATH_MSG_DEBUG("in execute()");
  return m_digTool->digitize(Gaudi::Hive::currentContext());
}

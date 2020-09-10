/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ZdcAnalysis/ZdcAnalysisAlg.h"

namespace ZDC
{

  ZdcAnalysisAlg ::
  ZdcAnalysisAlg (const std::string& name, ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
  {
    declareProperty ("zdcAnalysisTool", m_zdcAnalysisTool, "the IZdcAnalysisTool tool we are using");
  }



  StatusCode ZdcAnalysisAlg ::
  initialize ()
  {
    ANA_CHECK (m_zdcAnalysisTool.retrieve());
    return StatusCode::SUCCESS;
  }



  StatusCode ZdcAnalysisAlg ::
  execute ()
  {
    ANA_CHECK (m_zdcAnalysisTool->reprocessZdc());
    return StatusCode::SUCCESS;
  }

} // namespace ZDC

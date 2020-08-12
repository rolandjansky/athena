/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDCANALYSIS_ZDCANALYSISALG_H
#define ZDCANALYSIS_ZDCANALYSISALG_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <ZdcAnalysis/IZdcAnalysisTool.h>

namespace ZDC
{
  /// \brief an algorithm to wrap \ref IZdcAnalysisTool

  class ZdcAnalysisAlg final : public EL::AnaAlgorithm
  {
  public:
    ZdcAnalysisAlg (const std::string& name, ISvcLocator* pSvcLocator);

    StatusCode initialize () override;
    StatusCode execute () override;

  private:
    ToolHandle<IZdcAnalysisTool> m_zdcAnalysisTool;
  };

} // namespace ZDC

#endif




/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELTOTPIDTOOL_PIXELDEDXAODFIX_H
#define PIXELTOTPIDTOOL_PIXELDEDXAODFIX_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

namespace Trk {
  class ITrackSummaryHelperTool;
}

namespace InDet {
  class PixeldEdxAODFix : public AthAlgorithm
  {
  public:
    PixeldEdxAODFix(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~PixeldEdxAODFix();
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();
  
  private:
    ToolHandle< Trk::ITrackSummaryHelperTool > m_summarytool;
    std::vector<std::string> m_tpcontainernames;
  };

}

#endif

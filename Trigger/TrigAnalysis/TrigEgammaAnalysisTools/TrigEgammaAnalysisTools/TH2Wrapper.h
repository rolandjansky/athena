/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaAnalysisTools_TH2Wrapper_H
#define TrigEgammaAnalysisTools_TH2Wrapper_H

#include <string>
#include "GaudiKernel/ToolHandle.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

namespace TrigEgammaAnalysisTools {

    class TH2Wrapper {

    public:
        TH2Wrapper(const ToolHandle<GenericMonitoringTool>& groupHandle, const std::string& histname);
        ~TH2Wrapper() {}
        void Fill(double valueX, double valueY) const;
        void Fill(double valueX, double valueY, double weight) const;

    private:
        const ToolHandle<GenericMonitoringTool>& m_groupHandle;
        std::string m_histname;
    };

} // end of namespace TrigEgammaAnalysisTools

#endif

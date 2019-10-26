/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaAnalysisTools_TH1Wrapper_H
#define TrigEgammaAnalysisTools_TH1Wrapper_H

#include <string>
#include "GaudiKernel/ToolHandle.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

namespace TrigEgammaAnalysisTools {

    class TH1Wrapper {

    public:
        TH1Wrapper(const ToolHandle<GenericMonitoringTool>& groupHandle, const std::string& histname);
        ~TH1Wrapper() {}
        void Fill(double value) const;
        void Fill(double value, double weight) const;
        void Fill(const char * /*namex*/, double /*w*/) const;
        int GetNbinsX() const { return 0; } // To be redesigned and removed
        void AddBinContent(int /*bin*/) const {} // To be redesigned and removed

    private:
        const ToolHandle<GenericMonitoringTool>& m_groupHandle;
        std::string m_histname;
    };

} // end of namespace TrigEgammaAnalysisTools

#endif

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigEgammaAnalysisTools/TH2Wrapper.h"

#include "AthenaMonitoringKernel/MonitoredGroup.h"
#include "AthenaMonitoringKernel/MonitoredScalar.h"


namespace TrigEgammaAnalysisTools {

    TH2Wrapper::TH2Wrapper(const ToolHandle<GenericMonitoringTool>& groupHandle, const std::string& histname) :
        m_groupHandle(groupHandle),
        m_histname(histname)
        {}

    void TH2Wrapper::Fill(double valueX, double valueY) const {
        auto x = Monitored::Scalar<double>(m_histname+"#1", valueX);
        auto y = Monitored::Scalar<double>(m_histname+"#2", valueY);
        Monitored::Group(m_groupHandle, x, y).fill();
    }
    void TH2Wrapper::Fill(double valueX, double valueY, double weight) const {
        // should distinguish TH2 weighted filling and TProfile2D filling
        auto x = Monitored::Scalar<double>(m_histname+"#1", valueX);
        auto y = Monitored::Scalar<double>(m_histname+"#2", valueY);
        auto w = Monitored::Scalar<double>(m_histname+"#w", weight);
        Monitored::Group(m_groupHandle, x, y, w).fill();
    }

} // end of namespace TrigEgammaAnalysisTools

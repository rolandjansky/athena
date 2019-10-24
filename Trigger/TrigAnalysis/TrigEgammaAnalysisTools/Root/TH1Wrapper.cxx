/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigEgammaAnalysisTools/TH1Wrapper.h"

#include "AthenaMonitoringKernel/MonitoredGroup.h"
#include "AthenaMonitoringKernel/MonitoredScalar.h"


namespace TrigEgammaAnalysisTools {

    TH1Wrapper::TH1Wrapper(const ToolHandle<GenericMonitoringTool>& groupHandle, const std::string& histname) :
        m_groupHandle(groupHandle),
        m_histname(histname)
        {}

    void TH1Wrapper::Fill(double value) const {
        auto v = Monitored::Scalar<double>(m_histname, value);
        Monitored::Group(m_groupHandle, v).fill();
    }

    void TH1Wrapper::Fill(double value, double weight) const {
        // should distinguish TH1 weighted filling and TProfile filling
        auto v = Monitored::Scalar<double>(m_histname, value);
        auto w = Monitored::Scalar<double>(m_histname+"#w", weight);
        Monitored::Group(m_groupHandle, v, w).fill();
    }

    void TH1Wrapper::Fill(const char * namex, double /*w*/) const {
        auto count = Monitored::Scalar<std::string>( m_histname, namex );
        Monitored::Group(m_groupHandle, count).fill();
    }

} // end of namespace TrigEgammaAnalysisTools

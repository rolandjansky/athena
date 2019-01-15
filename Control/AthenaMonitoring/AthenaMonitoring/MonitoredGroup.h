/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoring_MonitoredGroup_h
#define AthenaMonitoring_MonitoredGroup_h

#include <functional>
#include <string>
#include <vector>

#include "AthenaMonitoring/GenericMonitoringTool.h"
#include "AthenaMonitoring/HistogramFiller.h"
#include "AthenaMonitoring/IMonitoredVariable.h"
#include "GaudiKernel/ToolHandle.h"

namespace Monitored {

  namespace impl {
  /**
   * @brief Group local monitoring quantities and retain correlation when filling histograms
   *
   * In order to maintain correlations when filling histograms (e.g. eta and phi of a track) the
   * monitored quantities need to be grouped within a Monitored::Group. The filling of the histogram
   * occurs when the Monitored::Group object goes out of scope or when fill() is called explicitly.
   *
   * \code
   * {
   *   auto phi = Monitored::Scalar("phi");
   *   auto eta = Monitored::Scalar("eta");
   *   auto mon = Monitored::Group(m_monTool, phi, eta);
   * }
   * \endcode
   **/
  class Group {
  public:
    /**
     * @brief Named constructor
     * @param tool            a handle to a monitoring tool, if invalid nothing is done
     * @param monitoredGroup  list of variables to be monitored
     **/
    template <typename... T>
    Group(const ToolHandle<GenericMonitoringTool>& tool, T&&... monitoredGroup)
        : m_tool(tool),
          m_autoFill(true),
          m_monitoredGroup{monitoredGroup...},
          m_histogramsFillers(!m_tool.empty() ? m_tool->getHistogramsFillers(m_monitoredGroup)
                                              : std::vector<HistogramFiller*>()) {}

    virtual ~Group() {
      if (m_autoFill) {
        fill();
      }
      for (auto filler : m_histogramsFillers) {
        delete filler;
      }
    }

    /**
     * @brief explicitely fill the monitoring output
     **/
    virtual void fill() {
      for (auto filler : m_histogramsFillers) {
        filler->fill();
      }
    }
    /**
     * @brief enables/disables filling when Monitored::Group leaves the scope
     *
     * Typically one time fill, this should be left enabled (true)
     * while in tight loops this should disabled "setAutoFill(false)"
     * and explicit call fill() used with the same Monitored::Group object.
     **/
    void setAutoFill(bool isEnabled) { m_autoFill = isEnabled; }

  protected:
    ToolHandle<GenericMonitoringTool> m_tool;
    bool m_autoFill;
    const std::vector<std::reference_wrapper<IMonitoredVariable>> m_monitoredGroup;
    const std::vector<HistogramFiller*> m_histogramsFillers;
  };

  template <typename... T>
  void fill(const ToolHandle<GenericMonitoringTool>& tool, T&&... variables) {
    if (!tool.empty()) {
      for (auto filler : tool->getHistogramsFillers({std::forward<T>(variables)...})) {
        filler->fill();
      }
    }
  }
  }

#if __cplusplus >= 201700
  using impl::Group;
#else
  template <typename... T>
  static impl::Group Group(const ToolHandle<GenericMonitoringTool>& tool,
                           T&&... monitoredGroup) {
    return impl::Group(tool, monitoredGroup...);
  }
#endif

} // namespace Monitored

#endif /* AthenaMonitoring_MonitoredGroup_h */

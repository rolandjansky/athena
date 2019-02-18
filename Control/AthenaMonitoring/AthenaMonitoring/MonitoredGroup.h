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

  /**
   * @brief Group local monitoring quantities and retain correlation when filling histograms
   *
   * In order to maintain correlations when filling histograms (e.g. eta and phi of a track) the
   * monitored quantities need to be grouped within a Monitored::Group. The filling of the
   * histogram occurs when the Monitored::Group object goes out of scope or when fill() is called
   * explicitly.
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
    Group(const ToolHandle<GenericMonitoringTool>& tool, T&&... monitoredGroup) :
        m_tool(tool),
        m_autoFill(true),
        m_monitoredGroup{monitoredGroup...},
        m_histogramsFillers(!m_tool.empty() ? m_tool->getHistogramsFillers(m_monitoredGroup)
                                            : std::vector<HistogramFiller*>())
    {}

    virtual ~Group()
    {
      if (m_autoFill) {
        fill();
      }
      for (auto filler : m_histogramsFillers) {
        delete filler;
      }
    }

    /**
     * @brief Explicitly fill the monitoring histograms and disable autoFill
     *
     * This will fill the monitoring histograms and also call setAutoFill(false)
     * in order to disable the automatic filling when the Monitored::Group goes
     * out of scope. A typical use-pattern is in tight loops in order not to
     * re-create the Monitored::Group object many times:
     *
     * \code
     *   auto pt = Monitored::Scalar("pt");
     *   auto mon = Monitored::Group(m_monTool, pt);
     *   for (...) {
     *      pt = ...;    // assign pt
     *      mon.fill();  // fill pt histogram
     *   }
     * \endcode
     *
     **/
    virtual void fill()
    {
      setAutoFill(false);
      for (auto filler : m_histogramsFillers) {
        filler->fill();
      }
    }

    /**
     * @brief enables/disables filling when Monitored::Group leaves the scope
     *
     * By default Monitored::Group will perform a one time fill each time it goes
     * out of scope. This feature can be disabled by calling setAutoFill(false).
     * The same is achieved by calling fill() manually.
     **/
    void setAutoFill(bool isEnabled) { m_autoFill = isEnabled; }

  protected:
    ToolHandle<GenericMonitoringTool> m_tool;
    bool m_autoFill;
    const std::vector<std::reference_wrapper<IMonitoredVariable>> m_monitoredGroup;
    const std::vector<HistogramFiller*> m_histogramsFillers;
  };

  template <typename... T>
  void fill(const ToolHandle<GenericMonitoringTool>& tool, T&&... variables)
  {
    if (!tool.empty()) {
      for (auto filler : tool->getHistogramsFillers({std::forward<T>(variables)...})) {
        filler->fill();
      }
    }
  }
} // namespace Monitored

#endif /* AthenaMonitoring_MonitoredGroup_h */

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoringKernel_MonitoredGroup_h
#define AthenaMonitoringKernel_MonitoredGroup_h

#include <functional>
#include <string>
#include <vector>
#include <stdexcept>

#include "GaudiKernel/ToolHandle.h"

#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "AthenaMonitoringKernel/HistogramFiller.h"
#include "AthenaMonitoringKernel/IMonitoredVariable.h"

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
    Group(const ToolHandle<GenericMonitoringTool>& tool, T&&... monitoredGroup) 
      : m_tool(tool),
        m_autoFill(true),
        m_monitoredGroup{monitoredGroup...},
        m_histogramsFillers(!m_tool.empty() ? m_tool->getHistogramsFillers(m_monitoredGroup) : std::vector<std::shared_ptr<Monitored::HistogramFiller>>()) { }

     Group(const ToolHandle<GenericMonitoringTool>& tool, const std::vector<std::reference_wrapper<IMonitoredVariable>>& monitoredGroup)
      : m_tool(tool),
        m_autoFill(true),
        m_monitoredGroup(monitoredGroup),
        m_histogramsFillers(!m_tool.empty() ? m_tool->getHistogramsFillers(m_monitoredGroup) : std::vector<std::shared_ptr<Monitored::HistogramFiller>>()) { }

     Group(const ToolHandle<GenericMonitoringTool>& tool, std::vector<std::reference_wrapper<IMonitoredVariable>>&& monitoredGroup)
      : m_tool(tool),
        m_autoFill(true),
        m_monitoredGroup(std::move(monitoredGroup)),
        m_histogramsFillers(!m_tool.empty() ? m_tool->getHistogramsFillers(m_monitoredGroup) : std::vector<std::shared_ptr<Monitored::HistogramFiller>>()) { }

    virtual ~Group() {
      if (m_autoFill) {
        fill();
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
    virtual void fill() {
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
    const std::vector<std::shared_ptr<Monitored::HistogramFiller>> m_histogramsFillers;
  };

  template <typename... T>
  void fill(const ToolHandle<GenericMonitoringTool>& tool, T&&... variables) {
    if (!tool.empty()) {
      for (auto filler : tool->getHistogramsFillers({std::forward<T>(variables)...})) {
        filler->fill();
      }
    }
  }

  // Sub-namespace for internal-use functions
  namespace detail {
    /** Finds the index of an element in a tool handle array by its string name */
    template <typename T = int>
    int findToolIndex( const ToolHandleArray<GenericMonitoringTool>& toolArray, const std::string& name ) {
      auto it = std::find_if(toolArray.begin(),toolArray.end(),[&](const auto& r) {return r.name()==name;});
      return it!=toolArray.end()
        ? std::distance(toolArray.begin(),it)
        : throw std::runtime_error("The tool "+name+" could not be found in the tool array.");
    }
  }

  /** Builds an array of indices (base case) */
  template<typename V,typename std::enable_if_t<std::is_integral_v<V>>* =nullptr>
  std::vector<V> buildToolMap(ToolHandleArray<GenericMonitoringTool> tools, const std::string& baseName, int nHist) {
    std::vector<int> indexArray;
    for ( int iHist=0; iHist<nHist; iHist++ ) {
      std::string groupName = baseName + "_" + std::to_string(iHist);
      indexArray.push_back(detail::findToolIndex(tools,groupName));
    }
    return indexArray;
  }

  /** Builds an N-dimensional array of indices (recursive) */
  template<typename V,typename std::enable_if_t<!std::is_integral_v<V>>* =nullptr,typename...T>
  std::vector<V> buildToolMap(ToolHandleArray<GenericMonitoringTool> tools, const std::string& baseName, int nHist, T... dimensions) {
    std::vector<V> indexArray;
    for ( int iHist=0; iHist<nHist; iHist++ ) {
      std::string groupName = baseName + "_" + std::to_string(iHist);
      indexArray.push_back(buildToolMap<typename V::value_type>(tools,groupName,dimensions...));
    }
    return indexArray;
  }

  /** Builds a map of indices (base case) */
  template<typename V,typename std::enable_if_t<std::is_integral_v<V>>* =nullptr>
  std::map<std::string,int> buildToolMap(ToolHandleArray<GenericMonitoringTool> tools, const std::string& baseName, std::vector<std::string> labels) {
    std::map<std::string,int> indexMap;
    for ( auto label : labels ) {
      std::string groupName = baseName + "_" + label;
      indexMap[label] = detail::findToolIndex(tools,groupName);
    }
    return indexMap;
  }

  /** Builds an N-dimensional map of indices (recursive) */
  template<typename V,typename std::enable_if_t<!std::is_integral_v<V>>* =nullptr,typename...T>
  std::map<std::string,V> buildToolMap(ToolHandleArray<GenericMonitoringTool> tools, const std::string& baseName, std::vector<std::string> labels, T... dimensions) {
    std::map<std::string,V> indexMap;
    for ( auto label : labels ) {
      std::string groupName = baseName + "_" + label;
      indexMap[label] = buildToolMap<typename V::mapped_type>(tools,groupName,dimensions...);
    }
    return indexMap;
  }

} // namespace Monitored

#endif /* AthenaMonitoringKernel_MonitoredGroup_h */

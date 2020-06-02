/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
   * @brief %Group of local monitoring quantities and retain correlation when filling histograms
   *
   * In order to maintain correlations when filling histograms (e.g. eta and phi of a track) the
   * monitored quantities need to be grouped within a Monitored::Group. The filling of the
   * histogram occurs when the Monitored::Group object goes out of scope or when fill() is called
   * explicitly.
   *
   * The actual histogram definitions, i.e. which variable gets filled into which histogram,
   * is configured entirely in Python. See GenericMonitoringTool.defineHistogram for the available
   * options.
   *
   * #### Examples

   * %Monitored quantities can be used to fill 2D histograms.
   * No code change is needed for it however the monitored variables need to be grouped correctly
   *   @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx fill2D_correct
   * The same variables can be re-used to perform multiple fill operations:
   *   @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx fillFromScalarIndependentScopes
   * The automatic filling at the end of the scope can be disabled:
   *   @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx fillExplicitly_noop
   * Instead <b>explicit filling</b> can be used:
   *   @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx fillExplicitly_fill
   * Histograms can be <b>filled conditionally</b> with one variable serving as a (boolean) cut mask:
   *   @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx fillWithCutMask
   * The same can be done with a Monitored::Collection:
   *   @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx fillWithCutMask_collection
   * A <b>weight variable</b> can be used for scalars:
   *   @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx fillWithWeight
   * and collections:
   *   @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx fillWithWeight_collection
   *
   * @see GenericMonitoringTool.defineHistogram
   **/
  class Group {
  public:
    /**
     * @brief %Group of monitored variables.
     *
     * The filling of the associated histograms occurs when the object goes out of scope
     * or when fill() is called explicitly.
     *
     * @param tool            a handle to a monitoring tool, if invalid nothing is done
     * @param monitoredGroup  list of variables to be monitored
     **/
    template <typename... T>
    Group(const ToolHandle<GenericMonitoringTool>& tool, T&&... monitoredGroup) 
      : m_tool(tool),
      m_autoFill(true),
      m_monitoredGroup{monitoredGroup...}
    { }

     Group(const ToolHandle<GenericMonitoringTool>& tool, const std::vector<std::reference_wrapper<IMonitoredVariable>>& monitoredGroup)
      : m_tool(tool),
      m_autoFill(true),
      m_monitoredGroup(monitoredGroup)
      { }

     Group(const ToolHandle<GenericMonitoringTool>& tool, std::vector<std::reference_wrapper<IMonitoredVariable>>&& monitoredGroup)
      : m_tool(tool),
      m_autoFill(true),
      m_monitoredGroup(std::move(monitoredGroup))
	{ }
    
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
     * @snippet Control/AthenaMonitoringKernel/test/GenericMonFilling_test.cxx fillExplicitly_fill
     **/
    virtual void fill() {
      if ( m_tool.empty() ) return;
      setAutoFill(false);
      m_tool->invokeFillers(m_monitoredGroup);
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
  };

  template <typename... T>
  void fill(const ToolHandle<GenericMonitoringTool>& tool, T&&... variables) {
    if (!tool.empty()) {
      tool->invokeFillers({std::forward<T>(variables)...});
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

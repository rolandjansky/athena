/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoring_MonitoredScope_h
#define AthenaMonitoring_MonitoredScope_h

#include <functional>
#include <vector>
#include <string>
#include <iostream>

#include "AthenaMonitoring/IMonitoredVariable.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"
#include "AthenaMonitoring/HistogramFiller.h"

namespace Monitored {
  /**
   * @brief Group local monitoring quantities and retain correlation when filling histograms 
   *
   * In order to maintain correlations when filling histograms (e.g. eta and phi of a track) the
   * monitored quantities need to be grouped within a MonitoredScope. The filling of the histogram
   * occurs when the MonitoredScope object goes out of scope or when save() is called explicitly.
   *
   * \code
   * {
   *   auto phi = MonitoredScalar::declare("phi");
   *   auto eta = MonitoredScalar::declare("eta");
   *   auto mon = MonitoredScope::declare(m_monTool, phi, eta);   
   * }
   * \endcode
   **/
  class MonitoredScope {
      public:

      /**
       * @brief Named constructor 
       * @param tool            a handle to a monitoring tool, if invalid nothing is done
       * @param scopeMonitored  list of variables to be monitored
       **/
      template <typename... T>
      static MonitoredScope declare(const ToolHandle<GenericMonitoringTool>& tool,
                                    T&&... scopeMonitored) {
          return MonitoredScope(tool, {std::forward<T>(scopeMonitored)...});
      }
        
      virtual ~MonitoredScope() {
          if (m_autoSave) {
              save();
          }

          for (auto filler : m_histogramsFillers) {
              delete filler;
          }
      }

      /**
       * @brief explicitely fill the monitoring output
       **/
      virtual void save() {
          for (auto filler : m_histogramsFillers) {
              filler->fill();
          }
      }
      /**
       * @brief enables/disables filling when MonitoredScope leaves the scope
       *
       * Tpically one time fill, this should be left enabled (true) 
       * while in tight loops this should disabled "setAutoSave(false)"
       * and explicit call save() used with the same scope object.
       **/
      void setAutoSave(bool isEnabled) {
          m_autoSave = isEnabled;
      }
      protected:
      ToolHandle<GenericMonitoringTool> m_tool;
      bool m_autoSave;
      const std::vector<std::reference_wrapper<IMonitoredVariable>> m_scopeMonitored;
      const std::vector<HistogramFiller*> m_histogramsFillers;
        
      MonitoredScope(const ToolHandle<GenericMonitoringTool>& tool, 
                     std::initializer_list<std::reference_wrapper<IMonitoredVariable>> scopeMonitored)
        : m_tool(tool), 
          m_autoSave(true), 
          m_scopeMonitored(scopeMonitored), 
          m_histogramsFillers(!m_tool.empty() ? m_tool->getHistogramsFillers(m_scopeMonitored) : std::vector<HistogramFiller*>()) { }
  };
}

#endif /* AthenaMonitoring_MonitoredScope_h */

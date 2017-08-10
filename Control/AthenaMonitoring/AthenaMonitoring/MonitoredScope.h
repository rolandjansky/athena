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
   * @brief Class used to collect local (scope) quantities and ship them to monitotoring output (histograms)
   * For examples @see GenericMonFilling_test.cxx
   **/
    class MonitoredScope {
    public:
      /**
       * @brief Named constructor 
       * @arg tool - a handle to a monitoring tool, if invalid nothing is done (cheap)
       * @arg scopeMonitored - list of varaibles to be shipped to monitoring output
       **/
      
        template <typename... T>
        static MonitoredScope declare(ToolHandle<GenericMonitoringTool> tool, T&&... scopeMonitored) {
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
	 * @berief enables/disables filling whne the MonitoredScope leaves the scope
	 * Tpically one time fill, this should be left intact (true) 
	 * while in tight loop this should disabled "setAutoSave(false)"
	 * and explicit call to save used with the same scope object.
	 **/
        void setAutoSave(bool isEnabled) {
            m_autoSave = isEnabled;
        }
    protected:
        ToolHandle<GenericMonitoringTool> m_tool;
        bool m_autoSave;
        const std::vector<std::reference_wrapper<IMonitoredVariable>> m_scopeMonitored;
        const std::vector<HistogramFiller*> m_histogramsFillers;
        
        MonitoredScope(ToolHandle<GenericMonitoringTool> tool, std::initializer_list<std::reference_wrapper<IMonitoredVariable>> scopeMonitored)
          : m_tool(tool), 
            m_autoSave(true), 
            m_scopeMonitored(scopeMonitored), 
            m_histogramsFillers(!m_tool.empty() ? m_tool->getHistogramsFillers(m_scopeMonitored) : std::vector<HistogramFiller*>()) { }
    };
}

#endif /* AthenaMonitoring_MonitoredScope_h */

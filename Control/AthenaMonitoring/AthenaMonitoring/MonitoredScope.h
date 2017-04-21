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
    class MonitoredScope {
    public:
        template <typename... T>
        static MonitoredScope declare(ToolHandle<GenericMonitoringTool> tool, T&&... scopeMonitored) {
            return MonitoredScope(tool, {std::forward<T>(scopeMonitored)...});
        }
        
        virtual ~MonitoredScope() {
            if (mAutoSave) {
                save();
            }

            for (auto filler : mHistogramsFillers) {
                delete filler;
            }
        }
        
        virtual void save() {
            for (auto filler : mHistogramsFillers) {
                filler->fill();
            }
        }
        
        void setAutoSaveEnabled(bool isEnabled) {
            mAutoSave = isEnabled;
        }
    protected:
        ToolHandle<GenericMonitoringTool> mTool;
        bool mAutoSave;
        const std::vector<std::reference_wrapper<IMonitoredVariable>> mScopeMonitored;
        const std::vector<HistogramFiller*> mHistogramsFillers;
        
        MonitoredScope(ToolHandle<GenericMonitoringTool> tool, std::initializer_list<std::reference_wrapper<IMonitoredVariable>> scopeMonitored)
          : mTool(tool), 
            mAutoSave(true), 
            mScopeMonitored(scopeMonitored), 
            mHistogramsFillers(!mTool.empty() ? mTool->getHistogramsFillers(mScopeMonitored) : std::vector<HistogramFiller*>()) { }
    };
}

#endif /* AthenaMonitoring_MonitoredScope_h */

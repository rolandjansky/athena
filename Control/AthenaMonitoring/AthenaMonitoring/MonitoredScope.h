//
//  MonitoredScope.h
//  AthenaMonitoring
//
//  Created by Piotr Sarna on 03/04/2017.
//

#ifndef MonitoredScope_h
#define MonitoredScope_h

#include <functional>
#include <vector>
#include <string>
#include <iostream>

#include "AthenaMonitoring/IMonitoredVariable.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"

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
        
        void save() {
            for (auto filler : mHistogramsFillers) {
                filler->fill();
            }
        }
        
        void setAutoSaveEnabled(bool isEnabled) {
            mAutoSave = isEnabled;
        }
    private:
        ToolHandle<GenericMonitoringTool> mTool;
        bool mAutoSave;
        const std::vector<std::reference_wrapper<IMonitoredVariable>> mScopeMonitored;
        const std::vector<GenericMonitoringTool::HistogramFiller*> mHistogramsFillers;
        
        MonitoredScope(ToolHandle<GenericMonitoringTool> tool, std::initializer_list<std::reference_wrapper<IMonitoredVariable>> scopeMonitored)
        : mTool(tool), mAutoSave(true), mScopeMonitored(scopeMonitored), mHistogramsFillers(mTool->getHistogramsFillers(mScopeMonitored)) { }
    };
    
    template <typename... T>
    static MonitoredScope declareScope(ToolHandle<GenericMonitoringTool> tool, T&&... scopeMonitored) {
        return MonitoredScope::declare(tool, scopeMonitored...);
    }
}

#endif /* MonitoredScope_h */

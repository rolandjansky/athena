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

namespace Monitored {
    class MonitoredScope {
    public:
        template <typename... T>
        static MonitoredScope declare(/*ToolHandle<IMonitoringTool>*/std::string tool, T&&... scopeMonitored) {
            return MonitoredScope(tool, {std::forward<T>(scopeMonitored)...});
        }
        
        virtual ~MonitoredScope() {
            if (mAutoSave) {
                save();
            }
        }
        
        void save() {
            std::cout << mTool << ": " << std::endl;
            // in the impl. there will be loop over the histograms here
            for (auto monitored : mScopeMonitored) {
                std::cout << monitored.get().stringName() << std::endl;
                
                for (auto value : monitored.get().getVectorRepresentation()) {
                    std::cout << value << ", ";
                }
                
                std::cout << std::endl;
            }
            
            std::cout << std::endl;
        }
        
        void setAutoSaveEnabled(bool isEnabled) {
            mAutoSave = isEnabled;
        }
    private:
        /*ToolHandle<IMonitoringTool>*/ std::string mTool;
        bool mAutoSave;
        const std::vector<std::reference_wrapper<IMonitoredVariable>> mScopeMonitored;
        
        MonitoredScope(/*ToolHandle<IMonitoringTool>*/ std::string tool, std::initializer_list<std::reference_wrapper<IMonitoredVariable>> scopeMonitored)
        : mTool(tool), mAutoSave(true), mScopeMonitored(scopeMonitored) { }
    };
    
    template <typename... T>
    static MonitoredScope declareScope(/*ToolHandle<IMonitoringTool>*/std::string tool, T&&... scopeMonitored) {
        return MonitoredScope::declare(tool, scopeMonitored...);
    }
}

#endif /* MonitoredScope_h */

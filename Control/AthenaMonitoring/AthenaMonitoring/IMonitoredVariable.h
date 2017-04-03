//
//  IMonitoredVariable.h
//  AthenaMonitoring
//
//  Created by Piotr Sarna on 03/04/2017.
//

#ifndef IMonitoredVariable_h
#define IMonitoredVariable_h

#include <string>
#include <vector>

namespace Monitored {
    class MonitoredScope;
    
    class IMonitoredVariable {
    public:
        friend MonitoredScope;
        virtual ~IMonitoredVariable() {}
        
        const std::string& stringName() const { return mStrName; }
    protected:
        const std::string mStrName;
        
        IMonitoredVariable(const std::string& strName)
        : mStrName(strName) {}
        
        virtual const std::vector<double> getVectorRepresentation() const = 0;
    };
}

#endif /* IMonitoredVariable_h */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

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
        
        const std::string& name() const { return mName; }
        virtual const std::vector<double> getVectorRepresentation() const = 0;
    protected:
        const std::string mName;
        
        IMonitoredVariable(std::string name)
        : mName(std::move(name)) {}
    };
}

#endif /* IMonitoredVariable_h */

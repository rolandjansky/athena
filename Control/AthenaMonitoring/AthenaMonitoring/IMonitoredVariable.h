/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoring_IMonitoredVariable_h
#define AthenaMonitoring_IMonitoredVariable_h

#include <string>
#include <vector>

namespace Monitored {
    class MonitoredGroup;
    
    class IMonitoredVariable {
    public:
        friend MonitoredGroup;
        virtual ~IMonitoredVariable() {}
        
        const std::string& name() const { return m_name; }
        virtual const std::vector<double> getVectorRepresentation() const = 0;
    protected:
        const std::string m_name;
        
        IMonitoredVariable(std::string name)
        : m_name(std::move(name)) {}
    };
}

#endif /* AthenaMonitoring_IMonitoredVariable_h */

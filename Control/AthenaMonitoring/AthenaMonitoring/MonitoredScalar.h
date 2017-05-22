/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoring_MonitoredScalar_h
#define AthenaMonitoring_MonitoredScalar_h

#include <vector>

#include "AthenaMonitoring/IMonitoredVariable.h"
#include "AthenaMonitoring/MonitoredHelpers.h"

namespace Monitored {
    namespace MonitoredScalar {
        template<class T>
        class MonitoredScalar;
        
        template<class T>
        MonitoredScalar<T> declare(std::string name, const T& defaultValue = {});
        
        template<class T>
        class MonitoredScalar : public IMonitoredVariable {
        public:
            static_assert(MonitoredHelpers::has_double_operator<T>::value, "Value must be convertable to double");
            friend MonitoredScalar<T> declare<T>(std::string name, const T& defaultValue);
            
            MonitoredScalar(MonitoredScalar&&) = default;
            
            void operator=(T value) { m_value = value; }
            operator T() const { return m_value; }
            
            const std::vector<double> getVectorRepresentation() const override { return { double(m_value) }; }
        private:
            T m_value;
            
            MonitoredScalar(std::string name, const T& defaultValue = {})
              : IMonitoredVariable(std::move(name)), m_value(defaultValue) { }
            MonitoredScalar(MonitoredScalar const&) = delete;
            MonitoredScalar& operator=(MonitoredScalar const&) = delete;
        };
        
        template<class T>
        MonitoredScalar<T> declare(std::string name, const T& defaultValue) {
            return MonitoredScalar<T>(std::move(name), defaultValue);
        }
    }
}

#endif /* AthenaMonitoring_MonitoredScalar_h */

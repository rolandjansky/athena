//
//  MonitoredScalar.h
//  AthenaMonitoring
//
//  Created by Piotr Sarna on 03/04/2017.
//

#ifndef MonitoredScalar_h
#define MonitoredScalar_h

#include <vector>

#include "AthenaMonitoring/IMonitoredVariable.h"
#include "AthenaMonitoring/MonitoredHelpers.h"

namespace Monitored {
    namespace MonitoredScalar {
        template<class T>
        class MonitoredScalar;
        
        template<class T>
        MonitoredScalar<T> declare(const std::string strName, const T& defaultValue = {});
        
        template<class T>
        class MonitoredScalar : public IMonitoredVariable {
        public:
            static_assert(MonitoredHelpers::has_double_operator<T>::value, "Value must be convertable to double");
            friend MonitoredScalar<T> declare<T>(const std::string strName, const T& defaultValue);
            
            MonitoredScalar(MonitoredScalar&&) = default;
            
            void operator=(T value) { mValue = value; }
            operator T() const { return mValue; }
        private:
            T mValue;
            
            MonitoredScalar(const std::string strName, const T& defaultValue = {})
            : IMonitoredVariable(strName), mValue(defaultValue) { }
            MonitoredScalar(MonitoredScalar const&) = delete;
            MonitoredScalar& operator=(MonitoredScalar const&) = delete;
            
            const std::vector<double> getVectorRepresentation() const override { return { double(mValue) }; }
        };
        
        template<class T>
        MonitoredScalar<T> declare(const std::string strName, const T& defaultValue) {
            return MonitoredScalar<T>(strName, defaultValue);
        }
    }
    
    template<class T>
    MonitoredScalar::MonitoredScalar<T> declareScalar(const std::string strName, const T& defaultValue) {
        return MonitoredScalar::declare(strName, defaultValue);
    }
}

#endif /* MonitoredScalar_h */

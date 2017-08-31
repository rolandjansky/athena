/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoring_MonitoredScalar_h
#define AthenaMonitoring_MonitoredScalar_h

#include <vector>

#include "AthenaMonitoring/IMonitoredVariable.h"

namespace Monitored {

    /**
     * Monitoring of scalars
     */
    namespace MonitoredScalar {
        template<class T>
        class MonitoredScalar;
        
        template<class T>
        MonitoredScalar<T> declare(std::string name, const T& defaultValue = {});
        
        /**
         * Monitoring of scalars
         *
         * This class is not supposed to be used by the end user.
         */
        template<class T>
        class MonitoredScalar : public IMonitoredVariable {
        public:
            static_assert(std::is_convertible<T,double>::value,"Value must be convertable to double");

            /// @brief .     \if empty doc string required due to doxygen bug 787131 \endif
            friend MonitoredScalar<T> declare<T>(std::string name, const T& defaultValue);
            
            MonitoredScalar(MonitoredScalar&&) = default;
            
            T operator=(T value) { m_value = value;  return value; }
	    
            operator T() const { return m_value; }
            operator T&() { return m_value; }
            
            const std::vector<double> getVectorRepresentation() const override { return { double(m_value) }; }
        private:
            T m_value;
            
            MonitoredScalar(std::string name, const T& defaultValue = {})
              : IMonitoredVariable(std::move(name)), m_value(defaultValue) { }
            MonitoredScalar(MonitoredScalar const&) = delete;
            MonitoredScalar& operator=(MonitoredScalar const& ) = delete;
        };
        
        /**
         * Declare a monitored scalar
         *
         * Monitoring for any double-convertable scalar
         *
         * @param name         Name of monitored quantity
         * @param defaultValue default value assigned to the monitored scalar
         *
         * \code
         *   auto phi = MonitoredScalar::declare("phi", 4.2);       // deduced double
         *   auto eta = MonitoredScalar::declare<float>("eta", 0);  // explicit float
         * \endcode
         */
        template<class T>
        MonitoredScalar<T> declare(std::string name, const T& defaultValue) {
            return MonitoredScalar<T>(std::move(name), defaultValue);
        }
    }
}

#endif /* AthenaMonitoring_MonitoredScalar_h */

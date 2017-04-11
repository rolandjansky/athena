/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoring_MonitoringHelpers_h
#define AthenaMonitoring_MonitoringHelpers_h

#include <type_traits>

namespace Monitored {
    namespace MonitoredHelpers {
        template <typename T>
        struct get_collection_values_type {
            typedef typename T::value_type value_type;
        };
        
        template <typename T, size_t N>
        struct get_collection_values_type<T[N]> {
            typedef T value_type;
        };
        
        template <typename, typename = void>
        struct has_double_operator {
            enum { value = false };
        };
        
        template <typename T>
        struct has_double_operator<T, decltype(void(double(std::declval<const T&>())))> {
            enum { value = true };
        };
        
        template <typename T>
        struct are_collection_values_have_double_operator {
            enum { value = has_double_operator<typename get_collection_values_type<T>::value_type>::value };
        };
    }
}

#endif /* AthenaMonitoring_MonitoringHelpers_h */

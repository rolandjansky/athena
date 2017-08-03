/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoring_MonitoredCollection_h
#define AthenaMonitoring_MonitoredCollection_h

#include <functional>
#include <vector>
#include <type_traits>

#include "AthenaMonitoring/IMonitoredVariable.h"

namespace Monitored {

    namespace MonitoredCollection {
  
        // Forward declares
        template<class T> class MonitoredValuesCollection;
        template<class T> class MonitoredObjectsCollection;

        /**
         * Declare a monitored (double-convertable) collection
         *
         * Any iterable container with elements covertable to double can be monitored.
         *
         * @param name        Name of monitored quantity
         * @param collection  Collection to be monitored (e.g. STL container or array)
         *
         * \code
         *   std::vector<float> eta( {0.2, 0.1} );
         *   auto m = MonitoredCollection::declare( "Eta", eta );
         * \endcode
         **/
        template<class T>
        MonitoredValuesCollection<T> declare(std::string name, const T& collection) {
          return MonitoredValuesCollection<T>(std::move(name), collection);
        }
        
         /**
         * Declare a monitored collection of objects
         *
         * A converter function/accessor needs to be provided to extract the relevant quantity.
         *
         * @param name        Name of monitored quantity
         * @param collection  Collection to be monitored (e.g. STL container or array)
         * @param converterToDouble  Function taking an element and returning a double
         *
         * \code
         *   std::vector<Track> tracks;
         *   auto phi = MonitoredCollection::declare( "Phi", tracks, []( const Track& t ) { return t.phi(); } );
         * \endcode
         **/
        template<class T>
        MonitoredObjectsCollection<T> declare(std::string name, const T& collection, 
                                              std::function<double(const typename MonitoredObjectsCollection<T>::value_type&)> converterToDouble) {
            return MonitoredObjectsCollection<T>(std::move(name), collection, 
                                                 std::move(converterToDouble));
        }

        namespace detail {
            /// Get element type for containers
            template <typename T>
            struct get_value_type {
                typedef typename T::value_type value_type;
            };
            /// Get element type for arrays
            template <typename T, size_t N>
            struct get_value_type<T[N]> {
                typedef T value_type;
            };
        }
  
        
        /**
         * Monitored collection
         *
         * This class is not supposed to be used by the end user.
         **/
        template<class T>
        class MonitoredValuesCollection : public IMonitoredVariable {
        public:
            /// Type of the collection elements
            using value_type = typename detail::get_value_type<T>::value_type;

            static_assert(std::is_convertible<value_type, double>::value, 
                          "Collection values must be convertable to double");

            friend MonitoredValuesCollection<T> declare<T>(std::string name, const T& collection);
            
            MonitoredValuesCollection(MonitoredValuesCollection&&) = default;
            
            const std::vector<double> getVectorRepresentation() const override {
                return std::vector<double>(std::begin(m_collection), std::end(m_collection));
            }
        private:
            const T& m_collection;
            
            MonitoredValuesCollection(std::string name, const T& collection)
              : IMonitoredVariable(std::move(name)), m_collection(collection) { }
            MonitoredValuesCollection(MonitoredValuesCollection const&) = delete;
            MonitoredValuesCollection& operator=(MonitoredValuesCollection const&) = delete;
        };
        

        /**
         * Monitored object collection
         *
         * This class is not supposed to be used by the end user.
         **/
        template<class T>
        class MonitoredObjectsCollection : public IMonitoredVariable {
        public:
            /// Type of the collection elements
            using value_type = typename detail::get_value_type<T>::value_type;

            friend MonitoredObjectsCollection<T> declare<T>(std::string name, const T& collection, 
                                                            std::function<double(const value_type&)> converterToDouble);
            
            MonitoredObjectsCollection(MonitoredObjectsCollection&&) = default;
            
            const std::vector<double> getVectorRepresentation() const override {
                // Could be replaced with std::size in C++17
                auto N = std::distance(std::begin(m_collection), std::end(m_collection));
                
                // Reserve space and fill vector
                std::vector<double> result;
                result.reserve(N);
                for(auto value : m_collection) result.push_back(m_converterToDouble(value));
                
                return result;
            }
        private:
            const T& m_collection;
            std::function<double(const value_type&)> m_converterToDouble;
            
            MonitoredObjectsCollection(std::string name, const T& collection, 
                                       std::function<double(const value_type&)> converterToDouble)
            : IMonitoredVariable(std::move(name)), 
              m_collection(collection), 
              m_converterToDouble(std::move(converterToDouble)) { }
            MonitoredObjectsCollection(MonitoredObjectsCollection const&) = delete;
            MonitoredObjectsCollection& operator=(MonitoredObjectsCollection const&) = delete;
        };
    }
}

#endif /* AthenaMonitoring_MonitoredCollection_h */

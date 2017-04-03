//
//  MonitoredCollection.h
//  AthenaMonitoring
//
//  Created by Piotr Sarna on 03/04/2017.
//

#ifndef MonitoredCollection_h
#define MonitoredCollection_h

#include <functional>

#include "AthenaMonitoring/IMonitoredVariable.h"
#include "AthenaMonitoring/MonitoredHelpers.h"

namespace Monitored {
    namespace MonitoredCollection {
        template<class T>
        class MonitoredValuesCollection;
        
        template<class T>
        MonitoredValuesCollection<T> declare(const std::string strName, const T& collection);
        
        template<class T>
        class MonitoredObjectsCollection;
        
        template<class T>
        MonitoredObjectsCollection<T> declare(const std::string strName, const T& collection, std::function<double(const typename MonitoredHelpers::get_collection_values_type<T>::value_type&)> converterToDouble);
        
        template<class T>
        class MonitoredValuesCollection : public IMonitoredVariable {
        public:
            static_assert(MonitoredHelpers::are_collection_values_have_double_operator<T>::value, "Collection values must be convertable to double");
            friend MonitoredValuesCollection<T> declare<T>(const std::string strName, const T& collection);
            
            MonitoredValuesCollection(MonitoredValuesCollection&&) = default;
            
            const std::vector<double> getVectorRepresentation() const override {
                return std::vector<double>(std::begin(mCollection), std::end(mCollection));
            }
        private:
            const T& mCollection;
            
            MonitoredValuesCollection(const std::string strName, const T& collection)
            : IMonitoredVariable(strName), mCollection(collection) { }
            MonitoredValuesCollection(MonitoredValuesCollection const&) = delete;
            MonitoredValuesCollection& operator=(MonitoredValuesCollection const&) = delete;
        };
        
        template<class T>
        class MonitoredObjectsCollection : public IMonitoredVariable {
        public:
            friend MonitoredObjectsCollection<T> declare<T>(const std::string strName, const T& collection, std::function<double(const typename MonitoredHelpers::get_collection_values_type<T>::value_type&)> converterToDouble);
            
            MonitoredObjectsCollection(MonitoredObjectsCollection&&) = default;
            
            const std::vector<double> getVectorRepresentation() const override {
                auto distance = std::distance(std::begin(mCollection), std::end(mCollection));
                
                std::vector<double> result;
                result.reserve(distance);
                
                for(auto value : mCollection) {
                    result.push_back(mConverterToDouble(value));
                }
                
                return result;
            }
        private:
            const T& mCollection;
            std::function<double(const typename MonitoredHelpers::get_collection_values_type<T>::value_type&)> mConverterToDouble;
            
            MonitoredObjectsCollection(const std::string strName, const T& collection, std::function<double(const typename MonitoredHelpers::get_collection_values_type<T>::value_type&)> converterToDouble)
            : IMonitoredVariable(strName), mCollection(collection), mConverterToDouble(converterToDouble) { }
            MonitoredObjectsCollection(MonitoredObjectsCollection const&) = delete;
            MonitoredObjectsCollection& operator=(MonitoredObjectsCollection const&) = delete;
        };
        
        template<class T>
        MonitoredValuesCollection<T> declare(const std::string strName, const T& collection) {
            return MonitoredValuesCollection<T>(strName, collection);
        }
        
        template<class T>
        MonitoredObjectsCollection<T> declare(const std::string strName, const T& collection, std::function<double(const typename MonitoredHelpers::get_collection_values_type<T>::value_type&)> converterToDouble) {
            return MonitoredObjectsCollection<T>(strName, collection, converterToDouble);
        }
    }
    
    template<class T>
    MonitoredCollection::MonitoredValuesCollection<T> declareCollection(const std::string strName, const T& collection) {
        return MonitoredCollection::declare(strName, collection);
    }
    
    template<class T>
    MonitoredCollection::MonitoredObjectsCollection<T> declareCollection(const std::string strName, const T& collection, std::function<double(const typename MonitoredHelpers::get_collection_values_type<T>::value_type&)> converterToDouble) {
        return MonitoredCollection::declare(strName, collection, converterToDouble);
    }
}

#endif /* MonitoredCollection_h */

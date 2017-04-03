/*
 * Author: Piotr Sarna
 */

// mandatory headers
#include <type_traits>
#include <functional>

// tests and examples headers
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <array>
#include <set>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Monitoring {
    namespace MonitoringHelpers {
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
    
    namespace MonitoredScalar {
        template<class T>
        class MonitoredScalar;
        
        template<class T>
        MonitoredScalar<T> declare(const std::string strName, const T& defaultValue = {});
        
        template<class T>
        class MonitoredScalar : public IMonitoredVariable {
        public:
            static_assert(MonitoringHelpers::has_double_operator<T>::value, "Value must be convertable to double");
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
    
    namespace MonitoredCollection {
        template<class T>
        class MonitoredValuesCollection;
        
        template<class T>
        MonitoredValuesCollection<T> declare(const std::string strName, const T& collection);
        
        template<class T>
        class MonitoredObjectsCollection;
        
        template<class T>
        MonitoredObjectsCollection<T> declare(const std::string strName, const T& collection, std::function<double(const typename MonitoringHelpers::get_collection_values_type<T>::value_type&)> converterToDouble);
        
        template<class T>
        class MonitoredValuesCollection : public IMonitoredVariable {
        public:
            static_assert(MonitoringHelpers::are_collection_values_have_double_operator<T>::value, "Collection values must be convertable to double");
            friend MonitoredValuesCollection<T> declare<T>(const std::string strName, const T& collection);
            
            MonitoredValuesCollection(MonitoredValuesCollection&&) = default;
        private:
            const T& mCollection;
            
            MonitoredValuesCollection(const std::string strName, const T& collection)
                : IMonitoredVariable(strName), mCollection(collection) { }
            MonitoredValuesCollection(MonitoredValuesCollection const&) = delete;
            MonitoredValuesCollection& operator=(MonitoredValuesCollection const&) = delete;
            
            const std::vector<double> getVectorRepresentation() const override {
                return std::vector<double>(std::begin(mCollection), std::end(mCollection));
            }
        };
        
        template<class T>
        class MonitoredObjectsCollection : public IMonitoredVariable {
        public:
            friend MonitoredObjectsCollection<T> declare<T>(const std::string strName, const T& collection, std::function<double(const typename MonitoringHelpers::get_collection_values_type<T>::value_type&)> converterToDouble);
            
            MonitoredObjectsCollection(MonitoredObjectsCollection&&) = default;
        private:
            const T& mCollection;
            std::function<double(const typename MonitoringHelpers::get_collection_values_type<T>::value_type&)> mConverterToDouble;
            
            MonitoredObjectsCollection(const std::string strName, const T& collection, std::function<double(const typename MonitoringHelpers::get_collection_values_type<T>::value_type&)> converterToDouble)
                : IMonitoredVariable(strName), mCollection(collection), mConverterToDouble(converterToDouble) { }
            MonitoredObjectsCollection(MonitoredObjectsCollection const&) = delete;
            MonitoredObjectsCollection& operator=(MonitoredObjectsCollection const&) = delete;
            
            const std::vector<double> getVectorRepresentation() const override {
                auto distance = std::distance(std::begin(mCollection), std::end(mCollection));
                
                std::vector<double> result;
                result.reserve(distance);
                
                for(auto value : mCollection) {
                    result.push_back(mConverterToDouble(value));
                }
                
                return result;
            }
        };
        
        template<class T>
        MonitoredValuesCollection<T> declare(const std::string strName, const T& collection) {
            return MonitoredValuesCollection<T>(strName, collection);
        }
        
        template<class T>
        MonitoredObjectsCollection<T> declare(const std::string strName, const T& collection, std::function<double(const typename MonitoringHelpers::get_collection_values_type<T>::value_type&)> converterToDouble) {
            return MonitoredObjectsCollection<T>(strName, collection, converterToDouble);
        }
    }

    template<class T>
    MonitoredScalar::MonitoredScalar<T> declareScalar(const std::string strName, const T& defaultValue) {
        return MonitoredScalar::declare(strName, defaultValue);
    }

    template<class T>
    MonitoredCollection::MonitoredValuesCollection<T> declareCollection(const std::string strName, const T& collection) {
        return MonitoredCollection::declare(strName, collection);
    }
    
    template<class T>
    MonitoredCollection::MonitoredObjectsCollection<T> declareCollection(const std::string strName, const T& collection, std::function<double(const typename MonitoringHelpers::get_collection_values_type<T>::value_type&)> converterToDouble) {
        return MonitoredCollection::declare(strName, collection, converterToDouble);
    }

    template <typename... T>
    static MonitoredScope declareScope(/*ToolHandle<IMonitoringTool>*/std::string tool, T&&... scopeMonitored) {
        return MonitoredScope::declare(tool, scopeMonitored...);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Tests and examples of use
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Monitoring;

class Scalar {
public:
    Scalar() : mValue(0) { }
    Scalar(double value) : mValue(value) { }
    
    void operator=(double value) { mValue = value; }
    operator double() const { return mValue; }
private:
    double mValue;
};

class Track {
public:
    Track() : m_eta(0), m_phi(0) {}
    Track(double e, double p) : m_eta(e), m_phi(p) {}
    double eta() const { return m_eta; }
    double phi() const { return m_phi; }
private:
    double m_eta, m_phi;
};

// this is the most typical case
void scalarTest () {
    std::cout << "scalarTest" << std::endl;
    
    auto roiPhi = MonitoredScalar::declare("phi", -99.0); //deducted double
    auto roiEta = MonitoredScalar::declare<double>("eta", -99); //explicite double
    auto scalarObj = declareScalar("scalar", Scalar(15)); //class object convertable to number
    // auto error = MonitoredScalar::declare("error", "compile error value");

    auto monitorIt = MonitoredScope::declare("MonToolHandle", roiPhi, roiEta, scalarObj);  // this is binding to histograms
    
    //    auto roiPhiCopy = roiPhi; //error - copy constuctor prohibited
    //    roiEta = roiPhi; //error - assign prohibited
    
    //     do some computation
    roiPhi = 0.45;
    roiEta = -0.34;
    scalarObj = 7.1;
} // save happens here

void valuesCollectionTest() {
    std::cout << "valuesCollectionTest" << std::endl;
    
    std::vector<int> someVectorValues;
    std::list<Scalar> someListValues;
    std::set<double> someSetValues;
    std::array<double, 2> someArrayValues;
    double someRawArrayValues[2];
    
    auto vector = MonitoredCollection::declare("vector values", someVectorValues);
    auto list = MonitoredCollection::declare("list values", someListValues);
    auto set = MonitoredCollection::declare("set values", someSetValues);
    auto array = MonitoredCollection::declare("array values", someArrayValues);
    auto rawArray = declareCollection("raw array values", someRawArrayValues);
    
    auto monitorIt = declareScope("MonToolHandle", vector, list, set, array, rawArray); // this is binding to histograms
    
    someVectorValues.push_back(3);
    someVectorValues.push_back(1);
    
    someListValues.push_back(Scalar(3.5f));
    someListValues.push_back(Scalar(7.11f));
    
    someSetValues.insert(0.4);
    someSetValues.insert(-0.6);
    
    someArrayValues[0] = 1.3;
    someArrayValues[1] = 0.2;
    
    someRawArrayValues[0] = 3.1415;
    someRawArrayValues[1] = -3;
} // save happens here

void objectsCollectionTest() {
    std::cout << "objectsCollectionTest" << std::endl;
    
    Track tracks[2];
    
    auto eta = MonitoredCollection::declare("eta", tracks, &Track::eta);
    auto phi = declareCollection("phi", tracks, [](const Track& t) { return t.phi(); });
    
    auto monitorIt = MonitoredScope::declare("MonToolHandle", eta, phi); // this is binding to histograms
    
    tracks[0] = Track(0.1, 0.9);
    tracks[1] = Track(1.3, 1.);
} // save happens here

void inLoopTest() {
    std::cout << "inLoopTest" << std::endl;
    
    auto phi = MonitoredScalar::declare<double>("phi", -99); //explicite double
    auto eta = MonitoredScalar::declare("eta", -99.1); //deducted double
    
    auto monitorIt = MonitoredScope::declare("MonToolHandle", phi, eta); // this is binding to histograms
    monitorIt.setAutoSaveEnabled(false); // required to avoid doubled last value
    
    for ( int i = 0; i < 3; ++i ) {
        phi = i+0.2;
        monitorIt.save(); // fast
    }
} // save happens here

int main(int argc, const char * argv[]) {
    scalarTest();
    valuesCollectionTest();
    objectsCollectionTest();
    inLoopTest();
} 

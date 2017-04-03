// tests and examples headers
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <array>
#include <set>

#include "AthenaMonitoring/MonitoredScope.h"
#include "AthenaMonitoring/MonitoredScalar.h"
#include "AthenaMonitoring/MonitoredCollection.h"

using namespace Monitored;

////////////////////////////////////////////////////////////////////////////////////////////////////
// Tests and examples of use
////////////////////////////////////////////////////////////////////////////////////////////////////

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
    //    auto error = MonitoredScalar::declare("error", "compile error value");
    
    auto monitorIt = MonitoredScope::declare("MonToolHandle", roiPhi, roiEta, scalarObj);  // this is binding to histograms
    
    //    auto roiPhiCopy = roiPhi; //error - copy constuctor prohibited
    //    roiEta = roiPhi; //error - assign prohibited
    //
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

int main(void) {
    scalarTest();
    valuesCollectionTest();
    objectsCollectionTest();
    inLoopTest();
} 

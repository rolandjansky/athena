/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "DataModel/DataVector.h"
#include "DataModel/ElementLink.h"
#include "DataModel/ElementLinkVector.h"
#include "GaudiKernel/ClassID.h"


#include "TrigInterfaces/FexAlgo.h"
#include "TrigInterfaces/HypoAlgo.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigPassBits.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "xAODTrigger/TrigPassBits.h"


//////////////////////////////////////////////////////////////////

class TestClass {
public:
  TestClass(int arg){a = arg;}
  ~TestClass() { std::cerr << " deleting TestClass: "<< a << std::endl; }
  int a;
};

CLASS_DEF(TestClass, 6421, 1)

class TestClassContainer : public DataVector<TestClass> {
public:
  virtual ~TestClassContainer() {
    std::cerr << " deleting TestClassContainer: " << std::endl;
  }
};

CLASS_DEF(TestClassContainer, 64210, 1)


using namespace HLT;

class TestHypoAlgo  : HLT::HypoAlgo {
public:
  TestHypoAlgo(const std::string& name, ISvcLocator* pSvcLocator) 
    : HypoAlgo(name, pSvcLocator) {}
  StatusCode hltIntitialize() { return StatusCode::SUCCESS; }

  
  virtual HLT::ErrorCode hltExecute(const TriggerElement* output, 
				    bool& decision) {

    const TrigRoiDescriptor *t(0);
    getFeature(output, t, "duno");

    attachFeature(output, t, "duno2" );
    decision = false;
    return HLT::OK;
  }
};


class TestFexAlgo  : HLT::FexAlgo {
public:
  TestFexAlgo(const std::string& name, ISvcLocator* pSvcLocator) 
    : FexAlgo(name, pSvcLocator) {}

  StatusCode hltIntitialize() { return StatusCode::SUCCESS; }
  virtual HLT::ErrorCode hltExecute(const TriggerElement* /*input*/, 
				    TriggerElement* output) {

    // get features
    const TrigRoiDescriptor *t(0);
    getFeature(output, t, "duno");

    ElementLink<TrigRoiDescriptorCollection> el;
    getFeatureLink<TrigRoiDescriptorCollection, TrigRoiDescriptor>(output, el, "duno");
    
    std::vector<const TrigRoiDescriptor*> tvec;
    getFeatures(output, tvec, "duno");


    ElementLinkVector<TrigRoiDescriptorCollection> elvec;
    getFeaturesLinks<TrigRoiDescriptorCollection, TrigRoiDescriptor>(output, elvec, "duno");

    std::string s;
    getStoreGateKey(t, s);

    // attach features
    attachFeature(output, t, "duno2" );
    recordAndAttachFeature(output, t, s, "duno3");

    reAttachFeature(output, t, s, "duno3");
    


    return HLT::OK;

  }

  
};













int main () {

}

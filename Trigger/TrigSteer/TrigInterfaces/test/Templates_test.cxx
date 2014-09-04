/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "DataModel/DataVector.h"
#include "DataModel/ElementLink.h"
#include "DataModel/ElementLinkVector.h"
#include "GaudiKernel/ClassID.h"


#include "TrigInterfaces/FexAlgo.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

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
    
    TrigFeatureLink roifl1;
    object2FeatureLink(output, "duno3", t, roifl1);

    TrigFeatureLink roifl2;
    TrigRoiDescriptorCollection* roicol = new TrigRoiDescriptorCollection();    
    TrigRoiDescriptor* special = new TrigRoiDescriptor();
    roicol->push_back(new TrigRoiDescriptor());
    roicol->push_back(special);
    roicol->push_back(new TrigRoiDescriptor());
    attachFeature(output, roicol, "collection");
    

    object2FeatureLink(output, "collection", special, roicol,  roifl2);

    const TrigRoiDescriptor* back;
    featureLink2Object(roifl2, back);
    featureLink2Object(roifl1, back);





    return HLT::OK;


  }
};













int main () {

}

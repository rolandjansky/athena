/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGUPGRADETEST_TESTINPUTMAKER_H
#define TRIGUPGRADETEST_TESTINPUTMAKER_H 


//#include <string>
#include "DecisionHandling/InputMakerBase.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "AthContainers/ConstDataVector.h"
#include "StoreGate/ReadHandleKeyArray.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

namespace HLTTest {

  /**
   * @class TestInputMaker
   * @brief Used at the start of a sequence: retrieves filtered collection via menu decision from previous step and writes it out directly so it can be used as input by the reco alg that follows in sequence. This test version creates TC collection output for emulation tests
   **/

  using namespace TrigCompositeUtils;
  
  class TestInputMaker
    : public ::InputMakerBase
  { 
  public: 
    TestInputMaker( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~TestInputMaker(); 
    virtual StatusCode  initialize() override;
    virtual StatusCode  execute(const EventContext&) const override;
    virtual StatusCode  finalize() override;

  private: 
    TestInputMaker();
 
    // Use this to customise the example for a different input feature
    typedef TrigRoiDescriptor FeatureOBJ;
    typedef TrigRoiDescriptorCollection FeatureContainer;


    SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_recoOutput { this, "Output", "undefined", "name of the output collection for input to next reco alg in sequence" };
   
    StringProperty m_linkName   {this, "LinkName", "initialRoI",  "name of the link to the features in the decision, e.g. 'feature', 'initialRoI'"};
    //StringProperty m_outputType {this, "OutputType","outputType", "reserved for future use"};

  }; 

} //> end namespace HLTTest
#endif //> !TRIGUPGRADETEST_TESTINPUTMAKER_H

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGUPGRADETEST_TESTINPUTMAKER_H
#define TRIGUPGRADETEST_TESTINPUTMAKER_H 1


#include <string>
#include "xAODTrigger/TrigCompositeContainer.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "AthenaBaseComps/AthAlgorithm.h"
//#include "GaudiKernel/ToolHandle.h"
#include "AthContainers/ConstDataVector.h"
#include "StoreGate/ReadHandleKeyArray.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "ITestHypoTool.h"

namespace HLTTest {

  /**
   * @class TestInputMaker
   * @brief Used at the start of a sequence: retrieves filtered collection via menu decision from previous step and writes it out directly so it can be used as input by the reco alg that follows in sequence.
   **/

  using namespace TrigCompositeUtils;
  
  class TestInputMaker
    : public ::AthAlgorithm
  { 
  public: 
    TestInputMaker( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~TestInputMaker(); 

    //TestInputMaker &operator=(const TestInputMaker &alg); 

    StatusCode  initialize() override;
    StatusCode  execute() override;
    StatusCode  finalize() override;

  private: 
    TestInputMaker();
 
    typedef TrigRoiDescriptor FeatureOBJ;
    typedef TrigRoiDescriptorCollection FeatureContainer;


    SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_recoOutput { this, "Output", "undefined", "name of the output collection for input to next reco alg in sequence" };
    
    SG::ReadHandleKeyArray <TrigCompositeUtils::DecisionContainer> m_inputDecisionsKey  { this, "InputDecisions",  {}, "Input Decisions (implicit)" };
    SG::WriteHandleKeyArray<TrigCompositeUtils::DecisionContainer> m_outputDecisionsKey { this, "OutputDecisions", {}, "Ouput Decisions" };
    //SG::WriteHandleKeyArray< ConstDataVector<TrigCompositeUtils::DecisionContainer> > m_outputDecisionsKey{ this, "OutputDecisions", {}, "Output Decisions" };

    StringProperty m_linkName   {this, "LinkName", "initialRoI",  "name of the link to the features in the decision, e.g. 'feature', 'initialRoI'"};
    StringProperty m_outputType {this, "OutputType","outputType", "reserved for future use"};

  }; 

} //> end namespace HLTTest
#endif //> !TRIGUPGRADETEST_TESTINPUTMAKER_H

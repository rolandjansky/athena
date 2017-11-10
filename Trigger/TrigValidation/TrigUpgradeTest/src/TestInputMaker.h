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
    //SG::WriteHandleKey<ConstDataVector<xAOD::TrigCompositeContainer> > m_recoOutput;
    //    SG::WriteHandleKey<ConstDataVector<TrigRoiDescriptorCollection> > m_recoOutput;
    //SG::WriteHandleKey<ConstDataVector<TestCluster> > m_recoOutput;

    SG::ReadHandleKeyArray<TrigCompositeUtils::DecisionContainer> m_inputs;
    SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_recoOutput;
    StringProperty m_linkName; 
    StringProperty m_outputType; 
  }; 

} //> end namespace HLTTest
#endif //> !TRIGUPGRADETEST_TESTINPUTMAKER_H

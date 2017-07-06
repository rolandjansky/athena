/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGUPGRADETEST_TESTHYPOALG_H
#define TRIGUPGRADETEST_TESTHYPOALG_H 1


#include <string>
#include "xAODTrigger/TrigCompositeContainer.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "AthenaBaseComps/AthAlgorithm.h"
//#include "GaudiKernel/ToolHandle.h"
#include "ITestHypoTool.h"
namespace HLTTest {

  /**
   * @class TestHypoAlg
   * @brief Invokes HypoTools and stores the result of the decision
   **/

  using namespace TrigCompositeUtils;
  
  class TestHypoAlg
    : public ::AthAlgorithm
  { 
  public: 
    TestHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~TestHypoAlg(); 

    //TestHypoAlg &operator=(const TestHypoAlg &alg); 

    StatusCode  initialize() override;
    StatusCode  execute() override;
    StatusCode  finalize() override;

  private: 
    TestHypoAlg();
    ToolHandleArray<ITestHypoTool> m_tools;
    SG::ReadHandleKey<xAOD::TrigCompositeContainer> m_recoInput;
    SG::WriteHandleKey<DecisionContainer> m_output;
  }; 

} //> end namespace HLTTest
#endif //> !TRIGUPGRADETEST_TESTHYPOALG_H

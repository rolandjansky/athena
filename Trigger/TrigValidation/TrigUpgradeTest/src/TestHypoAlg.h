/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGUPGRADETEST_TESTHYPOALG_H
#define TRIGUPGRADETEST_TESTHYPOALG_H 1


#include <string>
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "ITestHypoTool.h"
namespace HLTTest {

  /**
   * @class TestHypoAlg
   * @brief Invokes HypoTools and stores the result of the decision
   **/

  using namespace TrigCompositeUtils;
  
  class TestHypoAlg : public ::AthReentrantAlgorithm 
  
  { 
  public: 
    TestHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~TestHypoAlg(); 


    StatusCode  initialize() override;
    virtual StatusCode  execute_r( const EventContext& context ) const override;
    StatusCode  finalize() override;

  private: 
    TestHypoAlg();
    ToolHandleArray<ITestHypoTool> m_tools                       { this, "HypoTools", {}, "Hypo tools" };
    SG::ReadHandleKey<xAOD::TrigCompositeContainer> m_recoInput  { this, "Input", "Input", "Key for reco input"};
    SG::ReadHandleKey <DecisionContainer> m_previousDecisions    { this, "previousDecisions", "previousDecisions", "Key for decisions per RoI" };
    SG::WriteHandleKey<DecisionContainer> m_output               { this, "Output", "Output", "Key for decision output"};

  
  }; 

} //> end namespace HLTTest
#endif //> !TRIGUPGRADETEST_TESTHYPOALG_H

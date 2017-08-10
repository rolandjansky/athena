/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGUPGRADETEST_TESTCOMBOHYPOALG_H
#define TRIGUPGRADETEST_TESTCOMBOHYPOALG_H 1


#include <string>
#include "xAODTrigger/TrigCompositeContainer.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "AthenaBaseComps/AthAlgorithm.h"

namespace HLTTest {

/**
 * @class $(klass)s
 * @brief 
 **/
  using namespace TrigCompositeUtils;
class TestComboHypoAlg
  : public ::AthAlgorithm
{ 
 public: 
  TestComboHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TestComboHypoAlg(); 

  //TestComboHypoAlg &operator=(const TestComboHypoAlg &alg); 

  StatusCode  initialize() override;
  StatusCode  execute() override;
  StatusCode  finalize() override;
  
 private: 

  SG::ReadHandleKey<xAOD::TrigCompositeContainer> m_recoInput1;
  SG::ReadHandleKey<xAOD::TrigCompositeContainer> m_recoInput2;
  SG::WriteHandleKey<DecisionContainer> m_output1;
  SG::WriteHandleKey<DecisionContainer> m_output2;
  std::string m_decisionLabel; // I am lazy and do not create tools for now, alg is able to make just one decision
  std::string m_property1;
  std::string m_property2;
  float m_threshold1;
  float m_threshold2;
  bool passed( const Decision* d1, const Decision* d2 ) const;
}; 

} //> end namespace HLTTest
#endif //> !TRIGUPGRADETEST_TESTCOMBOHYPOALG_H

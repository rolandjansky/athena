/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGUPGRADETEST_TESTMERGER_H
#define TRIGUPGRADETEST_TESTMERGER_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
namespace HLTTest {
  using namespace TrigCompositeUtils;
  class TestMerger
    : public ::AthAlgorithm
  { 

  public: 

    TestMerger( const std::string& name, ISvcLocator* pSvcLocator );

    //    virtual ~TestMerger(){}


    StatusCode  initialize() override;
    StatusCode  execute() override;
    StatusCode  finalize() override;

  private: 


    TestMerger();
    std::vector<std::string> m_inputs;
    SG::WriteHandleKey<DecisionContainer> m_outputKey;
  }; 

} //> end namespace HLTTest
#endif //> !TRIGUPGRADETEST_TESTMERGER_H

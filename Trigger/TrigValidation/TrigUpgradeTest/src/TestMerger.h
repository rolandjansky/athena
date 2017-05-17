/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGUPGRADETEST_TESTMERGER_H
#define TRIGUPGRADETEST_TESTMERGER_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"

namespace HLTTest {

  class TestMerger
    : public ::AthAlgorithm
  { 

  public: 

    // Copy constructor: 

    /// Constructor with parameters: 
    TestMerger( const std::string& name, ISvcLocator* pSvcLocator );

    /// Destructor: 
    virtual ~TestMerger(); 

    // Assignment operator: 
    //TestMerger &operator=(const TestMerger &alg); 

    // Athena algorithm's Hooks
    StatusCode  initialize() override;
    StatusCode  execute() override;
    StatusCode  finalize() override;

  private: 

    /// Default constructor: 
    TestMerger();
    std::vector<std::string> m_inputs;

  }; 

} //> end namespace HLTTest
#endif //> !TRIGUPGRADETEST_TESTMERGER_H

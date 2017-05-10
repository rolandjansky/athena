/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CPANALYSISEXAMPLES_TEST__TRACKLINKTEST_H
#define CPANALYSISEXAMPLES_TEST__TRACKLINKTEST_H 1

#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"
#ifdef XAOD_ANALYSIS
#include "AsgTools/AnaToolHandle.h" //use asg::AnaToolHandle instead of regular ToolHandles for full dual-use experience!
#endif

namespace Test {

class TrackLinkTest: public ::AthAnalysisAlgorithm { 
 public: 
  TrackLinkTest( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TrackLinkTest(); 

  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();
  
  virtual StatusCode beginInputFile();

 private: 

}; 
} //> end namespace Test
#endif //> !CPANALYSISEXAMPLES_TEST__TRACKLINKTEST_H

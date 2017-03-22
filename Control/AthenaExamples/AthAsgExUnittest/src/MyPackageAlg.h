// -*- mode: c++ -*-
#ifndef ATHASGEXUNITTEST_MYPACKAGEALG_H
#define ATHASGEXUNITTEST_MYPACKAGEALG_H 1

#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"

#ifdef XAOD_ANALYSIS
#include "AsgTools/AnaToolHandle.h" //use asg::AnaToolHandle instead of regular ToolHandles for full dual-use experience!
#endif

//#include <cmath>

#include "AthAsgExUnittest/IMyPackageTool.h"


class MyPackageAlg: public ::AthAnalysisAlgorithm { 
public: 

  MyPackageAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~MyPackageAlg(); 

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();  
  virtual StatusCode beginInputFile();

private: 

  int m_property;
  ToolHandle<IMyPackageTool> m_tool;

}; 

#endif //> !ATHASGEXUNITTEST_MYPACKAGEALG_H

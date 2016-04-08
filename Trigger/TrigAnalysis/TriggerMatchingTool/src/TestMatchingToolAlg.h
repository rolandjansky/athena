/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMATCHINGTOOL_TESTMATCHINGTOOLALG_H
#define TRIGGERMATCHINGTOOL_TESTMATCHINGTOOLALG_H 1

#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"

#include "TriggerMatchingTool/IMatchingTool.h"

class TestMatchingToolAlg: public ::AthAnalysisAlgorithm { 
 public: 
  TestMatchingToolAlg( const std::string& name, ISvcLocator* pSvcLocator ) : AthAnalysisAlgorithm( name, pSvcLocator ) {}
  virtual ~TestMatchingToolAlg() {}

  virtual StatusCode  initialize();
  virtual StatusCode  execute();

 private: 

  ToolHandle<Trig::IMatchingTool> m_tmt;

}; 

#endif //> !TRIGGERMATCHINGTOOL_TESTMATCHINGTOOLALG_H

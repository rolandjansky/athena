// -*- mode: c++ -*-
//
//  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
//
#ifndef ATHEXUNITTEST_ATHEXUNITTESTALG_H
#define ATHEXUNITTEST_ATHEXUNITTESTALG_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthExUnittest/IAthExUnittestTool.h"

class AthExUnittestAlg: public AthAlgorithm { 
public: 

  AthExUnittestAlg( const std::string& name,
		    ISvcLocator* pSvcLocator );
  virtual ~AthExUnittestAlg(); 

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();  
  virtual StatusCode beginInputFile();

private: 

  int m_property;
  ToolHandle<IAthExUnittestTool> m_tool;

}; 

#endif //> !ATHEXUNITTEST_ATHEXUNITTESTALG_H

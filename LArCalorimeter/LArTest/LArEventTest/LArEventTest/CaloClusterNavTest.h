/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCLUSTERNAVTEST_H
#define CALOCLUSTERNAVTEST_H

#include "AthenaBaseComps/AthAlgorithm.h"

class CaloClusterNavTest: public AthAlgorithm
{

 public:    
  
  CaloClusterNavTest(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~CaloClusterNavTest();
  
  virtual StatusCode execute() ; 
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  
 private: 
 std::vector<std::string> m_clusterContainerNames; 
};

#endif 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCLUSTERNAVTEST_H
#define CALOCLUSTERNAVTEST_H

#include "GaudiKernel/Algorithm.h"

class StoreGateSvc; 
class CaloClusterNavTest: public Algorithm
{

 public:    
  
  CaloClusterNavTest(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~CaloClusterNavTest();
  
  virtual StatusCode execute() ; 
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  
 private: 

 StoreGateSvc* m_storeGate ; 
 std::vector<std::string> m_clusterContainerNames; 
};

#endif 

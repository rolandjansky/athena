/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaBaseComps/AthAlgorithm.h"
#include <stdint.h>
#include <vector>
#include <string>

/////////////////////////////////////////////////////////////////////////////

class IROBDataProviderSvc;

class MTHelloWorldLvl1:public AthAlgorithm {
public:
  MTHelloWorldLvl1(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  IROBDataProviderSvc*  m_pIROBDataProviderSvc; 
  BooleanProperty       m_retrieveLvl1;
  BooleanProperty       m_retrieveROBs;
  IntegerArrayProperty  m_listRobIds;
  std::vector<uint32_t> m_robIds;
};

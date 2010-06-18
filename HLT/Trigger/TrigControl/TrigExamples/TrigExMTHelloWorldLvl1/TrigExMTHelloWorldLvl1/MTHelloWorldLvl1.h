/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Algorithm.h"
#include <stdint.h>

/////////////////////////////////////////////////////////////////////////////

class StoreGateSvc;
class IROBDataProviderSvc;

class MTHelloWorldLvl1:public Algorithm {
public:
  MTHelloWorldLvl1(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  StoreGateSvc*         m_StoreGateSvc;
  IROBDataProviderSvc*  m_pIROBDataProviderSvc; 
  BooleanProperty       m_retrieveLvl1;
  BooleanProperty       m_retrieveROBs;
  IntegerArrayProperty  m_listRobIds;
  std::vector<uint32_t> m_robIds;
};

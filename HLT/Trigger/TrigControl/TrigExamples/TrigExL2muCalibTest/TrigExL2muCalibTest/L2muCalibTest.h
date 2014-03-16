/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Algorithm.h"
#include <stdint.h>

/////////////////////////////////////////////////////////////////////////////

class StoreGateSvc;
class IJobOptionsSvc;

class L2muCalibTest:public Algorithm {
public:
  L2muCalibTest(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  IJobOptionsSvc*       m_jobOptionsSvc;
  StoreGateSvc*         m_StoreGateSvc;
  BooleanProperty       m_allowOksConfig;
  BooleanProperty       m_dumpCatalogue;
  StringProperty        m_muonCalBufferName;
  IntegerProperty       m_muonCalBufferSize;
};

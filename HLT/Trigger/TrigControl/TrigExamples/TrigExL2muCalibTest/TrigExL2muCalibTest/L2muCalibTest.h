/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IIncidentListener.h"
#include <stdint.h>

/////////////////////////////////////////////////////////////////////////////

class StoreGateSvc;
class IJobOptionsSvc;
class Incident;

class L2muCalibTest:public Algorithm,
                    virtual public IIncidentListener {
public:
  L2muCalibTest(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  // handler for "UpdateAfterFork" actions
  void handle(const Incident& incident);
  
private:
  IJobOptionsSvc*       m_jobOptionsSvc;
  StoreGateSvc*         m_StoreGateSvc;
  BooleanProperty       m_allowOksConfig;
  BooleanProperty       m_dumpCatalogue;
  StringProperty        m_muonCalBufferName;
  IntegerProperty       m_muonCalBufferSize;
};

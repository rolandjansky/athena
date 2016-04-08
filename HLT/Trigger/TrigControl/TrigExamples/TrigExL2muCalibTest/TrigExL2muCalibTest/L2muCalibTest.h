/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L2MUCALIBTEST
#define L2MUCALIBTEST

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/IIncidentListener.h"
#include "TrigNavigation/Navigation.h"
#include <stdint.h>

/////////////////////////////////////////////////////////////////////////////

class IJobOptionsSvc;
class Incident;

class L2muCalibTest:public AthAlgorithm,
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
  BooleanProperty       m_allowOksConfig;
  BooleanProperty       m_dumpCatalogue;
  StringProperty        m_muonCalBufferName;
  IntegerProperty       m_muonCalBufferSize;
  BooleanProperty       m_readBackDataScoutingCollection;
  StringProperty        m_keyDataScoutingROB;
  StringProperty        m_keyDataScoutingContainer;

  // Navigation
  ToolHandle<HLT::Navigation> m_navigationForEF; 
  HLT::Navigation* m_nav;
};

#endif

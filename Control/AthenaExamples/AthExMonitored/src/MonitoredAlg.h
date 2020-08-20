/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXMONITORED_MONITOREDALG_H
#define ATHEXMONITORED_MONITOREDALG_H 1

#include "GaudiKernel/ToolHandle.h"
#include "Gaudi/Property.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

#include <string>

class MonitoredAlg : public AthAlgorithm {
public:
  MonitoredAlg( const std::string& name, ISvcLocator* pSvcLocator );
  StatusCode initialize();
  StatusCode execute();

private:
  ToolHandle<GenericMonitoringTool> m_monTool{this,"MonTool","","Monitoring tool"};
};


#endif

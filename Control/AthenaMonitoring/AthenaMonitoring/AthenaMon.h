/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAMON_H
#define ATHENAMON_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "AthenaMonitoring/IMonitorToolBase.h"

class AthenaMon:public AthAlgorithm {
 public:

  AthenaMon (const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~AthenaMon();
  
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();
  
  virtual StatusCode start();
  virtual StatusCode stop();
  
 protected:
  
	std::vector<IMonitorToolBase*> m_monTools;
  
 private:
  
  std::vector<std::string>  m_monToolNames;
  
  std::vector<std::string>  m_THistSvc_streamnameMapping;
  
  long m_everyNevents;
  long m_eventCounter = 0;
  
  bool m_bookHistsinInitialize;
  
};

#endif

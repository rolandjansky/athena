/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECALGS_APPSTOPALG_H
#define RECALGS_APPSTOPALG_H

// Gaudi includes
#include "AthenaBaseComps/AthAlgorithm.h"

/** @class AppStopAlg
 * installs a signal handler and calls ApplicationMgr::stopRun() when caught
*/

class AppStopAlg : public AthAlgorithm
{
 public:
  
  /** Standard Athena-Algorithm Constructor */
  AppStopAlg(const std::string& name, ISvcLocator* pSvcLocator);
  
  /** Default Destructor */
  ~AppStopAlg() {};
  
  /** standard Athena-Algorithm method */
  StatusCode          initialize();
  /** standard Athena-Algorithm method */
  StatusCode          execute();
  /** standard Athena-Algorithm method */
  StatusCode          finalize()
  { return StatusCode::SUCCESS; };
  
 private:
  StatusCode installHandler();
  bool m_handlerInstalled=false;
  /** member variables for algorithm properties: */
  int m_signal;
  bool m_installAfterFirst;

};

#endif 

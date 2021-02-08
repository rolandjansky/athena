/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "AppStopAlg.h"
#include "GaudiKernel/IEventProcessor.h"
#include "csignal"


namespace
{
  volatile std::sig_atomic_t gSignalStatus=0;
}
 

void signal_handler(int /*signal*/,siginfo_t *, void *) {
  gSignalStatus = 1;
}
 

AppStopAlg::AppStopAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator) {  
  declareProperty("SignalNumber",m_signal=15);
  declareProperty("InstallAfterFirstEvent",m_installAfterFirst=true);
}


StatusCode AppStopAlg::installHandler() {
  ATH_MSG_INFO("Handling signal: " << m_signal);

  struct sigaction act;
  struct sigaction oldact;

  act.sa_handler=nullptr;
  sigemptyset(&act.sa_mask);
  act.sa_flags=SA_RESTART | SA_SIGINFO;
  act.sa_sigaction=::signal_handler;
  if (sigaction(m_signal,&act,&oldact)==-1) {
    ATH_MSG_ERROR("Failed to install signal handler for signal "<< m_signal);
    return StatusCode::FAILURE;
  }
  
  //const void* oldhandle= (oldact.sa_flags & SA_SIGINFO) ? (void*)oldact.sa_sigaction : (void*)oldact.sa_handler;

  if (oldact.sa_handler==SIG_DFL || oldact.sa_handler==SIG_IGN) {
    ATH_MSG_DEBUG("No previosly defined signal handler for signal " << m_signal);
  }
  else  {
    ATH_MSG_INFO("Replacing previously defined signal handler for signal "  << m_signal);
  }
  m_handlerInstalled=true;
  return StatusCode::SUCCESS;
}


StatusCode AppStopAlg::initialize() {
  if (!m_installAfterFirst) {
    return installHandler();
  }
  else {
    return StatusCode::SUCCESS; 
  }
}

StatusCode AppStopAlg::execute() {
  if (!m_handlerInstalled) {
    return installHandler();
  }


  if (gSignalStatus!=0) {
    
    ATH_MSG_INFO("Got signal " << m_signal << ". Stopping the application");
    IEventProcessor* appMgr=nullptr;
    ATH_CHECK(service("ApplicationMgr",appMgr));
    
    if (!appMgr) {
      ATH_MSG_ERROR("Failed to retrieve ApplicationMgr as IEventProcessor");
      return StatusCode::FAILURE;
    }
    ATH_CHECK(appMgr->stopRun());
  }
  
  return StatusCode::SUCCESS;
    
}

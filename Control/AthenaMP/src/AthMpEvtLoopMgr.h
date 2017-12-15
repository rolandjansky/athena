/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAMP_ATHMPEVTLOOPMGR_H
#define ATHENAMP_ATHMPEVTLOOPMGR_H

#include "GaudiKernel/IEventProcessor.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaInterprocess/FdsRegistry.h"
#include "boost/shared_ptr.hpp"

class IAthenaMPTool;
class ISvcLocator;
template <class TYPE> class SvcFactory;

class AthMpEvtLoopMgr
   :virtual public ::IEventProcessor 
   , public AthService
{
  friend class SvcFactory<AthMpEvtLoopMgr>;

  AthMpEvtLoopMgr(const std::string& name, ISvcLocator* svcLocator);
  virtual ~AthMpEvtLoopMgr();

 public:
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual StatusCode queryInterface(const InterfaceID& riid, 
				    void** ppvInterface);

  virtual StatusCode nextEvent(int maxevt);
  virtual StatusCode executeEvent(void* par);
  virtual StatusCode executeRun(int maxevt);
  virtual StatusCode stopRun();

 private:
  ServiceHandle<IEventProcessor> m_evtProcessor;
  IService*                      m_evtSelector;
  int                            m_nWorkers;
  std::string                    m_workerTopDir;
  std::string                    m_outputReportName;
  std::string                    m_strategy;
  bool                           m_isPileup;
  bool                           m_collectSubprocessLogs;
  ToolHandleArray<IAthenaMPTool> m_tools;
  int                            m_nChildProcesses;
  int                            m_nPollingInterval;      // in milliseconds
  int                            m_nMemSamplingInterval;  // in seconds
  int                            m_nEventsBeforeFork;
  pid_t                          m_masterPid;

  // vectors for collecting memory samples
  std::vector<unsigned long>     m_samplesRss;
  std::vector<unsigned long>     m_samplesPss;
  std::vector<unsigned long>     m_samplesSize;
  std::vector<unsigned long>     m_samplesSwap;
  
  AthMpEvtLoopMgr();
  AthMpEvtLoopMgr(const AthMpEvtLoopMgr&);
  AthMpEvtLoopMgr& operator = (const AthMpEvtLoopMgr&);

  StatusCode wait();
  StatusCode generateOutputReport(); 
  boost::shared_ptr<AthenaInterprocess::FdsRegistry> extractFds();
  StatusCode afterRestart(int& maxevt);
  StatusCode updateSkipEvents(int skipEvents);
}; 

#endif

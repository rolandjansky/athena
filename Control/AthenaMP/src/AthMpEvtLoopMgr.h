/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAMP_ATHMPEVTLOOPMGR_H
#define ATHENAMP_ATHMPEVTLOOPMGR_H

#include "GaudiKernel/IEventProcessor.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaInterprocess/FdsRegistry.h"
#include <memory>

class IAthenaMPTool;
class ISvcLocator;

class AthMpEvtLoopMgr
   :virtual public ::IEventProcessor 
   , public AthService
{
 public:
  AthMpEvtLoopMgr(const std::string& name, ISvcLocator* svcLocator);
  virtual ~AthMpEvtLoopMgr();

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual StatusCode queryInterface(const InterfaceID& riid, 
				    void** ppvInterface);

  virtual StatusCode nextEvent(int maxevt);
  virtual StatusCode executeEvent(EventContext &&ctx);
  virtual StatusCode executeRun(int maxevt);
  virtual StatusCode stopRun();

  EventContext createEventContext();

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
  unsigned int                   m_eventPrintoutInterval;
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
  std::shared_ptr<AthenaInterprocess::FdsRegistry> extractFds();
  StatusCode updateSkipEvents(int skipEvents);
}; 

#endif

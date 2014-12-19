/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAMPTOOLS_TOKENPROCESSOR_H
#define ATHENAMPTOOLS_TOKENPROCESSOR_H 1

#include "AthenaMPToolBase.h"

#include "AthenaInterprocess/SharedQueue.h"
#include "yampl/Exceptions.h"

#include "boost/shared_ptr.hpp"
#include <queue>

class IEventShare;
class IChronoStatSvc;
namespace yampl {
  class ISocketFactory;
  class ISocket;
}

class TokenProcessor : public AthenaMPToolBase
{
 public:
  TokenProcessor(const std::string& type
		 , const std::string& name
		 , const IInterface* parent);

  virtual ~TokenProcessor();
  
  StatusCode initialize();
  StatusCode finalize();

  // _________IAthenaMPTool_________   
  int makePool(int maxevt, int nprocs, const std::string& topdir);
  StatusCode exec();
  StatusCode wait_once(int& numFinishedProc);

  void reportSubprocessStatuses();
  void subProcessLogs(std::vector<std::string>&);
  virtual AthenaMP::AllWorkerOutputs_ptr generateOutputReport();

  // _____ Actual working horses ________
  AthenaInterprocess::ScheduledWork* bootstrap_func();
  AthenaInterprocess::ScheduledWork* exec_func();
  AthenaInterprocess::ScheduledWork* fin_func();

 private:
  TokenProcessor();
  TokenProcessor(const TokenProcessor&);
  TokenProcessor& operator= (const TokenProcessor&);

  // Decode process results
  // 1. Store number of processed events for FUNC_EXEC
  // 2. If doFinalize flag is set then serialize process finalizations
  int decodeProcessResult(const AthenaInterprocess::ProcessResult* presult, bool doFinalize);
  StatusCode startProcess();

  bool m_isPileup;        // Are we doing pile-up digitization?
  int  m_rankId;          // Each worker has its own unique RankID from the range (0,...,m_nprocs-1) 
  int  m_nEventsBeforeFork;

  ServiceHandle<IChronoStatSvc>     m_chronoStatSvc;
  IEventShare*                      m_evtShare;

  StringProperty                    m_channel2Scatterer;
  StringProperty                    m_channel2EvtSel;

  AthenaInterprocess::SharedQueue*  m_sharedRankQueue;          

  std::map<pid_t,int>               m_nProcessedEvents; // Number of processed events by PID
  std::queue<pid_t>                 m_finQueue;         // PIDs of processes queued for finalization

  yampl::ISocketFactory*            m_socketFactory;
  yampl::ISocket*                   m_socket2Scatterer;
  std::string                       m_outputFileReport;
};

#endif

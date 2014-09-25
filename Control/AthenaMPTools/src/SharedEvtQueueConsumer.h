/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAMPTOOLS_SHAREDEVTQUEUECONSUMER_H
#define ATHENAMPTOOLS_SHAREDEVTQUEUECONSUMER_H 1

#include "AthenaMPToolBase.h"

#include "AthenaInterprocess/SharedQueue.h"
#include <queue>

class IEventSeek;
class IEventShare;
class IChronoStatSvc;

class SharedEvtQueueConsumer : public AthenaMPToolBase
{
 public:
  SharedEvtQueueConsumer(const std::string& type
			 , const std::string& name
			 , const IInterface* parent);

  virtual ~SharedEvtQueueConsumer();
  
  StatusCode initialize();
  StatusCode finalize();

  // _________IAthenaMPTool_________   
  int makePool(int maxevt, int nprocs, const std::string& topdir);
  StatusCode exec();
  StatusCode wait_once(int& numFinishedProc);

  void reportSubprocessStatuses();
  void subProcessLogs(std::vector<std::string>&);

  // _____ Actual working horses ________
  AthenaInterprocess::ScheduledWork* bootstrap_func();
  AthenaInterprocess::ScheduledWork* exec_func();
  AthenaInterprocess::ScheduledWork* fin_func();

 private:
  SharedEvtQueueConsumer();
  SharedEvtQueueConsumer(const SharedEvtQueueConsumer&);
  SharedEvtQueueConsumer& operator= (const SharedEvtQueueConsumer&);

  // Decode process results
  // 1. Store number of processed events for FUNC_EXEC
  // 2. If doFinalize flag is set then serialize process finalizations
  int decodeProcessResult(const AthenaInterprocess::ProcessResult* presult, bool doFinalize);

  bool m_useSharedReader; // Work in pair with a SharedReader
  bool m_isPileup;        // Are we doing pile-up digitization?
  int  m_rankId;          // Each worker has its own unique RankID from the range (0,...,m_nprocs-1) 
  int  m_nEventsBeforeFork;

  ServiceHandle<IChronoStatSvc>  m_chronoStatSvc;
  IEventSeek*                    m_evtSeek;
  IEventShare*                   m_evtShare;

  AthenaInterprocess::SharedQueue*  m_sharedEventQueue;          
  AthenaInterprocess::SharedQueue*  m_sharedRankQueue;          

  std::map<pid_t,int>               m_nProcessedEvents; // Number of processed events by PID
  std::queue<pid_t>                 m_finQueue;         // PIDs of processes queued for finalization
};

#endif

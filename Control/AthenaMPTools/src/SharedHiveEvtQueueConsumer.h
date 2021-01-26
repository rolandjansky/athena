/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAMPTOOLS_SHAREDHIVEEVTQUEUECONSUMER_H
#define ATHENAMPTOOLS_SHAREDHIVEEVTQUEUECONSUMER_H

#include "AthenaMPToolBase.h"

#include "AthenaInterprocess/SharedQueue.h"
#include <queue>
#include "GaudiKernel/IScheduler.h"
#include "GaudiKernel/IEvtSelector.h"

class IEventSeek;
class IEvtSelectorSeek;
class IEventShare;
class IDataShare;
class IChronoStatSvc;

class SharedHiveEvtQueueConsumer final : public AthenaMPToolBase
{
 public:
  SharedHiveEvtQueueConsumer(const std::string& type
			 , const std::string& name
			 , const IInterface* parent);

  virtual ~SharedHiveEvtQueueConsumer() override;
  
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  // _________IAthenaMPTool_________   
  virtual int makePool(int maxevt, int nprocs, const std::string& topdir) override;
  virtual StatusCode exec() override;
  virtual StatusCode wait_once(pid_t& pid) override;

  virtual void reportSubprocessStatuses() override;
  virtual void subProcessLogs(std::vector<std::string>&) override;

  // _____ Actual working horses ________
  virtual std::unique_ptr<AthenaInterprocess::ScheduledWork> bootstrap_func() override;
  virtual std::unique_ptr<AthenaInterprocess::ScheduledWork> exec_func() override;
  virtual std::unique_ptr<AthenaInterprocess::ScheduledWork> fin_func() override;

 private:
  SharedHiveEvtQueueConsumer();
  SharedHiveEvtQueueConsumer(const SharedHiveEvtQueueConsumer&);
  SharedHiveEvtQueueConsumer& operator= (const SharedHiveEvtQueueConsumer&);

  StatusCode initHive();

  // Decode process results
  // 1. Store number of processed events for FUNC_EXEC
  // 2. If doFinalize flag is set then serialize process finalizations
  int decodeProcessResult(const AthenaInterprocess::ProcessResult* presult, bool doFinalize);

  // Properties
  bool m_useSharedReader; // Work in pair with a SharedReader
  bool m_useSharedWriter; // Work in pair with a SharedWriter
  bool m_isPileup;        // Are we doing pile-up digitization?
  bool m_isRoundRobin;    // Are we running in the "reproducible mode"?
  int  m_nEventsBeforeFork;
  bool m_debug;

  int  m_rankId;          // Each worker has its own unique RankID from the range (0,...,m_nprocs-1) 

  ServiceHandle<IChronoStatSvc>  m_chronoStatSvc;
  IEventSeek*                    m_evtSeek;
  IEvtSelectorSeek*              m_evtSelSeek;
  IEvtSelector::Context*         m_evtContext;
  IEventShare*                   m_evtShare;
  IDataShare*                    m_dataShare;

  AthenaInterprocess::SharedQueue*  m_sharedEventQueue;          
  AthenaInterprocess::SharedQueue*  m_sharedRankQueue;          

  std::map<pid_t,int>               m_nProcessedEvents; // Number of processed events by PID
  std::queue<pid_t>                 m_finQueue;         // PIDs of processes queued for finalization

  SmartIF<IScheduler> m_schedulerSvc;

};

#endif

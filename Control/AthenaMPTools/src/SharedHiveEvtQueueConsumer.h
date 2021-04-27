/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAMPTOOLS_SHAREDHIVEEVTQUEUECONSUMER_H
#define ATHENAMPTOOLS_SHAREDHIVEEVTQUEUECONSUMER_H

#include "AthenaMPToolBase.h"

#include "AthenaInterprocess/SharedQueue.h"
#include <queue>
#include "GaudiKernel/IScheduler.h"
#include "GaudiKernel/IEvtSelector.h"

class IDataShare;
class IEvtSelectorSeek;
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
  Gaudi::Property<int>  m_nEventsBeforeFork{
      this, "EventsBeforeFork", 0,
      "The number of events before forking the workers. The default is 0."};

  Gaudi::Property<bool> m_debug{
      this, "Debug", false,
      "Perform extra debugging if true. The default is false."};

  Gaudi::Property<bool> m_useSharedWriter{
      this, "UseSharedWriter", false,
      "Use SharedWriter to merge worker outputs on-the-fly if true. The default is false."};


  int  m_rankId;          // Each worker has its own unique RankID from the range (0,...,m_nprocs-1) 

  ServiceHandle<IChronoStatSvc>  m_chronoStatSvc;
  IDataShare*                    m_dataShare;
  IEvtSelectorSeek*              m_evtSelSeek;
  IEvtSelector::Context*         m_evtContext;

  AthenaInterprocess::SharedQueue*  m_sharedEventQueue;          
  AthenaInterprocess::SharedQueue*  m_sharedRankQueue;          

  std::map<pid_t,int>               m_nProcessedEvents; // Number of processed events by PID
  std::queue<pid_t>                 m_finQueue;         // PIDs of processes queued for finalization

  SmartIF<IScheduler> m_schedulerSvc;

};

#endif

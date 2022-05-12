/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAMPTOOLS_EVTRANGEPROCESSOR_H
#define ATHENAMPTOOLS_EVTRANGEPROCESSOR_H

#include "AthenaMPToolBase.h"

#include "AthenaInterprocess/SharedQueue.h"
#include "yampl/Exceptions.h"

#include <deque>
#include <map>

class IEvtSelectorSeek;
class IChronoStatSvc;
class IIncidentSvc;
namespace yampl {
  class ISocketFactory;
  class ISocket;
}

class EvtRangeProcessor final : public AthenaMPToolBase
{
 public:
  EvtRangeProcessor(const std::string& type
		    , const std::string& name
		    , const IInterface* parent);

  virtual ~EvtRangeProcessor() override;
  
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  // _________IAthenaMPTool_________   
  virtual int makePool(int maxevt, int nprocs, const std::string& topdir) override;
  virtual StatusCode exec() override;
  virtual StatusCode wait_once(pid_t& pid) override;

  virtual void reportSubprocessStatuses() override;
  virtual void subProcessLogs(std::vector<std::string>&) override;
  virtual AthenaMP::AllWorkerOutputs_ptr generateOutputReport() override;

  // _____ Actual working horses ________
  virtual std::unique_ptr<AthenaInterprocess::ScheduledWork> bootstrap_func() override;
  virtual std::unique_ptr<AthenaInterprocess::ScheduledWork> exec_func() override;
  virtual std::unique_ptr<AthenaInterprocess::ScheduledWork> fin_func() override;

 private:
  EvtRangeProcessor();
  EvtRangeProcessor(const EvtRangeProcessor&);
  EvtRangeProcessor& operator= (const EvtRangeProcessor&);

  StatusCode startProcess();
  StatusCode setNewInputFile(const std::string& newFile);
  void reportError(yampl::ISocket* socket,AthenaMPToolBase::ESRange_Status status);

  enum ProcessState {
    PROC_STATE_INIT
    , PROC_STATE_EXEC
    , PROC_STATE_FIN
    , PROC_STATE_STOP
  };

  int  m_rankId;          // Each worker has its own unique RankID from the range (0,...,m_nprocs-1) 
  int  m_nEventsBeforeFork;
  int  m_activeWorkers;   // Keep track of the number of workers
  std::string m_inpFile;  // Cached name of the input file. To avoid reopening

  ServiceHandle<IChronoStatSvc>     m_chronoStatSvc;
  ServiceHandle<IIncidentSvc>       m_incidentSvc;
  IEvtSelectorSeek*                 m_evtSeek;

  StringProperty                    m_channel2Scatterer;
  StringProperty                    m_channel2EvtSel;

  AthenaInterprocess::SharedQueue*  m_sharedRankQueue;          
  AthenaInterprocess::SharedQueue*  m_sharedFailedPidQueue;          

  std::map<pid_t,int>               m_nProcessedEvents; // Number of processed events by PID
  std::deque<pid_t>                 m_finQueue;         // PIDs of processes queued for finalization
  std::map<pid_t,ProcessState>      m_procStates;       // Map for keeping track of states of the subprocesses

  bool m_debug;
};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAMPTOOLS_TOKENPROCESSOR_H
#define ATHENAMPTOOLS_TOKENPROCESSOR_H 1

#include "AthenaMPToolBase.h"

#include "AthenaInterprocess/SharedQueue.h"
#include "yampl/Exceptions.h"

#include "boost/shared_ptr.hpp"
#include <deque>
#include <set>

class IEventSeek;
class IChronoStatSvc;
class IIncidentSvc;
namespace yampl {
  class ISocketFactory;
  class ISocket;
}

class EvtRangeProcessor : public AthenaMPToolBase
{
 public:
  EvtRangeProcessor(const std::string& type
		    , const std::string& name
		    , const IInterface* parent);

  virtual ~EvtRangeProcessor();
  
  StatusCode initialize();
  StatusCode finalize();

  // _________IAthenaMPTool_________   
  int makePool(int maxevt, int nprocs, const std::string& topdir);
  StatusCode exec();
  StatusCode wait_once(pid_t& pid);

  void reportSubprocessStatuses();
  void subProcessLogs(std::vector<std::string>&);
  virtual AthenaMP::AllWorkerOutputs_ptr generateOutputReport();

  // _____ Actual working horses ________
  std::unique_ptr<AthenaInterprocess::ScheduledWork> bootstrap_func();
  std::unique_ptr<AthenaInterprocess::ScheduledWork> exec_func();
  std::unique_ptr<AthenaInterprocess::ScheduledWork> fin_func();

 private:
  EvtRangeProcessor();
  EvtRangeProcessor(const EvtRangeProcessor&);
  EvtRangeProcessor& operator= (const EvtRangeProcessor&);

  // Decode process results
  // 1. Store number of processed events for FUNC_EXEC
  int decodeProcessResult(const AthenaInterprocess::ProcessResult* presult);
  StatusCode startProcess();
  StatusCode setNewInputFile(const std::string& newFile);
  void reportError(yampl::ISocket* socket,AthenaMPToolBase::ESRange_Status status);

  bool m_isPileup;        // Are we doing pile-up digitization?
  int  m_rankId;          // Each worker has its own unique RankID from the range (0,...,m_nprocs-1) 
  int  m_nEventsBeforeFork;

  ServiceHandle<IChronoStatSvc>     m_chronoStatSvc;
  ServiceHandle<IIncidentSvc>       m_incidentSvc;
  IEventSeek*                       m_evtSeek;

  StringProperty                    m_channel2Scatterer;
  StringProperty                    m_channel2EvtSel;

  AthenaInterprocess::SharedQueue*  m_sharedRankQueue;          
  AthenaInterprocess::SharedQueue*  m_sharedFailedPidQueue;          

  std::map<pid_t,int>               m_nProcessedEvents; // Number of processed events by PID
  std::deque<pid_t>                 m_finQueue;         // PIDs of processes queued for finalization
  std::set<pid_t>                   m_execSet;          // PIDs of processes currently in EXEC

  bool m_debug;
};

#endif

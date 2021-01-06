/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAMPTOOLS_TOKENSCATTERER_H
#define ATHENAMPTOOLS_TOKENSCATTERER_H

#include "AthenaMPToolBase.h"
#include "yampl/Exceptions.h"
#include <map>

typedef std::map<pid_t,std::string> Pid2RangeID;

namespace yampl {
  class ISocket;
}

class EvtRangeScatterer final : public AthenaMPToolBase
{
 public:
  EvtRangeScatterer(const std::string& type
		    , const std::string& name
		    , const IInterface* parent);

  virtual ~EvtRangeScatterer() override;
  
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  // _________IAthenaMPTool_________   
  virtual int makePool(int maxevt, int nprocs, const std::string& topdir) override;
  virtual StatusCode exec() override;

  virtual void subProcessLogs(std::vector<std::string>&) override;
  virtual AthenaMP::AllWorkerOutputs_ptr generateOutputReport() override;

  // _____ Actual working horses ________
  virtual std::unique_ptr<AthenaInterprocess::ScheduledWork> bootstrap_func() override;
  virtual std::unique_ptr<AthenaInterprocess::ScheduledWork> exec_func() override;
  virtual std::unique_ptr<AthenaInterprocess::ScheduledWork> fin_func() override;

 private:
  EvtRangeScatterer();
  EvtRangeScatterer(const EvtRangeScatterer&);
  EvtRangeScatterer& operator= (const EvtRangeScatterer&);

  // Get rid of
  // 1. Leading and trailing spaces
  // 2. Leading "u\'" and trailing "\'"
  void trimRangeStrings(std::string&);

  // Helper functuion for receiving new messages from the socket2Processor channel
  // If this is an output file report, then it is forwarded to the pilot and an empty string is returned to the caller
  std::string getNewRangeRequest(yampl::ISocket* socket2Processor
				 , yampl::ISocket* socket2Pilot
				 , int& procReportPending);

  // Poll the failed PID queue to see if any of the Processors has failed
  pid_t pollFailedPidQueue(AthenaInterprocess::SharedQueue*  sharedFailedPidQueue
			   , yampl::ISocket* socket2Pilot
			   , int& procReportPending);

  StringProperty           m_processorChannel;
  StringProperty           m_eventRangeChannel;
  bool                     m_doCaching;
  Pid2RangeID              m_pid2RangeID;        // Current RangeID-s by PIDs
};

#endif

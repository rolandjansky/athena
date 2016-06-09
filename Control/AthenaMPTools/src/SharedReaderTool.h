/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAMPTOOLS_SHAREDREADERTOOL_H
#define ATHENAMPTOOLS_SHAREDREADERTOOL_H 1

#include "AthenaMPToolBase.h"

#include "AthenaInterprocess/SharedQueue.h"
#include "yampl/Exceptions.h"
#include "boost/shared_ptr.hpp"

#if defined(__GNUC__) && __GNUC__>=6 && !defined(__clang__)
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wterminate"
# include "yampl/utils/SharedMemory.h"
# pragma GCC diagnostic pop
#else
# include "yampl/utils/SharedMemory.h"
#endif


class IEventShare;

class SharedReaderTool : public AthenaMPToolBase
{
 public:
  SharedReaderTool(const std::string& type
		   , const std::string& name
		   , const IInterface* parent);

  virtual ~SharedReaderTool();
  
  StatusCode initialize();
  StatusCode finalize();

  // _________IAthenaMPTool_________   
  virtual int makePool(int maxevt, int nprocs, const std::string& topdir);
  virtual StatusCode exec();

  virtual void subProcessLogs(std::vector<std::string>&);
  virtual AthenaMP::AllWorkerOutputs_ptr generateOutputReport();

  // _____ Actual working horses ________
  std::unique_ptr<AthenaInterprocess::ScheduledWork> bootstrap_func();
  std::unique_ptr<AthenaInterprocess::ScheduledWork> exec_func();
  std::unique_ptr<AthenaInterprocess::ScheduledWork> fin_func();

 private:
  SharedReaderTool();
  SharedReaderTool(const SharedReaderTool&);
  SharedReaderTool& operator= (const SharedReaderTool&);

  int  m_nEvtRequested;    // Max event received from AppMgr

  boost::interprocess::mapped_region* m_shmemSegment; // Shared memory segment for getting maximum number of events to read

  IEventShare*                   m_evtShare;
};

#endif

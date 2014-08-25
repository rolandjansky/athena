/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAMPTOOLS_SHAREDREADERTOOL_H
#define ATHENAMPTOOLS_SHAREDREADERTOOL_H 1

#include "AthenaMPToolBase.h"

#include "AthenaInterprocess/SharedQueue.h"
#include "yampl/Exceptions.h"
#include "yampl/utils/SharedMemory.h"

#include "boost/shared_ptr.hpp"

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
  AthenaInterprocess::ScheduledWork* bootstrap_func();
  AthenaInterprocess::ScheduledWork* exec_func();
  AthenaInterprocess::ScheduledWork* fin_func();

 private:
  SharedReaderTool();
  SharedReaderTool(const SharedReaderTool&);
  SharedReaderTool& operator= (const SharedReaderTool&);

  boost::shared_ptr<yampl::SharedMemory> m_shmemSegment; // Shared memory segment for getting maximum number of events to read

  IEventShare*                   m_evtShare;
};

#endif

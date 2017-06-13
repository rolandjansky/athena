/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAMPTOOLS_SHAREDWRITERTOOL_H
#define ATHENAMPTOOLS_SHAREDWRITERTOOL_H 1

#include "AthenaMPToolBase.h"

/*
#include "AthenaInterprocess/SharedQueue.h"
#include "yampl/Exceptions.h"
#include "yampl/utils/SharedMemory.h"

#include "boost/shared_ptr.hpp"
*/

class IEventShare;
class IConversionSvc;

class SharedWriterTool : public AthenaMPToolBase
{
 public:
  SharedWriterTool(const std::string& type
		   , const std::string& name
		   , const IInterface* parent);

  virtual ~SharedWriterTool();
  
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
  SharedWriterTool();
  SharedWriterTool(const SharedWriterTool&);
  SharedWriterTool& operator= (const SharedWriterTool&);

  IEventShare*                   m_evtShare;
  IConversionSvc*             m_cnvSvc;

};

#endif

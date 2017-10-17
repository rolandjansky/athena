/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAMPTOOLS_SHAREDWRITERTOOL_H
#define ATHENAMPTOOLS_SHAREDWRITERTOOL_H 1

#include "AthenaMPToolBase.h"

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

  int  m_rankId;          // Each worker has its own unique RankID from the range (0,...,m_nprocs-1)
  int  m_writer;          // Number of writer stream servers

  AthenaInterprocess::SharedQueue*  m_sharedRankQueue;
  IConversionSvc*             m_cnvSvc;

};

#endif

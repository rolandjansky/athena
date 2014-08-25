/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAMPTOOLS_TOKENSCATTERER_H
#define ATHENAMPTOOLS_TOKENSCATTERER_H 1

#include "AthenaMPToolBase.h"
#include "yampl/Exceptions.h"

class TokenScatterer : public AthenaMPToolBase
{
 public:
  TokenScatterer(const std::string& type
		 , const std::string& name
		 , const IInterface* parent);

  virtual ~TokenScatterer();
  
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
  TokenScatterer();
  TokenScatterer(const TokenScatterer&);
  TokenScatterer& operator= (const TokenScatterer&);

  // Get rid of
  // 1. Leading and trailing spaces
  // 2. Leading "u\'" and trailing "\'"
  void trimRangeStrings(std::string&);

  StringProperty           m_processorChannel;
  StringProperty           m_eventRangeChannel;
};

#endif

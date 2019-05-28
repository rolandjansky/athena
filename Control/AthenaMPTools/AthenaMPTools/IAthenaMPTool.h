/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAMPTOOLS_IATHENAMPTOOL_H
#define ATHENAMPTOOLS_IATHENAMPTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "AthenaInterprocess/FdsRegistry.h"

#include <memory>
#include <vector>
#include <map>

namespace AthenaMP {
  struct WorkerOutput{
    std::string filename;
    std::string technology;
    std::string description;
    std::string access_mode;
    bool        shared;
  };
  typedef std::vector<WorkerOutput>                 SingleWorkerOutputs;
  typedef std::map<std::string,SingleWorkerOutputs> AllWorkerOutputs;
  typedef AllWorkerOutputs::iterator                AllWorkerOutputsIterator;

  typedef std::unique_ptr<AllWorkerOutputs>         AllWorkerOutputs_ptr;
}

static const InterfaceID IID_IAthenaMPTool( "IAthenaMPTool", 1, 0 );

class IAthenaMPTool : virtual public IAlgTool
{
 public:
  static const InterfaceID& interfaceID() { return IID_IAthenaMPTool; }

  // Returns number of children processes in the pool. -1 in case of failure
  virtual int makePool(int maxevt, int nprocs, const std::string& topdir) = 0;

  virtual StatusCode exec() = 0;

  // Pid of the finished process, 0 if no process finished, <0 if wait on the group failed
  virtual StatusCode wait_once(pid_t& pid) = 0;

  virtual void reportSubprocessStatuses() = 0;
  virtual void subProcessLogs(std::vector<std::string>&) = 0;
  virtual AthenaMP::AllWorkerOutputs_ptr generateOutputReport() = 0; 

  virtual void useFdsRegistry(std::shared_ptr<AthenaInterprocess::FdsRegistry>) = 0;
  virtual void setRandString(const std::string& randStr) = 0;

  // Brute force: kill all children
  virtual void killChildren() = 0;
};

#endif

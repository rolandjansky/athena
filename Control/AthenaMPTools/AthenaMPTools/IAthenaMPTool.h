/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAMPTOOLS_IATHENAMPTOOL_H
#define ATHENAMPTOOLS_IATHENAMPTOOL_H 1

#include "GaudiKernel/IAlgTool.h"
#include "AthenaInterprocess/FdsRegistry.h"

#include "boost/shared_ptr.hpp"
#include <vector>
#include <map>

namespace AthenaMP {
  struct WorkerOutput{
    std::string _filename;
    std::string _technology;
    std::string _description;
    std::string _access_mode;
    bool        _shared;
  };
  typedef std::vector<WorkerOutput>                 SingleWorkerOutputs;
  typedef std::map<std::string,SingleWorkerOutputs> AllWorkerOutputs;
  typedef AllWorkerOutputs::iterator                AllWorkerOutputsIterator;

  typedef boost::shared_ptr<AllWorkerOutputs>       AllWorkerOutputs_ptr;
}

static const InterfaceID IID_IAthenaMPTool( "IAthenaMPTool", 1, 0 );

class IAthenaMPTool : virtual public IAlgTool
{
 public:
  static const InterfaceID& interfaceID() { return IID_IAthenaMPTool; }

  // Returns number of children processes in the pool. -1 in case of failure
  virtual int makePool(int maxevt, int nprocs, const std::string& topdir) = 0;

  virtual StatusCode exec() = 0;
  virtual StatusCode wait_once(pid_t& pid) = 0; // Pid of the finished process, 0 if no process finished, <0 if wait on the group failed

  virtual void reportSubprocessStatuses() = 0;
  virtual void subProcessLogs(std::vector<std::string>&) = 0;
  virtual AthenaMP::AllWorkerOutputs_ptr generateOutputReport() = 0; 

  virtual void useFdsRegistry(boost::shared_ptr<AthenaInterprocess::FdsRegistry>) = 0;
  virtual void setRandString(const std::string& randStr) = 0;

  // Brutal force: kill all children
  virtual void killChildren() = 0;
};

#endif

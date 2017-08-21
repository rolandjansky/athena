/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAMPTOOLS_ATHENAMPTOOLBASE_H
#define ATHENAMPTOOLS_ATHENAMPTOOLBASE_H 1

#include "AthenaMPTools/IAthenaMPTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IEventProcessor.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/IFileMgr.h"
#include "GaudiKernel/IIoComponentMgr.h"

#include "AthenaInterprocess/ProcessGroup.h"
#include "AthenaInterprocess/IMessageDecoder.h"

#include <boost/filesystem.hpp>
#include <memory>

class IEvtSelector;

class AthenaMPToolBase : public AthAlgTool
  , public IAthenaMPTool
  , public AthenaInterprocess::IMessageDecoder
{
 public:
  AthenaMPToolBase(const std::string& type
		   , const std::string& name
		   , const IInterface* parent);

  virtual ~AthenaMPToolBase();
  
  StatusCode initialize();
  StatusCode finalize();

  // _________IAthenaMPTool_________   
  virtual int makePool(int maxevt, int nprocs, const std::string& topdir) = 0;
  virtual StatusCode exec() = 0;
  virtual StatusCode wait_once(pid_t& pid);

  virtual void reportSubprocessStatuses();
  virtual void subProcessLogs(std::vector<std::string>&) = 0;
  virtual AthenaMP::AllWorkerOutputs_ptr generateOutputReport();

  virtual void useFdsRegistry(boost::shared_ptr<AthenaInterprocess::FdsRegistry>);
  virtual void setRandString(const std::string& randStr);

  virtual void killChildren();

  // _________IMessageDecoder_________
  std::unique_ptr<AthenaInterprocess::ScheduledWork> operator()(const AthenaInterprocess::ScheduledWork&);

  // _____ Actual working horses ________
  virtual std::unique_ptr<AthenaInterprocess::ScheduledWork> bootstrap_func() = 0;
  virtual std::unique_ptr<AthenaInterprocess::ScheduledWork> exec_func() = 0;
  virtual std::unique_ptr<AthenaInterprocess::ScheduledWork> fin_func() = 0;

 protected:
  enum ESRange_Status {
    ESRANGE_SUCCESS
    , ESRANGE_NOTFOUND
    , ESRANGE_SEEKFAILED
    , ESRANGE_PROCFAILED
    , ESRANGE_FILENOTMADE
    , ESRANGE_BADINPFILE
  };

  enum Func_Flag {
    FUNC_BOOTSTRAP
    , FUNC_EXEC
    , FUNC_FIN
  };

  int mapAsyncFlag(Func_Flag flag, pid_t pid=0);
  int redirectLog(const std::string& rundir, bool addTimeStamp = true);
  int updateIoReg(const std::string& rundir);
  int reopenFds();
  int handleSavedPfc(const boost::filesystem::path& dest_path);

  void waitForSignal();

  int         m_nprocs;           // Number of workers spawned by the master process
  std::string m_subprocTopDir;    // Top run directory for subprocesses
  std::string m_subprocDirPrefix; // For ex. "worker__"
  std::string m_evtSelName;       // Name of the event selector

  AthenaInterprocess::ProcessGroup* m_processGroup;

  ServiceHandle<IEventProcessor> m_evtProcessor;
  ServiceHandle<IAppMgrUI>       m_appMgr;
  ServiceHandle<IFileMgr>        m_fileMgr;
  ServiceHandle<IIoComponentMgr> m_ioMgr;
  IEvtSelector*                  m_evtSelector;
  std::string                    m_fileMgrLog;
  boost::shared_ptr<AthenaInterprocess::FdsRegistry> m_fdsRegistry;
  std::string                    m_randStr;

 private:
  AthenaMPToolBase();
  AthenaMPToolBase(const AthenaMPToolBase&);
  AthenaMPToolBase& operator= (const AthenaMPToolBase&);
  int reopenFd(int fd, const std::string& name); // reopen individual descriptor

};

#endif

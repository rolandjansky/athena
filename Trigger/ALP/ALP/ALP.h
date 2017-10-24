// Dear emacs, this is -*- c++ -*-
/*

Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

Author: Sami Kama 2017
*/
//  Offline equivalent of HLTMPPU
//  


#ifndef ALP_ALP_H
#define ALP_ALP_H

#include <string>
#include <set>
#include <sys/types.h>
#include <unistd.h>
#include <deque>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <memory>
#include <chrono>

#include "hltinterface/HLTInterface.h"
#include "tbb/atomic.h"
#include "boost/thread/thread.hpp"
#include <boost/chrono/time_point.hpp>
#include <boost/chrono/system_clocks.hpp>


namespace hltinterface{
  class IInfoRegister;
  class DataSource;
  class IPCControlInterface;
  class GenericHLTContainer;
}

class TH1F;

class ALP : public hltinterface::HLTInterface {
public:
  ALP(); 
  ~ALP();
  bool configure(const boost::property_tree::ptree& args) override;
  bool connect(const boost::property_tree::ptree& args) override;
  bool prepareForRun(const boost::property_tree::ptree& args)override ;
  bool stopRun(const boost::property_tree::ptree& args)override;
  bool disconnect(const boost::property_tree::ptree& args)override;
  bool unconfigure(const boost::property_tree::ptree& args)override;
  bool publishStatistics(const boost::property_tree::ptree& args)override;
  void timeOutReached(const boost::property_tree::ptree& args)override;
  bool hltUserCommand(const boost::property_tree::ptree& args)override;
  bool process (const std::vector<eformat::ROBFragment<const uint32_t*> >& l1r, 
		hltinterface::HLTResult& hltr,
		const hltinterface::EventId& evId) override;
  bool prepareWorker(const boost::property_tree::ptree& args)override;
  bool finalizeWorker(const boost::property_tree::ptree& args)override;
private:
  pid_t forkChildren(int pos);
  bool doProcessLoop(const boost::property_tree::ptree& args,int childNo);
  void doNannyWork();
  void collectChildExitStatus();
  void terminateChildren(int timeOut);
  void printPtree(const boost::property_tree::ptree& args,std::string level);
  void startNanny();
  void stopNanny();
  void statsPublisher();
  void startMotherPublisher();
  void stopMotherPublisher();
  void doMotherPublication();
  //void publishMotherInfo(std::shared_ptr<ISInfoDictionary> dict,const std::string & name);
  void printOpenFDs(const std::string&);
  void printTasks(const std::string&);
  int  countThreads();
  void softTimeout();
  void hardTimeout();
  void runTimer();
  void waitForFreeMem(int maxSleep=100);
  pid_t m_myPid;
  pid_t m_myPgid;
  std::map<pid_t,int> m_myChildren,m_exitedChildren;
  std::map<int,pid_t> m_posPidMap;
  std::map<std::string,int> m_diedChildren;
  std::map<std::string,pid_t> m_childPidMap;
  std::deque<int> m_availableSlots;
  bool m_processEvents,m_terminationStarted;
  int m_numChildren;
  int m_FinalizeTimeout;
  boost::thread *m_nannyThread,*m_publisherThread,*m_motherPublisher;
  tbb::atomic<bool> m_nannyWork,m_publisherWork,m_timerWork;
  boost::mutex m_condMutex;
  hltinterface::HLTInterface  *m_HLTSteering;
  hltinterface::DataSource *m_dataSource;
  hltinterface::IInfoRegister *m_infoService;
  hltinterface::IPCControlInterface *m_ipcc;
  std::string m_myName;
  std::string m_ISSName;
  std::string m_childLogPath;
  boost::chrono::steady_clock::time_point m_lastPublish;
  std::chrono::steady_clock::time_point m_TOTimerStart;
  int m_publishInterval;
  int m_forkDelay;
  int m_preforkSleep;
  int m_softTimeout;
  int m_hardTimeout;
  int m_interEventSleep_ms;
  int m_interEventSpread_ms;
  int m_termStagger;
  unsigned int m_l1ResultTimeout;
  long m_evtNum;
  long m_lbNum;
  uint32_t m_CTPROBId;
  bool m_dumpFD;
  bool m_dumpThreads;
  bool m_saveConfigOpts;
  bool m_threadsExist;
  bool m_softTOTrigger,m_hardTOTrigger;
  bool m_keepNumForks;
  bool m_skipFinalize,m_skipFinalizeWorker,m_exitImmediately;
  int m_eventsInInterval,m_acceptedInInterval,m_rejectedInInterval;
  std::shared_ptr<hltinterface::GenericHLTContainer> m_motherInfo,m_childInfo;
  size_t m_MINumKills,m_MINumForks,m_MIUnexpectedChildExits,m_MINumRequested,m_MINumActive,m_MINumExited;
  size_t m_CINumEvents,m_CIAcceptedEvents,m_CIRejectedEvents,m_CIL1ResultFetchTimeouts,m_CISoftTimeouts,
    m_CILongestWaitForL1Result, m_CILongestProcessingTime, m_CIAverageProcessingTime, m_CIAverageAcceptTime, 
    m_CIAverageRejectTime, m_CIAverageL1ResultTime, m_CITimePercentInProcessing, m_CITimePercentInAccept, 
    m_CITimePercentInReject, m_CITimePercentInWait, m_CITimePercentInSend;
  std::vector<TH1F*> m_histos;
  boost::property_tree::ptree *m_configTree,*m_prepareForRunTree;
  std::unique_ptr<std::thread> m_timeoutThread;
  std::condition_variable m_timeoutCond;
  std::mutex m_timeoutMutex,m_statMutex;
  int m_myPos;
  std::chrono::milliseconds m_accDuration,//accept 
    m_rejDuration, //reject
    m_waitDuration, //waiting on L1Result
    m_sendDuration, // time to send
    m_procDuration, // time spend in processing
    m_totDuration; //total time
  std::chrono::milliseconds m_accDurationCum,//accept 
    m_rejDurationCum, //reject
    m_waitDurationCum, //waiting on L1Result
    m_sendDurationCum, // time to send
    m_procDurationCum, // time spend in processing
    m_totDurationCum; //total time
};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAMPTOOLS_SHAREDEVTQUEUEPROVIDER_H
#define ATHENAMPTOOLS_SHAREDEVTQUEUEPROVIDER_H 1

#include "AthenaMPToolBase.h"
#include "GaudiKernel/IIncidentListener.h"
#include "AthenaInterprocess/SharedQueue.h"

class IEventShare;

class SharedEvtQueueProvider : public AthenaMPToolBase
  , public IIncidentListener
{
 public:
  SharedEvtQueueProvider(const std::string& type
			 , const std::string& name
			 , const IInterface* parent);

  virtual ~SharedEvtQueueProvider();
  
  StatusCode initialize();
  StatusCode finalize();

  // _________IAthenaMPTool_________   
  int makePool(int maxevt, int nprocs, const std::string& topdir);
  StatusCode exec();

  void subProcessLogs(std::vector<std::string>&);
  virtual AthenaMP::AllWorkerOutputs_ptr generateOutputReport();

  // _________IIncidentListener___________
  void handle(const Incident& inc);

  // _____ Actual working horses ________
  std::unique_ptr<AthenaInterprocess::ScheduledWork> bootstrap_func();
  std::unique_ptr<AthenaInterprocess::ScheduledWork> exec_func();
  std::unique_ptr<AthenaInterprocess::ScheduledWork> fin_func();

 private:
  SharedEvtQueueProvider();
  SharedEvtQueueProvider(const SharedEvtQueueProvider&);
  SharedEvtQueueProvider& operator= (const SharedEvtQueueProvider&);

  // Properties
  bool m_isPileup;        // Are we doing pile-up digitization?
  int  m_nprocesses;      // We use this data member for adding negative numbers at the end of the event queue
                          // We cannot use m_nprocs for this purpose in order to avoid generating Output File Reports by Shared Queue Providers
  bool m_useSharedReader; // Are we doing the reading?
  int  m_nEventsBeforeFork;
  int  m_nChunkSize;
  int  m_nChunkStart;      // The beginning of the current chunk
  int  m_nPositionInChunk; // Position within the current chunk
  

  int  m_nEvtRequested;    // Max event received from AppMgr
  int  m_nEvtCounted;      // The number of events this tool has counted itself in the input files 
  
  AthenaInterprocess::SharedQueue*  m_sharedEventQueue;          
  IEventShare*             m_evtShare;

  // Add next event chunk to the queue
  void addEventsToQueue(); 

};

#endif

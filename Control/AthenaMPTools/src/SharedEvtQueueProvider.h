/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAMPTOOLS_SHAREDEVTQUEUEPROVIDER_H
#define ATHENAMPTOOLS_SHAREDEVTQUEUEPROVIDER_H 1

#include "AthenaMPToolBase.h"
#include "GaudiKernel/IIncidentListener.h"
#include "AthenaInterprocess/SharedQueue.h"

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
  AthenaInterprocess::ScheduledWork* bootstrap_func();
  AthenaInterprocess::ScheduledWork* exec_func();
  AthenaInterprocess::ScheduledWork* fin_func();

 private:
  SharedEvtQueueProvider();
  SharedEvtQueueProvider(const SharedEvtQueueProvider&);
  SharedEvtQueueProvider& operator= (const SharedEvtQueueProvider&);

  bool m_isPileup;        // Are we doing pile-up digitization?

  int  m_nEvtRequested;    // Max event received from AppMgr
  int  m_skipEvents;       // SkipEvent property value of the Event Selectors
  int  m_nEvtCounted;      // The number of events this tool has counted itself in the input files 
  int  m_nEvtAddPending;   // Number of pending events to be added to the queue
  int  m_preCountedEvents; // Somebody (TF) has already counted the events, no need to do that again
  bool m_needCountEvents;  // Flag indicating whether or not it is necessary to keep counting events
  int  m_nEventsBeforeFork;
  int  m_nEventsInInpFiles;// Total number of events in the input files opened so far

  AthenaInterprocess::SharedQueue*  m_sharedEventQueue;          

  // Get number of events in the current input file and store it in m_nEvtAddPending
  // It also determines whether or not we need to keep counting events
  // Returns 0 if success, 1 if failure   
  int nEventsInFile();              

  // Add pending events to the shared queue
  // Returns 0 if success, 1 if failure 
  int addEventsToQueue(); 

  // Update shared memory segment
  void updateShmem(int eventCount, bool countFinal);
};

#endif

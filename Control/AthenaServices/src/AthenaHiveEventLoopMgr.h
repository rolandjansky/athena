// -*- C++ -*-

/*
  Copyright (C) 2002-2017, 2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENASERVICES_ATHENAHIVEEVENTLOOPMGR_H
#define ATHENASERVICES_ATHENAHIVEEVENTLOOPMGR_H
/** @file AthenaHiveEventLoopMgr.h
    @brief The default ATLAS batch event loop manager.

  * $Id: AthenaHiveEventLoopMgr.h 780883 2016-10-27 20:16:15Z leggett $
*/

#include <string>
#include <vector>

#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/MinimalEventLoopMgr.h"
#include "GaudiKernel/IIncidentListener.h"
#include "AthenaKernel/Timeout.h"
#include "AthenaKernel/IAthenaEvtLoopPreSelectTool.h"
#include "AthenaKernel/IEventSeek.h"
#include "AthenaKernel/ICollectionSize.h"
#include "AthenaKernel/IConditionsCleanerSvc.h"

//////////////////////////////////////////////////
#include "GaudiKernel/IAlgResourcePool.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IHiveWhiteBoard.h"
#include "GaudiKernel/IScheduler.h"
#include "GaudiKernel/IAlgExecStateSvc.h"

// Standard includes
#include <functional>

// External Libraries
#include "tbb/concurrent_queue.h"
//////////////////////////////////////////////////

#ifndef EVENTINFO_EVENTID_H
# include "EventInfo/EventID.h"  /* number_type */
#endif

#include "StoreGate/ActiveStoreSvc.h"

// Forward declarations
class IConversionSvc;
class IDataManagerSvc;
class IDataProviderSvc;
class IIncidentSvc;
class ITimeKeeper;
class StoreGateSvc;
class ISvcLocator;

/** @class AthenaHiveEventLoopMgr
    @brief The default ATLAS batch event loop manager.
    
    @details It loops over input events according to
    job configuration. Among the main user-settable properties
    "FailureMode" controls behaviour of event loop depending on return code of Algorithms. 
    - 0: all non-SUCCESSes terminate job. 
    - 1: (DEFAULT) RECOVERABLE skips to next event, FAILURE terminates job.
    - 2: RECOVERABLE and FAILURE skip to next events
*/
class AthenaHiveEventLoopMgr 
  : virtual public IEventSeek,
    virtual public ICollectionSize,
    virtual public IIncidentListener,
            public MinimalEventLoopMgr,
            public Athena::TimeoutMaster
{
public:
  typedef IEvtSelector::Context   EvtContext;

protected:
  typedef ServiceHandle<IIncidentSvc> IIncidentSvc_t;
  /// Reference to the incident service
  IIncidentSvc_t     m_incidentSvc;

  typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
  /// Reference to StoreGateSvc;
  StoreGateSvc_t  m_eventStore;  ///< Property

  /// Reference to the Event Selector
  IEvtSelector*     m_evtSelector;
  /// Gaudi event selector Context (may be used as a cursor by the evt selector)
  EvtContext*       m_evtContext;
  /// @property Event selector Name. If empty string (default) take value from ApplicationMgr
  StringProperty    m_evtsel;

  typedef ServiceHandle<IDataManagerSvc> IDataManagerSvc_t;
  /// Reference to the Histogram Data Service
  IDataManagerSvc_t  m_histoDataMgrSvc;

  typedef ServiceHandle<IConversionSvc> IConversionSvc_t;
  /// @property Reference to the Histogram Persistency Service
  IConversionSvc_t   m_histoPersSvc;

  typedef ServiceHandle<ActiveStoreSvc> ActiveStoreSvc_t;
  /// @property Reference to the Histogram Persistency Service
  ActiveStoreSvc_t   m_activeStoreSvc;

  /// @property histogram persistency technology to use: "ROOT", "HBOOK", "NONE". By default ("") get property value from ApplicationMgr
  StringProperty    m_histPersName;

  /// the TimeKeeper service
  ITimeKeeper*      m_pITK;

  typedef EventID::number_type number_type;
  /// current run number
  number_type m_currentRun;
  bool m_firstRun;

  /// @property Name of TimeKeeper to use. NONE or empty string (default) means no time limit control on event loop
  StringProperty    m_timeKeeperName;
  /// @property update handler:sets up the time keeper
  void setupTimeKeeper(Property&);

  /// @property Failure mode 
  IntegerProperty m_failureMode;

  /// @property Print event heartbeat printouts every m_eventPrintoutInterval events
  UnsignedIntegerProperty m_eventPrintoutInterval;

  ///@property list of AthenaEventLoopPreselectTools
  typedef IAthenaEvtLoopPreSelectTool          tool_type;
  typedef ToolHandleArray< tool_type >         tool_store;
  typedef tool_store::const_iterator           tool_iterator;
  typedef std::vector<unsigned int>            tool_stats;
  typedef tool_stats::const_iterator           tool_stats_iterator;

  tool_stats m_toolInvoke; ///< tool called counter
  tool_stats m_toolReject; ///< tool returns StatusCode::FAILURE counter
  tool_stats m_toolAccept; ///< tool returns StatusCode::SUCCESS counter
  tool_store m_tools;         ///< internal tool store

  /// property update handler:sets up the Pre-selection tools
  void setupPreSelectTools(Property&);

  /// @property configure the policy wrt handling of when 'clear-event-store'
  /// has to happen: BeginEvent xor EndEvent.
  /// Default is to clear the store at the end of the event
  StringProperty m_clearStorePolicy;

  /// property update handler:set the clear-store policy value and check its
  /// value.
  void setClearStorePolicy(Property& clearStorePolicy);

  /// Dump out histograms as needed
  virtual StatusCode writeHistograms(bool force=false);

  /// Run the algorithms for the current event
  virtual StatusCode executeAlgorithms();

  /// Fire BeginRun EventIncident, run the algorithms beginRun hook
  StatusCode beginRunAlgorithms(const EventInfo&);

  /// Fire EndEvtLoop,EndRun, run the algorithms endRun hook
  StatusCode endRunAlgorithms();

  /// Initialize all algorithms and output streams
  StatusCode initializeAlgorithms();


  //***********************************************************//
  // for Hive
protected:

  /// Reference to the Whiteboard interface
  SmartIF<IHiveWhiteBoard>  m_whiteboard;

  /// Reference to the Algorithm resource pool
  SmartIF<IAlgResourcePool>  m_algResourcePool;

  /// Reference to the Algorithm Execution State Svc
  SmartIF<IAlgExecStateSvc>  m_aess;

  /// Property interface of ApplicationMgr
  SmartIF<IProperty>        m_appMgrProperty;

  /// A shortcut for the scheduler
  SmartIF<IScheduler> m_schedulerSvc;
  /// Clear a slot in the WB 
  StatusCode clearWBSlot(int evtSlot);
  /// Declare the root address of the event
  int declareEventRootAddress(const EventContext&);
  /// Create event context
  std::unique_ptr<EventContext> createEventContext(int createdEvents);
  /// Drain the scheduler from all actions that may be queued
  int drainScheduler(int& finishedEvents);
  /// Instance of the incident listener waiting for AbortEvent. 
  SmartIF< IIncidentListener >  m_abortEventListener;
  /// Name of the scheduler to be used
  std::string m_schedulerName;
  /// Name of the Whiteboard to be used
  std::string m_whiteboardName;
  /// Scheduled stop of event processing
  bool                m_scheduledStop;



public:
  /// Create event address using event selector
  StatusCode getEventRoot(IOpaqueAddress*& refpAddr);    



//***********************************************************//

public:
  /// Standard Constructor
  AthenaHiveEventLoopMgr(const std::string& nam, ISvcLocator* svcLoc);
  /// Standard Destructor
  virtual ~AthenaHiveEventLoopMgr();
  /// implementation of IAppMgrUI::initalize
  virtual StatusCode initialize() override;
  /// implementation of IAppMgrUI::finalize
  virtual StatusCode finalize() override;
  /// implementation of IAppMgrUI::nextEvent. maxevt==0 returns immediately
  virtual StatusCode nextEvent(int maxevt) override;
  /// implementation of IEventProcessor::executeEvent(void* par)
  virtual StatusCode executeEvent(void* par) override;
  /// implementation of IEventProcessor::executeRun(int maxevt)
  virtual StatusCode executeRun(int maxevt) override;
  /// implementation of IEventProcessor::stopRun()
  virtual StatusCode stopRun() override;
  /// implementation of IService::stop
  virtual StatusCode stop() override;
 

  /// Seek to a given event.
  virtual StatusCode seek(int evt) override;
  /// Return the current event count.
  virtual int curEvent() const override;
  /// Return the size of the collection.
  virtual int size() override;
  /// IIncidentListenet interfaces
  virtual void handle(const Incident& inc) override;

  /// interface dispatcher
  virtual StatusCode queryInterface( const InterfaceID& riid, 
                                     void** ppvInterface ) override;

  //FIXME hack to workaround pylcgdict problem...
  virtual const std::string& name() const  override { return Service::name(); } //FIXME 

private:
  AthenaHiveEventLoopMgr() = delete;
  AthenaHiveEventLoopMgr(const AthenaHiveEventLoopMgr&) = delete;
  AthenaHiveEventLoopMgr& operator= (const AthenaHiveEventLoopMgr&) = delete;

  unsigned int m_nevt;
  unsigned int m_timeStamp { 0 };
  /// @property histogram write/update interval
  UnsignedIntegerProperty m_writeInterval;
  bool m_writeHists;

  /// events processed
  unsigned int m_nev;
  unsigned int m_proc;
  bool m_useTools;
  bool m_doEvtHeartbeat;

  unsigned int m_flmbi, m_timeStampInt;

  // from MinimalEventLoopMgr
public:
  typedef std::list<SmartIF<IAlgorithm> >  ListAlg;
  // typedef std::list<IAlgorithm*>  ListAlgPtrs;
  // typedef std::list<std::string>   ListName;
  // typedef std::vector<std::string> VectorName;

private:
  StoreGateSvc* eventStore() const;
  const EventInfo* m_pEvent;

   ServiceHandle<Athena::IConditionsCleanerSvc> m_conditionsCleaner;

   // Save a copy of the last event context to use
   // at the end of event processing.
   EventContext m_lastEventContext;
};

#endif // ATHENASERVICES_ATHENAHIVEEVENTLOOPMGR_H

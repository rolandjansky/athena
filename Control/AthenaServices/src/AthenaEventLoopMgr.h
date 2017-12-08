// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENASERVICES_ATHENAEVENTLOOPMGR_H
#define ATHENASERVICES_ATHENAEVENTLOOPMGR_H
/** @file AthenaEventLoopMgr.h
    @brief The default ATLAS batch event loop manager.

  * $Id: AthenaEventLoopMgr.h,v 1.14 2009-03-16 16:38:38 leggett Exp $
*/

#include <string>
#include <vector>

#ifndef GAUDIKERNEL_IEVTSELECTOR_H
# include "GaudiKernel/IEvtSelector.h"
#endif

#ifndef GAUDIKERNEL_PROPERTY_H
# include "GaudiKernel/Property.h"
#endif 

#ifndef GAUDIKERNEL_SERVICEHANDLE_H
# include "GaudiKernel/ServiceHandle.h"
#endif 

#ifndef GAUDIKERNEL_TOOLHANDLE_H
#include "GaudiKernel/ToolHandle.h"
#endif

#include "GaudiKernel/IChronoStatSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/MinimalEventLoopMgr.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IAlgExecStateSvc.h"
#include "AthenaKernel/Timeout.h"
#include "AthenaKernel/IAthenaEvtLoopPreSelectTool.h"
#include "AthenaKernel/IEventSeek.h"
#include "AthenaKernel/ICollectionSize.h"
#include "AthenaKernel/IEvtSelectorSeek.h"

#ifndef EVENTINFO_EVENTID_H
# include "EventInfo/EventID.h"  /* number_type */
#endif

#include "StoreGate/ActiveStoreSvc.h"

// Forward declarations
class IConversionSvc;
class IDataManagerSvc;
class IIncidentSvc;
class ITimeKeeper;
class StoreGateSvc;
class EventContext;
////class ActiveStoreSvc;

class ISvcLocator;


/** @class AthenaEventLoopMgr
    @brief The default ATLAS batch event loop manager.
    
    @details It loops over input events according to
    job configuration. Among the main user-settable properties
    "FailureMode" controls behaviour of event loop depending on return code of Algorithms. 
    - 0: all non-SUCCESSes terminate job. 
    - 1: (DEFAULT) RECOVERABLE skips to next event, FAILURE terminates job.
    - 2: RECOVERABLE and FAILURE skip to next events
*/
class AthenaEventLoopMgr 
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
  virtual StatusCode executeAlgorithms(const EventContext&);

  /// Fire BeginRun EventIncident, run the algorithms beginRun hook
  StatusCode beginRunAlgorithms(const EventInfo&);

  /// Fire EndEvtLoop,EndRun, run the algorithms endRun hook
  StatusCode endRunAlgorithms();

  /// Initialize all algorithms and output streams
  StatusCode initializeAlgorithms();

protected:
  /// Reference to the Algorithm Execution State Svc
  SmartIF<IAlgExecStateSvc>  m_aess;

public:
  /// Standard Constructor
  AthenaEventLoopMgr(const std::string& nam, ISvcLocator* svcLoc);
  /// Standard Destructor
  virtual ~AthenaEventLoopMgr();
  /// implementation of IAppMgrUI::initalize
  virtual StatusCode initialize();
  /// implementation of IAppMgrUI::finalize
  virtual StatusCode finalize();
  /// implementation of IAppMgrUI::nextEvent. maxevt==0 returns immediately
  virtual StatusCode nextEvent(int maxevt);
  /// implementation of IEventProcessor::executeEvent(void* par)
  virtual StatusCode executeEvent(void* par);
  /// implementation of IEventProcessor::executeRun(int maxevt)
  virtual StatusCode executeRun(int maxevt);
  /// Seek to a given event.
  virtual StatusCode seek(int evt);
  /// Return the current event count.
  virtual int curEvent() const;
  /// Return the size of the collection.
  virtual int size();
  /// IIncidentListenet interfaces
  void handle(const Incident& inc);

  /// interface dispatcher
  virtual StatusCode queryInterface( const InterfaceID& riid, 
                                     void** ppvInterface );

  //FIXME hack to workaround pylcgdict problem...
  virtual const std::string& name() const { return Service::name(); } //FIXME 

  /// Return the current event context.
  //  const EventContext& eventContext() const { return *m_eventContext; }

private:
  AthenaEventLoopMgr(); ///< no implementation
  AthenaEventLoopMgr(const AthenaEventLoopMgr&); ///< no implementation
  AthenaEventLoopMgr& operator= (const AthenaEventLoopMgr&); ///< no implementation

  StatusCode installEventContext (const EventInfo* pEvent,
                                  unsigned int conditionsRun);

  int m_nevt;
  /// @property histogram write/update interval
  IntegerProperty m_writeInterval;
  bool m_writeHists;

  /// a @c std::cout like object
  mutable MsgStream m_msg;

  /// events processed
  unsigned int m_nev;
  unsigned int m_proc;
  bool m_useTools;
  StoreGateSvc* eventStore() const;

   EventContext m_eventContext;
   bool m_doChrono = false;
   ServiceHandle<IChronoStatSvc> m_chronoStatSvc;

};

#endif // STOREGATE_ATHENAEVENTLOOPMGR_H

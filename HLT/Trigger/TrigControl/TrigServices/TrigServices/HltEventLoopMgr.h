/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSERVICES_HLTEVENTLOOPMGR_H
#define TRIGSERVICES_HLTEVENTLOOPMGR_H

#include "TrigKernel/ITrigEventLoopMgr.h"
#include "EventInfo/EventID.h"  /* number_type */

#include "GaudiKernel/MinimalEventLoopMgr.h"
#include "GaudiKernel/SmartIF.h"

#include "TrigKernel/IHltTHistSvc.h"
#include "TrigROBDataProviderSvc/ITrigROBDataProviderSvc.h"


// Forward declarations
class IIncidentSvc;
class IAlgContextSvc;
class StoreGateSvc;
class ITHistSvc;
class IROBDataProviderSvc;
class CondAttrListCollection;
class TrigCOOLUpdateHelper;
namespace coral {
  class AttributeList;
}
class IAlgResourcePool;
class IHiveWhiteBoard;
class IScheduler;
#ifdef REENTRANT_GAUDI
 class IAlgExecMgr;
#endif



class HltEventLoopMgr : public MinimalEventLoopMgr,
                        virtual public ITrigEventLoopMgr
{

public:
  
  /// Standard constructor
  HltEventLoopMgr(const std::string& name, ISvcLocator* svcLoc);
  /// Standard destructor
  virtual ~HltEventLoopMgr();
  
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
  
  /// @name State transitions
  ///@{
  virtual StatusCode initialize();

  virtual StatusCode prepareForRun(const boost::property_tree::ptree& pt);
  
  virtual StatusCode hltUpdateAfterFork(const boost::property_tree::ptree& pt);
  ///@}
  
  /** Implementation of IEventProcessor::executeRun which calls IEventProcessor::nextEvent
   * @param maxevt number of events to process, -1 means all
   */
  virtual StatusCode executeRun(int maxevt=-1);
  
  /** Implementation of IEventProcessor::nextEvent which implements the event loop
   * @param maxevt number of events to process, -1 means all
   */
  virtual StatusCode nextEvent(int maxevt=-1);
  
  /** Implementation of IEventProcessor::executeEvent which processes a single event
   * @param par generic parameter
   */
  virtual StatusCode executeEvent(void* par);
  
  [[deprecated]]
  virtual StatusCode
  processRoIs(const std::vector<eformat::ROBFragment<const uint32_t*> >& l1_result,
              hltinterface::HLTResult& hlt_result,
              const hltinterface::EventId& evId);
  
  virtual StatusCode timeOutReached(const boost::property_tree::ptree& pt);
  
private:
  // ------------------------- Helper methods ----------------------------------
  
  /// Check if running in partition
  bool validPartition() const;

  /// Read DataFlow configuration properties
  void updateDFProps();
  
  /// Do whatever is necessary with RunParams (prepare) ptree
  const CondAttrListCollection* processRunParams(const boost::property_tree::ptree& pt);
  
  /// Clear per-event stores
  StatusCode clearTemporaryStores();
  
  /// Extract the single attr list off the SOR CondAttrListCollection
  const coral::AttributeList& getSorAttrList(const CondAttrListCollection* sor) const;
  
  /// Print the SOR record
  void printSORAttrList(const coral::AttributeList& atr, MsgStream& log) const;

  // ------------------------- Handles to required services/tools --------------
  typedef ServiceHandle<IIncidentSvc> IIncidentSvc_t;
  IIncidentSvc_t m_incidentSvc;
  
  typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
  StoreGateSvc_t m_evtStore;
  StoreGateSvc_t m_detectorStore;
  StoreGateSvc_t m_inputMetaDataStore;

  typedef ServiceHandle<IROBDataProviderSvc> IIROBDataProviderSvc_t;
  IIROBDataProviderSvc_t m_robDataProviderSvc;

  typedef ServiceHandle<ITHistSvc> ITHistSvc_t;
  ITHistSvc_t m_THistSvc;
  
  ToolHandle<TrigCOOLUpdateHelper> m_coolHelper;
  
  // ------------------------- Optional services/tools -------------------------
  IAlgContextSvc* m_algContextSvc{0};
  
  /// Reference to a THistSvc which implements also the Hlt additions
  SmartIF<IHltTHistSvc> m_hltTHistSvc;

  /// Reference to a ROBDataProviderSvc which implements also the Hlt additions
  SmartIF<ITrigROBDataProviderSvc> m_hltROBDataProviderSvc;


  // ------------------------- Properties --------------------------------------
  /// Application Name (="None" or "athenaHLT" for simulated data, "HLTMPPU-xx"? in online environment)
  StringProperty m_applicationName;
  /// Partition Name (="None" for offline, real partion name in online environment)
  StringProperty m_partitionName;
  /// JobOptions type (="NONE" or "DB", same as in PSC)
  StringProperty m_jobOptionsType;
  
  typedef SimpleProperty< std::vector<uint32_t> > Uint32ArrayProperty;
  /// list of all enabled ROBs which can be retrieved
  Uint32ArrayProperty m_enabledROBs;
  /// list of all enabled Sub Detectors which can be retrieved
  Uint32ArrayProperty m_enabledSubDetectors;

  // ------------------------- Other private members ---------------------------
  /// Current run number
  EventID::number_type m_currentRun{0};

protected:
  // ------------------------- Hive stuff from AthenaHiveEventLoopMgr ----------
  /// Reference to the Whiteboard interface
  SmartIF<IHiveWhiteBoard>  m_whiteboard;

  /// Reference to the Algorithm resource pool
  SmartIF<IAlgResourcePool>  m_algResourcePool;

  /// Property interface of ApplicationMgr
  SmartIF<IProperty>        m_appMgrProperty;

#ifdef REENTRANT_GAUDI
  SmartIF<IAlgExecMgr>      m_algExecMgr;
#endif

  /// A shortcut for the scheduler
  SmartIF<IScheduler> m_schedulerSvc;
  /// Clear a slot in the WB 
  StatusCode clearWBSlot(int evtSlot);
  /// Declare the root address of the event
  int declareEventRootAddress(const EventContext*);
  /// Create event context
  StatusCode createEventContext(EventContext*& eventContext, int createdEvents);
  /// Drain the scheduler from all actions that may be queued
  int drainScheduler(int& finishedEvents);
  /// Instance of the incident listener waiting for AbortEvent. 
  SmartIF< IIncidentListener >  m_abortEventListener;
  /// Name of the scheduler to be used
  StringProperty m_schedulerName;
  /// Name of the Whiteboard to be used
  StringProperty m_whiteboardName;
  /// Scheduled stop of event processing
  bool                m_scheduledStop;


};

//==============================================================================
inline bool HltEventLoopMgr::validPartition() const {
  return (m_partitionName.value()!="None" && m_partitionName.value()!="NONE");
}

#endif // TRIGSERVICES_HLTEVENTLOOPMGR_H

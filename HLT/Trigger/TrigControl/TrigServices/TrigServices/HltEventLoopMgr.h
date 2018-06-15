/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSERVICES_HLTEVENTLOOPMGR_H
#define TRIGSERVICES_HLTEVENTLOOPMGR_H

#include "TrigKernel/ITrigEventLoopMgr.h"
#include "EventInfo/EventID.h"  /* number_type */

#include "GaudiKernel/MinimalEventLoopMgr.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IEvtSelector.h"

#include "AthenaKernel/Timeout.h"

#include "TrigKernel/IHltTHistSvc.h"
#include "TrigROBDataProviderSvc/ITrigROBDataProviderSvc.h"

#include "eformat/write/FullEventFragment.h"


// Forward declarations
class IIncidentSvc;
class StoreGateSvc;
class EventInfo;
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
class IAlgExecStateSvc;


class HltEventLoopMgr : public MinimalEventLoopMgr,
                        virtual public ITrigEventLoopMgr,
                        virtual public Athena::TimeoutMaster
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

  /**
   * Implementation of IEventProcessor::executeRun which calls IEventProcessor::nextEvent
   * @param maxevt number of events to process, -1 means all
   */
  virtual StatusCode executeRun(int maxevt=-1);

  /**
   * Implementation of IEventProcessor::nextEvent which implements the event loop
   * @param maxevt number of events to process, -1 means all
   */
  virtual StatusCode nextEvent(int maxevt=-1);

  /**
   * Implementation of IEventProcessor::executeEvent which processes a single event
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

  // Update internally kept data from new sor
  void updateInternal(const coral::AttributeList & sor_attrlist);

  // Update internally kept data from new sor
  void updateMetadataStore(const coral::AttributeList & sor_attrlist) const;

  /// Clear per-event stores
  StatusCode clearTemporaryStores();

  /// Update the detector mask
  void updateDetMask(const std::pair<uint64_t, uint64_t>& dm);

  /// Extract the single attr list off the SOR CondAttrListCollection
  const coral::AttributeList& getSorAttrList(const CondAttrListCollection* sor) const;

  /// Print the SOR record
  void printSORAttrList(const coral::AttributeList& atr, MsgStream& log) const;

  /// Handle a failure to process an event
  void failedEvent(EventContext* eventContext, const EventInfo* eventInfo) const;

  // Build an HLT result FullEventFragment
  eformat::write::FullEventFragment* HltResult(const EventInfo* eventInfo) const;

  /// Send an HLT result FullEventFragment to the DataCollector
  void eventDone(eformat::write::FullEventFragment* hltrFragment) const;

  // ------------------------- Reimplemented AthenaHiveEventLoopMgr helpers ----
  /// Create event context
  StatusCode createEventContext(EventContext*& eventContext) const;

  /// Drain the scheduler from all actions that may be queued
  int drainScheduler() const;

  /// Clear an event slot in the whiteboard
  StatusCode clearWBSlot(int evtSlot) const;

  // ------------------------- Handles to required services/tools --------------
  ServiceHandle<IIncidentSvc>        m_incidentSvc;
  ServiceHandle<StoreGateSvc>        m_evtStore;
  ServiceHandle<StoreGateSvc>        m_detectorStore;
  ServiceHandle<StoreGateSvc>        m_inputMetaDataStore;
  ServiceHandle<IROBDataProviderSvc> m_robDataProviderSvc;
  ServiceHandle<ITHistSvc>           m_THistSvc;
  ServiceHandle<IEvtSelector>        m_evtSelector;

  /// Reference to the Whiteboard interface
  SmartIF<IHiveWhiteBoard> m_whiteboard;
  /// Reference to the Algorithm resource pool
  SmartIF<IAlgResourcePool> m_algResourcePool;
  /// Reference to the Algorithm Execution State Svc
  SmartIF<IAlgExecStateSvc> m_aess;
  /// Property interface of ApplicationMgr
  SmartIF<IProperty> m_appMgrProperty;
  /// A shortcut for the scheduler
  SmartIF<IScheduler> m_schedulerSvc;

  /// Helper tool for COOL updates
  ToolHandle<TrigCOOLUpdateHelper> m_coolHelper;

  // ------------------------- Optional services/tools -------------------------
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
  /// Name of the scheduler to be used
  StringProperty m_schedulerName;
  /// Name of the Whiteboard to be used
  StringProperty m_whiteboardName;

  typedef SimpleProperty< std::vector<uint32_t> > Uint32ArrayProperty;
  /// list of all enabled ROBs which can be retrieved
  Uint32ArrayProperty m_enabledROBs;
  /// list of all enabled Sub Detectors which can be retrieved
  Uint32ArrayProperty m_enabledSubDetectors;

  // ------------------------- Other private members ---------------------------
  /// typedef used for detector mask fields
  typedef EventID::number_type numt;
  /**
   * Detector mask0,1,2,3 - bit field indicating which TTC zones have been built into the event,
   * one bit per zone, 128 bit total, significance increases from first to last
   */
  std::tuple<numt, numt, numt, numt> m_detector_mask;

  /// Current run number
  EventID::number_type m_currentRun{0};
  /// Number of events which started processing
  size_t m_nevt;
  /// Current run number
  int m_threadPoolSize;

  /// Event selector context
  IEvtSelector::Context* m_evtSelContext;

};

//==============================================================================
inline bool HltEventLoopMgr::validPartition() const {
  return (m_partitionName.value()!="None" && m_partitionName.value()!="NONE");
}

#endif // TRIGSERVICES_HLTEVENTLOOPMGR_H

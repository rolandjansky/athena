/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSERVICES_HLTEVENTLOOPMGR_H
#define TRIGSERVICES_HLTEVENTLOOPMGR_H

// Trigger includes
#include "TrigKernel/ITrigEventLoopMgr.h"
#include "TrigKernel/HltPscErrorCode.h"
#include "TrigROBDataProviderSvc/ITrigROBDataProviderSvc.h"
#include "TrigOutputHandling/HLTResultMTMaker.h"

// Athena includes
#include "AthenaBaseComps/AthService.h"
#include "AthenaKernel/EventContextClid.h"
#include "AthenaKernel/Timeout.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h" // number_type
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

// Gaudi includes
#include "GaudiKernel/IEventProcessor.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/SmartIF.h"

// TDAQ includes
#include "eformat/write/FullEventFragment.h"

// System includes
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <memory>
#include <thread>

// Forward declarations
class CondAttrListCollection;
class IAlgExecStateSvc;
class IAlgorithm;
class IAlgResourcePool;
class IHiveWhiteBoard;
class IIncidentSvc;
class IJobOptionsSvc;
class IROBDataProviderSvc;
class IScheduler;
class ITHistSvc;
class StoreGateSvc;
class TrigCOOLUpdateHelper;
class IIoComponentMgr;

namespace coral {
  class AttributeList;
}
namespace HLT {
  class HLTResultMT;
}

/** @class HltEventLoopMgr
 *  @brief AthenaMT event loop manager for running HLT online
 **/
class HltEventLoopMgr : public extends<AthService, ITrigEventLoopMgr, IEventProcessor>,
                        public Athena::TimeoutMaster
{

public:

  /// Standard constructor
  HltEventLoopMgr(const std::string& name, ISvcLocator* svcLoc);
  /// Standard destructor
  virtual ~HltEventLoopMgr();

  /// @name Gaudi state transitions (overriden from AthService)
  ///@{
  virtual StatusCode initialize() override;
  virtual StatusCode stop() override;
  virtual StatusCode finalize() override;
  virtual StatusCode reinitialize() override;
  virtual StatusCode restart() override;
  ///@}

  /// @name State transitions of ITrigEventLoopMgr interface
  ///@{
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

  /**
   * Implementation of IEventProcessor::stopRun (obsolete for online runnning)
   */
  virtual StatusCode stopRun();

private:
  // ------------------------- Helper types ------------------------------------
  /// Enum type returned by the drainScheduler method
  enum class DrainSchedulerStatusCode {FAILURE=-2, RECOVERABLE=-1, SCHEDULER_EMPTY=0, SUCCESS=1};

  // ------------------------- Helper methods ----------------------------------

  /// Read DataFlow configuration properties
  void updateDFProps();

  /// Do whatever is necessary with RunParams (prepare) ptree
  StatusCode processRunParams(const boost::property_tree::ptree& pt);

  // Update internally kept data from new sor
  void updateInternal(const coral::AttributeList & sor_attrlist);

  // Update internally kept data from new sor
  void updateMetadataStore(const coral::AttributeList & sor_attrlist) const;

  /// Clear per-event stores
  StatusCode clearTemporaryStores();

  /// Update the detector mask
  void updateDetMask(const std::pair<uint64_t, uint64_t>& dm);

  /// Extract the single attr list off the SOR CondAttrListCollection
  const coral::AttributeList& getSorAttrList() const;

  /// Print the SOR record
  void printSORAttrList(const coral::AttributeList& atr) const;

  /** @brief Handle a failure to process an event
   *  @return FAILURE breaks the event loop
   **/
  StatusCode failedEvent(hltonl::PSCErrorCode errorCode,
                         const EventContext& eventContext);

  /// The method executed by the event timeout monitoring thread
  void runEventTimer();

  /// Drain the scheduler from all actions that may be queued
  DrainSchedulerStatusCode drainScheduler();

  /// Clear an event slot in the whiteboard
  StatusCode clearWBSlot(size_t evtSlot) const;

  /// Try to recover from a situation where scheduler and whiteboard see different number of free slots
  StatusCode recoverFromStarvation();

  /** @brief Try to drain the scheduler and clear all event data slots.
   *  Method of the last resort, used in attempts to recover from framework errors
   **/
  StatusCode drainAllSlots();

  // ------------------------- Handles to required services/tools --------------
  ServiceHandle<IIncidentSvc>        m_incidentSvc;
  ServiceHandle<IJobOptionsSvc>      m_jobOptionsSvc;
  ServiceHandle<StoreGateSvc>        m_evtStore;
  ServiceHandle<StoreGateSvc>        m_detectorStore;
  ServiceHandle<StoreGateSvc>        m_inputMetaDataStore;
  ServiceHandle<IROBDataProviderSvc> m_robDataProviderSvc;
  ServiceHandle<ITHistSvc>           m_THistSvc;
  ServiceHandle<IIoComponentMgr>     m_ioCompMgr;
  ServiceHandle<IEvtSelector>        m_evtSelector{this, "EvtSel", "EvtSel"};
  ServiceHandle<IConversionSvc>      m_outputCnvSvc{this, "OutputCnvSvc", "OutputCnvSvc"};
  ToolHandle<TrigCOOLUpdateHelper>   m_coolHelper{this, "CoolUpdateTool", "TrigCOOLUpdateHelper"};
  ToolHandle<HLTResultMTMaker>       m_hltResultMaker{this, "ResultMaker", "HLTResultMTMaker"};

  SmartIF<IHiveWhiteBoard> m_whiteboard;
  SmartIF<IAlgResourcePool> m_algResourcePool;
  SmartIF<IAlgExecStateSvc> m_aess;
  SmartIF<IScheduler> m_schedulerSvc;
  SmartIF<ITrigROBDataProviderSvc> m_hltROBDataProviderSvc;

  // ------------------------- Other properties --------------------------------------
  Gaudi::Property<std::string> m_schedulerName{
    this, "SchedulerSvc", "AvalancheSchedulerSvc", "Name of the scheduler"};

  Gaudi::Property<std::string> m_whiteboardName{
    this, "WhiteboardSvc", "EventDataSvc", "Name of the Whiteboard"};

  Gaudi::Property<std::vector<std::string> > m_topAlgNames{
    this, "TopAlg", {}, "List of top level algorithms names"};

  Gaudi::Property<float> m_hardTimeout{
    this, "HardTimeout", 10*60*1000/*=10min*/, "Hard event processing timeout in milliseconds"};

  Gaudi::Property<float> m_softTimeoutFraction{
    this, "SoftTimeoutFraction", 0.8, "Fraction of the hard timeout to be set as the soft timeout"};

  Gaudi::Property<int> m_maxFrameworkErrors{
    this, "MaxFrameworkErrors", 0,
    "Tolerable number of recovered framework errors before exiting (<0 means all are tolerated)"};

  Gaudi::Property<std::string> m_fwkErrorDebugStreamName{
    this, "FwkErrorDebugStreamName", "HLTMissingData",
    "Debug stream name for events with HLT framework errors"};

  Gaudi::Property<std::string> m_algErrorDebugStreamName{
    this, "AlgErrorDebugStreamName", "HLTError",
    "Debug stream name for events with HLT algorithm errors"};

  SG::WriteHandleKey<EventContext> m_eventContextWHKey{
    this, "EventContextWHKey", "EventContext", "StoreGate key for recording EventContext"};

  SG::ReadHandleKey<EventInfo> m_eventInfoRHKey{
    this, "EventInfoRHKey", "ByteStreamEventInfo", "StoreGate key for reading EventInfo"};

  Gaudi::Property<std::string> m_sorPath{
    this, "SORPath", "/TDAQ/RunCtrl/SOR_Params", "Path to StartOfRun parameters in detector store"};

  SG::ReadHandleKey<HLT::HLTResultMT> m_hltResultRHKey;    ///< StoreGate key for reading the HLT result

  // ------------------------- Other private members ---------------------------
  /// typedef used for detector mask fields
  typedef EventID::number_type numt;
  /**
   * Detector mask0,1,2,3 - bit field indicating which TTC zones have been built into the event,
   * one bit per zone, 128 bit total, significance increases from first to last
   */
  std::tuple<numt, numt, numt, numt> m_detector_mask{0xffffffff, 0xffffffff, 0, 0};

  /// "Event" context of current run (invalid event/slot)
  EventContext m_currentRunCtx;
  /// Event counter used for local bookkeeping; incremental per instance of HltEventLoopMgr, unrelated to global_id
  size_t m_localEventNumber;
  /// Event selector context
  IEvtSelector::Context* m_evtSelContext;
  /// Vector of top level algorithms
  std::vector<SmartIF<IAlgorithm> > m_topAlgList;
  /// Vector of event start-processing time stamps in each slot
  std::vector<std::chrono::steady_clock::time_point> m_eventTimerStartPoint;
  /// Vector of flags to tell if a slot is idle or processing
  std::vector<bool> m_isSlotProcessing; // be aware of vector<bool> specialisation
  /// Timeout mutex
  std::mutex m_timeoutMutex;
  /// Timeout condition variable
  std::condition_variable m_timeoutCond;
  /// Timeout thread
  std::unique_ptr<std::thread> m_timeoutThread;
  /// Soft timeout value
  int m_softTimeoutValue;
  /// Flag set to false if timer thread should be stopped
  std::atomic<bool> m_runEventTimer;
  /// Counter of framework errors
  int m_nFrameworkErrors;
  /// Application name
  std::string m_applicationName;
  /// Worker ID
  std::string m_workerId;


};

#endif // TRIGSERVICES_HLTEVENTLOOPMGR_H

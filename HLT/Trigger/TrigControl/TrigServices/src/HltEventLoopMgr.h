/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSERVICES_HLTEVENTLOOPMGR_H
#define TRIGSERVICES_HLTEVENTLOOPMGR_H

// Trigger includes
#include "TrigKernel/ITrigEventLoopMgr.h"
#include "TrigOutputHandling/HLTResultMTMaker.h"
#include "TrigSteeringEvent/OnlineErrorCode.h"
#include "TrigSteerMonitor/ISchedulerMonSvc.h"

// Athena includes
#include "AthenaBaseComps/AthService.h"
#include "AthenaKernel/EventContextClid.h"
#include "AthenaKernel/Timeout.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "CxxUtils/checker_macros.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

// Gaudi includes
#include "GaudiKernel/EventIDBase.h" // number_type
#include "GaudiKernel/IEventProcessor.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/SmartIF.h"
#include "Gaudi/Interfaces/IOptionsSvc.h"

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
class IScheduler;
class StoreGateSvc;
class TrigCOOLUpdateHelper;
class TrigSORFromPtreeHelper;
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
  ///@}

  /// @name State transitions of ITrigEventLoopMgr interface
  ///@{
  virtual StatusCode prepareForStart (const boost::property_tree::ptree &) override;
  virtual StatusCode prepareForRun ATLAS_NOT_THREAD_SAFE (const boost::property_tree::ptree& pt) override;
  virtual StatusCode hltUpdateAfterFork(const boost::property_tree::ptree& pt) override;
  ///@}

  /**
   * Implementation of IEventProcessor::executeRun which calls IEventProcessor::nextEvent
   * @param maxevt number of events to process, -1 means all
   */
  virtual StatusCode executeRun(int maxevt=-1) override;

  /**
   * Implementation of IEventProcessor::nextEvent which implements the event loop
   * @param maxevt number of events to process, -1 means all
   */
  virtual StatusCode nextEvent(int maxevt=-1) override;

  /**
   * Implementation of IEventProcessor::executeEvent which processes a single event
   * @param ctx the current EventContext
   */
  virtual StatusCode executeEvent( EventContext &&ctx ) override;

  /**
   * create an Event Context object
   */
  virtual EventContext createEventContext() override;

  /**
   * Implementation of IEventProcessor::stopRun (obsolete for online runnning)
   */
  virtual StatusCode stopRun() override;

private:
  // ------------------------- Helper types ------------------------------------
  /// Enum type returned by the drainScheduler method
  enum class DrainSchedulerStatusCode {FAILURE=-2, RECOVERABLE=-1, SCHEDULER_EMPTY=0, SUCCESS=1};

  // ------------------------- Helper methods ----------------------------------

  /// Read DataFlow configuration properties
  void updateDFProps();

  // Update internally kept data from new sor
  void updateInternal(const coral::AttributeList & sor_attrlist);

  // Update internally kept data from new sor
  void updateMetadataStore(const coral::AttributeList & sor_attrlist) const;

  /// Set magnetic field currents from ptree
  StatusCode updateMagField(const boost::property_tree::ptree& pt) const;

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
  StatusCode failedEvent(HLT::OnlineErrorCode errorCode,
                         const EventContext& eventContext);

  /// The method executed by the event timeout monitoring thread
  void runEventTimer();

  /// Produce a subset of IAlgExecStateSvc::algExecStates with only non-success StatusCodes
  std::unordered_map<std::string_view,StatusCode> algExecErrors(const EventContext& eventContext) const;

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
  ServiceHandle<Gaudi::Interfaces::IOptionsSvc>      m_jobOptionsSvc;
  ServiceHandle<StoreGateSvc>        m_evtStore;
  ServiceHandle<StoreGateSvc>        m_detectorStore;
  ServiceHandle<StoreGateSvc>        m_inputMetaDataStore;
  ServiceHandle<IIoComponentMgr>     m_ioCompMgr;
  ServiceHandle<IEvtSelector>        m_evtSelector{this, "EvtSel", "EvtSel"};
  ServiceHandle<IConversionSvc>      m_outputCnvSvc{this, "OutputCnvSvc", "OutputCnvSvc"};
  ServiceHandle<ISchedulerMonSvc>    m_schedulerMonSvc{this, "SchedulerMonSvc", "SchedulerMonSvc"};
  ToolHandle<TrigCOOLUpdateHelper>   m_coolHelper{this, "CoolUpdateTool", "TrigCOOLUpdateHelper"};
  ToolHandle<HLTResultMTMaker>       m_hltResultMaker{this, "ResultMaker", "HLTResultMTMaker"};
  ToolHandle<GenericMonitoringTool>  m_monTool{this, "MonTool", "", "Monitoring tool"};

  SmartIF<IHiveWhiteBoard> m_whiteboard;
  SmartIF<IAlgResourcePool> m_algResourcePool;
  SmartIF<IAlgExecStateSvc> m_aess;
  SmartIF<IScheduler> m_schedulerSvc;

  std::unique_ptr<TrigSORFromPtreeHelper> m_sorHelper;

  // ------------------------- Other properties --------------------------------------
  Gaudi::Property<std::string> m_schedulerName{
    this, "SchedulerSvc", "AvalancheSchedulerSvc", "Name of the scheduler"};

  Gaudi::Property<std::string> m_whiteboardName{
    this, "WhiteboardSvc", "EventDataSvc", "Name of the Whiteboard"};

  Gaudi::Property<float> m_hardTimeout{
    this, "HardTimeout", 10*60*1000/*=10min*/, "Hard event processing timeout in milliseconds"};

  Gaudi::Property<float> m_softTimeoutFraction{
    this, "SoftTimeoutFraction", 0.8, "Fraction of the hard timeout to be set as the soft timeout"};

  Gaudi::Property<int> m_maxFrameworkErrors{
    this, "MaxFrameworkErrors", 10,
    "Tolerable number of recovered framework errors before exiting (<0 means all are tolerated)"};

  Gaudi::Property<std::string> m_fwkErrorDebugStreamName{
    this, "FwkErrorDebugStreamName", "HLTMissingData",
    "Debug stream name for events with HLT framework errors"};

  Gaudi::Property<std::string> m_algErrorDebugStreamName{
    this, "AlgErrorDebugStreamName", "HltError",
    "Debug stream name for events with HLT algorithm errors"};

  Gaudi::Property<std::string> m_timeoutDebugStreamName{
    this, "TimeoutDebugStreamName", "HltTimeout",
    "Debug stream name for events with HLT timeout"};

  Gaudi::Property<std::string> m_truncationDebugStreamName{
    this, "TruncationDebugStreamName", "TruncatedHLTResult",
    "Debug stream name for events with HLT result truncation"};

  Gaudi::Property<std::string> m_sorPath{
    this, "SORPath", "/TDAQ/RunCtrl/SOR_Params", "Path to StartOfRun parameters in detector store"};

  Gaudi::Property<bool> m_setMagFieldFromPtree{
    this, "setMagFieldFromPtree", false, "Read magnet currents from ptree"};

  Gaudi::Property<unsigned int> m_forceRunNumber{
    this, "forceRunNumber", 0, "Override run number"};

  Gaudi::Property<unsigned int> m_forceLumiblock{
    this, "forceLumiblock", 0, "Override lumiblock number"};

  Gaudi::Property<unsigned long long> m_forceSOR_ns{
    this, "forceStartOfRunTime", 0, "Override SOR time (epoch in nano-seconds)"};

  Gaudi::Property<bool> m_rewriteLVL1{
    this, "RewriteLVL1", false,
    "Encode L1 results to ByteStream and write to the output. Possible only with athenaHLT, not online."};

  Gaudi::Property<bool> m_monitorScheduler{
    this, "MonitorScheduler", false, "Enable SchedulerMonSvc to collect scheduler status data in online histograms"};

  SG::WriteHandleKey<EventContext> m_eventContextWHKey{
    this, "EventContextWHKey", "EventContext", "StoreGate key for recording EventContext"};

  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoRHKey{
    this, "EventInfoRHKey", "EventInfo", "StoreGate key for reading xAOD::EventInfo"};

  SG::ReadHandleKey<xAOD::TrigCompositeContainer> m_l1TriggerResultRHKey{
    this, "L1TriggerResultRHKey", "L1TriggerResult", "StoreGate key for reading L1TriggerResult for RewriteLVL1"};

  SG::ReadHandleKey<HLT::HLTResultMT> m_hltResultRHKey;    ///< StoreGate key for reading the HLT result

  // ------------------------- Other private members ---------------------------
  /// typedef used for detector mask fields
  typedef EventIDBase::number_type numt;
  /**
   * Detector mask0,1,2,3 - bit field indicating which TTC zones have been built into the event,
   * one bit per zone, 128 bit total, significance increases from first to last
   */
  std::tuple<numt, numt, numt, numt> m_detector_mask{0xffffffff, 0xffffffff, 0, 0};

  /// "Event" context of current run (invalid event/slot)
  EventContext m_currentRunCtx;
  /// Event counter used for local bookkeeping; incremental per instance of HltEventLoopMgr, unrelated to global_id
  size_t m_localEventNumber{0};
  /// Event selector context
  IEvtSelector::Context* m_evtSelContext{nullptr};
  /// Vector of event start-processing time stamps in each slot
  std::vector<std::chrono::steady_clock::time_point> m_eventTimerStartPoint;
  /// Vector of time stamps telling when each scheduler slot was freed
  std::vector<std::chrono::steady_clock::time_point> m_freeSlotStartPoint;
  /// Vector of flags to tell if a slot is idle or processing
  std::vector<bool> m_isSlotProcessing; // be aware of vector<bool> specialisation
  /// Timeout mutex
  std::mutex m_timeoutMutex;
  /// Timeout condition variable
  std::condition_variable m_timeoutCond;
  /// Timeout thread
  std::unique_ptr<std::thread> m_timeoutThread;
  /// Soft timeout value set to HardTimeout*SoftTimeoutFraction at initialisation
  std::chrono::milliseconds m_softTimeoutValue{0};
  /// Flag set to false if timer thread should be stopped
  std::atomic<bool> m_runEventTimer{true};
  /// Counter of framework errors
  int m_nFrameworkErrors{0};
  /// Application name
  std::string m_applicationName;
  /// Worker ID
  int m_workerID{0};
  /// Worker PID
  int m_workerPID{0};

};

#endif // TRIGSERVICES_HLTEVENTLOOPMGR_H

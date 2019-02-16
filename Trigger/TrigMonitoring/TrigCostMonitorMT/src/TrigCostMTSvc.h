/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTMONITORMT_TRIGCOSTMTSVC_H
#define TRIGCOSTMONITORMT_TRIGCOSTMTSVC_H

#include <atomic>

#include "GaudiKernel/ToolHandle.h"

#include "AthenaBaseComps/AthService.h"
#include "AthContainers/ConstDataVector.h"
#include "StoreGate/WriteHandle.h"

#include "TrigTimeAlgs/TrigTimeStamp.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "TrigCostMonitorMT/ITrigCostMTSvc.h"
#include "TrigCostDataStore.h"

/**
 * @class TrigCostMTSvc
 * @brief AthenaMT service to collect trigger cost data from all threads and summarise it at the end of the event
 *
 * The main hooks into this service are: 
 *  L1Decoder - To clear the internal storage and flag the event for monitoring.
 *  TrigCostMTAuditor - To inform the service when algorithms start and stop executing
 *  HLTROBDataProviderSvc - To inform the service about requests for data ROBs 
 *  HLTSummaryAlg - To inform the service when the HLT has finished, and to receive the persistent payload 
 */
class TrigCostMTSvc : public extends <AthService, ITrigCostMTSvc> {
  public:

  /**
   * @brief Standard ATLAS Service constructor
   * @param[in] name The service's name
   * @param[in] svcloc A pointer to a service location service
   */
  TrigCostMTSvc(const std::string& name, ISvcLocator* pSvcLocator);

  /**
   * @brief Destructor. Currently nothing to delete.
   */
  virtual ~TrigCostMTSvc();

  /**
   * @brief Initialise, create enough storage to store m_eventSlots.
   */
  virtual StatusCode initialize() override;

  /**
   * @brief Finalize, act on m_saveHashes.
   */
  virtual StatusCode finalize() override;

  /**
   * @brief Implementation of ITrigCostMTSvc::startEvent.
   * @param[in] context The event context
   * @param[in] enableMonitoring Sets if the event should be monitored or not. Not monitoring will save CPU
   * @return Success unless monitoring is enabled and the service's data stores can not be cleared for some reason
   */
  virtual StatusCode startEvent(const EventContext& context, const bool enableMonitoring = true) override; 

  /**
   * @brief Implementation of ITrigCostMTSvc::processAlg.
   * @param[in] context The event context
   * @param[in] caller Name of the algorithm to audit CPU usage for
   * @param[in] type If we are Before or After the algorithm's execution
   */
  virtual StatusCode processAlg(const EventContext& context, const std::string& caller, const AuditType type) override; 

  /**
   * @brief Implementation of ITrigCostMTSvc::endEvent.
   * @param[in] context The event context
   * @param[out] outputHandle Write handle to fill with execution summary if the event was monitored
   */
  virtual StatusCode endEvent(const EventContext& context, SG::WriteHandle<xAOD::TrigCompositeContainer>& outputHandle) override; 

  private: 

  Gaudi::Property<bool>      m_monitorAll{this, "MonitorAll", false, "Monitor every HLT event, e.g. for offline validation."};
  Gaudi::Property<bool>      m_printTimes{this, "PrintTimes", false, "Sends per-algorithm timing to MSG::INFO."};
  Gaudi::Property<bool>      m_saveHashes{this, "SaveHashes", false, "Store a copy of the hash dictionary for easier debugging"};
  Gaudi::Property<unsigned>  m_eventSlots{this, "EventSlots", 0,     "Number of concurrent processing slots."};

  /**
   * @return If the event is flagged as being monitored. Allows for a quick return if not
   * @param[in] context The event context
   */
  bool isMonitoredEvent(const EventContext& context) const;

  /**
   * Sanity check that the job is respecting the number of slots which were declared at config time
   * @return Success if the m_eventMonitored array is range, Failure if access request would overflow
   * @param[in] context The event context
   */
  StatusCode checkSlot(const EventContext& context) const;

  /**
   * @breif Internal function to return a RoI from an extended event context context
   * @param[in] context The event context
   * @return RoIId from the ATLAS extended event context. Or, AlgorithmIdentifier::s_noView = -1 for no RoIIdentifier
   */
  int32_t getROIID(const EventContext& context);


  std::unique_ptr< std::atomic<bool>[] >  m_eventMonitored; //!< Used to cache if the event in a given slot is being monitored.

  TrigCostDataStore<TrigTimeStamp> m_algStartTime; //!< Thread-safe store of algorithm start times.
  TrigCostDataStore<TrigTimeStamp> m_algStopTime; //!< Thread-safe store of algorithm stop times.
  TrigCostDataStore<std::thread::id> m_algThreadID; //!< Thread-safe store of algorithm's thread ID hash.
  TrigCostDataStore<int32_t> m_algROIID; //!< Thread-safe store of algorithm's ROI identifier.

};

#endif // TRIGCOSTMONITORMT_TRIGCOSTMTSVC_H

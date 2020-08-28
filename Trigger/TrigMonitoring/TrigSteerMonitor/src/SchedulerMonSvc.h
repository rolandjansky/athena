/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGSTEERMONITOR_SCHEDULERMONSVC_H
#define TRIGSTEERMONITOR_SCHEDULERMONSVC_H

// Local includes
#include "TrigSteerMonitor/ISchedulerMonSvc.h"

// Athena includes
#include "AthenaBaseComps/AthService.h"
#include "AthenaMonitoringKernel/Monitored.h"

// Gaudi includes
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IScheduler.h"

// System includes
#include <atomic>
#include <chrono>

/**
 * @class SchedulerMonSvc
 * @brief Service monitoring the Scheduler status and producing relevant online histograms
 **/
class SchedulerMonSvc : public extends<AthService, ISchedulerMonSvc>  {
public:

  // Clock type used in IScheduler::OccupancySnapshot::time
  using ClockType = decltype(IScheduler::OccupancySnapshot::time)::clock;

  SchedulerMonSvc(const std::string& name, ISvcLocator* svcLoc);
  virtual ~SchedulerMonSvc() override = default;

  // IService methods
  virtual StatusCode initialize() override;

  // ISchedulerMonSvc methods
  /// Start querying and monitoring Scheduler status
  virtual StatusCode startMonitoring() override;
  /// Stop querying and monitoring Scheduler status
  virtual StatusCode stopMonitoring() override;

private:
  // Properties and handles
  Gaudi::Property<std::string> m_schedulerName {
    this, "SchedulerName", "AvalancheSchedulerSvc", "Name of the scheduler"};
  Gaudi::Property<unsigned int> m_samplingPeriodMillisec {
    this, "SamplingPeriodMillisec", 5, "Target sampling period in milliseconds"};
  ToolHandle<GenericMonitoringTool> m_monTool {
    this, "MonTool", "", "Monitoring tool"};

  // Other private members
  SmartIF<IScheduler> m_scheduler {nullptr};
  std::atomic_bool m_running {false};
  ClockType::time_point m_startTime {};
  std::atomic_size_t m_lastSnapCounter {0};
};

#endif // TRIGSTEERMONITOR_SCHEDULERMONSVC_H

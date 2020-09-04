/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "SchedulerMonSvc.h"

namespace {
  /// This is AlgsExecutionStates::State::MAXVALUE which cannot be used here directly because it is not in a public header
  static constexpr size_t s_numAlgStates = 8;
  static constexpr std::array<size_t,8> s_algStateNumbers = {0,1,2,3,4,5,6,7};
}

// =============================================================================
SchedulerMonSvc::SchedulerMonSvc(const std::string& name, ISvcLocator* svcLoc)
: base_class(name, svcLoc) {}

// =============================================================================
StatusCode SchedulerMonSvc::initialize() {
  if (!m_monTool.empty()) ATH_CHECK(m_monTool.retrieve());
  return StatusCode::SUCCESS;
}

// =============================================================================
StatusCode SchedulerMonSvc::startMonitoring() {
  // Get a handle to the scheduler
  if (!m_scheduler.isValid()) {
    m_scheduler = serviceLocator()->service<IScheduler>(m_schedulerName, false);
    if (!m_scheduler.isValid()) {
      ATH_MSG_ERROR("Failed to retrieve the Scheduler service with name " << m_schedulerName);
      return StatusCode::FAILURE;
    }
  }

  // Flag the monitoring as running (prevents going past this point twice)
  if (bool expected = false; not m_running.compare_exchange_strong(expected, true)) {
    ATH_MSG_ERROR("startMonitoring called but it is already running");
    return StatusCode::FAILURE;
  }

  // Construct the callback and pass it to the scheduler monitoring API
  auto monCallback = [this](IScheduler::OccupancySnapshot snap) -> void {
    auto monTime = Monitored::Timer("TIME_monCallback");
    // Calculate and update snap counters
    const ClockType::duration wallTime = snap.time - m_startTime;
    const size_t thisSnapCounter = std::chrono::duration_cast<std::chrono::milliseconds>(wallTime).count() / m_samplingPeriodMillisec.value();
    const size_t lastSnapCounter = m_lastSnapCounter.exchange(thisSnapCounter);
    const int periodsSinceLastSnap = thisSnapCounter - lastSnapCounter;

    // If new snap comes before next sampling point, discard it
    if (periodsSinceLastSnap <= 0) {
      ATH_MSG_DEBUG("Discarding snap because periodsSinceLastSnap=" << periodsSinceLastSnap << " is not positive");
      return;
    }

    // Monitor total state counts across all slots
    std::vector<int> stateTotalCounts(s_numAlgStates, 0);
    for (size_t slot=0; slot < snap.states.size(); ++slot) {
      for (size_t state=0; state < snap.states[slot].size(); ++state) {
        stateTotalCounts[state] += snap.states[slot][state];
      }
    }
    auto mon_stateNumber = Monitored::Collection("AlgStates", s_algStateNumbers);
    auto mon_stateTotalCounts = Monitored::Collection("StateTotalCounts", stateTotalCounts);

    // Monitor number of free slots
    auto mon_freeSlots = Monitored::Scalar("FreeSlots", m_scheduler->freeSlots());

    // Fill monitoring histograms once for each sampling period passed since the last fill
    // If multiple sampling periods passed, it means the scheduler state didn't change during that time
    for (size_t snapNumber=lastSnapCounter+1; snapNumber<=thisSnapCounter; ++snapNumber) {
      auto mon_snapNumber = Monitored::Scalar("SnapNumber", snapNumber);
      auto mon_wallTimeSec = Monitored::Scalar("WallTimeSeconds", snapNumber*m_samplingPeriodMillisec.value()*1e-3);
      Monitored::Group(m_monTool, mon_snapNumber, mon_wallTimeSec, mon_freeSlots, mon_stateNumber, mon_stateTotalCounts);
    }
    monTime.stop();
    Monitored::Group(m_monTool, monTime);
  };

  // Start monitoring
  m_startTime = ClockType::now();
  m_scheduler->recordOccupancy(m_samplingPeriodMillisec.value(), std::move(monCallback));

  ATH_MSG_INFO("Scheduler monitoring started");

  return StatusCode::SUCCESS;
}

// =============================================================================
StatusCode SchedulerMonSvc::stopMonitoring() {
  if (bool expected = true; not m_running.compare_exchange_strong(expected, false)) {
    ATH_MSG_WARNING("stopMonitoring called but it was not running");
    return StatusCode::SUCCESS;
  }
  m_scheduler->recordOccupancy(-1, {});
  ATH_MSG_INFO("Scheduler monitoring stopped");
  return StatusCode::SUCCESS;
}

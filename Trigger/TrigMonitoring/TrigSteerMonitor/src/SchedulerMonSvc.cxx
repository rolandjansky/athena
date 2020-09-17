/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "SchedulerMonSvc.h"

using namespace std::string_literals;
using namespace std::literals::string_view_literals;

namespace {
  /// This is AlgsExecutionStates::State which cannot be used here directly because it is not in a public header
  enum class AlgState : size_t {
    INITIAL      = 0,
    CONTROLREADY = 1,
    DATAREADY    = 2,
    RESOURCELESS = 3,
    SCHEDULED    = 4,
    EVTACCEPTED  = 5,
    EVTREJECTED  = 6,
    ERROR        = 7,
    MAXVALUE     = 8
  };
  /// AlgState enum as strings
  static constexpr std::array<std::string_view,8> s_algStateNames = {{
    "INITIAL"sv, "CONTROLREADY"sv, "DATAREADY"sv, "RESOURCELESS"sv, "SCHEDULED"sv, "EVTACCEPTED"sv, "EVTREJECTED"sv, "ERROR"sv
  }};
  /// AlgState enum as size_t
  static constexpr std::array<size_t,8> s_algStateNumbers = {0,1,2,3,4,5,6,7};
  /// AlgStates counted as "active"
  static constexpr std::array<size_t,4> s_activeAlgStateNumbers = {
    static_cast<size_t>(AlgState::CONTROLREADY),
    static_cast<size_t>(AlgState::DATAREADY),
    static_cast<size_t>(AlgState::RESOURCELESS),
    static_cast<size_t>(AlgState::SCHEDULED)
  };
  /// Cast two numbers to double and divide one by the other
  template<typename Ta, typename Tb> constexpr double divAsDouble(const Ta& a, const Tb& b) {
    return static_cast<double>(a) / static_cast<double>(b);
  }
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

  // Get the number of threads
  int numThreads = std::stoi( SmartIF<IProperty>(m_scheduler)->getProperty("ThreadPoolSize").toString() );

  // Flag the monitoring as running (prevents going past this point twice)
  if (bool expected = false; not m_running.compare_exchange_strong(expected, true)) {
    ATH_MSG_ERROR("startMonitoring called but it is already running");
    return StatusCode::FAILURE;
  }

  // Construct the callback and pass it to the scheduler monitoring API
  auto monCallback = [this, &numThreads](IScheduler::OccupancySnapshot snap) -> void {
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
    std::vector<int> stateTotalCounts(static_cast<size_t>(AlgState::MAXVALUE), 0);
    for (size_t slot=0; slot < snap.states.size(); ++slot) {
      for (size_t state=0; state < snap.states[slot].size(); ++state) {
        stateTotalCounts[state] += snap.states[slot][state];
      }
    }
    auto mon_stateNumber = Monitored::Collection("AlgStates", s_algStateNumbers);
    auto mon_stateTotalCounts = Monitored::Collection("StateTotalCounts", stateTotalCounts);

    // Monitor alg state counts absolute numbers and ratios to N threads and N active states
    std::vector<Monitored::Scalar<int>> mon_stateCounts;
    std::vector<Monitored::Scalar<double>> mon_stateCountsOverThreads;
    std::vector<Monitored::Scalar<double>> mon_stateCountsOverActive;
    mon_stateCounts.reserve(static_cast<size_t>(AlgState::MAXVALUE));
    mon_stateCountsOverThreads.reserve(static_cast<size_t>(AlgState::MAXVALUE));
    mon_stateCountsOverActive.reserve(static_cast<size_t>(AlgState::MAXVALUE));
    int activeCount = 0;
    for (size_t i : s_activeAlgStateNumbers) {
      activeCount += stateTotalCounts[i];
    }
    for (size_t i : s_algStateNumbers) {
      mon_stateCounts.emplace_back(s_algStateNames[i].data(), stateTotalCounts[i]);
      mon_stateCountsOverThreads.emplace_back(s_algStateNames[i].data()+"_Over_Threads"s, divAsDouble(stateTotalCounts[i], numThreads));
      double toActive = (activeCount > 0) ? divAsDouble(stateTotalCounts[i], activeCount) : 0;
      mon_stateCountsOverActive.emplace_back(s_algStateNames[i].data()+"_Over_Active"s, toActive);
    }

    // Monitor number of free slots
    auto mon_freeSlots = Monitored::Scalar("FreeSlots", m_scheduler->freeSlots());

    // Reserve vector of references with size equal to the number of variables added into the vector in the loop below
    std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>> allMonVars;
    allMonVars.reserve(5 + mon_stateCounts.size() + mon_stateCountsOverThreads.size() + mon_stateCountsOverActive.size());
    // Fill monitoring histograms once for each sampling period passed since the last fill
    // If multiple sampling periods passed, it means the scheduler state didn't change during that time
    for (size_t snapNumber=lastSnapCounter+1; snapNumber<=thisSnapCounter; ++snapNumber) {
      auto mon_snapNumber = Monitored::Scalar("SnapNumber", snapNumber);
      auto mon_wallTimeSec = Monitored::Scalar("WallTimeSeconds", snapNumber*m_samplingPeriodMillisec.value()*1e-3);
      allMonVars.clear();
      allMonVars.insert(allMonVars.end(), mon_stateCounts.begin(), mon_stateCounts.end());
      allMonVars.insert(allMonVars.end(), mon_stateCountsOverThreads.begin(), mon_stateCountsOverThreads.end());
      allMonVars.insert(allMonVars.end(), mon_stateCountsOverActive.begin(), mon_stateCountsOverActive.end());
      allMonVars.insert(allMonVars.end(), {mon_stateNumber, mon_stateTotalCounts, mon_freeSlots, mon_snapNumber, mon_wallTimeSec});
      Monitored::Group(m_monTool, allMonVars);
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

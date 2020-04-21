/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*
 * @authors: Alaettin Serhan Mete, Hasan Ozturk - alaettin.serhan.mete@cern.ch, haozturk@cern.ch
 */

// Framework includes
#include "GaudiKernel/ThreadLocalContext.h"

// PerfMonComps includes
#include "PerfMonMTSvc.h"
#include "PerfMonUtils.h"  // borrow from existing code

// STD includes
#include <algorithm>

using json = nlohmann::json;  // for convenience

/*
 * Constructor
 */
PerfMonMTSvc::PerfMonMTSvc(const std::string& name, ISvcLocator* pSvcLocator)
    : AthService(name, pSvcLocator), m_eventCounter{0} {
  // Estimate the job configuration time -- see initialize as well
  m_jobCfg_time = PMonMT::get_wall_time();

  // Initial capture upon construction
  m_measurement.capture_snapshot();
  m_snapshotData[0].addPointStart_snapshot(m_measurement);
}

PerfMonMTSvc::~PerfMonMTSvc() {}

/*
 * Query Interface
 */
StatusCode PerfMonMTSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if (!ppvInterface) {
    return StatusCode::FAILURE;
  }

  if (riid == IPerfMonMTSvc::interfaceID()) {
    *ppvInterface = static_cast<IPerfMonMTSvc*>(this);
    return StatusCode::SUCCESS;
  }

  return AthService::queryInterface(riid, ppvInterface);
}

/*
 * Initialize the Service
 */
StatusCode PerfMonMTSvc::initialize() {
  // Print where we are
  ATH_MSG_INFO("Initializing " << name());

  // Three main snapshots : Initialize, Event Loop, and Finalize
  m_snapshotStepNames.push_back("Initialize");
  m_snapshotStepNames.push_back("Event Loop");
  m_snapshotStepNames.push_back("Finalize");

  // Set wall time offset
  m_eventLevelData.set_wall_time_offset(m_wallTimeOffset);
  if (m_wallTimeOffset > 0) {
    m_jobCfg_time -= m_wallTimeOffset;
  }

  /// Configure the auditor
  if (!PerfMon::makeAuditor("PerfMonMTAuditor", auditorSvc(), msg()).isSuccess()) {
    ATH_MSG_ERROR("Could not register auditor [PerfMonMTAuditor]!");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

/*
 * Finalize the Service
 */
StatusCode PerfMonMTSvc::finalize() {
  // Print where we are
  ATH_MSG_INFO("Finalizing " << name());

  // Final capture upon finalization
  m_measurement.capture_snapshot();
  m_snapshotData[2].addPointStop_snapshot(m_measurement);

  // Report everything
  report();

  return StatusCode::SUCCESS;
}

/*
 * Start Auditing
 */
void PerfMonMTSvc::startAud(const std::string& stepName, const std::string& compName) {
  /*
   * This if statement is temporary. It will be removed.
   * In current implementation the very first thing called is stopAud function
   * for PerfMonMTSvc. There are some components before it. We miss them.
   * It should be fixed.
   */
  if (compName != "AthenaHiveEventLoopMgr" && compName != "PerfMonMTSvc") {
    // Snapshots, i.e. Initialize, Event Loop, etc.
    startSnapshotAud(stepName, compName);

    // Nothing more to do if we don't listen to components
    if (!m_doComponentLevelMonitoring) return;

    // Serial or parallel
    if (!isLoop())
      startCompAud_serial(stepName, compName);
    else
      startCompAud_MT(stepName, compName);
  }
}

/*
 * Stop Auditing
 */
void PerfMonMTSvc::stopAud(const std::string& stepName, const std::string& compName) {
  // Don't self-monitor
  if (compName != "AthenaHiveEventLoopMgr" && compName != "PerfMonMTSvc") {
    // Snapshots, i.e. Initialize, Event Loop, etc.
    stopSnapshotAud(stepName, compName);

    // Nothing more to do if we don't listen to components
    if (!m_doComponentLevelMonitoring) return;

    // Serial or parallel
    if (!isLoop())
      stopCompAud_serial(stepName, compName);
    else
      stopCompAud_MT(stepName, compName);
  }
}

/*
 * Start Snapshot Auditing
 */
void PerfMonMTSvc::startSnapshotAud(const std::string& stepName, const std::string& compName) {
  // Last thing to be called before the event loop begins
  if (compName == "AthRegSeq" && stepName == "Start") {
    m_measurement.capture_snapshot();
    m_snapshotData[1].addPointStart_snapshot(m_measurement);
  }

  // Last thing to be called before finalize step begins
  if (compName == "AthMasterSeq" && stepName == "Finalize") {
    m_measurement.capture_snapshot();
    m_snapshotData[2].addPointStart_snapshot(m_measurement);
  }
}

/*
 * Stop Snapshot Auditing
 */
void PerfMonMTSvc::stopSnapshotAud(const std::string& stepName, const std::string& compName) {
  // First thing to be called after the initialize step ends
  if (compName == "AthMasterSeq" && stepName == "Initialize") {
    m_measurement.capture_snapshot();
    m_snapshotData[0].addPointStop_snapshot(m_measurement);
  }

  // First thing to be called after the event loop ends
  if (compName == "AthMasterSeq" && stepName == "Stop") {
    m_measurement.capture_snapshot();
    m_snapshotData[1].addPointStop_snapshot(m_measurement);
  }
}

/*
 * Start Serial Component Auditing
 */
void PerfMonMTSvc::startCompAud_serial(const std::string& stepName, const std::string& compName) {
  // Current step - component pair. Ex: Initialize-StoreGateSvc
  PMonMT::StepComp currentState = generate_serial_state(stepName, compName);

  // Capture the time
  m_measurement.capture_compLevel_serial();

  /*
   *  Dynamically create a MeasurementData instance for the current step-component pair
   *  This space will be freed after results are reported.
   */
  m_compLevelDataMap[currentState] = new PMonMT::MeasurementData;
  m_compLevelDataMap[currentState]->addPointStart_serial(m_measurement);
}

/*
 * Stop Serial Component Auditing
 */
void PerfMonMTSvc::stopCompAud_serial(const std::string& stepName, const std::string& compName) {
  // Capture the time
  m_measurement.capture_compLevel_serial();

  // Current step - component pair. Ex: Initialize-StoreGateSvc
  PMonMT::StepComp currentState = generate_serial_state(stepName, compName);

  m_compLevelDataMap[currentState]->addPointStop_serial(m_measurement);
}

/*
 * Start Parallel Component Auditing
 */
void PerfMonMTSvc::startCompAud_MT(const std::string& stepName, const std::string& compName) {
  std::lock_guard<std::mutex> lock(m_mutex_capture);

  uint64_t eventID = getEventID();

  PMonMT::StepCompEvent currentState = generate_parallel_state(stepName, compName, eventID);
  m_measurement.capture_compLevel_MT(currentState);
  m_parallelCompLevelData.addPointStart_MT(m_measurement, currentState);
}

/*
 * Stop Parallel Component Auditing
 */
void PerfMonMTSvc::stopCompAud_MT(const std::string& stepName, const std::string& compName) {
  std::lock_guard<std::mutex> lock(m_mutex_capture);

  uint64_t eventID = getEventID();

  PMonMT::StepCompEvent currentState = generate_parallel_state(stepName, compName, eventID);
  m_measurement.capture_compLevel_MT(currentState);
  m_parallelCompLevelData.addPointStop_MT(m_measurement, currentState);
}

/*
 * Event-level Monitoring
 */
void PerfMonMTSvc::eventLevelMon() {
  std::lock_guard<std::mutex> lock(m_mutex_capture);

  // If enabled, do event level monitoring
  if (m_doEventLoopMonitoring) {
    if (isCheckPoint()) {
      // Capture
      m_measurement.capture_eventLevel_MT(m_eventCounter);
      m_eventLevelData.record_eventLevel(m_measurement, m_eventCounter);
      // Report instantly
      report2Log_EventLevel_instant();
    }
  }
  incrementEventCounter();
}

/*
 * Internal atomic event counter
 * Should be able to use EventContext for this
 */
void PerfMonMTSvc::incrementEventCounter() { m_eventCounter++; }

/*
 * Is it event-level monitoring check point yet?
 */
bool PerfMonMTSvc::isCheckPoint() {
  if (m_checkPointType == "Arithmetic")
    return (m_eventCounter % m_checkPointFactor == 0);
  else
    return isPower(m_eventCounter, m_checkPointFactor);
}

bool PerfMonMTSvc::isPower(uint64_t input, uint64_t base) {
  while (input >= base && input % base == 0) input /= base;
  return (input == 1);
}

/*
 * Report the results to the log and the JSON file
 */
void PerfMonMTSvc::report() {
  // Write into log file
  report2Log();

  // Write into JSON
  if (m_reportResultsToJSON) {
    report2JsonFile();
  }
}

/*
 * Log reporting
 */
void PerfMonMTSvc::report2Log() {
  // Header
  report2Log_Description();

  // Detailed tables
  const bool procExists = doesDirectoryExist("/proc");
  if (!procExists) {
    ATH_MSG_INFO("There is no /proc directory in this system, therefore memory monitoring has failed!");
  }

  // Component level
  if (m_printDetailedTables && procExists && m_doComponentLevelMonitoring) {
    // Serial
    report2Log_Time_Mem_Serial();
    // Parallel
    report2Log_CompLevel_Time_Parallel();
  }

  // Event-level
  if (m_printDetailedTables && procExists && m_doEventLoopMonitoring) {
    report2Log_EventLevel();
  }

  // Summary and system information
  report2Log_Summary();
  report2Log_CpuInfo();
}

void PerfMonMTSvc::report2Log_Description() const {
  ATH_MSG_INFO("=======================================================================================");
  ATH_MSG_INFO("                                 PerfMonMTSvc Report                                   ");
  ATH_MSG_INFO("=======================================================================================");
  ATH_MSG_INFO("!!! PLEASE NOTE THAT THIS SERVICE IS CURRENTLY IN R&D PHASE");
  ATH_MSG_INFO("=======================================================================================");
  if (m_reportResultsToJSON) {
    ATH_MSG_INFO("*** Full set of information can also be found in: " << m_jsonFileName.toString());
    ATH_MSG_INFO("*** In order to make plots using the results run the following commands:");
    ATH_MSG_INFO("*** $ perfmonmt-plotter " << m_jsonFileName.toString());
    ATH_MSG_INFO("=======================================================================================");
  }
}

void PerfMonMTSvc::report2Log_Time_Mem_Serial() {
  using boost::format;

  ATH_MSG_INFO("=======================================================================================");
  ATH_MSG_INFO("                             Component Level Monitoring                                ");
  ATH_MSG_INFO("                                   (Serial Steps)                                      ");
  ATH_MSG_INFO("=======================================================================================");

  ATH_MSG_INFO(format("%1% %|15t|%2% %|31t|%3% %|44t|%4% %|64t|%5%") % "Step" % "CPU Time [ms]" % "Vmem [kB]" %
               "Malloc [kB]" % "Component");

  divideData2Steps_serial();

  for (auto vec_itr : m_stdoutVec_serial) {
    // Sort the results by CPU time for the time being
    std::vector<std::pair<PMonMT::StepComp, PMonMT::MeasurementData*>> pairs;
    for (auto itr = vec_itr.begin(); itr != vec_itr.end(); ++itr) pairs.push_back(*itr);

    sort(pairs.begin(), pairs.end(),
         [=](std::pair<PMonMT::StepComp, PMonMT::MeasurementData*>& a,
             std::pair<PMonMT::StepComp, PMonMT::MeasurementData*>& b) {
           return a.second->getDeltaCPU() > b.second->getDeltaCPU();
         });

    int counter = 0;
    for (auto it : pairs) {
      // Only write out a certian number of components
      if (counter >= m_printNSerialComps) {
        break;
      }
      counter++;

      ATH_MSG_INFO(format("%1% %|15t|%2% %|31t|%3% %|44t|%4% %|64t|%5%") % it.first.stepName %
                   it.second->getDeltaCPU() % it.second->getDeltaVmem() % it.second->getDeltaMalloc() %
                   it.first.compName);
    }
    ATH_MSG_INFO("=======================================================================================");
  }
}

// Report the event level measurement as soon as it is captured
void PerfMonMTSvc::report2Log_EventLevel_instant() const {
  double cpu_time = m_eventLevelData.getEventLevelCpuTime(m_eventCounter);
  double wall_time = m_eventLevelData.getEventLevelWallTime(m_eventCounter);

  long vmem = m_eventLevelData.getEventLevelVmem(m_eventCounter);
  long rss = m_eventLevelData.getEventLevelRss(m_eventCounter);
  long pss = m_eventLevelData.getEventLevelPss(m_eventCounter);
  long swap = m_eventLevelData.getEventLevelSwap(m_eventCounter);

  ATH_MSG_INFO("CPU Time: " << scaleTime(cpu_time) << ", Wall Time: " << scaleTime(wall_time)
                            << ", Vmem: " << scaleMem(vmem) << ", Rss: " << scaleMem(rss) << ", Pss: " << scaleMem(pss)
                            << ", Swap: " << scaleMem(swap));
}

void PerfMonMTSvc::report2Log_EventLevel() {
  using boost::format;

  ATH_MSG_INFO("                                Event Level Monitoring                                 ");
  ATH_MSG_INFO("=======================================================================================");

  ATH_MSG_INFO(format("%1% %|16t|%2$.2f %|28t|%3$.2f %|40t|%4% %|52t|%5% %|64t|%6% %|76t|%7%") % "Event" % "CPU [s]" %
               "Wall [s]" % "Vmem [kB]" % "Rss [kB]" % "Pss [kB]" % "Swap [kB]");

  for (const auto& it : m_eventLevelData.getEventLevelData()) {
    ATH_MSG_INFO(format("%1% %|16t|%2$.2f %|28t|%3$.2f %|40t|%4% %|52t|%5% %|64t|%6% %|76t|%7%") % it.first %
                 (it.second.cpu_time * 0.001) % (it.second.wall_time * 0.001) % it.second.mem_stats.at("vmem") %
                 it.second.mem_stats.at("rss") % it.second.mem_stats.at("pss") % it.second.mem_stats.at("swap"));
    // Add to leak estimate
    if (it.first >= std::max(uint64_t(10), uint64_t(m_checkPointFactor))) {
      m_fit_vmem.addPoint(it.first, it.second.mem_stats.at("vmem"));
      m_fit_pss.addPoint(it.first, it.second.mem_stats.at("pss"));
    }
  }
  ATH_MSG_INFO("=======================================================================================");
}

void PerfMonMTSvc::report2Log_CompLevel_Time_Parallel() {
  using boost::format;

  ATH_MSG_INFO("                             Component Level Monitoring                                ");
  ATH_MSG_INFO("                                  (Parallel Steps)                                     ");
  ATH_MSG_INFO("=======================================================================================");

  ATH_MSG_INFO(format("%1%  %|22t|%2$.2f  %|42t|%3$.2f  %|65t|%4%") % "Step" % "Call Count" % "CPU Time [ms]" %
               "Component");

  parallelDataAggregator();
  divideData2Steps_parallel();

  for (auto vec_itr : m_stdoutVec_parallel) {
    // Sort the results by CPU time for the time being
    std::vector<std::pair<PMonMT::StepComp, PMonMT::Measurement>> pairs;
    for (auto itr = vec_itr.begin(); itr != vec_itr.end(); ++itr) pairs.push_back(*itr);

    sort(pairs.begin(), pairs.end(),
         [=](std::pair<PMonMT::StepComp, PMonMT::Measurement>& a, std::pair<PMonMT::StepComp, PMonMT::Measurement>& b) {
           return a.second.cpu_time > b.second.cpu_time;  // sort by cpu times
         });

    int counter = 0;
    for (auto it : pairs) {
      // Only write out a certian number of components
      if (counter >= m_printNParallelComps) {
        break;
      }
      counter++;

      ATH_MSG_INFO(format("%1%  %|22t|%2$.2f  %|42t|%3$.2f  %|65t|%4%") % it.first.stepName % it.second.call_count %
                   it.second.cpu_time % it.first.compName);
    }
    ATH_MSG_INFO("=======================================================================================");
  }
}

void PerfMonMTSvc::report2Log_Summary() {
  using boost::format;

  ATH_MSG_INFO("                                 Snaphots Summary                                      ");
  ATH_MSG_INFO("=======================================================================================");

  ATH_MSG_INFO(format("%1% %|13t|%2% %|25t|%3% %|37t|%4% %|44t|%5% %|55t|%6% %|66t|%7% %|77t|%8%") % "Step" %
               "dCPU [s]" % "dWall [s]" % "<CPU>" % "dVmem [kB]" % "dRss [kB]" % "dPss [kB]" % "dSwap [kB]");

  ATH_MSG_INFO("---------------------------------------------------------------------------------------");

  for (unsigned int idx = 0; idx < 3; idx++) {
    ATH_MSG_INFO(format("%1% %|13t|%2% %|25t|%3% %|37t|%4$.2f %|44t|%5% %|55t|%6% %|66t|%7% %|77t|%8%") %
                 m_snapshotStepNames[idx] % (m_snapshotData[idx].getDeltaCPU() * 0.001) %
                 (m_snapshotData[idx].getDeltaWall() * 0.001) %
                 (m_snapshotData[idx].getDeltaCPU() / m_snapshotData[idx].getDeltaWall()) %
                 m_snapshotData[idx].getMemMonDeltaMap("vmem") % m_snapshotData[idx].getMemMonDeltaMap("rss") %
                 m_snapshotData[idx].getMemMonDeltaMap("pss") % m_snapshotData[idx].getMemMonDeltaMap("swap"));
  }

  ATH_MSG_INFO("***************************************************************************************");

  ATH_MSG_INFO(format("%1% %|35t|%2% ") % "Number of events processed:" % m_eventCounter);
  ATH_MSG_INFO(format("%1% %|35t|%2$.0f ") % "CPU usage per event [ms]:" %
               (m_snapshotData[1].getDeltaCPU() / m_eventCounter));
  ATH_MSG_INFO(format("%1% %|35t|%2$.3f ") % "Events per second:" %
               (m_eventCounter / m_snapshotData[1].getDeltaWall() * 1000.));

  ATH_MSG_INFO("***************************************************************************************");

  ATH_MSG_INFO(format("%1% %|35t|%2% ") % "Max Vmem: " % scaleMem(m_measurement.vmemPeak));
  ATH_MSG_INFO(format("%1% %|35t|%2% ") % "Max Rss: " % scaleMem(m_measurement.rssPeak));
  ATH_MSG_INFO(format("%1% %|35t|%2% ") % "Max Pss: " % scaleMem(m_measurement.pssPeak));
  if (m_doEventLoopMonitoring) {
    ATH_MSG_INFO("***************************************************************************************");
    ATH_MSG_INFO(format("%1% %|35t|%2% ") % "Leak estimate per event Vmem: " % scaleMem(m_fit_vmem.slope()));
    ATH_MSG_INFO(format("%1% %|35t|%2% ") % "Leak estimate per event Pss: " % scaleMem(m_fit_pss.slope()));
    ATH_MSG_INFO("  >> Estimated using the last " << m_fit_vmem.nPoints()
                                                  << " measurements from the Event Level Monitoring");
  }

  ATH_MSG_INFO("***************************************************************************************");

  ATH_MSG_INFO(format("%1% %|35t|%2% ") % "Job Cfg: " % scaleTime(m_jobCfg_time));

  ATH_MSG_INFO("=======================================================================================");
}

void PerfMonMTSvc::report2Log_CpuInfo() const {
  using boost::format;

  ATH_MSG_INFO("                                  System Information                                   ");
  ATH_MSG_INFO("=======================================================================================");

  ATH_MSG_INFO(format("%1% %|34t|%2% ") % "CPU Model:" % get_cpu_model_info());
  ATH_MSG_INFO(format("%1% %|35t|%2% ") % "Number of Available Cores:" % get_cpu_core_info());

  ATH_MSG_INFO("=======================================================================================");
}

void PerfMonMTSvc::report2JsonFile() {
  json j;

  // CPU and Wall-time
  report2JsonFile_Summary(j);  // Snapshots
  if (m_doEventLoopMonitoring) {
    report2JsonFile_EventLevel_Time_Parallel(j);  // Event Level
  }
  if (m_doComponentLevelMonitoring) {
    report2JsonFile_Time_Serial(j);              // Serial Components
    report2JsonFile_CompLevel_Time_Parallel(j);  // Parallel Components
  }

  // Memory
  const bool procExists = doesDirectoryExist("/proc");

  if (procExists) {
    if (m_doComponentLevelMonitoring) {
      report2JsonFile_Mem_Serial(j);  // Serial Components
    }
    if (m_doEventLoopMonitoring) {
      report2JsonFile_EventLevel_Mem_Parallel(j);  // Event Level
    }
  }

  // Write
  std::ofstream o(m_jsonFileName);
  o << std::setw(4) << j << std::endl;
}

void PerfMonMTSvc::report2JsonFile_Summary(nlohmann::json& j) const {
  // Report snapshot level results
  for (int i = 0; i < 3; i++) {
    // Clean this part!
    double wall_time = m_snapshotData[i].getDeltaWall();
    double cpu_time = m_snapshotData[i].getDeltaCPU();

    j["Snapshot_level"][m_snapshotStepNames[i]] = {{"cpu_time", cpu_time}, {"wall_time", wall_time}};
  }
}
void PerfMonMTSvc::report2JsonFile_Time_Serial(nlohmann::json& j) const {
  // Report component level time measurements in serial steps
  for (auto& it : m_compLevelDataMap) {
    std::string stepName = it.first.stepName;
    std::string compName = it.first.compName;

    double wall_time = it.second->getDeltaWall();
    double cpu_time = it.second->getDeltaCPU();

    // nlohmann::json syntax
    j["TimeMon_Serial"][stepName][compName] = {{"cpu_time", cpu_time}, {"wall_time", wall_time}};
  }
}

void PerfMonMTSvc::report2JsonFile_EventLevel_Time_Parallel(nlohmann::json& j) const {
  // Report event level CPU measurements
  for (const auto& it : m_eventLevelData.getEventLevelData()) {
    std::string checkPoint = std::to_string(it.first);
    double cpu_time = it.second.cpu_time;
    double wall_time = it.second.wall_time;

    j["TimeMon_Parallel"][checkPoint] = {{"cpu_time", cpu_time}, {"wall_time", wall_time}};
  }
}

void PerfMonMTSvc::report2JsonFile_CompLevel_Time_Parallel(nlohmann::json& j) const {
  // Report component level time measurements in parallel steps
  for (auto& it : m_aggParallelCompLevelDataMap) {
    std::string stepName = it.first.stepName;
    std::string compName = it.first.compName;

    double wall_time = it.second.wall_time;
    double cpu_time = it.second.cpu_time;

    j["TimeMon_Parallel"][stepName][compName] = {{"cpu_time", cpu_time}, {"wall_time", wall_time}};
  }
}

void PerfMonMTSvc::report2JsonFile_Mem_Serial(nlohmann::json& j) const {
  // Report component level memory measurements in serial steps
  for (auto& it : m_compLevelDataMap) {
    std::string stepName = it.first.stepName;
    std::string compName = it.first.compName;

    long vmem = it.second->getMemMonDeltaMap("vmem");
    long rss = it.second->getMemMonDeltaMap("rss");
    long pss = it.second->getMemMonDeltaMap("pss");
    long swap = it.second->getMemMonDeltaMap("swap");

    // nlohmann::json syntax
    j["MemMon_Serial"][stepName][compName] = {{"vmem", vmem}, {"rss", rss}, {"pss", pss}, {"swap", swap}};

    // Free the dynamically allocated space
    delete it.second;
  }
}

void PerfMonMTSvc::report2JsonFile_EventLevel_Mem_Parallel(nlohmann::json& j) {
  // Report event level memory measurements
  for (const auto& it : m_eventLevelData.getEventLevelData()) {
    std::string checkPoint = std::to_string(it.first);

    long vmem = it.second.mem_stats.at("vmem");
    long rss = it.second.mem_stats.at("rss");
    long pss = it.second.mem_stats.at("pss");
    long swap = it.second.mem_stats.at("swap");

    j["MemMon_Parallel"][checkPoint] = {{"vmem", vmem}, {"rss", rss}, {"pss", pss}, {"swap", swap}};
  }
}

bool PerfMonMTSvc::isLoop() const {
  EventIDBase::event_number_t eventID = getEventID();
  return (eventID == std::numeric_limits<EventIDBase::event_number_t>::max()) ? false : true;
}

/* If this function is invoked outside the event loop, it returns
 * std::numeric_limits<EventIDBase::event_number_t>::max() This is how we detect whether we are in the event loop or not
 */
EventIDBase::event_number_t PerfMonMTSvc::getEventID() const {
  auto ctx = Gaudi::Hive::currentContext();
  EventIDBase::event_number_t eventID = ctx.eventID().event_number();
  return eventID;
}

PMonMT::StepComp PerfMonMTSvc::generate_serial_state(const std::string& stepName, const std::string& compName) const {
  PMonMT::StepComp currentState;
  currentState.stepName = stepName;
  currentState.compName = compName;
  return currentState;
}

PMonMT::StepCompEvent PerfMonMTSvc::generate_parallel_state(const std::string& stepName, const std::string& compName,
                                                            const uint64_t& eventNumber) const {
  PMonMT::StepCompEvent currentState;
  currentState.stepName = stepName;
  currentState.compName = compName;
  currentState.eventNumber = eventNumber;
  return currentState;
}

void PerfMonMTSvc::parallelDataAggregator() {
  std::map<PMonMT::StepComp, PMonMT::Measurement>::iterator sc_itr;

  for (auto& sce_itr : m_parallelCompLevelData.m_compLevel_delta_map) {
    PMonMT::StepComp currentState = generate_serial_state(sce_itr.first.stepName, sce_itr.first.compName);

    // If the current state exists in the map, then aggregate it. o/w create a instance for it.
    sc_itr = m_aggParallelCompLevelDataMap.find(currentState);
    if (sc_itr != m_aggParallelCompLevelDataMap.end()) {
      m_aggParallelCompLevelDataMap[currentState].call_count++;

      m_aggParallelCompLevelDataMap[currentState].cpu_time += sce_itr.second.cpu_time;
      m_aggParallelCompLevelDataMap[currentState].wall_time += sce_itr.second.wall_time;

      m_aggParallelCompLevelDataMap[currentState].mem_stats["vmem"] += sce_itr.second.mem_stats["vmem"];
      m_aggParallelCompLevelDataMap[currentState].mem_stats["rss"] += sce_itr.second.mem_stats["rss"];
      m_aggParallelCompLevelDataMap[currentState].mem_stats["pss"] += sce_itr.second.mem_stats["pss"];
      m_aggParallelCompLevelDataMap[currentState].mem_stats["swap"] += sce_itr.second.mem_stats["swap"];
    } else {
      m_aggParallelCompLevelDataMap[currentState] = sce_itr.second;
      m_aggParallelCompLevelDataMap[currentState].call_count = 1;  // this component in this step is called once so far.
    }
  }
}

void PerfMonMTSvc::divideData2Steps_serial() {
  for (auto it : m_compLevelDataMap) {
    if (it.first.stepName == "Initialize")
      m_compLevelDataMap_ini[it.first] = it.second;
    else if (it.first.stepName == "Finalize")
      m_compLevelDataMap_fin[it.first] = it.second;
    else if (it.first.stepName == "preLoadProxy")
      m_compLevelDataMap_plp[it.first] = it.second;
    else if (it.first.stepName == "Callback")
      m_compLevelDataMap_cbk[it.first] = it.second;
  }
  m_stdoutVec_serial.push_back(m_compLevelDataMap_ini);
  m_stdoutVec_serial.push_back(m_compLevelDataMap_fin);
  m_stdoutVec_serial.push_back(m_compLevelDataMap_plp);
  m_stdoutVec_serial.push_back(m_compLevelDataMap_cbk);
}

void PerfMonMTSvc::divideData2Steps_parallel() {
  for (auto it : m_aggParallelCompLevelDataMap) {
    if (it.first.stepName == "Execute")
      m_aggParallelCompLevelDataMap_evt[it.first] = it.second;
    else if (it.first.stepName == "preLoadProxy")
      m_aggParallelCompLevelDataMap_plp[it.first] = it.second;
    else if (it.first.stepName == "Callback")
      m_aggParallelCompLevelDataMap_cbk[it.first] = it.second;
  }

  m_stdoutVec_parallel.push_back(m_aggParallelCompLevelDataMap_evt);
  m_stdoutVec_parallel.push_back(m_aggParallelCompLevelDataMap_plp);
  m_stdoutVec_parallel.push_back(m_aggParallelCompLevelDataMap_cbk);
}

std::string PerfMonMTSvc::scaleTime(double timeMeas) const {
  std::ostringstream ss;
  ss << std::fixed;
  ss << std::setprecision(2);

  double result = 0;

  std::string significance[5] = {"ms", "sec", "mins", "hours", "days"};
  int scaleFactor = 0;

  if (timeMeas > 1000 * 60 * 60 * 24) {
    int dayCount = timeMeas / (1000 * 60 * 60 * 24);
    timeMeas = std::fmod(timeMeas, (1000 * 60 * 60 * 24));
    result += dayCount;
    scaleFactor++;
  }
  if (timeMeas > 1000 * 60 * 60) {
    int hourCount = timeMeas / (1000 * 60 * 60);
    timeMeas = std::fmod(timeMeas, (1000 * 60 * 60));
    result += hourCount * 1.e-3;
    scaleFactor++;
  }
  if (timeMeas > 1000 * 60) {
    int minCount = timeMeas / (1000 * 60);
    timeMeas = std::fmod(timeMeas, (1000 * 60));
    result += minCount * 1.e-6;
    scaleFactor++;
  }
  if (timeMeas > 1000) {
    int secCount = timeMeas / 1000;
    timeMeas = std::fmod(timeMeas, 1000);
    result += secCount * 1.e-9;
    scaleFactor++;
  }
  if (timeMeas >= 0) {
    result += timeMeas * 1.e-12;
    scaleFactor++;
  }
  result = result * std::pow(1000, (5 - scaleFactor));

  ss << result;
  std::string stringObj = ss.str() + " " + significance[scaleFactor - 1];

  return stringObj;
}

std::string PerfMonMTSvc::scaleMem(long memMeas) const {
  std::ostringstream ss;
  ss << std::fixed;
  ss << std::setprecision(2);

  double result = 0;

  std::string significance[4] = {"KB", "MB", "GB", "TB"};
  int scaleFactor = 0;

  if (memMeas > 1024 * 1024 * 1024) {
    int teraCount = memMeas / (1024 * 1024 * 1024);
    memMeas = memMeas % (1024 * 1024 * 1024);
    result += teraCount;
    scaleFactor++;
  }
  if (memMeas > 1024 * 1024) {
    int gigaCount = memMeas / (1024 * 1024);
    memMeas = memMeas % (1024 * 1024);
    result += gigaCount * (1.0 / 1024);
    scaleFactor++;
  }
  if (memMeas > 1024) {
    int megaCount = memMeas / (1024);
    memMeas = memMeas % (1024);
    result += megaCount * (1.0 / (1024 * 1024));
    scaleFactor++;
  }
  if (memMeas >= 0) {
    result += memMeas * (1.0 / (1024 * 1024 * 1024));
    scaleFactor++;
  }

  result = result * std::pow(1024, (4 - scaleFactor));

  ss << result;
  std::string stringObj = ss.str() + " " + significance[scaleFactor - 1];

  return stringObj;
}

std::string PerfMonMTSvc::get_cpu_model_info() const {
  std::string cpu_model;

  std::ifstream file("/proc/cpuinfo");
  std::string line;
  if (file.is_open()) {
    std::string delimiter = ":";
    while (getline(file, line)) {
      std::string key = line.substr(0, line.find(delimiter));
      if (key == "model name	") {
        cpu_model = line.substr(line.find(delimiter) + 1, line.length());
        break;
      }
    }
    file.close();
    return cpu_model;
  } else {
    return "Unable to open /proc/cpuinfo";
  }
}

int PerfMonMTSvc::get_cpu_core_info() const {
  int logical_core_num = 0;

  std::ifstream file("/proc/cpuinfo");
  std::string line;
  if (file.is_open()) {
    std::string delimiter = ":";
    while (getline(file, line)) {
      std::string key = line.substr(0, line.find(delimiter));
      if (key == "processor	") {
        logical_core_num++;
      }
    }
    file.close();
    return logical_core_num;
  } else {
    return -1;
  }
}

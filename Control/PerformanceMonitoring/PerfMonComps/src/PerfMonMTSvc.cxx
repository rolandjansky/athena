/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*
 * @authors: Alaettin Serhan Mete, Hasan Ozturk - alaettin.serhan.mete@cern.ch, haozturk@cern.ch
 */

// Framework includes
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ThreadLocalContext.h"

// PerfMonComps includes
#include "PerfMonMTSvc.h"
#include "PerfMonUtils.h"  // borrow from existing code

// STD includes
#include <algorithm>

/*
 * Constructor
 */
PerfMonMTSvc::PerfMonMTSvc(const std::string& name, ISvcLocator* pSvcLocator)
    : AthService(name, pSvcLocator), m_eventCounter{0}, m_eventLoopMsgCounter{0} {
  // Four main snapshots : Configure, Initialize, Execute, and Finalize
  m_snapshotData.resize(NSNAPSHOTS); // Default construct

  // Initial capture upon construction
  m_measurement_snapshots.capture_snapshot();
  m_snapshotData[CONFIGURE].addPointStop_snapshot(m_measurement_snapshots);
  m_snapshotData[INITIALIZE].addPointStart_snapshot(m_measurement_snapshots);
}

/*
 * Destructor
 */
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

  // Set to be listener to SvcPostFinalize
  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc/IncidentSvc", name());
  ATH_CHECK(incSvc.retrieve());
  incSvc->addListener(this, IncidentType::SvcPostFinalize);

  // Check if /proc exists, if not memory statistics are not available
  const bool procExists = PMonMT::doesDirectoryExist("/proc");
  if(!procExists) {
    ATH_MSG_INFO("The system doesn't support /proc. Therefore, memory measurements are not available");
  }

  // Print some information minimal information about our configuration
  ATH_MSG_INFO("Service is configured for [" << m_numberOfThreads.toString() << "] threads " <<
               "analyzing [" << m_numberOfSlots.toString() << "] events concurrently");
  ATH_MSG_INFO("Component-level measurements are [" << (m_doComponentLevelMonitoring ? "Enabled" : "Disabled") << "]");
  if (m_numberOfThreads > 1 && m_doComponentLevelMonitoring) {
    ATH_MSG_INFO("  >> Component-level memory monitoring in the event-loop is disabled in jobs with more than 1 thread");
  }

  // Slot specific component-level data map
  m_compLevelDataMapVec.resize(m_numberOfSlots); // Default construct

  // Set wall time offset
  m_eventLevelData.set_wall_time_offset(m_wallTimeOffset);
  if (m_wallTimeOffset > 0) {
    m_snapshotData[CONFIGURE].add2DeltaWall(-m_wallTimeOffset);
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

  return StatusCode::SUCCESS;
}

/*
 * Capture finalizations and report in SvcPostFinalize
 */
void PerfMonMTSvc::handle(const Incident& inc) {
  if (inc.type() == IncidentType::SvcPostFinalize) {
    // Final capture upon post-finalization
    m_measurement_snapshots.capture_snapshot();
    m_snapshotData[FINALIZE].addPointStop_snapshot(m_measurement_snapshots);

    // Report everything
    report();
  }
  return;
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

    // Start component auditing
    auto ctx = Gaudi::Hive::currentContext(); 
    startCompAud(stepName, compName, ctx);
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

    // Stop component auditing
    auto ctx = Gaudi::Hive::currentContext(); 
    stopCompAud(stepName, compName, ctx);
  }
}

/*
 * Start Snapshot Auditing
 */
void PerfMonMTSvc::startSnapshotAud(const std::string& stepName, const std::string& compName) {
  // Last thing to be called before the event loop begins
  if (compName == "AthOutSeq" && stepName == "Start") {
    m_measurement_snapshots.capture_snapshot();
    m_snapshotData[EXECUTE].addPointStart_snapshot(m_measurement_snapshots);
  }

  // Last thing to be called before finalize step begins
  if (compName == "AthMasterSeq" && stepName == "Finalize") {
    m_measurement_snapshots.capture_snapshot();
    m_snapshotData[FINALIZE].addPointStart_snapshot(m_measurement_snapshots);
  }
}

/*
 * Stop Snapshot Auditing
 */
void PerfMonMTSvc::stopSnapshotAud(const std::string& stepName, const std::string& compName) {
  // First thing to be called after the initialize step ends
  if (compName == "AthMasterSeq" && stepName == "Initialize") {
    m_measurement_snapshots.capture_snapshot();
    m_snapshotData[INITIALIZE].addPointStop_snapshot(m_measurement_snapshots);
  }

  // First thing to be called after the event loop ends
  if (compName == "AthMasterSeq" && stepName == "Stop") {
    m_measurement_snapshots.capture_snapshot();
    m_snapshotData[EXECUTE].addPointStop_snapshot(m_measurement_snapshots);
  }
}

/*
 * Start Component Auditing
 */
void PerfMonMTSvc::startCompAud(const std::string& stepName, const std::string& compName, const EventContext& ctx) {
  // Lock for data integrity
  std::lock_guard<std::mutex> lock(m_mutex_capture);

  // Memory measurement is only done outside the loop except when there is only a single thread
  const bool doMem = !ctx.valid() || (m_numberOfThreads == 1);

  // Generate State
  PMonMT::StepComp currentState = generate_state(stepName, compName);

  // Check if this is the first time calling if so create the mesurement data if not use the existing one. 
  // Metrics are collected per slot then aggregated before reporting 
  data_map_t& compLevelDataMap = m_compLevelDataMapVec[ctx.valid() ? ctx.slot() : 0];
  if(compLevelDataMap.find(currentState) == compLevelDataMap.end()) {
    compLevelDataMap[currentState] = new PMonMT::MeasurementData();
  }

  // Capture and store
  PMonMT::Measurement meas;
  meas.capture_component(doMem);
  compLevelDataMap[currentState]->addPointStart_component(meas, doMem);

  // Debug
  ATH_MSG_DEBUG("Start Audit: ctx " << ctx.valid() << " evt " << ctx.evt() << " slot " << ctx.slot() <<
                " component " << compName << " step " << stepName);
  ATH_MSG_DEBUG("Start CPU " << meas.cpu_time << " VMem " << meas.vmem << " Malloc " << meas.malloc);
}

/*
 * Stop Component Auditing
 */
void PerfMonMTSvc::stopCompAud(const std::string& stepName, const std::string& compName, const EventContext& ctx) {
  // Lock for data integrity
  std::lock_guard<std::mutex> lock(m_mutex_capture);

  // Memory measurement is only done outside the loop except when there is only a single thread
  const bool doMem = !ctx.valid() || (m_numberOfThreads == 1);

  // Capture
  PMonMT::Measurement meas;
  meas.capture_component(doMem); // No memory in the event-loop

  // Generate State
  PMonMT::StepComp currentState = generate_state(stepName, compName);

  // Store
  data_map_t& compLevelDataMap = m_compLevelDataMapVec[ctx.valid() ? ctx.slot() : 0];
  compLevelDataMap[currentState]->addPointStop_component(meas, doMem);

  // Debug
  ATH_MSG_DEBUG("Stop Audit: ctx " << ctx.valid() << " evt " << ctx.evt() << " slot " << ctx.slot() <<
                " component " << compName << " step " << stepName);
  ATH_MSG_DEBUG("Stop CPU " << meas.cpu_time << " VMem " << meas.vmem << " Malloc " << meas.malloc);
  ATH_MSG_DEBUG("  >> Start CPU " << compLevelDataMap[currentState]->m_tmp_cpu << " VMem "
                                 << compLevelDataMap[currentState]->m_tmp_vmem << " Malloc "
                                 << compLevelDataMap[currentState]->m_tmp_malloc);
  ATH_MSG_DEBUG("  >> CSum CPU  " << compLevelDataMap[currentState]->m_delta_cpu << " VMem "
                                 << compLevelDataMap[currentState]->m_delta_vmem << " Malloc "
                                 << compLevelDataMap[currentState]->m_delta_malloc);
}

/*
 * Event-level Monitoring
 */
void PerfMonMTSvc::eventLevelMon() {
  // Lock for data integrity
  std::lock_guard<std::mutex> lock(m_mutex_capture);

  // Increment the internal counter
  incrementEventCounter();

  // Monitor
  if (m_doEventLoopMonitoring && isCheckPoint()) {
    // Capture
    m_measurement_events.capture_event();
    m_eventLevelData.record_event(m_measurement_events, m_eventCounter);
    // Report instantly - no more than m_eventLoopMsgLimit times
    if(m_eventLoopMsgCounter < m_eventLoopMsgLimit) {
      report2Log_EventLevel_instant();
      m_eventLoopMsgCounter++;
    }
  }
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
  // Always check 1, 10, 25 for short tests
  if (m_eventCounter == 1 || m_eventCounter == 10 || m_eventCounter == 25)
    return true;

  // Check the user settings
  if (m_checkPointType == "Arithmetic")
    return (m_eventCounter % m_checkPointFactor == 0);
  else if (m_checkPointType == "Geometric")
    return isPower(m_eventCounter, m_checkPointFactor);
  else
    return false;
}

/*
 * Helper function for geometric printing
 */
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

  // Component-level
  if (m_printDetailedTables && m_doComponentLevelMonitoring) {
    report2Log_ComponentLevel();
  }

  // Event-level
  if (m_printDetailedTables && m_doEventLoopMonitoring) {
    report2Log_EventLevel();
  }

  // Summary and system information
  report2Log_Summary();
  report2Log_CpuInfo();
}

/*
 * Report header to log
 */
void PerfMonMTSvc::report2Log_Description() const {
  ATH_MSG_INFO("=======================================================================================");
  ATH_MSG_INFO("                                 PerfMonMTSvc Report                                   ");
  ATH_MSG_INFO("=======================================================================================");
  ATH_MSG_INFO("IMPORTANT : PLEASE NOTE THAT THIS SERVICE IS CURRENTLY IN R&D PHASE.");
  ATH_MSG_INFO("            FOR FURTHER INFORMATION/QUERIES PLEASE GET IN TOUCH WITH THE SPOT TEAM.");
  ATH_MSG_INFO("=======================================================================================");
  if (m_reportResultsToJSON) {
    ATH_MSG_INFO("*** Full set of information can also be found in: " << m_jsonFileName.toString());
    ATH_MSG_INFO("*** In order to make plots using the results run the following commands:");
    ATH_MSG_INFO("*** $ perfmonmt-plotter.py -i " << m_jsonFileName.toString());
    ATH_MSG_INFO("*** In order to print tables using the results run the following commands:");
    ATH_MSG_INFO("*** $ perfmonmt-printer.py -i " << m_jsonFileName.toString());
    ATH_MSG_INFO("=======================================================================================");
  }
}

/*
 * Report component-level information to log
 */
void PerfMonMTSvc::report2Log_ComponentLevel() {
  using boost::format;

  ATH_MSG_INFO("=======================================================================================");
  ATH_MSG_INFO("                             Component Level Monitoring                                ");
  ATH_MSG_INFO("=======================================================================================");

  ATH_MSG_INFO(format("%1% %|15t|%2% %|25t|%3% %|40t|%4% %|55t|%5% %|75t|%6%") % "Step" % "Count" % "CPU Time [ms]" % 
               "Vmem [kB]" % "Malloc [kB]" % "Component");

  ATH_MSG_INFO("---------------------------------------------------------------------------------------");

  aggregateSlotData(); // aggregate data from slots
  divideData2Steps(); // divive data into steps for ordered printing

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
      if (counter >= m_printNComps) {
        break;
      }
      counter++;

      ATH_MSG_INFO(format("%1% %|15t|%2% %|25t|%3$.2f %|40t|%4$.0f %|55t|%5$.0f %|75t|%6%") % it.first.stepName %
                   it.second->getCallCount() % it.second->getDeltaCPU() % it.second->getDeltaVmem() %
                   it.second->getDeltaMalloc() % it.first.compName);
    }
    if(counter>0) { 
      ATH_MSG_INFO("=======================================================================================");
    }
  }
}

/*
 * Report event-level information to log as we capture it
 */
void PerfMonMTSvc::report2Log_EventLevel_instant() const {
  double cpu_time = m_eventLevelData.getEventLevelCpuTime(m_eventCounter);
  double wall_time = m_eventLevelData.getEventLevelWallTime(m_eventCounter);

  long vmem = m_eventLevelData.getEventLevelVmem(m_eventCounter);
  long rss = m_eventLevelData.getEventLevelRss(m_eventCounter);
  long pss = m_eventLevelData.getEventLevelPss(m_eventCounter);
  long swap = m_eventLevelData.getEventLevelSwap(m_eventCounter);

  ATH_MSG_INFO("Event [" << std::setw(5) << m_eventCounter << "] CPU Time: " << scaleTime(cpu_time) << 
               ", Wall Time: " <<  scaleTime(wall_time) << ", Vmem: " << scaleMem(vmem) << 
               ", Rss: " << scaleMem(rss) << ", Pss: " << scaleMem(pss) << ", Swap: " << scaleMem(swap));
}

/*
 * Report event-level information to log
 */
void PerfMonMTSvc::report2Log_EventLevel() {
  using boost::format;

  ATH_MSG_INFO("                                Event Level Monitoring                                 ");
  ATH_MSG_INFO("                 (Only first " << m_eventLoopMsgLimit.toString() <<
               " measurements are explicitly printed)");
  ATH_MSG_INFO("=======================================================================================");

  ATH_MSG_INFO(format("%1% %|16t|%2% %|28t|%3% %|40t|%4% %|52t|%5% %|64t|%6% %|76t|%7%") % "Event" % "CPU [s]" %
               "Wall [s]" % "Vmem [kB]" % "Rss [kB]" % "Pss [kB]" % "Swap [kB]");

  ATH_MSG_INFO("---------------------------------------------------------------------------------------");

  m_eventLoopMsgCounter = 0; // reset counter

  for (const auto& it : m_eventLevelData.getEventLevelData()) {
    if(m_eventLoopMsgCounter < m_eventLoopMsgLimit) {
      ATH_MSG_INFO(format("%1% %|16t|%2$.2f %|28t|%3$.2f %|40t|%4% %|52t|%5% %|64t|%6% %|76t|%7%") % it.first %
                   (it.second.cpu_time * 0.001) % (it.second.wall_time * 0.001) % it.second.mem_stats.at("vmem") %
                   it.second.mem_stats.at("rss") % it.second.mem_stats.at("pss") % it.second.mem_stats.at("swap"));
      m_eventLoopMsgCounter++;
    }
    // Add to leak estimate
    if (it.first >= 25) {
      m_fit_vmem.addPoint(it.first, it.second.mem_stats.at("vmem"));
      m_fit_pss.addPoint(it.first, it.second.mem_stats.at("pss"));
    }
  }
  ATH_MSG_INFO("=======================================================================================");
}

/*
 * Report summary information to log
 */
void PerfMonMTSvc::report2Log_Summary() {
  using boost::format;

  ATH_MSG_INFO("                                 Snaphots Summary                                      ");
  ATH_MSG_INFO("=======================================================================================");

  ATH_MSG_INFO(format("%1% %|13t|%2% %|25t|%3% %|37t|%4% %|44t|%5% %|55t|%6% %|66t|%7% %|77t|%8%") % "Step" %
               "dCPU [s]" % "dWall [s]" % "<CPU>" % "dVmem [kB]" % "dRss [kB]" % "dPss [kB]" % "dSwap [kB]");

  ATH_MSG_INFO("---------------------------------------------------------------------------------------");

  for (unsigned int idx = 0; idx < NSNAPSHOTS; idx++) {
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
               (m_snapshotData[EXECUTE].getDeltaCPU() / m_eventCounter));
  ATH_MSG_INFO(format("%1% %|35t|%2$.3f ") % "Events per second:" %
               (m_eventCounter / m_snapshotData[EXECUTE].getDeltaWall() * 1000.));

  if (m_doEventLoopMonitoring) {
    ATH_MSG_INFO("***************************************************************************************");
    ATH_MSG_INFO(format("%1% %|35t|%2% ") % "Max Vmem: " % scaleMem(m_measurement_events.vmemPeak));
    ATH_MSG_INFO(format("%1% %|35t|%2% ") % "Max Rss: " % scaleMem(m_measurement_events.rssPeak));
    ATH_MSG_INFO(format("%1% %|35t|%2% ") % "Max Pss: " % scaleMem(m_measurement_events.pssPeak));
    ATH_MSG_INFO("***************************************************************************************");
    ATH_MSG_INFO(format("%1% %|35t|%2% ") % "Leak estimate per event Vmem: " % scaleMem(m_fit_vmem.slope()));
    ATH_MSG_INFO(format("%1% %|35t|%2% ") % "Leak estimate per event Pss: " % scaleMem(m_fit_pss.slope()));
    ATH_MSG_INFO("  >> Estimated using the last " << m_fit_vmem.nPoints()
                                                  << " measurements from the Event Level Monitoring");
    ATH_MSG_INFO("  >> Events prior to the first 25 are omitted...");
  }

  ATH_MSG_INFO("=======================================================================================");
}

/*
 * Report CPU information to log
 */
void PerfMonMTSvc::report2Log_CpuInfo() const {
  using boost::format;

  ATH_MSG_INFO("                                  System Information                                   ");
  ATH_MSG_INFO("=======================================================================================");

  ATH_MSG_INFO(format("%1% %|34t|%2% ") % "CPU Model:" % get_cpu_model_info());
  ATH_MSG_INFO(format("%1% %|35t|%2% ") % "Number of Available Cores:" % get_cpu_core_info());

  ATH_MSG_INFO("=======================================================================================");
}

/*
 * Report data to JSON
 */
void PerfMonMTSvc::report2JsonFile() {
  nlohmann::json j;

  // CPU and Wall-time
  report2JsonFile_Summary(j);  // Snapshots

  // Memory
  if (m_doComponentLevelMonitoring) {
    report2JsonFile_ComponentLevel(j);  // Component-level
  }
  if (m_doEventLoopMonitoring) {
    report2JsonFile_EventLevel(j);  // Event-level
  }

  // Write and close the JSON file
  std::ofstream o(m_jsonFileName);
  o << std::setw(4) << j << std::endl;
  o.close();

  // Compress the JSON file into tar.gz
  auto cmd = "tar -czf " + m_jsonFileName + ".tar.gz " + m_jsonFileName + ";";
  int rc = std::system(cmd.c_str());
  if(rc!=0) {
    ATH_MSG_WARNING("Couldn't compress the JSON file...");
    return;
  }

  // Remove the uncompressed JSON file to save disk-space
  rc = std::remove(m_jsonFileName.toString().c_str());
  if(rc!=0) {
    ATH_MSG_WARNING("Couldn't remove the uncompressed JSON file...");
    return;
  }
}

/*
 * Report summary data to JSON
 */
void PerfMonMTSvc::report2JsonFile_Summary(nlohmann::json& j) const {
  
  // Report snapshot level results
  for(int i=0; i < NSNAPSHOTS; i++){

    const std::string step = m_snapshotStepNames[i];
    const double dCPU = m_snapshotData[i].getDeltaCPU();
    const double dWall = m_snapshotData[i].getDeltaWall();
    const double cpuUtil = dCPU / dWall;
    const long dVmem = m_snapshotData[i].getMemMonDeltaMap("vmem");
    const long dRss = m_snapshotData[i].getMemMonDeltaMap("rss");
    const long dPss = m_snapshotData[i].getMemMonDeltaMap("pss");
    const long dSwap = m_snapshotData[i].getMemMonDeltaMap("swap");

    j["summary"]["snapshotLevel"][step] = {{"dCPU", dCPU},
                                           {"dWall", dWall},
                                           {"cpuUtil", cpuUtil},
                                           {"dVmem", dVmem},
                                           {"dRss", dRss},
                                           {"dPss", dPss},
                                           {"dSwap", dSwap}};

  }

  // Report the total number of events
  const long nEvents = m_eventCounter;
  j["summary"]["nEvents"] = nEvents;

  // Report Peaks
  const long vmemPeak = m_measurement_events.vmemPeak;
  const long rssPeak = m_measurement_events.rssPeak;
  const long pssPeak = m_measurement_events.pssPeak;

  j["summary"]["peaks"] = {{"vmemPeak", vmemPeak},
                           {"rssPeak", rssPeak},
                           {"pssPeak", pssPeak}};

  // Report leak estimates
  const long vmemLeak = m_fit_vmem.slope();
  const long pssLeak = m_fit_pss.slope();
  const long nPoints = m_fit_vmem.nPoints();

  j["summary"]["leakEstimates"] = {{"vmemLeak", vmemLeak},
                                   {"pssLeak", pssLeak},
                                   {"nPoints", nPoints}};

  // Report Sys info
  const std::string cpuModel = get_cpu_model_info();
  const int coreNum = get_cpu_core_info();

  j["summary"]["sysInfo"] = {{"cpuModel", cpuModel},
                             {"coreNum", coreNum}};
}

void PerfMonMTSvc::report2JsonFile_ComponentLevel(nlohmann::json& j) const {

  for (const auto& dataMapPerStep : m_stdoutVec_serial) {
    
    for(const auto& meas : dataMapPerStep){

      const std::string step = meas.first.stepName;
      const std::string component = meas.first.compName; 
      const uint64_t count = meas.second->getCallCount();
      const double cpuTime = meas.second->getDeltaCPU();
      const long vmem  = meas.second->getDeltaVmem(); 
      const int mall = meas.second->getDeltaMalloc();

      j["componentLevel"][step][component] = {{"count", count},
                                              {"cpuTime", cpuTime},
                                              {"vmem", vmem},
                                              {"malloc", mall}};
    }    

  }

}

void PerfMonMTSvc::report2JsonFile_EventLevel(nlohmann::json& j) const {

  for (const auto& it : m_eventLevelData.getEventLevelData()) {
    
    const uint64_t event = it.first;
    const double cpuTime = it.second.cpu_time;
    const double wallTime = it.second.wall_time;
    const long vmem = it.second.mem_stats.at("vmem");
    const long rss = it.second.mem_stats.at("rss");
    const long pss = it.second.mem_stats.at("pss");
    const long swap = it.second.mem_stats.at("swap");

    j["eventLevel"][std::to_string(event)] = {{"cpuTime", cpuTime},
                                              {"wallTime", wallTime},
                                              {"vmem", vmem},
                                              {"rss", rss},
                                              {"pss", pss},
                                              {"swap", swap}};


  }
}

/*
 * Generate a "state" that is use as a key for the component-level data
 */
PMonMT::StepComp PerfMonMTSvc::generate_state(const std::string& stepName, const std::string& compName) const {
  PMonMT::StepComp currentState;
  currentState.stepName = stepName;
  currentState.compName = compName;
  return currentState;
}

/*
 * Aggregate component-level data from all slots
 */
void PerfMonMTSvc::aggregateSlotData() {
  // Loop over data from all slots
  for (const auto& slotData : m_compLevelDataMapVec) {
    for (const auto& it : slotData) {
      // Copy the first slot data and sum the rest
      if(m_compLevelDataMap.find(it.first) == m_compLevelDataMap.end()) {
        m_compLevelDataMap[it.first] = it.second;
      } else {
        m_compLevelDataMap[it.first]->add2CallCount(it.second->getCallCount());
        m_compLevelDataMap[it.first]->add2DeltaCPU(it.second->getDeltaCPU());
        m_compLevelDataMap[it.first]->add2DeltaVmem(it.second->getDeltaVmem());
        m_compLevelDataMap[it.first]->add2DeltaMalloc(it.second->getDeltaMalloc());
      }
    }
  }
}

/*
 * Divide component-level data into steps, for printing
 */
void PerfMonMTSvc::divideData2Steps() {
  for (auto it : m_compLevelDataMap) {
    if (it.first.stepName == "Initialize")
      m_compLevelDataMap_ini[it.first] = it.second;
    else if (it.first.stepName == "Execute")
      m_compLevelDataMap_evt[it.first] = it.second;
    else if (it.first.stepName == "Finalize")
      m_compLevelDataMap_fin[it.first] = it.second;
    else if (it.first.stepName == "preLoadProxy")
      m_compLevelDataMap_plp[it.first] = it.second;
    else if (it.first.stepName == "Callback")
      m_compLevelDataMap_cbk[it.first] = it.second;
  }
  m_stdoutVec_serial.push_back(m_compLevelDataMap_ini);
  m_stdoutVec_serial.push_back(m_compLevelDataMap_evt);
  m_stdoutVec_serial.push_back(m_compLevelDataMap_fin);
  m_stdoutVec_serial.push_back(m_compLevelDataMap_plp);
  m_stdoutVec_serial.push_back(m_compLevelDataMap_cbk);
}

std::string PerfMonMTSvc::scaleTime(double timeMeas) const {
  // Not a huge fan of this, we should eventually unify the types
  // Just want to be explicit about what's happening
  long ms = (long) timeMeas; 

  // Compute hrs and offset
  auto hrs = ms / 3600000;
  ms -= hrs * 3600000;
  // Compute mins and offset
  auto mins = ms / 60000;
  ms -= mins * 60000;
  // Compute secs and offset
  auto secs = ms / 1000;
  ms -= secs * 1000;

  // Primarily care about H:M:S
  std::stringstream ss;
  ss.fill('0');
  ss << std::setw(2) << hrs << "h" << 
        std::setw(2) << mins << "m" << 
        std::setw(2) << secs << "s";
  return ss.str();
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

/*
 * Collect some hardware information
 */
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

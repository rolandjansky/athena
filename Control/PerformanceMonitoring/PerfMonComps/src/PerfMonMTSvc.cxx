/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/*
 * @authors: Alaettin Serhan Mete, Hasan Ozturk - alaettin.serhan.mete@cern.ch, haozturk@cern.ch
 */

// Thread-safety-checker
#include "CxxUtils/checker_macros.h"
ATLAS_CHECK_FILE_THREAD_SAFETY;

// Framework includes
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ThreadLocalContext.h"

// PerfMonComps includes
#include "PerfMonMTSvc.h"
#include "PerfMonUtils.h"  // borrow from existing code
#include "SemiDetMisc.h"   // borrow from existing code

// STD includes
#include <algorithm>

// Boost includes
#include "boost/filesystem.hpp"
#include "boost/format.hpp"

// TBB
#include "tbb/task_arena.h"

/*
 * Constructor
 */
PerfMonMTSvc::PerfMonMTSvc(const std::string& name, ISvcLocator* pSvcLocator)
    : AthService(name, pSvcLocator), m_isFirstEvent{false}, m_eventCounter{0}, m_eventLoopMsgCounter{0} {
  // Five main snapshots : Configure, Initialize, FirstEvent, Execute, and Finalize
  m_snapshotData.resize(NSNAPSHOTS); // Default construct

  // Initial capture upon construction
  m_measurementSnapshots.capture();
  m_snapshotData[CONFIGURE].addPointStop(m_measurementSnapshots);
  m_snapshotData[INITIALIZE].addPointStart(m_measurementSnapshots);
}

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
  const long highestPriority = static_cast<long>(-1);
  const long lowestPriority = 0;
  incSvc->addListener(this, IncidentType::BeginEvent, highestPriority);
  incSvc->addListener(this, "EndAlgorithms", lowestPriority);
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

  // Thread specific component-level data map
  m_compLevelDataMapVec.resize(m_numberOfThreads+1); // Default construct

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
 * Handle relevant incidents
 */
void PerfMonMTSvc::handle(const Incident& inc) {
  // Begin event processing
  if (inc.type() == IncidentType::BeginEvent) {
    // Lock for data integrity
    std::lock_guard<std::mutex> lock(m_mutex_capture);

    // Increment the internal counter
    m_eventCounter++;

    // Monitor
    if (m_doEventLoopMonitoring && isCheckPoint()) {
      // Capture
      m_measurementEvents.capture();
      m_eventLevelData.recordEvent(m_measurementEvents, m_eventCounter);
      // Report instantly - no more than m_eventLoopMsgLimit times
      if(m_eventLoopMsgCounter < m_eventLoopMsgLimit) {
        report2Log_EventLevel_instant();
        m_eventLoopMsgCounter++;
      }
    }
  }
  // End event processing (as signaled by SG clean-up)
  // By convention the first event is executed serially
  // Therefore, we treat it a little bit differently
  else if (m_eventCounter == 1 && inc.type() == "EndAlgorithms") {
    m_measurementSnapshots.capture();
    m_snapshotData[FIRSTEVENT].addPointStop(m_measurementSnapshots);
    m_snapshotData[EXECUTE].addPointStart(m_measurementSnapshots);
    // Normally this flag is set in stopCompAud but we don't
    // go in there unless m_doComponentLevelMonitoring is true.
    // If it's false, we toggle it here but
    // this is mostly for completeness since in that mode
    // this flag is not really used at the moment.
    if(!m_doComponentLevelMonitoring) {
      m_isFirstEvent = false;
    }
  }
  // Finalize ourself and print the metrics in SvcPostFinalize
  else if (inc.type() == IncidentType::SvcPostFinalize) {
    // Final capture upon post-finalization
    m_measurementSnapshots.capture();
    m_snapshotData[FINALIZE].addPointStop(m_measurementSnapshots);

    // Report everything
    report();
  }
  return;
}
/*
 * Start Auditing
 */
void PerfMonMTSvc::startAud(const std::string& stepName, const std::string& compName) {
  // Snapshots, i.e. Initialize, Event Loop, etc.
  startSnapshotAud(stepName, compName);

  /*
   * Perform component monitoring only if the user asked for it.
   * By default we don't monitor a set of common components.
   * Once we adopt C++20, we can switch this from count to contains.
   */
  if (m_doComponentLevelMonitoring && !m_exclusionSet.count(compName)) {
    // Start component auditing
    auto const &ctx = Gaudi::Hive::currentContext();
    startCompAud(stepName, compName, ctx);
  }
}

/*
 * Stop Auditing
 */
void PerfMonMTSvc::stopAud(const std::string& stepName, const std::string& compName) {
  // Snapshots, i.e. Initialize, Event Loop, etc.
  stopSnapshotAud(stepName, compName);

  // Check if we should monitor this component
  if (m_doComponentLevelMonitoring && !m_exclusionSet.count(compName)) {
    // Stop component auditing
    auto const &ctx = Gaudi::Hive::currentContext();
    stopCompAud(stepName, compName, ctx);
  }
}

/*
 * Start Snapshot Auditing
 */
void PerfMonMTSvc::startSnapshotAud(const std::string& stepName, const std::string& compName) {
  // Last thing to be called before the event loop begins
  if (compName == "AthOutSeq" && stepName == "Start") {
    m_measurementSnapshots.capture();
    m_snapshotData[FIRSTEVENT].addPointStart(m_measurementSnapshots);
    m_isFirstEvent = true;
  }

  // Last thing to be called before finalize step begins
  if (compName == "AthMasterSeq" && stepName == "Finalize") {
    m_measurementSnapshots.capture();
    m_snapshotData[FINALIZE].addPointStart(m_measurementSnapshots);
  }
}

/*
 * Stop Snapshot Auditing
 */
void PerfMonMTSvc::stopSnapshotAud(const std::string& stepName, const std::string& compName) {
  // First thing to be called after the initialize step ends
  if (compName == "AthMasterSeq" && stepName == "Initialize") {
    m_measurementSnapshots.capture();
    m_snapshotData[INITIALIZE].addPointStop(m_measurementSnapshots);
  }

  // First thing to be called after the event loop ends
  if (compName == "AthMasterSeq" && stepName == "Stop") {
    m_measurementSnapshots.capture();
    m_snapshotData[EXECUTE].addPointStop(m_measurementSnapshots);
  }
}

/*
 * Start Component Auditing
 */
void PerfMonMTSvc::startCompAud(const std::string& stepName, const std::string& compName, const EventContext& ctx) {
  // Get the thread index
  const unsigned int ithread = (ctx.valid() && tbb::this_task_arena::current_thread_index() > -1) ? tbb::this_task_arena::current_thread_index() : 0;

  // Memory measurement is only done outside the loop except when there is only a single thread
  const bool doMem = !ctx.valid() || (m_numberOfThreads == 1);

  // Generate State
  PMonMT::StepComp currentState = generate_state(stepName, compName);

  // Check if this is the first time calling if so create the mesurement data if not use the existing one.
  // Metrics are collected per thread then aggregated before reporting
  data_map_unique_t& compLevelDataMap = m_compLevelDataMapVec[ithread];
  if(compLevelDataMap.find(currentState) == compLevelDataMap.end()) {
    compLevelDataMap.insert({currentState, std::make_unique<PMonMT::ComponentData>()});
  }

  // Capture and store
  PMonMT::ComponentMeasurement meas;
  meas.capture(doMem);
  compLevelDataMap[currentState]->addPointStart(meas, doMem);

  // Debug
  ATH_MSG_DEBUG("Start Audit - Component " << compName       << " , "
                               "Step "     << stepName       << " , "
                               "Event "    << ctx.evt()      << " , "
                               "Slot "     << ctx.slot()     << " , "
                               "Context "  << ctx.valid()    << " , "
                               "Thread "   << ithread        << " , "
                               "Cpu "      << meas.cpu_time  << " ms, "
                               "Wall "     << meas.wall_time << " ms, "
                               "Vmem "     << meas.vmem      << " kb, "
                               "Malloc "   << meas.malloc    << " kb");
}

/*
 * Stop Component Auditing
 */
void PerfMonMTSvc::stopCompAud(const std::string& stepName, const std::string& compName, const EventContext& ctx) {
  // Get the thread index
  const unsigned int ithread = (ctx.valid() && tbb::this_task_arena::current_thread_index() > -1) ? tbb::this_task_arena::current_thread_index() : 0;

  // Memory measurement is only done outside the loop except when there is only a single thread
  const bool doMem = !ctx.valid() || (m_numberOfThreads == 1);

  // Capture
  PMonMT::ComponentMeasurement meas;
  meas.capture(doMem); // No memory in the event-loop

  // Generate State
  PMonMT::StepComp currentState = generate_state(stepName, compName);

  // Store
  data_map_unique_t& compLevelDataMap = m_compLevelDataMapVec[ithread];
  compLevelDataMap[currentState]->addPointStop(meas, doMem);

  // Once the first time IncidentProcAlg3 is excuted, toggle m_isFirstEvent to false.
  // Doing it this way, instead of at EndAlgorithms incident, makes sure there is no
  // mismatch in start-stop calls to IncidentProcAlg3.
  // It's a little ad-hoc but I don't think this workflow will change much anytime soon.
  if ( m_isFirstEvent && compName == "IncidentProcAlg3" && stepName == "Execute") {
    m_isFirstEvent = false;
  }

  // Debug
  ATH_MSG_DEBUG("Stop Audit - Component " << compName       << " , "
                              "Step "     << stepName       << " , "
                              "Event "    << ctx.evt()      << " , "
                              "Slot "     << ctx.slot()     << " , "
                              "Context "  << ctx.valid()    << " , "
                              "Thread "   << ithread        << " , "
                              "Cpu ("     << compLevelDataMap[currentState]->m_tmp_cpu << ":"
                                          << meas.cpu_time  << ":"
                                          << (meas.cpu_time - compLevelDataMap[currentState]->m_tmp_cpu) << ":"
                                          << compLevelDataMap[currentState]->m_delta_cpu << ") ms, "
                              "Wall ("    << compLevelDataMap[currentState]->m_tmp_wall << ":"
                                          << meas.wall_time << ":"
                                          << (meas.wall_time - compLevelDataMap[currentState]->m_tmp_wall) << ":"
                                          << compLevelDataMap[currentState]->m_delta_wall << ") ms, "
                              "Vmem ("    << compLevelDataMap[currentState]->m_tmp_vmem << ":"
                                          << meas.vmem << ":"
                                          << (meas.vmem - compLevelDataMap[currentState]->m_tmp_vmem) << ":"
                                          << compLevelDataMap[currentState]->m_delta_vmem << ") kb, "
                              "Malloc ("  << compLevelDataMap[currentState]->m_tmp_malloc << ":"
                                          << meas.malloc    << ":"
                                          << (meas.malloc - compLevelDataMap[currentState]->m_tmp_malloc) << ":"
                                          << compLevelDataMap[currentState]->m_delta_malloc << ") kb");
}

/*
 * Is it event-level monitoring check point yet?
 */
bool PerfMonMTSvc::isCheckPoint() {
  // Always check 1, 2, 10, 25 for short tests
  if (m_eventCounter <= 2 || m_eventCounter == 10 || m_eventCounter == 25)
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
 * Helper finction to estimate CPU efficiency
 */
int PerfMonMTSvc::getCpuEfficiency() const {

  // In AthenaMT only the event-loop is executed concurrently
  // In this metric, we scale the event-loop wall-time by
  // the number of slots to take the concurrency into account
  // Then we divide the total cpu-time by this number
  // It's A metric not THE metric...

  const double totalCpuTime =
   m_snapshotData[CONFIGURE].getDeltaCPU()  +
   m_snapshotData[INITIALIZE].getDeltaCPU() +
   m_snapshotData[FIRSTEVENT].getDeltaCPU() +
   m_snapshotData[EXECUTE].getDeltaCPU()    +
   m_snapshotData[FINALIZE].getDeltaCPU();

  const double scaledWallTime =
   m_snapshotData[CONFIGURE].getDeltaWall()  * 1. +
   m_snapshotData[INITIALIZE].getDeltaWall() * 1. +
   m_snapshotData[FIRSTEVENT].getDeltaWall() * 1. +
   m_snapshotData[EXECUTE].getDeltaWall()    * m_numberOfSlots +
   m_snapshotData[FINALIZE].getDeltaWall()   * 1.;

  return ( scaledWallTime > 0 ? totalCpuTime / scaledWallTime * 100. : 0 );

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
  report2Log_EnvInfo();
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
    std::vector<std::pair<PMonMT::StepComp, PMonMT::ComponentData*>> pairs;
    for (auto itr = vec_itr.begin(); itr != vec_itr.end(); ++itr) pairs.push_back(*itr);

    sort(pairs.begin(), pairs.end(),
         [=](std::pair<PMonMT::StepComp, PMonMT::ComponentData*>& a,
             std::pair<PMonMT::StepComp, PMonMT::ComponentData*>& b) {
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

  int64_t vmem = m_eventLevelData.getEventLevelMemory(m_eventCounter, "vmem");
  int64_t rss = m_eventLevelData.getEventLevelMemory(m_eventCounter, "rss");
  int64_t pss = m_eventLevelData.getEventLevelMemory(m_eventCounter, "pss");
  int64_t swap = m_eventLevelData.getEventLevelMemory(m_eventCounter, "swap");

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
  ATH_MSG_INFO("        (Only the first " << m_eventLoopMsgLimit.toString() <<
               " and the last measurements are explicitly printed)");
  ATH_MSG_INFO("=======================================================================================");

  ATH_MSG_INFO(format("%1% %|16t|%2% %|28t|%3% %|40t|%4% %|52t|%5% %|64t|%6% %|76t|%7%") % "Event" % "CPU [s]" %
               "Wall [s]" % "Vmem [kB]" % "Rss [kB]" % "Pss [kB]" % "Swap [kB]");

  ATH_MSG_INFO("---------------------------------------------------------------------------------------");

  m_eventLoopMsgCounter = 0; // reset counter
  uint64_t nMeasurements = m_eventLevelData.getNMeasurements();

  for (const auto& it : m_eventLevelData.getEventLevelData()) {
    // Print
    if(m_eventLoopMsgCounter < m_eventLoopMsgLimit || m_eventLoopMsgCounter == nMeasurements - 1) {
      if(m_eventLoopMsgCounter > m_eventLoopMsgLimit) {
        ATH_MSG_INFO(format("%|=87|") % "...");
      }
      ATH_MSG_INFO(format("%1% %|16t|%2$.2f %|28t|%3$.2f %|40t|%4% %|52t|%5% %|64t|%6% %|76t|%7%") % it.first %
                   (it.second.cpu_time * 0.001) % (it.second.wall_time * 0.001) % it.second.mem_stats.at("vmem") %
                   it.second.mem_stats.at("rss") % it.second.mem_stats.at("pss") % it.second.mem_stats.at("swap"));
    }
    m_eventLoopMsgCounter++;
    // Add to leak estimate
    if (it.first >= m_memFitLowerLimit) {
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

  ATH_MSG_INFO("                                Snapshots Summary                                      ");
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
  const double cpu_exec_total = m_snapshotData[FIRSTEVENT].getDeltaCPU() + m_snapshotData[EXECUTE].getDeltaCPU();
  const double wall_exec_total = m_snapshotData[FIRSTEVENT].getDeltaWall() + m_snapshotData[EXECUTE].getDeltaWall();

  ATH_MSG_INFO(format("%1% %|35t|%2% ") % "Number of events processed:" % m_eventCounter);
  ATH_MSG_INFO(format("%1% %|35t|%2$.0f ") % "CPU usage per event [ms]:" %
               (m_eventCounter > 0 ? cpu_exec_total / m_eventCounter : 0));
  ATH_MSG_INFO(format("%1% %|35t|%2$.3f ") % "Events per second:" %
               (wall_exec_total > 0 ? m_eventCounter / wall_exec_total * 1000. : 0));
  ATH_MSG_INFO(format("%1% %|35t|%2% ") % "CPU utilization efficiency [%]:" % getCpuEfficiency());

  if (m_doEventLoopMonitoring) {
    ATH_MSG_INFO("***************************************************************************************");
    ATH_MSG_INFO(format("%1% %|35t|%2% ") % "Max Vmem: " % scaleMem(m_eventLevelData.getEventLevelMemoryMax("vmem")));
    ATH_MSG_INFO(format("%1% %|35t|%2% ") % "Max Rss: " % scaleMem(m_eventLevelData.getEventLevelMemoryMax("rss")));
    ATH_MSG_INFO(format("%1% %|35t|%2% ") % "Max Pss: " % scaleMem(m_eventLevelData.getEventLevelMemoryMax("pss")));
    ATH_MSG_INFO(format("%1% %|35t|%2% ") % "Max Swap: " % scaleMem(m_eventLevelData.getEventLevelMemoryMax("swap")));
    ATH_MSG_INFO("***************************************************************************************");
    ATH_MSG_INFO(format("%1% %|35t|%2% ") % "Leak estimate per event Vmem: " % scaleMem(m_fit_vmem.slope()));
    ATH_MSG_INFO(format("%1% %|35t|%2% ") % "Leak estimate per event Pss: " % scaleMem(m_fit_pss.slope()));
    ATH_MSG_INFO("  >> Estimated using the last " << m_fit_vmem.nPoints()
                                                  << " measurements from the Event Level Monitoring");
    ATH_MSG_INFO("  >> Events prior to the first " << m_memFitLowerLimit.toString() << " are omitted...");
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
  ATH_MSG_INFO(format("%1% %|35t|%2% ") % "Total Memory:" % scaleMem(get_memory_info()));

  ATH_MSG_INFO("=======================================================================================");
}

/*
 * Report run-time enviroment information
 */
void PerfMonMTSvc::report2Log_EnvInfo() const {
  using boost::format;
  using boost::filesystem::path;

  ATH_MSG_INFO("                               Environment Information                                 ");
  ATH_MSG_INFO("=======================================================================================");

  ATH_MSG_INFO(format("%1% %|35t|%2% ") % "Malloc Library:" % path(PMonSD::symb2lib("malloc")).filename().string());
  ATH_MSG_INFO(format("%1% %|35t|%2% ") % "Math Library:" % path(PMonSD::symb2lib("atan2")).filename().string());

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
    const int64_t dVmem = m_snapshotData[i].getMemMonDeltaMap("vmem");
    const int64_t dRss = m_snapshotData[i].getMemMonDeltaMap("rss");
    const int64_t dPss = m_snapshotData[i].getMemMonDeltaMap("pss");
    const int64_t dSwap = m_snapshotData[i].getMemMonDeltaMap("swap");

    j["summary"]["snapshotLevel"][step] = {{"dCPU", dCPU},
                                           {"dWall", dWall},
                                           {"cpuUtil", cpuUtil},
                                           {"dVmem", dVmem},
                                           {"dRss", dRss},
                                           {"dPss", dPss},
                                           {"dSwap", dSwap}};

  }

  // Report the total number of events
  const int64_t nEvents = m_eventCounter;
  j["summary"]["nEvents"] = nEvents;

  // Report Peaks
  const int64_t vmemPeak = m_eventLevelData.getEventLevelMemoryMax("vmem");
  const int64_t rssPeak = m_eventLevelData.getEventLevelMemoryMax("rss");
  const int64_t pssPeak = m_eventLevelData.getEventLevelMemoryMax("pss");
  const int64_t swapPeak = m_eventLevelData.getEventLevelMemoryMax("swap");

  j["summary"]["peaks"] = {{"vmemPeak", vmemPeak},
                           {"rssPeak", rssPeak},
                           {"pssPeak", pssPeak},
                           {"swapPeak", swapPeak}};

  // Report leak estimates
  const int64_t vmemLeak = m_fit_vmem.slope();
  const int64_t pssLeak = m_fit_pss.slope();
  const int64_t nPoints = m_fit_vmem.nPoints();

  j["summary"]["leakEstimates"] = {{"vmemLeak", vmemLeak},
                                   {"pssLeak", pssLeak},
                                   {"nPoints", nPoints}};

  // Report Sys info
  const std::string cpuModel = get_cpu_model_info();
  const int coreNum = get_cpu_core_info();
  const int64_t totMem = get_memory_info();

  j["summary"]["sysInfo"] = {{"cpuModel", cpuModel},
                             {"coreNum", coreNum},
                             {"totMem", totMem}};

  // Report Enviroment info
  const std::string mallocLib = boost::filesystem::path(PMonSD::symb2lib("malloc")).filename().string();
  const std::string mathLib = boost::filesystem::path(PMonSD::symb2lib("atan2")).filename().string();

  j["summary"]["envInfo"] = {{"mallocLib", mallocLib},
                             {"mathLib", mathLib}};

  // Report CPU utilization efficiency;
  const int cpuUtilEff = getCpuEfficiency();
  j["summary"]["misc"] = {{"cpuUtilEff", cpuUtilEff}};

}

void PerfMonMTSvc::report2JsonFile_ComponentLevel(nlohmann::json& j) const {

  for (const auto& dataMapPerStep : m_stdoutVec_serial) {

    for(const auto& meas : dataMapPerStep){

      const std::string step = meas.first.stepName;
      const std::string component = meas.first.compName;
      const uint64_t count = meas.second->getCallCount();
      const double cpuTime = meas.second->getDeltaCPU();
      const double wallTime = meas.second->getDeltaWall();
      const int64_t vmem  = meas.second->getDeltaVmem();
      const int64_t mall = meas.second->getDeltaMalloc();

      j["componentLevel"][step][component] = {{"count", count},
                                              {"cpuTime", cpuTime},
                                              {"wallTime", wallTime},
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
    const int64_t vmem = it.second.mem_stats.at("vmem");
    const int64_t rss = it.second.mem_stats.at("rss");
    const int64_t pss = it.second.mem_stats.at("pss");
    const int64_t swap = it.second.mem_stats.at("swap");

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
  currentState.stepName = (m_isFirstEvent && stepName == "Execute") ? "FirstEvent" : stepName;
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
        m_compLevelDataMap.insert({it.first, it.second.get()});
      } else {
        m_compLevelDataMap[it.first]->add2CallCount(it.second->getCallCount());
        m_compLevelDataMap[it.first]->add2DeltaCPU(it.second->getDeltaCPU());
        m_compLevelDataMap[it.first]->add2DeltaWall(it.second->getDeltaWall());
        m_compLevelDataMap[it.first]->add2DeltaVmem(it.second->getDeltaVmem());
        m_compLevelDataMap[it.first]->add2DeltaMalloc(it.second->getDeltaMalloc());
      }
      // Do a quick consistency check here and print any suspicious measurements.
      // Timing measurements should always be positive definite
      if(it.second->getDeltaCPU() < 0) {
        ATH_MSG_WARNING("Negative CPU-time measurement of " << it.second->getDeltaCPU() <<
                        " ms for component " << it.first.compName <<
                        " in step " << it.first.stepName);
      }
      if(it.second->getDeltaWall() < 0) {
        ATH_MSG_WARNING("Negative Wall-time measurement of " << it.second->getDeltaWall() <<
                        " ms for component " << it.first.compName <<
                        " in step " << it.first.stepName);
      }
    }
  }
}

/*
 * Divide component-level data into steps, for printing
 */
void PerfMonMTSvc::divideData2Steps() {
  for (const auto &it : m_compLevelDataMap) {
    if (it.first.stepName == "Initialize")
      m_compLevelDataMap_ini[it.first] = it.second;
    else if (it.first.stepName == "FirstEvent")
      m_compLevelDataMap_1stevt[it.first] = it.second;
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
  m_stdoutVec_serial.push_back(m_compLevelDataMap_1stevt);
  m_stdoutVec_serial.push_back(m_compLevelDataMap_evt);
  m_stdoutVec_serial.push_back(m_compLevelDataMap_fin);
  m_stdoutVec_serial.push_back(m_compLevelDataMap_plp);
  m_stdoutVec_serial.push_back(m_compLevelDataMap_cbk);
}

std::string PerfMonMTSvc::scaleTime(double timeMeas) const {
  // Not a huge fan of this, we should eventually unify the types
  // Just want to be explicit about what's happening
  auto ms = static_cast<int64_t>(timeMeas);

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

std::string PerfMonMTSvc::scaleMem(int64_t memMeas) const {

  // Check if there is anything to be done
  if (memMeas == 0) {
    return "0.00 KB" ;
  }

  // Prepare for the result
  std::ostringstream ss;
  ss << std::fixed;
  ss << std::setprecision(2);

  // The input is in KB
  std::vector<std::string> significance = {"KB", "MB", "GB", "TB"};

  // Get the absolute value
  int64_t absMemMeas = std::abs(memMeas);
  // Find the order, note that this is an int operation
  int64_t order = std::log(absMemMeas)/std::log(1024);
  // Compute the final value preserving the sign
  double value = memMeas/std::pow(1024, order);
  // Convert the result to a string
  ss << value;

  return ss.str() + " " + significance[order];
}

/*
 * Collect some hardware information
 */
std::string PerfMonMTSvc::get_info_from_file(const std::string& fileName,
                                             const std::string& fieldName) const {
  // Helper function to read files of type Key : Value
  // Returns the last instance if there are multiple matches
  // This is because we use this method to get the processor count
  std::string result{""};

  std::ifstream file{fileName};
  std::string line{""};

  while (std::getline(file, line)) {
    if (line.empty()) continue;
    size_t splitIdx = line.find(':');
    if (splitIdx != std::string::npos) {
      std::string val = line.substr(splitIdx + 1);
      if (val.empty()) continue;
      if (line.size() >= fieldName.size() &&
          line.compare(0, fieldName.size(), fieldName) == 0) {
        result = val;
      }
    }
  }

  file.close();

  return result;
}

std::string PerfMonMTSvc::get_cpu_model_info() const {
  return get_info_from_file("/proc/cpuinfo","model name") +
         get_info_from_file("/proc/cpuinfo","cache size");
}

int PerfMonMTSvc::get_cpu_core_info() const {
  std::string val = get_info_from_file("/proc/cpuinfo","processor");
  if (val.empty()) return 0;
  return std::stoi(val) + 1;
}

uint64_t PerfMonMTSvc::get_memory_info() const {
  std::string val = get_info_from_file("/proc/meminfo","MemTotal");
  if (val.empty()) return 0;
  val = val.substr(0, val.size() - 3);  // strip the trailing kB
  return std::stoull(val);
}

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/*
 * @authors: Alaettin Serhan Mete, Hasan Ozturk - alaettin.serhan.mete@cern.ch, haozturk@cern.ch
 */

#ifndef PERFMONCOMPS_PERFMONMTSVC_H
#define PERFMONCOMPS_PERFMONMTSVC_H

// Thread-safety-checker
#include "CxxUtils/checker_macros.h"
ATLAS_CHECK_FILE_THREAD_SAFETY;

// Framework includes
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IIncidentListener.h"

// PerfMonKernel includes
#include "PerfMonKernel/IPerfMonMTSvc.h"

// PerfMonComps includes
#include "LinFitSglPass.h"
#include "PerfMonMTUtils.h"

// Containers
#include <set>
#include <deque>

// Input/Output includes
#include <fstream>
#include <iomanip>
#include <nlohmann/json.hpp>

// Other Libraries
#include <algorithm>
#include <cmath>
#include <functional>
#include <memory>
#include <mutex>

class PerfMonMTSvc : virtual public IPerfMonMTSvc, virtual public IIncidentListener, public AthService {
 public:
  /// Standard Gaudi Service constructor
  PerfMonMTSvc(const std::string& name, ISvcLocator* pSvcLocator);

  // Destructor
  virtual ~PerfMonMTSvc() = default;

  /// Function declaring the interface(s) implemented by the service
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;

  /// Incident service handle for post-finalize
  virtual void handle( const Incident& incident ) override;

  /// Standard Gaudi Service initialization
  virtual StatusCode initialize() override;

  /// Standard Gaudi Service finalization
  virtual StatusCode finalize() override;

  /// Start Auditing
  virtual void startAud(const std::string& stepName, const std::string& compName) override;

  /// Stop Auditing
  virtual void stopAud(const std::string& stepName, const std::string& compName) override;

  /// Snapshot Auditing: Take snapshots at the beginning and at the end of each step
  void startSnapshotAud(const std::string& stepName, const std::string& compName);
  void stopSnapshotAud(const std::string& stepName, const std::string& compName);

  /// Component Level Auditing: Take measurements at the beginning and at the end of each component call
  void startCompAud(const std::string& stepName, const std::string& compName, const EventContext& ctx);
  void stopCompAud(const std::string& stepName, const std::string& compName, const EventContext& ctx);

  /// Report the results
  void report();

  /// Report to log
  void report2Log();
  void report2Log_Description() const;
  void report2Log_ComponentLevel();
  void report2Log_EventLevel_instant() const;
  void report2Log_EventLevel();
  void report2Log_Summary();  // make it const
  void report2Log_CpuInfo() const;
  void report2Log_EnvInfo() const;

  /// Report to the JSON File
  void report2JsonFile();
  void report2JsonFile_Summary(nlohmann::json& j) const;
  void report2JsonFile_ComponentLevel(nlohmann::json& j) const;
  void report2JsonFile_EventLevel(nlohmann::json& j) const;

  /// A few helper functions
  bool isPower(uint64_t input, uint64_t base);  // check if input is power of base or not

  void aggregateSlotData();
  void divideData2Steps();

  std::string scaleTime(double timeMeas) const;
  std::string scaleMem(int64_t memMeas) const;

  bool isCheckPoint();

  /// A few helper methods to get system information
  /// These should be carried to PerfMonMTUtils at some point
  std::string get_info_from_file(const std::string& fileName, const std::string& fieldName) const;
  std::string get_cpu_model_info() const;
  int get_cpu_core_info() const;
  uint64_t get_memory_info() const;

  PMonMT::StepComp generate_state(const std::string& stepName, const std::string& compName) const;

 private:
  /// Measurement to capture snapshots
  PMonMT::SnapshotMeasurement m_measurementSnapshots;

  /// Measurement to capture events
  PMonMT::SnapshotMeasurement m_measurementEvents;

  /// Do event loop monitoring
  Gaudi::Property<bool> m_doEventLoopMonitoring{
      this, "doEventLoopMonitoring", true,
      "True if event loop monitoring is enabled, false o/w. Event loop monitoring may cause a decrease in the "
      "performance due to the usage of locks."};
  /// Do component level monitoring
  Gaudi::Property<bool> m_doComponentLevelMonitoring{
      this, "doComponentLevelMonitoring", false,
      "True if component level monitoring is enabled, false o/w. Component monitoring may cause a decrease in the "
      "performance due to the usage of locks."};
  /// Report results to JSON
  Gaudi::Property<bool> m_reportResultsToJSON{this, "reportResultsToJSON", true, "Report results into the json file."};
  /// Name of the JSON file
  Gaudi::Property<std::string> m_jsonFileName{this, "jsonFileName", "PerfMonMTSvc_result.json",
                                              "Name of the JSON file that contains the results."};
  /// Print detailed tables
  Gaudi::Property<bool> m_printDetailedTables{this, "printDetailedTables", true,
                                              "Print detailed component-level metrics."};
  /// Type of event level monitoring
  Gaudi::Property<std::string> m_checkPointType{
      this, "checkPointType", "Arithmetic",
      "Type of the check point sequence: Arithmetic(0, k, 2k...) or Geometric(0,k,k^2...)."};
  /// Lower limit (in number of events) for the memory fit
  Gaudi::Property<uint64_t> m_memFitLowerLimit{
      this, "memFitLowerLimit", 25,
      "Lower limit (in number of events) for the memory fit."};
  /// Frequency of event level monitoring
  Gaudi::Property<uint64_t> m_checkPointFactor{
      this, "checkPointFactor", 50,
      "Common difference if check point sequence is arithmetic, Common ratio if it is Geometric."};
  /// Offset for the wall-time, comes from configuration
  Gaudi::Property<double> m_wallTimeOffset{this, "wallTimeOffset", 0, "Job start wall time in miliseconds."};
  /// Print the top N components
  Gaudi::Property<int> m_printNComps{
      this, "printNComps", 50, "Maximum number of components to be printed."};
  /// Get the number of threads
  Gaudi::Property<int> m_numberOfThreads{this, "numberOfThreads", 1, "Number of threads in the job."};
  /// Get the number of slots
  Gaudi::Property<int> m_numberOfSlots{this, "numberOfSlots", 1, "Number of slots in the job."};
  /// Set the number of messages for the event-level report
  Gaudi::Property<uint64_t> m_eventLoopMsgLimit{this, "eventLoopMsgLimit", 10, "Maximum number of event-level messages."};

  /// Exclude some common components from monitoring
  /// In the future this might be converted to a inclusion set
  /// which would allow user to monitor only a set of algorithms...
  const std::set<std::string> m_exclusionSet = {"AthMasterSeq", "AthAlgEvtSeq", "AthAllAlgSeq", "AthAlgSeq", "AthOutSeq",
    "AthCondSeq", "AthBeginSeq", "AthEndSeq", "AthenaEventLoopMgr", "AthenaHiveEventLoopMgr", "PerfMonMTSvc"};

  /// Snapshots data
  std::vector<PMonMT::SnapshotData> m_snapshotData;
  std::vector<std::string> m_snapshotStepNames = {"Configure", "Initialize", "FirstEvent", "Execute", "Finalize"};
  enum Snapshots {CONFIGURE, INITIALIZE, FIRSTEVENT, EXECUTE, FINALIZE, NSNAPSHOTS};

  // Store event level measurements
  PMonMT::EventLevelData m_eventLevelData;

  // Lock for capturing event loop measurements
  std::mutex m_mutex_capture;

  // Are we processing the first event?
  std::atomic<bool> m_isFirstEvent;

  // Count the number of events processed
  std::atomic<uint64_t> m_eventCounter;

  // Instant event-loop report counter
  std::atomic<uint64_t> m_eventLoopMsgCounter;

  /*
   * Data structure  to store component level measurements
   */
  typedef std::map<PMonMT::StepComp, PMonMT::ComponentData*> data_map_t;
  typedef std::map<PMonMT::StepComp, std::unique_ptr<PMonMT::ComponentData>> data_map_unique_t;
  // Here I'd prefer to use SG::SlotSpecificObj<data_map_t>
  // However, w/ invalid context it seems to segfault
  // Can investigate in the future, for now std::vector should be OK
  data_map_t m_compLevelDataMap;

  // m_compLevelDataMap is divided into following maps and these are stored in the m_stdoutVec_serial.
  // There should be a more clever way!
  std::vector<data_map_unique_t> m_compLevelDataMapVec; // all
  data_map_t m_compLevelDataMap_ini;  // initialize
  data_map_t m_compLevelDataMap_1stevt;  // first event
  data_map_t m_compLevelDataMap_evt;  // execute
  data_map_t m_compLevelDataMap_fin;  // finalize
  data_map_t m_compLevelDataMap_plp;  // preLoadProxy
  data_map_t m_compLevelDataMap_cbk;  // callback

  std::vector<data_map_t> m_stdoutVec_serial;

  // Leak estimates
  PerfMon::LinFitSglPass m_fit_vmem;
  PerfMon::LinFitSglPass m_fit_pss;

  // Estimate CPU efficiency
  int getCpuEfficiency() const;

};  // class PerfMonMTSvc

#endif  // PERFMONCOMPS_PERFMONMTSVC_H

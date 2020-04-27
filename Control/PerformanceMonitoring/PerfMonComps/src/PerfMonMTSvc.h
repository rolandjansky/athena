/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*
 * @authors: Alaettin Serhan Mete, Hasan Ozturk - alaettin.serhan.mete@cern.ch, haozturk@cern.ch
 */

#ifndef PERFMONCOMPS_PERFMONMTSVC_H
#define PERFMONCOMPS_PERFMONMTSVC_H

// Framework includes
#include "AthenaBaseComps/AthService.h"

// PerfMonKernel includes
#include "PerfMonKernel/IPerfMonMTSvc.h"

// PerfMonComps includes
#include "LinFitSglPass.h"
#include "PerfMonMTUtils.h"

// Containers
#include <set>

// Input/Output includes
#include <fstream>
#include <iomanip>
#include <nlohmann/json.hpp>

#include "boost/format.hpp"

// Other Libraries
#include <algorithm>
#include <cmath>
#include <functional>

/*
 * In the snapshot level monitoring, currently we monitor 3 steps as a whole:
 * Initialize, Event Loop and Finalize
 */
#define SNAPSHOT_NUM 3

class PerfMonMTSvc : virtual public IPerfMonMTSvc, public AthService {
 public:
  /// Standard Gaudi Service constructor
  PerfMonMTSvc(const std::string& name, ISvcLocator* pSvcLocator);

  // Destructor
  ~PerfMonMTSvc();

  /// Function declaring the interface(s) implemented by the service
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;

  /// Standard Gaudi Service initialization
  virtual StatusCode initialize() override;

  /// Standard Gaudi Service finalization
  virtual StatusCode finalize() override;

  /// Start Auditing
  virtual void startAud(const std::string& stepName, const std::string& compName) override;

  /// Stop Auditing
  virtual void stopAud(const std::string& stepName, const std::string& compName) override;

  /// Count the number of processed events
  void incrementEventCounter();

  // Do event level monitoring
  virtual void eventLevelMon() override;

  /// Snapshot Auditing: Take snapshots at the beginning and at the end of each step
  void startSnapshotAud(const std::string& stepName, const std::string& compName);

  void stopSnapshotAud(const std::string& stepName, const std::string& compName);

  /// Component Level Auditing in Serial Steps
  void startCompAud_serial(const std::string& stepName, const std::string& compName);

  void stopCompAud_serial(const std::string& stepName, const std::string& compName);

  void startCompAud_MT(const std::string& stepName, const std::string& compName);
  void stopCompAud_MT(const std::string& stepName, const std::string& compName);

  // Report the results
  void report();

  // Report to stdout
  void report2Log();
  void report2Log_Description() const;
  void report2Log_Time_Mem_Serial();
  void report2Log_EventLevel_instant() const;
  void report2Log_EventLevel();
  void report2Log_CompLevel_Time_Parallel();
  void report2Log_Summary();  // make it const
  void report2Log_CpuInfo() const;

  // Report to the JSON File
  void report2JsonFile();
  void report2JsonFile_Summary(nlohmann::json& j) const;
  void report2JsonFile_Time_Serial(nlohmann::json& j) const;
  void report2JsonFile_EventLevel_Time_Parallel(nlohmann::json& j) const;
  void report2JsonFile_CompLevel_Time_Parallel(nlohmann::json& j) const;
  void report2JsonFile_Mem_Serial(nlohmann::json& j) const;
  void report2JsonFile_EventLevel_Mem_Parallel(nlohmann::json& j);

  EventIDBase::event_number_t getEventID() const;

  bool isPower(uint64_t input, uint64_t base);  // check if input is power of base or not
  bool isLoop() const;                // Returns true if the execution is at the event loop, false o/w.

  void divideData2Steps_serial();
  void divideData2Steps_parallel();

  void parallelDataAggregator();

  std::string scaleTime(double timeMeas) const;
  std::string scaleMem(long memMeas) const;

  bool isCheckPoint();

  std::string get_cpu_model_info() const;
  int get_cpu_core_info() const;

  PMonMT::StepComp generate_serial_state(const std::string& stepName, const std::string& compName) const;

  PMonMT::StepCompEvent generate_parallel_state(const std::string& stepName, const std::string& compName,
                                                const uint64_t& eventNumber) const;

  double get_wall_time();

 private:
  /// Measurement to capture the peaks
  PMonMT::Measurement m_peaks;

  /// Measurement to capture
  PMonMT::Measurement m_measurement;

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
  /// Frequency of event level monitoring
  Gaudi::Property<uint64_t> m_checkPointFactor{
      this, "checkPointFactor", 10,
      "Common difference if check point sequence is arithmetic, Common ratio if it is Geometric."};
  /// Offset for the wall-time, comes from configuration
  Gaudi::Property<double> m_wallTimeOffset{this, "wallTimeOffset", 0, "Job start wall time in miliseconds."};
  /// Print the top N serial components
  Gaudi::Property<int> m_printNSerialComps{
      this, "printNSerialComps", 50, "Maximum number of components to be printed under the serial components list."};
  /// Print the top N parallel components
  Gaudi::Property<int> m_printNParallelComps{
      this, "printNParallelComps", 50,
      "Maximum number of components to be printed under the parallel components list."};

  // An array to store snapshot measurements: Init - EvtLoop - Fin
  PMonMT::MeasurementData m_snapshotData[SNAPSHOT_NUM];

  // TODO: It gives error when defining this variable as a class member. Fix it.
  // const static std::string m_snapshotStepNames[3];
  std::vector<std::string> m_snapshotStepNames;

  // Store event level measurements
  PMonMT::MeasurementData m_eventLevelData;

  // Lock for capturing event loop measurements
  std::mutex m_mutex_capture;

  // Count the number of events processed
  std::atomic<unsigned long long> m_eventCounter;

  /* Data structure  to store component level measurements
   * We use pointer to the MeasurementData, because we use new keyword while creating them. Clear!
   */
  std::map<PMonMT::StepComp, PMonMT::MeasurementData*> m_compLevelDataMap;

  // m_compLevelDataMap is divided into following maps and these are stored in the m_stdoutVec_serial.
  // There should be a more clever way!
  std::map<PMonMT::StepComp, PMonMT::MeasurementData*> m_compLevelDataMap_ini;
  std::map<PMonMT::StepComp, PMonMT::MeasurementData*> m_compLevelDataMap_fin;
  std::map<PMonMT::StepComp, PMonMT::MeasurementData*> m_compLevelDataMap_plp;  // preLoadProxy
  std::map<PMonMT::StepComp, PMonMT::MeasurementData*> m_compLevelDataMap_cbk;  // callback

  std::vector<std::map<PMonMT::StepComp, PMonMT::MeasurementData*> > m_stdoutVec_serial;

  //
  // Comp level measurements inside event loop
  PMonMT::MeasurementData m_parallelCompLevelData;

  std::map<PMonMT::StepComp, PMonMT::Measurement> m_aggParallelCompLevelDataMap;

  // m_aggParallelCompLevelDataMap is divided into following maps and these are stored in the m_stdoutVec_parallel.
  // There should be a more clever way!
  std::map<PMonMT::StepComp, PMonMT::Measurement> m_aggParallelCompLevelDataMap_evt;
  std::map<PMonMT::StepComp, PMonMT::Measurement> m_aggParallelCompLevelDataMap_plp;
  std::map<PMonMT::StepComp, PMonMT::Measurement> m_aggParallelCompLevelDataMap_cbk;

  std::vector<std::map<PMonMT::StepComp, PMonMT::Measurement> > m_stdoutVec_parallel;

  // Estimated job configuration time
  double m_jobCfg_time;

  // Leak estimates
  PerfMon::LinFitSglPass m_fit_vmem;
  PerfMon::LinFitSglPass m_fit_pss;

};  // class PerfMonMTSvc

#endif  // PERFMONCOMPS_PERFMONMTSVC_H

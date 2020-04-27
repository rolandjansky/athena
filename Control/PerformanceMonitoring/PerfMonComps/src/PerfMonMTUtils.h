/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*
 * @authors: Alaettin Serhan Mete, Hasan Ozturk - alaettin.serhan.mete@cern.ch, haozturk@cern.ch
 */

#ifndef PERFMONCOMPS_PERFMONMTUTILS_H
#define PERFMONCOMPS_PERFMONMTUTILS_H

// STL includes
#include <fcntl.h>     // for open function
#include <malloc.h>    // for mallinfo function
#include <sys/stat.h>  // to check whether /proc/* exists in the machine

#include <chrono>
#include <ctime>
#include <fstream>

typedef std::map<std::string, long> memory_map_t;  // Component : Memory Measurement(kB)

/*
 * Inline function prototypes
 */
inline memory_map_t operator-(memory_map_t& map1, memory_map_t& map2);
inline bool doesDirectoryExist(const std::string dir);

/*
 * Necessary tools
 */
namespace PMonMT {
// Base methods for collecting data
double get_thread_cpu_time();
double get_process_cpu_time();
double get_wall_time();
// Non-efficient memory measurement
memory_map_t get_mem_stats();
// Efficient memory measurements
double get_malloc();
double get_vmem();

// Step name and Component name pairs. Ex: Initialize - StoreGateSvc
struct StepComp {
  std::string stepName;
  std::string compName;

  // Overload < operator, because we are using a custom key(StepComp)  for std::map
  bool operator<(const StepComp& sc) const {
    return std::make_pair(this->stepName, this->compName) < std::make_pair(sc.stepName, sc.compName);
  }
};

// Step Name, Component name and event number triple. e.g. Execute - StoreGateSvc - 3
struct StepCompEvent {
  std::string stepName;
  std::string compName;
  uint64_t eventNumber;

  // Overload < operator, because we are using a custom key(StepCompEvent)  for std::map
  bool operator<(const StepCompEvent& sce) const {
    return std::make_pair(this->eventNumber, this->compName) < std::make_pair(sce.eventNumber, sce.compName);
  }
};

// Basic Measurement
struct Measurement {
  typedef std::map<int, Measurement> event_meas_map_t;  // Event number: Measurement

  // Variables to store measurements
  long call_count;  // how many times a component in a step is called
  double cpu_time;
  double wall_time;
  memory_map_t mem_stats;  // Vmem, Rss, Pss, Swap

  // These two metrics are collected in a more efficient way than other memory metrics.
  int vmem;
  int malloc;

  // Event level measurements
  event_meas_map_t eventLevel_meas_map;  // [Event count so far]: Measurement

  // Component level measurements for the parallel steps
  std::map<StepCompEvent, Measurement> compLevel_meas_map;

  // Peak values for Vmem, Rss and Pss
  long vmemPeak = LONG_MIN;
  long rssPeak = LONG_MIN;
  long pssPeak = LONG_MIN;

  // Capture snapshot measurements
  void capture_snapshot() {
    cpu_time = get_process_cpu_time();
    wall_time = get_wall_time();

    if (doesDirectoryExist("/proc")) {
      mem_stats = get_mem_stats();
      if (mem_stats["vmem"] > vmemPeak) vmemPeak = mem_stats["vmem"];
      if (mem_stats["rss"] > rssPeak) rssPeak = mem_stats["rss"];
      if (mem_stats["pss"] > pssPeak) pssPeak = mem_stats["pss"];
    }
  }

  // [Component Level Monitoring - Serial Steps] : Record the measurement for the current state
  void capture_compLevel_serial() {
    cpu_time = get_process_cpu_time();
    wall_time = get_wall_time();

    // Efficient Memory Measurements
    malloc = get_malloc();
    if (doesDirectoryExist("/proc")) {
      vmem = get_vmem();
      if (vmem > vmemPeak) vmemPeak = vmem;
    }
  }

  // [Component Level Monitoring - Parallel Steps] : Record the measurement for the current state
  void capture_compLevel_MT(StepCompEvent sce) {
    cpu_time = get_thread_cpu_time();
    wall_time = get_wall_time();

    Measurement meas;

    meas.cpu_time = cpu_time;
    meas.wall_time = wall_time;

    // Capture for component level measurements
    compLevel_meas_map[sce] = meas;
  }

  // [Event Level Monitoring - Parallel Steps] : Record the measurement for the current checkpoint
  void capture_eventLevel_MT(int eventCount) {
    cpu_time = get_process_cpu_time();
    wall_time = get_wall_time();
    Measurement meas;

    if (doesDirectoryExist("/proc")) {
      mem_stats = get_mem_stats();
      meas.mem_stats = mem_stats;

      if (mem_stats["vmem"] > vmemPeak) vmemPeak = mem_stats["vmem"];
      if (mem_stats["rss"] > rssPeak) rssPeak = mem_stats["rss"];
      if (mem_stats["pss"] > pssPeak) pssPeak = mem_stats["pss"];
    }

    meas.cpu_time = cpu_time;
    meas.wall_time = wall_time;

    // Capture for event level measurements
    eventLevel_meas_map[eventCount] = meas;
  }

  Measurement() : cpu_time{0.}, wall_time{0.} {}
};

// Basic Data
struct MeasurementData {
  typedef std::map<uint64_t, Measurement> event_meas_map_t;  // Event number: Measurement

  // These variables are used to calculate and store the serial component level measurements
  double m_tmp_cpu, m_delta_cpu;
  double m_tmp_wall, m_delta_wall;
  memory_map_t m_memMon_tmp_map;
  memory_map_t m_memMon_delta_map;

  int m_tmp_vmem, m_delta_vmem;
  int m_tmp_malloc, m_delta_malloc;

  // This map is used to store the event level measurements
  event_meas_map_t m_eventLevel_delta_map;

  // These maps are used to calculate component level measurements inside the event loop
  std::map<StepCompEvent, Measurement> m_compLevel_tmp_map;
  std::map<StepCompEvent, Measurement> m_compLevel_delta_map;

  // Wall time offset for event level monitoring
  double m_offset_wall;

  // [Component Level Monitoring - Serial Steps] : Record the measurement for the current state
  void addPointStart_snapshot(const Measurement& meas) {
    m_tmp_cpu = meas.cpu_time;
    m_tmp_wall = meas.wall_time;

    // Non-efficient memory measurements
    if (doesDirectoryExist("/proc")) m_memMon_tmp_map = meas.mem_stats;
  }

  // [Component Level Monitoring - Serial Steps] : Record the measurement for the current state
  void addPointStop_snapshot(Measurement& meas) {
    m_delta_cpu = meas.cpu_time - m_tmp_cpu;
    m_delta_wall = meas.wall_time - m_tmp_wall;

    // Non-efficient memory measurements
    if (doesDirectoryExist("/proc")) m_memMon_delta_map = meas.mem_stats - m_memMon_tmp_map;
  }

  // [Component Level Monitoring - Serial Steps] : Record the measurement for the current state
  void addPointStart_serial(const Measurement& meas) {
    m_tmp_cpu = meas.cpu_time;
    m_tmp_wall = meas.wall_time;

    // Efficient memory measurements
    m_tmp_malloc = meas.malloc;
    if (doesDirectoryExist("/proc")) m_tmp_vmem = meas.vmem;
  }

  // [Component Level Monitoring - Serial Steps] : Record the measurement for the current state
  void addPointStop_serial(Measurement& meas) {
    m_delta_cpu = meas.cpu_time - m_tmp_cpu;
    m_delta_wall = meas.wall_time - m_tmp_wall;

    // Efficient memory measurements
    m_delta_malloc = meas.malloc - m_tmp_malloc;
    if (doesDirectoryExist("/proc")) m_delta_vmem = meas.vmem - m_tmp_vmem;
  }

  // [Component Level Monitoring - Parallel Steps] : Record the measurement for the current state
  void addPointStart_MT(Measurement& meas, StepCompEvent sce) {
    m_compLevel_tmp_map[sce] = meas.compLevel_meas_map[sce];
  }

  // [Component Level Monitoring - Parallel Steps] : Record the measurement for the current state
  void addPointStop_MT(Measurement& meas, StepCompEvent sce) {
    m_compLevel_delta_map[sce].cpu_time = meas.compLevel_meas_map[sce].cpu_time - m_compLevel_tmp_map[sce].cpu_time;
    m_compLevel_delta_map[sce].wall_time = meas.compLevel_meas_map[sce].wall_time - m_compLevel_tmp_map[sce].wall_time;

    if (doesDirectoryExist("/proc")) {
      m_compLevel_delta_map[sce].mem_stats["vmem"] =
          meas.compLevel_meas_map[sce].mem_stats["vmem"] - m_compLevel_tmp_map[sce].mem_stats["vmem"];
      m_compLevel_delta_map[sce].mem_stats["rss"] =
          meas.compLevel_meas_map[sce].mem_stats["rss"] - m_compLevel_tmp_map[sce].mem_stats["rss"];
      m_compLevel_delta_map[sce].mem_stats["pss"] =
          meas.compLevel_meas_map[sce].mem_stats["pss"] - m_compLevel_tmp_map[sce].mem_stats["pss"];
      m_compLevel_delta_map[sce].mem_stats["swap"] =
          meas.compLevel_meas_map[sce].mem_stats["swap"] - m_compLevel_tmp_map[sce].mem_stats["swap"];
    }
  }

  // [Event Level Monitoring - Parallel Steps] : Record the measurement for the current checkpoint
  void record_eventLevel(Measurement& meas, int eventCount) {
    m_eventLevel_delta_map[eventCount].cpu_time = meas.eventLevel_meas_map[eventCount].cpu_time;
    m_eventLevel_delta_map[eventCount].wall_time = meas.eventLevel_meas_map[eventCount].wall_time - m_offset_wall;

    if (doesDirectoryExist("/proc")) {
      m_eventLevel_delta_map[eventCount].mem_stats["vmem"] = meas.eventLevel_meas_map[eventCount].mem_stats["vmem"];
      m_eventLevel_delta_map[eventCount].mem_stats["rss"] = meas.eventLevel_meas_map[eventCount].mem_stats["rss"];
      m_eventLevel_delta_map[eventCount].mem_stats["pss"] = meas.eventLevel_meas_map[eventCount].mem_stats["pss"];
      m_eventLevel_delta_map[eventCount].mem_stats["swap"] = meas.eventLevel_meas_map[eventCount].mem_stats["swap"];
    }
  }

  void set_wall_time_offset(double wall_time_offset) { m_offset_wall = wall_time_offset; }

  event_meas_map_t getEventLevelData() const { return m_eventLevel_delta_map; }

  double getEventLevelCpuTime(unsigned long long event_count) const {
    return m_eventLevel_delta_map.at(event_count).cpu_time;
  }

  double getEventLevelWallTime(unsigned long long event_count) const {
    return m_eventLevel_delta_map.at(event_count).wall_time;
  }

  double getEventLevelVmem(unsigned long long event_count) const {
    return m_eventLevel_delta_map.at(event_count).mem_stats.at("vmem");
  }

  double getEventLevelRss(unsigned long long event_count) const {
    return m_eventLevel_delta_map.at(event_count).mem_stats.at("rss");
  }

  double getEventLevelPss(unsigned long long event_count) const {
    return m_eventLevel_delta_map.at(event_count).mem_stats.at("pss");
  }

  double getEventLevelSwap(unsigned long long event_count) const {
    return m_eventLevel_delta_map.at(event_count).mem_stats.at("swap");
  }

  double getDeltaCPU() const { return m_delta_cpu; }

  double getDeltaWall() const { return m_delta_wall; }

  int getDeltaVmem() const { return m_delta_vmem; }

  int getDeltaMalloc() const { return m_delta_malloc; }

  long getMemMonDeltaMap(std::string mem_stat) { return m_memMon_delta_map[mem_stat]; }

  MeasurementData() : m_tmp_cpu{0.}, m_delta_cpu{0.}, m_tmp_wall{0.}, m_delta_wall{0.} {}
};

}  // namespace PMonMT

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

/*
 * Thread specific CPU time measurement in ms
 */
inline double PMonMT::get_thread_cpu_time() {
  // Get the thread clock id
  clockid_t thread_cid;
  pthread_getcpuclockid(pthread_self(), &thread_cid);

  // Get the thread specific CPU time
  struct timespec ctime;
  clock_gettime(thread_cid, &ctime);

  // Return the measurement in ms
  return static_cast<double>(ctime.tv_sec * 1.e3 + ctime.tv_nsec * 1.e-6);
}

/*
 * Process specific CPU time measurement in ms
 */
inline double PMonMT::get_process_cpu_time() { return static_cast<double>(std::clock() * (1.e3 / CLOCKS_PER_SEC)); }

/*
 * Wall-time measurement since epoch in ms
 */
inline double PMonMT::get_wall_time() {
  return static_cast<double>(std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1));
}

/*
 * Memory statistics
 */

// Read from proc's smaps file. It is costly to do this operation too often.
inline memory_map_t PMonMT::get_mem_stats() {
  memory_map_t result;
  std::string fileName = "/proc/self/smaps";
  std::ifstream smaps_file(fileName);

  std::string line;
  std::string key;
  std::string value;

  while (getline(smaps_file, line)) {
    std::stringstream ss(line);
    ss >> key >> value;

    if (key == "Size:") {
      result["vmem"] += stol(value);
    }
    if (key == "Rss:") {
      result["rss"] += stol(value);
    }
    if (key == "Pss:") {
      result["pss"] += stol(value);
    }
    if (key == "Swap:") {
      result["swap"] += stol(value);
    }
  }
  return result;
}

// This operation is less costly than the previous one. Since statm is a much smaller file compared to smaps.
inline double PMonMT::get_vmem() {
  const std::string fileName = "/proc/self/statm";
  std::ifstream statm_file(fileName);

  std::string vmem_in_pages;  // vmem measured in pages
  std::string line;
  if (getline(statm_file, line)) {
    std::stringstream ss(line);
    ss >> vmem_in_pages;  // The first number in this file is the vmem measured in pages
  }

  const double page_size = sysconf(_SC_PAGESIZE) / 1024.0;  // page size in KB
  const double result = stod(vmem_in_pages) * page_size;

  return result;
}

// The code of the following function is borrowed from PMonSD and static variables are declared as thread_local
// See:
// https://acode-browser1.usatlas.bnl.gov/lxr/source/athena/Control/PerformanceMonitoring/PerfMonComps/src/SemiDetMisc.h#0443
inline double PMonMT::get_malloc() {
  struct mallinfo curr_mallinfo = mallinfo();
  int64_t uordblks_raw = curr_mallinfo.uordblks;
  if (sizeof(curr_mallinfo.uordblks) == sizeof(int32_t)) {
    const int64_t half_range = std::numeric_limits<int32_t>::max();
    thread_local int64_t last_uordblks = curr_mallinfo.uordblks;
    thread_local int64_t offset_uordblks = 0;
    if (uordblks_raw - last_uordblks > half_range) {
      // underflow detected
      offset_uordblks -= 2 * half_range;
    } else if (last_uordblks - uordblks_raw > half_range) {
      // overflow detected
      offset_uordblks += 2 * half_range;
    }
    last_uordblks = uordblks_raw;
    uordblks_raw += offset_uordblks;
  }
  // exact same code for hblkhd (a bit of code duplication...):
  int64_t hblkhd_raw = curr_mallinfo.hblkhd;
  if (sizeof(curr_mallinfo.hblkhd) == sizeof(int32_t)) {
    const int64_t half_range = std::numeric_limits<int32_t>::max();
    thread_local int64_t last_hblkhd = curr_mallinfo.hblkhd;
    thread_local int64_t offset_hblkhd = 0;
    if (hblkhd_raw - last_hblkhd > half_range) {
      // underflow detected
      offset_hblkhd -= 2 * half_range;
    } else if (last_hblkhd - hblkhd_raw > half_range) {
      // overflow detected
      offset_hblkhd += 2 * half_range;
    }
    last_hblkhd = hblkhd_raw;
    hblkhd_raw += offset_hblkhd;
  }
  return (uordblks_raw + hblkhd_raw) / 1024.0;
}

inline memory_map_t operator-(memory_map_t& map1, memory_map_t& map2) {
  memory_map_t result_map;
  for (auto it : map1) {
    result_map[it.first] = map1[it.first] - map2[it.first];
  }
  return result_map;
}

inline bool doesDirectoryExist(const std::string dir) {
  struct stat buffer;
  return (stat(dir.c_str(), &buffer) == 0);
}

#endif  // PERFMONCOMPS_PERFMONMTUTILS_H

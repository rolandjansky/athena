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
// Simple check if directory exists
bool doesDirectoryExist(const std::string dir);

// Step name and Component name pairs. Ex: Initialize - StoreGateSvc
struct StepComp {
  std::string stepName;
  std::string compName;

  // Overload < operator, because we are using a custom key(StepComp)  for std::map
  bool operator<(const StepComp& sc) const {
    return std::make_pair(this->stepName, this->compName) < std::make_pair(sc.stepName, sc.compName);
  }
};

// Basic Measurement
struct Measurement {

  // Variables to store measurements
  double cpu_time, wall_time; // Timing
  memory_map_t mem_stats; // Memory: Vmem, Rss, Pss, Swap
  double vmem, malloc; // Memory: Vmem, Malloc (faster than above)

  // Peak values for Vmem, Rss and Pss
  long vmemPeak = LONG_MIN;
  long rssPeak = LONG_MIN;
  long pssPeak = LONG_MIN;

  // Capture snapshot measurements
  void capture_snapshot() {
    // Timing 
    cpu_time = get_process_cpu_time();
    wall_time = get_wall_time();

    // Memory 
    mem_stats = get_mem_stats();
  }

  // Capture component-level measurements
  void capture_component(const bool doMem = false) {
    // Timing 
    cpu_time = get_thread_cpu_time();
    wall_time = get_wall_time();

    // Memory if only necessary
    if (!doMem) return;

    // Efficient Memory Measurements
    malloc = get_malloc();
    vmem = get_vmem();
  }

  // Capture event-level measurements 
  void capture_event() {
    // Timing 
    cpu_time = get_process_cpu_time();
    wall_time = get_wall_time();

    // Memory
    mem_stats = get_mem_stats();

    if (mem_stats["vmem"] > vmemPeak) vmemPeak = mem_stats["vmem"];
    if (mem_stats["rss"] > rssPeak) rssPeak = mem_stats["rss"];
    if (mem_stats["pss"] > pssPeak) pssPeak = mem_stats["pss"];
  }

  Measurement() : cpu_time{0.}, wall_time{0.}, vmem{0.}, malloc{0.} {
    mem_stats["vmem"] = 0; mem_stats["pss"] = 0; mem_stats["rss"] = 0; mem_stats["swap"] = 0;
  }
};

// Basic Data
struct MeasurementData {
  typedef std::map<uint64_t, Measurement> event_meas_map_t;  // Event number: Measurement

  // These variables are used to calculate and store the serial component level measurements
  uint64_t m_call_count;
  double m_tmp_cpu, m_delta_cpu;
  double m_tmp_wall, m_delta_wall;
  memory_map_t m_memMon_tmp_map;
  memory_map_t m_memMon_delta_map;
  double m_tmp_vmem, m_delta_vmem;
  double m_tmp_malloc, m_delta_malloc;

  // This map is used to store the event level measurements
  event_meas_map_t m_eventLevel_delta_map;

  // Wall time offset for event level monitoring
  double m_offset_wall;

  // [Component Level Monitoring - Serial Steps] : Record the measurement for the current state
  void addPointStart_snapshot(const Measurement& meas) {
    m_tmp_cpu = meas.cpu_time;
    m_tmp_wall = meas.wall_time;

    // Non-efficient memory measurements
    m_memMon_tmp_map = meas.mem_stats;
  }

  // [Component Level Monitoring - Serial Steps] : Record the measurement for the current state
  void addPointStop_snapshot(Measurement& meas) {
    m_delta_cpu = meas.cpu_time - m_tmp_cpu;
    m_delta_wall = meas.wall_time - m_tmp_wall;

    // Non-efficient memory measurements
    m_memMon_delta_map = meas.mem_stats - m_memMon_tmp_map;
  }

  // [Component Level Monitoring] : Start
  void addPointStart_component(const Measurement& meas, const bool doMem = false) {
    // Timing
    m_tmp_cpu = meas.cpu_time;
    m_tmp_wall = meas.wall_time;

    // Memory if only necessary
    if (!doMem) return;

    // Efficient memory measurements
    m_tmp_malloc = meas.malloc;
    m_tmp_vmem = meas.vmem;
  } 

  // [Component Level Monitoring] : Stop
  // Unlike Snapshots, here we essentially keep a running sum
  void addPointStop_component(const Measurement& meas, const bool doMem = false) {
    // Call count
    m_call_count++;

    // Timing
    m_delta_cpu += meas.cpu_time - m_tmp_cpu;
    m_delta_wall += meas.wall_time - m_tmp_wall;

    // Memory if only necessary
    if (!doMem) return;

    // Efficient memory measurements
    m_delta_malloc += meas.malloc - m_tmp_malloc;
    m_delta_vmem += meas.vmem - m_tmp_vmem;
  } 

  // [Event Level Monitoring - Parallel Steps] : Record the measurement for the current checkpoint
  void record_event(const Measurement& meas, int eventCount) {
    // Timing
    m_eventLevel_delta_map[eventCount].cpu_time = meas.cpu_time;
    m_eventLevel_delta_map[eventCount].wall_time = meas.wall_time - m_offset_wall;

    // Memory
    m_eventLevel_delta_map[eventCount].mem_stats = meas.mem_stats;
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

  uint64_t getCallCount() const { return m_call_count; }
  void add2CallCount(uint64_t val) { m_call_count += val; }

  double getDeltaCPU() const { return m_delta_cpu; }
  void add2DeltaCPU(double val) { m_delta_cpu += val; }

  double getDeltaWall() const { return m_delta_wall; }
  void add2DeltaWall(double val) { m_delta_wall += val; }

  double getDeltaVmem() const { return m_delta_vmem; }
  void add2DeltaVmem(double val) { m_delta_vmem += val; }

  double getDeltaMalloc() const { return m_delta_malloc; }
  void add2DeltaMalloc(double val) { m_delta_malloc += val; }

  long getMemMonDeltaMap(std::string mem_stat) const { return m_memMon_delta_map.at(mem_stat); }

  MeasurementData() : m_call_count{0}, m_tmp_cpu{0.}, m_delta_cpu{0.}, m_tmp_wall{0.}, m_delta_wall{0.},
    m_tmp_vmem{0.}, m_delta_vmem{0.}, m_tmp_malloc{0.}, m_delta_malloc{0.}, m_offset_wall{0.} {
    m_memMon_tmp_map["vmem"] = 0; m_memMon_tmp_map["pss"] = 0; m_memMon_tmp_map["rss"] = 0; m_memMon_tmp_map["swap"] = 0;
    m_memMon_delta_map["vmem"] = 0; m_memMon_delta_map["pss"] = 0; m_memMon_delta_map["rss"] = 0; m_memMon_delta_map["swap"] = 0;
  }
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
// In a realistic RAWtoESD job, the smaps for the the whole application can get large.
// Therefore, this operation might take about 100 ms per call, which is fairly substantial.
// However, this is one of the most reliable way to get PSS.
// Therefore, keep it as is but don't call it too often!
inline memory_map_t PMonMT::get_mem_stats() {
  // Result object
  memory_map_t result;

  // Zero initialize
  result["vmem"] = result["rss"] = result["pss"] = result["swap"] = 0;

  // This is the input where we read the stats from
  static const std::string fileName = "/proc/self/smaps";
  std::ifstream smaps_file{fileName};

  std::string line{}, key{}, value{};

  // Loop over the file
  while (smaps_file) {
    // Read interesting key value pairs
    smaps_file >> key >> value;
    smaps_file.ignore(std::numeric_limits<std::streamsize>::max(),'\n');

    if(smaps_file) {
      if (key == "Size:") {
        result["vmem"] += std::stol(value);
      }
      if (key == "Rss:") {
        result["rss"] += std::stol(value);
      }
      if (key == "Pss:") {
        result["pss"] += std::stol(value);
      }
      if (key == "Swap:") {
        result["swap"] += std::stol(value);
      }
    }
  }

  return result;
}

// This operation is less costly than the previous one. Since statm is a much smaller file compared to smaps.
inline double PMonMT::get_vmem() {
  // Result
  double result = 0.;

  // This is where we read the stats from
  static const std::string fileName = "/proc/self/statm";
  std::ifstream statm_file{fileName};

  std::string vmem_in_pages{}, line{};  // vmem measured in pages

  // We simply get the first line
  if (getline(statm_file, line)) {
    std::stringstream ss{line};
    ss >> vmem_in_pages;  // The first number in this file is the vmem measured in pages
  }

  static const double page_size = sysconf(_SC_PAGESIZE) / 1024.0;  // page size in KB
  result = std::stod(vmem_in_pages) * page_size;

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

/*
 * Simple check if a given directory exists
 */
inline bool PMonMT::doesDirectoryExist(const std::string dir) {
  struct stat buffer;
  return (stat(dir.c_str(), &buffer) == 0);
}

#endif  // PERFMONCOMPS_PERFMONMTUTILS_H

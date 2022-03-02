/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/*
 * @authors: Alaettin Serhan Mete, Hasan Ozturk - alaettin.serhan.mete@cern.ch, haozturk@cern.ch
 */

#ifndef PERFMONCOMPS_PERFMONMTUTILS_H
#define PERFMONCOMPS_PERFMONMTUTILS_H

// PerfMon includes
#include "SemiDetMisc.h"   // borrow from existing code
#include "PerfMonEvent/mallinfo.h"

// STL includes
#include <fcntl.h>     // for open function
#include <malloc.h>    // for mallinfo function
#include <sys/stat.h>  // to check whether /proc/* exists in the machine

#include <chrono>
#include <cstdint>
#include <ctime>
#include <fstream>

typedef std::map<std::string, int64_t> MemoryMap_t;  // Component : Memory Measurement(kB)

/*
 * Inline function prototypes
 */
inline MemoryMap_t operator-(const MemoryMap_t& map1, const MemoryMap_t& map2);

/*
 * Necessary tools
 */
namespace PMonMT {

  // Base methods for collecting data
  double get_thread_cpu_time();
  double get_process_cpu_time();
  double get_wall_time();

  // Non-efficient memory measurement
  MemoryMap_t get_mem_stats();

  // Efficient memory measurements
  double get_vmem();

  // Simple check if directory exists
  bool doesDirectoryExist(const std::string& dir);

  // Step name and Component name pairs. Ex: Initialize - StoreGateSvc
  struct StepComp {
    std::string stepName;
    std::string compName;

    // Overload < operator, because we are using a custom key(StepComp)  for std::map
    bool operator<(const StepComp& sc) const {
      return std::make_pair(this->stepName, this->compName) < std::make_pair(sc.stepName, sc.compName);
    }
  };

  // ComponentMeasurement
  struct ComponentMeasurement {

    // Variables to store measurements
    double cpu_time, wall_time; // Timing
    double vmem, malloc; // Memory: Vmem, Malloc

    // Capture component-level measurements
    void capture(const bool doMem = false) {

      // Timing
      cpu_time = get_thread_cpu_time();
      wall_time = get_wall_time();

      // Memory if only necessary
      if (!doMem) return;

      // Memory
      malloc = PMonSD::get_malloc_kb();
      vmem = get_vmem();

    }

    // Constructor
    ComponentMeasurement() : cpu_time{0.}, wall_time{0.}, vmem{0.}, malloc{0.} { }

  }; // End ComponentMeasurement

  // Component Data
  struct ComponentData {

    // These variables are used to calculate and store the component level measurements
    uint64_t m_call_count;
    double m_tmp_cpu, m_delta_cpu;
    double m_tmp_wall, m_delta_wall;
    double m_tmp_vmem, m_delta_vmem;
    double m_tmp_malloc, m_delta_malloc;

    // [Component Level Monitoring] : Start
    void addPointStart(const ComponentMeasurement& meas, const bool doMem = false) {

      // Timing
      m_tmp_cpu = meas.cpu_time;
      m_tmp_wall = meas.wall_time;

      // Memory if only necessary
      if (!doMem) return;

      // Memory
      m_tmp_malloc = meas.malloc;
      m_tmp_vmem = meas.vmem;

    }

    // [Component Level Monitoring] : Stop
    void addPointStop(const ComponentMeasurement& meas, const bool doMem = false) {

      // Call count
      m_call_count++;

      // Timing
      m_delta_cpu += meas.cpu_time - m_tmp_cpu;
      m_delta_wall += meas.wall_time - m_tmp_wall;

      // Memory if only necessary
      if (!doMem) return;

      // Memory
      m_delta_malloc += meas.malloc - m_tmp_malloc;
      m_delta_vmem += meas.vmem - m_tmp_vmem;

    }

    // Convenience methods
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

    // Constructor
    ComponentData() : m_call_count{0}, m_tmp_cpu{0.}, m_delta_cpu{0.}, m_tmp_wall{0.}, m_delta_wall{0.},
      m_tmp_vmem{0.}, m_delta_vmem{0.}, m_tmp_malloc{0.}, m_delta_malloc{0.} { }

  }; // End ComponentData

  // Snapshot Measurement
  struct SnapshotMeasurement {

    // Variables to store measurements
    double cpu_time, wall_time; // Timing
    MemoryMap_t mem_stats; // Memory: Vmem, Rss, Pss, Swap

    // Capture snapshot measurements
    void capture() {

      // Timing
      cpu_time = get_process_cpu_time();
      wall_time = get_wall_time();

      // Memory
      mem_stats = get_mem_stats();

    }

    // Constructor
    SnapshotMeasurement() : cpu_time{0.}, wall_time{0.} {
      mem_stats["vmem"] = 0; mem_stats["pss"] = 0; mem_stats["rss"] = 0; mem_stats["swap"] = 0;
    }

  }; // End SnapshotMeasurement

  // Event Level Data
  struct EventLevelData {

    // This map is used to store the event level measurements
    typedef std::map<uint64_t, SnapshotMeasurement> EventMeasMap_t;  // Event number: Measurement
    EventMeasMap_t m_eventLevelDeltaMap;

    // [Event Level Monitoring] : Record the measurement for the current checkpoint
    void recordEvent(const SnapshotMeasurement& meas, const int eventCount) {

      // Timing
      m_eventLevelDeltaMap[eventCount].cpu_time = meas.cpu_time;
      m_eventLevelDeltaMap[eventCount].wall_time = meas.wall_time - m_offset_wall;

      // Memory
      m_eventLevelDeltaMap[eventCount].mem_stats = meas.mem_stats;

    }

    // Wall time offset for event level monitoring
    double m_offset_wall;

    // Convenience methods
    void set_wall_time_offset(const double wall_time_offset) { m_offset_wall = wall_time_offset; }

    EventMeasMap_t getEventLevelData() const {
      return m_eventLevelDeltaMap;
    }

    uint64_t getNMeasurements() const {
      return m_eventLevelDeltaMap.size();
    }

    double getEventLevelCpuTime(const uint64_t event_count) const {
      return m_eventLevelDeltaMap.at(event_count).cpu_time;
    }

    double getEventLevelWallTime(const uint64_t event_count) const {
      return m_eventLevelDeltaMap.at(event_count).wall_time;
    }

    int64_t getEventLevelMemory(const uint64_t event_count,
                                      const std::string& stat) const {
      return m_eventLevelDeltaMap.at(event_count).mem_stats.at(stat);
    }

    int64_t getEventLevelMemoryMax(const std::string& stat) const {
      int64_t result = 0;
      for (const auto& it : getEventLevelData()) {
        if (it.second.mem_stats.at(stat) > result) {
          result = it.second.mem_stats.at(stat);
        }
      }
      return result;
    }

  }; // Add EventLevelData

  // Snapshot Data
  struct SnapshotData {

    // These variables are used to calculate and store the serial component level measurements
    double m_tmp_cpu, m_delta_cpu;
    double m_tmp_wall, m_delta_wall;
    MemoryMap_t m_memMonTmpMap, m_memMonDeltaMap;

    // [Snapshot Level Monitoring] : Start
    void addPointStart(const SnapshotMeasurement& meas) {

      // Timing
      m_tmp_cpu = meas.cpu_time;
      m_tmp_wall = meas.wall_time;

      // Non-efficient memory measurements
      m_memMonTmpMap = meas.mem_stats;

    }

    // [Snapshot Level Monitoring] : Stop
    void addPointStop(const SnapshotMeasurement& meas) {

      // Timing
      m_delta_cpu = meas.cpu_time - m_tmp_cpu;
      m_delta_wall = meas.wall_time - m_tmp_wall;

      // Non-efficient memory measurements
      m_memMonDeltaMap = meas.mem_stats - m_memMonTmpMap;

    }

    // Convenience methods
    double getDeltaCPU() const { return m_delta_cpu; }
    void add2DeltaCPU(double val) { m_delta_cpu += val; }

    double getDeltaWall() const { return m_delta_wall; }
    void add2DeltaWall(double val) { m_delta_wall += val; }

    int64_t getMemMonDeltaMap(const std::string& mem_stat) const {
      return m_memMonDeltaMap.at(mem_stat);
    }

    // Constructor
    SnapshotData() : m_tmp_cpu{0.}, m_delta_cpu{0.}, m_tmp_wall{0.}, m_delta_wall{0.} {
      m_memMonTmpMap["vmem"] = 0; m_memMonTmpMap["pss"] = 0; m_memMonTmpMap["rss"] = 0; m_memMonTmpMap["swap"] = 0;
      m_memMonDeltaMap["vmem"] = 0; m_memMonDeltaMap["pss"] = 0; m_memMonDeltaMap["rss"] = 0; m_memMonDeltaMap["swap"] = 0;
    }

  }; // End SnapshotData

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
inline MemoryMap_t PMonMT::get_mem_stats() {
  // Result object
  MemoryMap_t result;

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

inline MemoryMap_t operator-(const MemoryMap_t& map1, const MemoryMap_t& map2) {
  MemoryMap_t result_map;
  for (auto it : map1) {
    result_map[it.first] = map1.at(it.first) - map2.at(it.first);
  }
  return result_map;
}

/*
 * Simple check if a given directory exists
 */
inline bool PMonMT::doesDirectoryExist(const std::string& dir) {
  struct stat buffer;
  return (stat(dir.c_str(), &buffer) == 0);
}

#endif  // PERFMONCOMPS_PERFMONMTUTILS_H

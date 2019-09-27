/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*
 * @authors: Alaettin Serhan Mete, Hasan Ozturk - alaettin.serhan.mete@cern.ch, haozturk@cern.ch
 */


#ifndef PERFMONCOMPS_PERFMONMTUTILS_H
#define PERFMONCOMPS_PERFMONMTUTILS_H

// STL includes
#include <ctime>
#include <chrono>
#include <fstream>

#include <sys/stat.h>  // to check whether /proc/* exists in the machine

// Following includes enables the usage of gettid() system call
#include <unistd.h>
#include <sys/syscall.h>
#define gettid() syscall(SYS_gettid)

typedef std::map< std::string, long > memory_map_t; // e.g. Vmem : 1200(kB)

/*
 * Inline function prototypes
*/
inline memory_map_t operator-( memory_map_t& map1,  memory_map_t& map2);
inline bool isDirectoryExist(const std::string dir);

/*
 * Necessary tools
 */
namespace PMonMT {
  // Base methods for collecting data
  double get_thread_cpu_time();
  double get_process_cpu_time();
  double get_wall_time();
  memory_map_t get_mem_stats();
  

  // Step name and Component name pairs. Ex: Initialize - StoreGateSvc
  struct StepComp {
  
    std::string stepName;
    std::string compName;
  
    // Overload < operator, because we are using a custom key(StepComp)  for std::map
    bool operator<(const StepComp& sc) const {
      return std::make_pair( this->stepName, this->compName ) < std::make_pair( sc.stepName, sc.compName );
    }
  
  };
                                   
  // Step Name, Component name and event number triple. e.g. Execute - StoreGateSvc - 3 
  struct StepCompEvent {
  
    std::string stepName;
    std::string compName;
    int eventNumber;
  
    //Overload < operator, because we are using a custom key(StepCompEvent)  for std::map
    bool operator<(const StepCompEvent& sce) const {
      return std::make_pair( this->eventNumber, this->compName ) < std::make_pair( sce.eventNumber, sce.compName );
    }
  };
  

  // Basic Measurement
  struct Measurement {

    // These two variables stores the measurements captured outside the event loop
    double cpu_time;
    double wall_time;

    memory_map_t mem_stats; // Vmem, Rss, Pss, Swap
    
    // This map stores the measurements captured in the event loop
    std::map< StepCompEvent, Measurement > parallel_meas_map;

    long vmemPeak = LONG_MIN;
    long rssPeak = LONG_MIN;
    long pssPeak = LONG_MIN;

    void capture() {
      cpu_time = get_process_cpu_time();
      wall_time = get_wall_time();

      if(isDirectoryExist("/proc")){
        mem_stats = get_mem_stats();
        if(mem_stats["vmem"] > vmemPeak)
          vmemPeak = mem_stats["vmem"];
        if(mem_stats["rss"] > rssPeak)
          rssPeak = mem_stats["rss"];
        if(mem_stats["pss"] > pssPeak)
          pssPeak = mem_stats["pss"];
      }
        
    }

    void capture_MT ( StepCompEvent sce ) {
           
      cpu_time = get_thread_cpu_time();
      wall_time = get_wall_time(); 

      Measurement meas;

      if(isDirectoryExist("/proc")){
        mem_stats = get_mem_stats();
        meas.mem_stats = mem_stats;

        if(mem_stats["vmem"] > vmemPeak)
          vmemPeak = mem_stats["vmem"];
        if(mem_stats["rss"] > rssPeak)
          rssPeak = mem_stats["rss"];
        if(mem_stats["pss"] > pssPeak)
          pssPeak = mem_stats["pss"];
      }
      
      meas.cpu_time = cpu_time;
      meas.wall_time = wall_time;

      parallel_meas_map[sce] = meas;
    }



    Measurement() : cpu_time{0.}, wall_time{0.} { }
  };

  // Basic Data
  struct MeasurementData {

    // These variables are used to calculate the serial monitoring
    double m_tmp_cpu, m_delta_cpu;
    double m_tmp_wall, m_delta_wall;
    
    memory_map_t m_memMon_tmp_map;
    memory_map_t m_memMon_delta_map;

    // These maps are used to calculate the parallel monitoring
    std::map< StepCompEvent, Measurement > m_parallel_tmp_map;
    std::map< StepCompEvent, Measurement > m_parallel_delta_map;    
    
    void addPointStart(const Measurement& meas) {          

      m_tmp_cpu = meas.cpu_time;
      m_tmp_wall = meas.wall_time;
      
      if(isDirectoryExist("/proc"))
        m_memMon_tmp_map = meas.mem_stats;
    }
    // make const
    void addPointStop(Measurement& meas)  {     

      m_delta_cpu = meas.cpu_time - m_tmp_cpu;
      m_delta_wall = meas.wall_time - m_tmp_wall;

      if(isDirectoryExist("/proc"))
        m_memMon_delta_map = meas.mem_stats - m_memMon_tmp_map;   
    }

    // Clear -> Make generic + make meas const
    void addPointStart_MT(Measurement& meas, StepCompEvent sce ){

      m_parallel_tmp_map[sce] = meas.parallel_meas_map[sce];
    }

    void addPointStop_MT (Measurement& meas, StepCompEvent sce  ){

      m_parallel_delta_map[sce].cpu_time = meas.parallel_meas_map[sce].cpu_time - m_parallel_tmp_map[sce].cpu_time;
      m_parallel_delta_map[sce].wall_time = meas.parallel_meas_map[sce].wall_time - m_parallel_tmp_map[sce].wall_time;

      if(isDirectoryExist("/proc")){
        m_parallel_delta_map[sce].mem_stats["vmem"] = meas.parallel_meas_map[sce].mem_stats["vmem"] - m_parallel_tmp_map[sce].mem_stats["vmem"];
        m_parallel_delta_map[sce].mem_stats["rss"] = meas.parallel_meas_map[sce].mem_stats["rss"] - m_parallel_tmp_map[sce].mem_stats["rss"];
        m_parallel_delta_map[sce].mem_stats["pss"] = meas.parallel_meas_map[sce].mem_stats["pss"] - m_parallel_tmp_map[sce].mem_stats["pss"];
        m_parallel_delta_map[sce].mem_stats["swap"] = meas.parallel_meas_map[sce].mem_stats["swap"] - m_parallel_tmp_map[sce].mem_stats["swap"];
      }
    }

    
    MeasurementData() : m_tmp_cpu{0.}, m_delta_cpu{0.}, m_tmp_wall{0.}, m_delta_wall{0.} { }
  };
  
}


///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

/*
 * Thread specific CPU time measurement in ms
 */
inline double PMonMT::get_thread_cpu_time() {
  // Get the thread clock id
  clockid_t thread_cid;
  pthread_getcpuclockid(pthread_self(),&thread_cid);

  // Get the thread specific CPU time
  struct timespec ctime;
  clock_gettime(thread_cid, &ctime);

  // Return the measurement in ms
  return static_cast<double>(ctime.tv_sec*1.e3 + ctime.tv_nsec*1.e-6);
}

/*
 * Process specific CPU time measurement in ms
 */
inline double PMonMT::get_process_cpu_time() {
  return static_cast<double>(std::clock()*(1.e3/CLOCKS_PER_SEC));
}

/*
 * Wall-time measurement since epoch in ms
 */
inline double PMonMT::get_wall_time() {
  return static_cast<double>(std::chrono::system_clock::now().time_since_epoch() /
                             std::chrono::milliseconds(1));
}

/*
 * Memory statistics for serial steps
 */
inline memory_map_t PMonMT::get_mem_stats(){

  memory_map_t result;
  pid_t tid = gettid(); // get kernel thread id
  std::string fileName = "/proc/self/task/" + std::to_string(tid) + "/smaps";
  std::ifstream smaps_file(fileName); 
 
  std::string line;
  std::string key;
  std::string value;

  while(getline(smaps_file, line)){

    std::stringstream ss(line);
    ss >> key >> value;

    if(key == "Size:"){
      result["vmem"] += stol(value);
    }
    if(key == "Rss:"){
      result["rss"] += stol(value);
    }
    if(key == "Pss:"){
      result["pss"] += stol(value);
    }
    if(key == "Swap:"){
      result["swap"] += stol(value);
    }

  }
  return result; 
}

inline memory_map_t operator-( memory_map_t& map1,  memory_map_t& map2){
  memory_map_t result_map;
  for(auto it : map1){
    result_map[it.first] = map1[it.first] - map2[it.first];
  }
  return result_map;
}


inline bool isDirectoryExist(const std::string dir){
  struct stat buffer;
  return (stat (dir.c_str(), &buffer) == 0);
}


#endif // PERFMONCOMPS_PERFMONMTUTILS_H

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PERFMONCOMPS_PERFMONMTUTILS_H
#define PERFMONCOMPS_PERFMONMTUTILS_H

// STL includes
#include <pthread.h>
#include <time.h>
#include <ctime>

// Necessary tools 
namespace PMonMT {

  // Base methods for collecting data
  double get_thread_cpu_time();
  double get_process_cpu_time();

  // Basic Measurement
  struct Measurement {
    double cpu_time;
    void capture() {
      cpu_time = get_thread_cpu_time();
    }
    Measurement() : cpu_time(-1) { }
  };

}

// Thread specific CPU time measurements
inline double PMonMT::get_thread_cpu_time() {

  // Get the thread clock id
  clockid_t thread_cid;  
  pthread_getcpuclockid(pthread_self(),&thread_cid);

  // Get the thread specific CPU time
  struct timespec ctime; 
  clock_gettime(thread_cid, &ctime);

  // Return the measurement in ms
  return double(ctime.tv_sec*1.e3 + ctime.tv_nsec*1.e-6);
} 

// Process specific CPU time measurements
inline double PMonMT::get_process_cpu_time() {
  // Simply return process CPU-time [precision is seconds here]
  return std::clock()/CLOCKS_PER_SEC*1.e3;
}

#endif // PERFMONCOMPS_PERFMONMTUTILS_H

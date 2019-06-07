/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PERFMONCOMPS_PERFMONMTUTILS_H
#define PERFMONCOMPS_PERFMONMTUTILS_H

// STL includes
#include <pthread.h>
#include <ctime>
#include <chrono>

/*
 * Necessary tools
 */
namespace PMonMT {
  // Base methods for collecting data
  double get_thread_cpu_time();
  double get_process_cpu_time();
  double get_wall_time();

  // Basic Measurement
  struct Measurement {
    double cpu_time;
    double wall_time;
    void capture() {
      cpu_time = get_process_cpu_time();
      wall_time = get_wall_time();
    }
    Measurement() : cpu_time{0.}, wall_time{0.} { }
  };

  // Basic Data
  struct MeasurementData {
    double m_tmp_cpu, m_delta_cpu;
    double m_tmp_wall, m_delta_wall;
    void addPointStart(const Measurement& meas) {
      m_tmp_cpu = meas.cpu_time;
      m_tmp_wall = meas.wall_time;
    }
    void addPointStop(const Measurement& meas)  {
      m_delta_cpu = meas.cpu_time - m_tmp_cpu;
      m_delta_wall = meas.wall_time - m_tmp_wall;
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

#endif // PERFMONCOMPS_PERFMONMTUTILS_H

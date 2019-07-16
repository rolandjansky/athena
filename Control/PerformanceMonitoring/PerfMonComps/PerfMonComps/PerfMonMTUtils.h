/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*
 * @authors: Alaettin Serhan Mete, Hasan Ozturk - alaettin.serhan.mete@cern.ch, haozturk@cern.ch
 */


#ifndef PERFMONCOMPS_PERFMONMTUTILS_H
#define PERFMONCOMPS_PERFMONMTUTILS_H

// STL includes
//#include <pthread.h>
#include <ctime>
#include <chrono>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

//#include "AthenaBaseComps/AthMessaging.h"

//#include <mutex>

/*
 * Necessary tools
 */
namespace PMonMT {
  // Base methods for collecting data
  double get_thread_cpu_time();
  double get_process_cpu_time();
  double get_wall_time();

  pthread_t get_thread_id();

  // Step name and Component name pairs. Ex: Initialize - StoreGateSvc
  struct StepComp {
  
    std::string stepName;
    std::string compName;
  
    // Overload < operator, because we are using a custom key(StepComp)  for std::map
    bool operator<(const StepComp& sc) const {
      return std::make_pair( this->stepName, this->compName ) < std::make_pair( sc.stepName, sc.compName );
    }
  
  };
                                   
  // Step Name, Component name and event number triple. e.g. Execute - StoreGateSvc - 3    Clear!
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
    double cpu_time;
    double wall_time;
    
    // Clear -> get rid of pair
    std::map< StepCompEvent, std::pair< double, double > > shared_measurement_map;

    std::map< StepCompEvent, int > thread_id_map;


    void capture() {
      cpu_time = get_process_cpu_time();
      wall_time = get_wall_time();

      /* Log to stdout
      IMessageSvc *msgSvc;
      MsgStream msg( msgSvc, "PerfMonMTUtils" );
      msg << MSG::INFO << std::fixed  <<  "capture: CPU: " << cpu_time << ", Wall: " << wall_time  << endmsg;
      */
      
    }

    // Could we make it argumentless?
    void capture_MT ( StepCompEvent sce ) {
      

     
      cpu_time = get_thread_cpu_time();
      wall_time = get_wall_time(); // Does it really needed?

      shared_measurement_map[ sce ] = std::make_pair(cpu_time,wall_time);
      
      thread_id_map[sce] = get_thread_id();

    }

    Measurement() : cpu_time{0.}, wall_time{0.} { }
  };

  // Basic Data
  struct MeasurementData {
    double m_tmp_cpu, m_delta_cpu;
    double m_tmp_wall, m_delta_wall;


    // Clear -> get rid of pair
    std::map< StepCompEvent, std::pair< double, double > > shared_measurement_tmp_map;

    std::map< StepCompEvent, std::pair< double, double > > shared_measurement_delta_map;
        
   
    std::map< StepCompEvent, int  > thread_id_tmp_map;
    std::map< StepCompEvent, int  > thread_id_delta_map; 
    

    void addPointStart(const Measurement& meas) {          

      m_tmp_cpu = meas.cpu_time;
      m_tmp_wall = meas.wall_time;
     
      /* Log to stdout
      IMessageSvc *msgSvc;
      MsgStream msg( msgSvc, "PerfMonMTUtils" );
      msg << MSG::INFO << "addPointStart: CPU: " << m_tmp_cpu << ", Wall: " << m_tmp_wall  << endmsg;
      */
    }
    void addPointStop(const Measurement& meas)  {     

      m_delta_cpu = meas.cpu_time - m_tmp_cpu;
      m_delta_wall = meas.wall_time - m_tmp_wall;

      /* Log to stdout
      IMessageSvc *msgSvc;
      MsgStream msg( msgSvc, "PerfMonMTUtils" );
      msg << MSG::INFO << "addPointStop: delta_CPU: " << meas.cpu_time << " - " << m_tmp_cpu << " = " << m_delta_cpu  << endmsg;
      msg << MSG::INFO << "addPointStop: delta_Wall: " << meas.wall_time << " - " << m_tmp_wall << " = " << m_delta_wall  << endmsg;
      */
    }

    // Clear -> Make generic + make meas const
    void addPointStart_MT(Measurement& meas, StepCompEvent sce ){

      shared_measurement_tmp_map[sce] = meas.shared_measurement_map[sce];

      thread_id_tmp_map[sce] = meas.thread_id_map[sce];

    }

    void addPointStop_MT (Measurement& meas, StepCompEvent sce  ){

      shared_measurement_delta_map[sce].first = meas.shared_measurement_map[sce].first - shared_measurement_tmp_map[sce].first;
      shared_measurement_delta_map[sce].second = meas.shared_measurement_map[sce].second - shared_measurement_tmp_map[sce].second;

      thread_id_delta_map[sce] = meas.thread_id_map[sce] - thread_id_tmp_map[sce];

      /* Log to stdout
      IMessageSvc *msgSvc;
      MsgStream msg( msgSvc, "PerfMonMTUtils" );
      msg << MSG::INFO << "addPointStop_thread: delta_CPU: " << meas.shared_measurement_map[sce].first << " - " << shared_measurement_tmp_map[sce].first << " = " << shared_measurement_delta_map[sce].first  << endmsg;
      
      msg << MSG::INFO << "addPointStop_thread: delta_CPU: " << meas.shared_measurement_map[sce].second << " - " << shared_measurement_tmp_map[sce].second << " = " << shared_measurement_delta_map[sce].second  << endmsg;
      */
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

inline pthread_t PMonMT::get_thread_id(){

  return pthread_self();

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

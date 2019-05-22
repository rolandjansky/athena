/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PERFMONCOMPS_PERFMONMTSVC_H
#define PERFMONCOMPS_PERFMONMTSVC_H

// STL includes
#include <pthread.h>
#include <time.h>
#include <ctime>

// Framework includes
#include "AthenaBaseComps/AthService.h"

// PerfMonKernel includes
#include "PerfMonKernel/IPerfMonMTSvc.h"


class PerfMonMTSvc : virtual public IPerfMonMTSvc, 
                     public AthService
{

  public:

    /// Standard Gaudi Service constructor  
    PerfMonMTSvc( const std::string& name, ISvcLocator* pSvcLocator );

    /// Function declaring the interface(s) implemented by the service
    virtual StatusCode queryInterface( const InterfaceID& riid, 
                                       void** ppvInterface ) override;

    /// Standard Gaudi Service initialization 
    virtual StatusCode initialize() override;

    /// Standard Gaudi Service finalization
    virtual StatusCode finalize() override;

    /// Start Auditing 
    virtual void startAud( const std::string& stepName, 
                           const std::string& compName ) override;

    /// Stop Auditing   
    virtual void stopAud ( const std::string& stepName, 
                           const std::string& compName ) override;
/*
  private:

    /// Measurement to caputre CPU time
    PMonMT::Measurement m_measurement;
*/
}; // class PerfMonMTSvc

// Necessary tools 
namespace PMonMT {
/*
  // Basic Measurement
  struct Measurement {
    double cpu_time;
    void capture() {
      cpu_time = get_cpu_time();
    }
    Measurement() : cpu_time(-1) { }
  };
*/
  // CPU time measurements
  inline double get_cpu_time() {
    // Get the thread clock id
    clockid_t thread_cid;  
    pthread_getcpuclockid(pthread_self(),&thread_cid);

    // Get the thread specific CPU time
    struct timespec ctime; 
    clock_gettime(thread_cid, &ctime);

    // Return the measurement in ms
    return double(ctime.tv_sec*1.e3 + ctime.tv_nsec*1.e-6);
  } 

  inline double get_process_cpu_time() {
    // Simply return process CPU-time [precision is seconds here]
    return std::clock()/CLOCKS_PER_SEC*1.e3;
  }

}

#endif // PERFMONCOMPS_PERFMONMTSVC_H

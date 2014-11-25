/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECALGS_TIMINGALG_H
#define RECALGS_TIMINGALG_H

// Gaudi includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include <string>
#include <sys/time.h>
#include <sys/resource.h>

/** @class TimingAlg
    
Several of the timing algs can be scheduled.
Each one adds the sum of user and system time 
from a getrusage call to the vector.

@author Rolf Seuster <seuster AT cern.ch>
*/

class TimingAlg : public AthAlgorithm
{
 public:
  
  /** Standard Athena-Algorithm Constructor */
  TimingAlg(const std::string& name, ISvcLocator* pSvcLocator);
  
  /** Default Destructor */
  ~TimingAlg() {};
  
  /** standard Athena-Algorithm method */
  StatusCode          initialize();
  /** standard Athena-Algorithm method */
  StatusCode          execute();
  /** standard Athena-Algorithm method */
  StatusCode          finalize()
  { return StatusCode::SUCCESS; };
  
 private:
  
  /** member variables for algorithm properties: */
  std::string  m_timingObjOutputName;
  
  bool m_determineCPUID;
  
  static unsigned int m_CPUID;
  unsigned int m_CPUID_a;
  unsigned int m_CPUID_b;
};

#endif 

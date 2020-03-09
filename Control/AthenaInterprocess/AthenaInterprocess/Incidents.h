/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAINTERPROCESS_INCIDENTS_H
#define ATHENAINTERPROCESS_INCIDENTS_H

#include "GaudiKernel/Incident.h"
#include "GaudiKernel/ThreadLocalContext.h"

/**
 * @class  UpdateAfterFork
 * @brief  Incident to signal that a new worker was forked
 *
 * This incident is fired by AthenaMP event processors to signal that a new worker
 * was forked and parameters can be updated before event processing starts.
 * It carries the "worker" ID and the worker process ID which can be used to
 * to create individualized names/parameters
 */

namespace AthenaInterprocess {
  class UpdateAfterFork: public Incident {
  public:   
    /**
     * Constructor
     * @param worker ID          worker "rank ID"
     * @param worker process ID  the process ID of the worker from the OS
     * @param source             sender of the incident
     */
    UpdateAfterFork(int workerID,
                    int workerProcessID,
                    const std::string& source,
                    const EventContext& ctx = Gaudi::Hive::currentContext()) :
    Incident(source, type(), ctx), m_workerID(workerID), m_workerProcessID(workerProcessID) {};
    
    /// Incident type
    static const std::string& type();
    
    /// assigned worker ID from processing unit
    int workerID() const { return m_workerID; }
    /// process ID of this worker from OS
    int workerProcessID() const { return m_workerProcessID; }
    
  private:
    int m_workerID;             //!< the assigned worker ID
    int m_workerProcessID;      //!< the assigned worker process ID from the OS
  };
  
  inline const std::string& UpdateAfterFork::type()
  {
    static const std::string type("UpdateAfterFork");
    return type;
  }
}
#endif

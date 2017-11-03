/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINTERFACES_INCIDENTS_H
#define TRIGINTERFACES_INCIDENTS_H

/**
 * @file   TrigInterfaces/Incidents.h
 * @brief  Incident classes used in HLT
 * @author Frank Winklmeier
 *
 * $Id: Incidents.h,v 1.1 2008-12-03 17:24:52 fwinkl Exp $
 */

// Gaudi includes
#include "GaudiKernel/Incident.h"

/**
 * @namespace HLT::Incidents
 * @brief HLT incident classes
 *
 * Every incident is derived from Gaudi's Incident class. Moreover,
 * the method type() is statically overloaded to return the type
 * of the incident (usually the same string as the class name).
 * This avoids the use of hardcoded strings in Incident sender and
 * listener. Typically, you would use them as follows:
 *
 * Sender:
 * \code
 * incSvc->fireIncident(Incidents::EventTimeout())
 * \endcode
 *
 * Listener:
 * \code
 * void MyAlgorithm::handle(const Incident& inc)
 * {
 *   if ( inc.type()==Incidents::EventTimeout::type() ) {...}
 * }
 * \endcode
 */

namespace HLT { namespace Incidents {

  /**
   * @class  EventTimeout
   * @brief  Event timeout incident
   *
   * This incident is fired by the PSC to indicate an event timeout
   * during online running. Only the steering should subscribe to
   * this incident since it requires a thread-safe incident handler.
   */
  class EventTimeout: public Incident {
  public:
    /**
     * Constructor
     * @param source    sender of the incident
     */
    EventTimeout(const std::string& source) :
      Incident(source, type()) {};

    /// Incident type
    static const std::string& type();
  };
  

  /**
   * @class  UpdateHLTPrescales
   * @brief  Incident to signal update of HLT prescales
   *
   * This incident is fired by the PSC to signal that the HLT prescales
   * have to be reloaded. It carries the "super lumi block" ID that is
   * incremented by the CTP for each update.
   */
  class UpdateHLTPrescales: public Incident {
  public:    
    /**
     * Constructor
     * @param super_lumi_block   super lumi block number
     * @param source             sender of the incident
     */
    UpdateHLTPrescales(unsigned int super_lumi_block,
                       const std::string& source) :
      Incident(source, type()), m_superLumiBlock(super_lumi_block) {};

    /// Incident type
    static const std::string& type();

    /// super lumi block number
    unsigned int super_lumi_block() const { return m_superLumiBlock; }

  private:
    unsigned int m_superLumiBlock;      //!< super lumi block ID
  };


  /**
   * @class  UpdateAfterFork
   * @brief  Incident to signal that a new worker was forked
   *
   * This incident is fired by the PSC to signal that a new worker
   * was forked and parameters can be updated before event processing starts. 
   * It carries the "worker" ID and the worker process ID which can be used to
   * to create individualized names/parameters
   */
  class UpdateAfterFork: public Incident {
  public:    
    /**
     * Constructor
     * @param worker ID          the number which is assigned to the worker by the processing unit
     * @param worker process ID  the process ID of the worker from the OS
     * @param source             sender of the incident
     */
    UpdateAfterFork(int workerID, int workerProcessID,
		    const std::string& source) :
      Incident(source, type()), m_workerID(workerID), m_workerProcessID(workerProcessID) {};

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


  
  //<<<<<<<<<<< INLINE MEMBER FUNCTIONS

  inline
  const std::string& EventTimeout::type()
  {
    static const std::string typ("EventTimeout");
    return typ;
  }
  
  inline
  const std::string& UpdateHLTPrescales::type()
  {
    static const std::string typ("UpdateHLTPrescales");
    return typ;
  }

  inline
  const std::string& UpdateAfterFork::type()
  {
    static const std::string typ("UpdateAfterFork");
    return typ;
  }

  //<<<<<<<<<<< INLINE MEMBER FUNCTIONS

  
}} // namespace HLT::Incidents

#endif

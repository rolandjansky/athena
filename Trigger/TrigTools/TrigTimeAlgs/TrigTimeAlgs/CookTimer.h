/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTIMEALGS_COOKTIMER_H
#define TRIGTIMEALGS_COOKTIMER_H

/**
 * @file   TrigTimeAlgs/CookTimer.h
 * @brief  Some specialized classes of Athena::AlgorithmTimer
 * @author Frank Winklmeier
 *
 * $Id: CookTimer.cxx,v 1.1 2008-12-10 15:29:00 fwinkl Exp $
 */

// Framework includes
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/AlgorithmTimer.h"

// STL includes
#include <string>

namespace HLT {

  /**
   * @class AbortingCookTimer
   * @brief Timer that aborts the job once the time is reached
   *
   * Before the job is aborted the CoreDumpSvc is asked to print its
   * current core dump records and finally a gdb traceback is produced.
   */
  class AbortingCookTimer: public Athena::AlgorithmTimer {
  public:
    /**
     * @brief Default Constructor; create disabled (anonymous) timer
     */
    AbortingCookTimer();
    
    /**
     * @brief Constructor; create and start a new timer
     * @param milliseconds Time in milliseconds until expiration of the timer (0 = disabled)
     */
    AbortingCookTimer(unsigned int milliseconds);
    
  private:
    void abortJob();    
  };

  
  class IncidentTimer: public Athena::AlgorithmTimer {
  public:
     /**
     * @brief Constructor; create and start a new timer
     * @param milliseconds  Time in milliseconds until expiration of the timer (0 = disabled)
     * @param incident      Name of the Incident to be fired
     */
    IncidentTimer(unsigned int milliseconds, const std::string& incident);

  private:
    std::string                 m_name;    ///< name of the incident to fire
    ServiceHandle<IIncidentSvc> m_incSvc;  ///< IncidentSvc
    void fireIncident();        ///< Method called by thread deliverying signal    
    IncidentTimer();            ///< no default constructor
  };

}

#endif

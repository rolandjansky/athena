/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TrigPSC/Utils.h
 * @author Frank Winklmeier
 * $Author: ricab $
 * $Revision: 11 $
 * $Date: 2013-05-14 17:22:39 +0200 (Tue, 14 May 2013) $
 *
 * @brief Some helpers for the PSC
 */

#ifndef PSC_UTILS_H
#define PSC_UTILS_H

#include <vector>
#include <string>
#include <sys/time.h>

namespace psc {
  
  namespace Utils {

    /**
     * @brief  Execute a python command in the python interpreter
     * @param  pyCmd python statement
     * @return Success/Failure
     */
    bool execPython (const std::string& pyCmd);
    
    /**
     * @brief  Execute a set of python commands in the python interpreter
     * @param  pyCmds vector of python statements
     * @return Success/Failure
     */
    bool execPython (const std::vector<std::string> pyCmds);

    /**
     * @brief  Include a python file via AthenaCommon.Include
     * @param  pyFileName File name
     * @param  showIncludes Trace included files
     * @return Success/Failure
     */
    bool pyInclude (const std::string& pyFileName);
    
    /**
     * @brief  Very simple timer class
     *
     * Time between constructor and destructor call is measured and printed
     */
    class ScopeTimer {
    public:
      /**
       * Create timer and start timing
       */
      ScopeTimer (const std::string& descr = "Timer");

      /**
       * Stop timer (if still running)
       */
      ~ScopeTimer() { if (m_running) stop(); }

      /**
       * Stop timer manually
       */
      void stop();
            
    private:
      std::string     m_descr;     ///< description of timer
      struct timeval  m_t1;        ///< start time
      bool            m_running;   ///< timer running?
    };
    
  } /* namespace Utils */  
} /* namespace psc */


#endif /* PSC_UTILS_H */

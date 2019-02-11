/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TrigPSC/src/Utils.cxx
 * @author Frank Winklmeier
 *
 * @brief Some helpers for the PSC
 */

#include "TrigPSC/Utils.h"
#include "TrigPSC/PscIssues.h"
#include "PathResolver/PathResolver.h"

#undef _POSIX_C_SOURCE
#include <Python.h>
#include <errno.h>

//--------------------------------------------------------------------------------
// Python helpers
//--------------------------------------------------------------------------------

bool psc::Utils::execPython (const std::string& pyCmd)
{
  ERS_DEBUG(1, pyCmd );  
  if ( PyRun_SimpleString(const_cast<char*>(pyCmd.c_str())) != 0 ) {
    ERS_PSC_ERROR("Error executing " << pyCmd);
    return false;
  }
  return true;
}


bool psc::Utils::execPython (const std::vector<std::string> pyCmds)
{
  std::vector<std::string>::const_iterator iter;  
  for (iter = pyCmds.begin(); iter != pyCmds.end(); ++iter) {
    if ( !psc::Utils::execPython(*iter) ) return false;
  }
  return true;
}


bool psc::Utils::pyInclude (const std::string& pyFileName)
{
  std::ostringstream oss;
  oss << "from AthenaCommon.Include import include" << std::endl;
  oss << "include(\"" << pyFileName << "\")";
  return psc::Utils::execPython(oss.str());
}


//--------------------------------------------------------------------------------
// ScopeTimer class
//--------------------------------------------------------------------------------

psc::Utils::ScopeTimer::ScopeTimer (const std::string& descr) :
  m_descr(descr),
  m_running(true)
{
  std::tm lt;
  m_t1 = std::chrono::system_clock::now();
  auto t = std::chrono::system_clock::to_time_t(m_t1);
  localtime_r(&t, &lt);
  ERS_LOG( m_descr << " started at time: " << std::put_time(&lt, "%Y-%m-%d %H:%M:%S") );
}

void psc::Utils::ScopeTimer::stop()
{
  auto t2 = std::chrono::system_clock::now();
  auto dt_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - m_t1);
  ERS_LOG( m_descr << " finished. Time used [ms] = " << dt_ms.count() );
  m_running = false;
}

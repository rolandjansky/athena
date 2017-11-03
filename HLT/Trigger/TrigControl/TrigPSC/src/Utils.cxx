/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TrigPSC/src/Utils.cxx
 * @author Frank Winklmeier
 * $Author: ricab $
 * $Revision: 11 $
 * $Date: 2013-05-14 17:22:39 +0200 (Tue, 14 May 2013) $
 *
 * @brief Some helpers for the PSC
 */

#include "TrigPSC/Utils.h"
#include "TrigPSC/PscIssues.h"
#include "PathResolver/PathResolver.h"

#undef _POSIX_C_SOURCE
#include <Python.h>
#include <sys/time.h>
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
  gettimeofday(&m_t1, 0);

  // Format time
  char buf[64];
  struct tm tms;
  time_t sec = m_t1.tv_sec;  
  localtime_r(&sec, &tms);  
  strftime(buf, 64, "%Y-%m-%d %H:%M:%S", &tms);

  // Print and append milliseconds
  ERS_LOG( m_descr << " started at time: " << buf
           << "," << static_cast<unsigned int>(m_t1.tv_usec/1000) );
}

void psc::Utils::ScopeTimer::stop()
{
  struct timeval t2;
  gettimeofday(&t2, 0);

  int secs = 0 ;
  if (t2.tv_sec >= m_t1.tv_sec)
    secs = t2.tv_sec - m_t1.tv_sec;

  int usecs = t2.tv_usec - m_t1.tv_usec;
  float mtime = static_cast<float>(secs)*1000 + static_cast<float>(usecs)/1000;
  ERS_LOG( m_descr << " finished. Time used [ms] = " << mtime );
}

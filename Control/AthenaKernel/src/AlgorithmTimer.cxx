/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   TrigTimeAlgs/AlgorithmTimer.cxx
 * @brief  Efficient realtime timers
 * @author Frank Winklmeier (based on AthenaKernel/Algorithmtimer by W. Lavrijsen, S. Binet)
 *
 * $Id: AlgorithmTimer.cxx,v 1.3 2009-05-14 16:58:21 dquarrie Exp $
 */

#include "AthenaKernel/AlgorithmTimer.h"

// Gaudi includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/Incident.h"

// Athena includes
#include "AthenaKernel/ICoreDumpSvc.h"
#include "AthenaKernel/getMessageSvc.h"

// #include "Utils.h"
#include <dlfcn.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>

// STL includes
#include <iostream>
#include <sstream>
#include <algorithm>
#include <string>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#ifdef __APPLE__
#endif

using namespace Athena;

// Statics
bool                        AlgorithmTimer::s_handlerInstalled = false;
struct sigaction            AlgorithmTimer::s_oldSigHandler;
std::list<AlgorithmTimer*>  AlgorithmTimer::s_registry;

namespace Athena {
  namespace AlgorithmTimerHandler
  {
    /**
     * @brief  Signal handler for AlgorithmTimer
     *
     * This is the signal handler registered by the first instance of AlgorithmTimer.
     * On invocation the siginfo_t struct carries a pointer to the actual
     * instance. The onAlarm function is made a friend of AlgorithmTimer, therefore
     * it can access all relevant information.
     */
    void onAlarmSignal(int sig, siginfo_t *info, void* extra)
    {
      /*
       * Check if the signal is coming from a AlgorithmTimer instance.
       */
      AlgorithmTimer* me = static_cast<AlgorithmTimer*>(info->si_value.sival_ptr);
      std::list<AlgorithmTimer*>::iterator iter = find(AlgorithmTimer::s_registry.begin(),
                                                       AlgorithmTimer::s_registry.end(),
                                                       me);
    
      /*
       * If signal from AlgorithmTimer, call user-defined callback
       */
      if ( iter != AlgorithmTimer::s_registry.end() )
        {
          std::cout.flush();
          std::cerr.flush();
          me->m_onAlarm();
        }
    
      /*
       * Call old signal handler
       * Need to distinguish between the two types of signal handlers
       */
      if (AlgorithmTimer::s_oldSigHandler.sa_flags & SA_SIGINFO)
        {
          if (AlgorithmTimer::s_oldSigHandler.sa_sigaction)
            AlgorithmTimer::s_oldSigHandler.sa_sigaction(sig,info,extra);
        }
      else
        {
          if (AlgorithmTimer::s_oldSigHandler.sa_handler)
            AlgorithmTimer::s_oldSigHandler.sa_handler(sig);
        }
    }
  
    /**
     * @brief  Function called by signals delivered via threads
     */
    void onAlarmThread(sigval_t sv)
    {
      AlgorithmTimer* me = static_cast<AlgorithmTimer*>(sv.sival_ptr);
      me->m_onAlarm();
    }
  }
}

//--------------------------------------------------------------------------------
// AlgorithmTimer
//--------------------------------------------------------------------------------

AlgorithmTimer::AlgorithmTimer(unsigned int milliseconds,
			       callbackFct_t callback,
			       AlgorithmTimerConfig conf) :
  m_timeout (0),
  m_timerid(),
  m_onAlarm(callback),
  m_gdb_details (0)
{  
  // prevent valgrind warning
  // new warning popped up, needs new glibc for this, see http://bugs.kde.org/show_bug.cgi?id=124478
  // possibly valgrind will be fixed, eventually.
  std::memset (&m_sigevent, 0, sizeof (m_sigevent));

  m_sigevent.sigev_value.sival_ptr = this;

  if (m_onAlarm == NULL)
    m_onAlarm = boost::bind(&AlgorithmTimer::abortJob,this);
  
  if ( conf & DELIVERYBYTHREAD ) {
    m_sigevent.sigev_notify = SIGEV_THREAD;
    m_sigevent.sigev_signo  = 0;
    m_sigevent.sigev_notify_function = AlgorithmTimerHandler::onAlarmThread;
    m_sigevent.sigev_notify_attributes = NULL;
  } else {
    m_sigevent.sigev_notify = SIGEV_SIGNAL;
    m_sigevent.sigev_signo  = SIGNO;
    
    /*
     * The signal handler uses this list to make sure the signal
     * indeed came from an AlgorithmTimer and not from somewhere else.
     */
    s_registry.push_back(this);
    
    /*
     * Use the same signal handler for all AlgorithmTimer instances.
     * For performance reasons the signal handler stays active
     * even after all AlgorithmTimer instances are deleted.
     */   
    if ( !s_handlerInstalled )
      installSignalHandler();
    
  }
  
  // Create the timer
  if ( conf & USEREALTIME )
    {
#ifndef __APPLE__
      timer_create(CLOCK_REALTIME, &m_sigevent, &m_timerid);
#endif
    }
  else
    {
      /*
       * can use virtual time only, if defined on this system
       */
#ifndef __APPLE__
#ifdef _POSIX_CPUTIME
      timer_create(_POSIX_CPUTIME, &m_sigevent, &m_timerid);
#else
      timer_create(CLOCK_REALTIME, &m_sigevent, &m_timerid);
      std::cerr << "\nNo _POSIX_CPUTIME defined on this system !\n\n";
      // we could also disable this timer in this case !
      // seconds = 0;
#endif
#endif
    }
  // Start the timer if requested
  if(milliseconds)
    start(milliseconds);
}

AlgorithmTimer::~AlgorithmTimer()
{
#ifndef __APPLE__
  timer_delete(m_timerid);
#endif
  s_registry.remove(this);
}

void AlgorithmTimer::start(unsigned int milliseconds)
{
  // Set the timer
  m_timeout = milliseconds;
#ifndef __APPLE__
  itimerspec spec;
  spec.it_value.tv_sec     = int(m_timeout/1000);
  spec.it_value.tv_nsec    = 1000000*(m_timeout%1000);
  spec.it_interval.tv_sec  = 0;
  spec.it_interval.tv_nsec = 0;
  
  timer_settime(m_timerid, 0, &spec, NULL);
#endif
}

void AlgorithmTimer::start(ScopedTimer& scope)
{
  scope.m_timer = this;
  start(scope.m_timeout);
}

unsigned int AlgorithmTimer::stop()
{
  // stop the timer, and retrieve remaining time
#ifndef __APPLE__
  itimerspec spec;
  spec.it_value.tv_sec     = 0;
  spec.it_value.tv_nsec    = 0;
  spec.it_interval.tv_sec  = 0;
  spec.it_interval.tv_nsec = 0;
  
  itimerspec ovalue;
  timer_settime(m_timerid, 0, &spec, &ovalue);
  return 1000*ovalue.it_value.tv_sec + int(ovalue.it_value.tv_nsec/1000000);
#else
  return 0;
#endif
}

unsigned int AlgorithmTimer::timeLeft() const
{
#ifndef __APPLE__
  itimerspec spec;
  timer_gettime( m_timerid, &spec );
  return 1000*spec.it_value.tv_sec + int(spec.it_value.tv_nsec/1000000);
#else
	return 0;
#endif 
}

bool AlgorithmTimer::installSignalHandler()
{
  if (s_handlerInstalled) return true;
  
  // Install new signal handler and backup old one
  struct sigaction sigact;
  memset (&sigact, 0, sizeof(sigact)); // quiet coverity
  sigact.sa_sigaction = AlgorithmTimerHandler::onAlarmSignal;
  sigemptyset(&sigact.sa_mask);
  sigact.sa_flags = SA_SIGINFO;
  int ret = sigaction(SIGNO, &sigact, &s_oldSigHandler);
  
  s_handlerInstalled = (ret == 0);
  return s_handlerInstalled;
}

/*
 * This is currently not being used, but we'll leave it here
 * for future reference.
 */
bool AlgorithmTimer::uninstallSignalHandler()
{
  if (!s_handlerInstalled) return true;
  
  int ret = sigaction(SIGNO, &s_oldSigHandler, 0);
  
  s_handlerInstalled = !(ret==0);
  return (ret==0);
}

AlgorithmTimer::callbackFct_t AlgorithmTimer::abortJob()
{
  /*
   * Print stack trace and abort the job.
   */  
  std::ostringstream os;
  os << "Timeout ";
  os << " (" << this->timeout() << " msec) reached";
  
  ServiceHandle<ICoreDumpSvc> coreDumpSvc("CoreDumpSvc", "AlgorithmTimer");
  if ( coreDumpSvc.retrieve().isSuccess() )
    {
      coreDumpSvc->setCoreDumpInfo("Reason", os.str());
      std::cerr << coreDumpSvc->dump() << std::endl;
    }
  else
    {
      std::cerr << os.str() << std::endl;
    }
  abort();
}

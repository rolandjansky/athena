/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_POSIXTIMEKEEPER_H
# define ATHENAKERNEL_POSIXTIMEKEEPER_H

#include <sys/times.h>

#ifndef ATHENAKERNEL_TIMEKEEPER_H
 #include "AthenaKernel/TimeKeeper.h"
#endif


/** @class POSIXTimeKeeper
  * @brief specialize TimeKeeper for POSIX systems 
  *        (and other Unix-like ones). Derived from CERNLIB TIMEL/TIMEX
  *        N.B. times are in 1/100 CPU sec units
  * 
  * @author Paolo Calafiura <pcalafiura@lbl.gov> 
  * $Id: POSIXTimeKeeper.h,v 1.4 2007-06-14 01:57:23 calaf Exp $
  */

class POSIXTimeKeeper : virtual public TimeKeeper {
public:
  /// ITimeKeeper Implementation
  //@{
  //INH  virtual bool nextIter();         ///< main user entry point
  //INH  virtual time_t timeL() const;    ///< time remaining before limit
  virtual time_t timeX() const;           ///< time used so far
  //INH  virtual bool timeOver() const;   ///< is there time for another iter?
  //@}

  /// TimeKeeper Implementation
  //@{
  virtual time_t allocTime() const = 0;   ///< allocated CPU time for job
  // cppcheck-suppress virtualCallInConstructor
  virtual void updateTime();
  virtual const std::string& unitLabel() const;
  //@}

protected:
  POSIXTimeKeeper();
  virtual ~POSIXTimeKeeper();

private:
  tms m_tms; //the POSIX struct holding CPU usage info
};

#endif // ATHENAKERNEL_POSIXTIMEKEEPERSVC_H

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_TIMEKEEPER_H
# define ATHENAKERNEL_TIMEKEEPER_H

#include <string>

#ifndef ATHENAKERNEL_ITIMEKEEPER_H
 #include "AthenaKernel/ITimeKeeper.h"
#endif

/** @class TimeKeeper
  * @brief base class implementint the core TimeKeeper functionality for
  *        all systems.
  * 
  * @author Paolo Calafiura <pcalafiura@lbl.gov> 
  * $Id: TimeKeeper.h,v 1.4 2007-06-14 01:57:23 calaf Exp $
  */

class TimeKeeper : virtual public ITimeKeeper {
public:
  /// ITimeKeeper Implementation
  //@{
  virtual bool nextIter();           ///< main user entry point
  virtual time_t timeL() const;      ///< time remaining before limit
  virtual time_t timeX() const = 0;  ///< time used so far
  virtual bool timeOver() const;     ///< is there time for another iter?
  //@}

  virtual time_t allocTime() const = 0;   ///< allocated job CPU time
  virtual void updateTime() = 0;          ///< update time usage
  virtual time_t averageIterTime() const; ///< moving average of iter time 
  virtual const std::string& unitLabel() const = 0;

protected:
  TimeKeeper();
  virtual ~TimeKeeper();

private:
  unsigned int m_cycles;
  time_t m_setupTime;
  static const unsigned int SKIPCYCLES;  //define job setup time
};

/// \name stream status
//@{
class MsgStream;
MsgStream& operator << (MsgStream&, const TimeKeeper&);
#include <iostream>
std::ostream& operator << (std::ostream&, const TimeKeeper&);
//@}
#endif // ATHENAKERNEL_TIMEKEEPERSVC_H

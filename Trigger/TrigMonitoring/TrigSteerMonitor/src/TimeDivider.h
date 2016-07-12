/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigSteerMonitor_TimeDivider_h
#define TrigSteerMonitor_TimeDivider_h

#include <unistd.h>
/** 
 * @brief Helper class to divide time into intervals aligned with wall clock
 * Once object is constructed say with arguments (6, 10, seconds) 
 * the time gets divided into 6 intervals each of 10s long.
 * If method isPassed is then called say for 17:00:01 it returns false 
 * and interval argument is set to 0, When next time function 
 * is called at 17:00:09 retult is the same. However if next call happens 
 * at 17:00:11 one gets return value == true and new interval == 1.
 *
 */

class TimeDivider {
public:
  enum unit {seconds, minutes};

  /** 
   * @brief construct the TimeDivider
   * @param intervals defines number of intervals we want to divide time into
   * @param duration of the time interval
   * @param u is unit (either seconds or minutes) 
   */
  TimeDivider(unsigned int intervals, unsigned int duration, unit u);

  /** 
   * @brief method to know if one shoudl switch to new interval
   * @param time time 
   * @param interval is interval in which we are now
   * @return true if one needs to switch to new interval
   */
  bool isPassed(time_t time, unsigned int& newinterval, unsigned int& oldinterval ) const;

  /** 
   * @brief method 
   * @param time time 
   * @param interval is interval in which we are now
   * @return true if one needs to switch to new interval
   */
  unsigned int forcePassed(time_t time, unsigned int& oldinterval );


private:
  unsigned int m_intervals;
  unsigned int m_duration;
  unit m_unit;
  mutable unsigned int m_current_interval;
  mutable time_t m_beginning;
};



#endif

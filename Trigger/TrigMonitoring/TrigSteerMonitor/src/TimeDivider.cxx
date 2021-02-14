/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include <time.h>

#include "TimeDivider.h"

TimeDivider::TimeDivider(unsigned int intervals, unsigned int duration, unit u )
  : m_intervals(intervals),
    m_duration(duration), 
    m_unit(u),
    m_current_interval(0) {
  unsigned int temp_iv;
  time_t temp_time = time(0);
  m_beginning = temp_time;
  isPassed(temp_time, temp_iv, temp_iv);
}

bool TimeDivider::isPassed(time_t time, unsigned int& newinterval,  unsigned int& oldinterval) const {

  // convert to tm structure for seconds & minutes
  tm t = *gmtime(&time);
  unsigned count = 0;
  if (m_unit == seconds) {
    count  = t.tm_sec;
  } else if ( m_unit == minutes ) {
    count  = t.tm_min;
  }
  
  // calculate next interval 
  unsigned int iv = (count % (m_intervals*m_duration))/m_duration;

  // is new interval differnt than recently returned? If so signal it.
  if ( m_current_interval != iv ) {
    oldinterval = m_current_interval;
    newinterval = m_current_interval = iv;
    m_beginning = time;
    return true;      
  }
  newinterval = oldinterval = m_current_interval;
  return false;
}

unsigned int TimeDivider::forcePassed(time_t time, unsigned int& interval ) {
   interval = m_current_interval;
   return std::max((time - m_beginning), (long int)1); // never return 0
}


/* this can be used to test TimeDivider
  int main() {
  TimeDivider ts(6, 10, TimeDivider::seconds);
  TimeDivider sts(10, 5, TimeDivider::seconds);
  TimeDivider tmin(3, 1, TimeDivider::minutes);

  for ( int i = 0; i < 900; ++i ) {
  
  usleep(500000);
  time_t t = time(0);
  tm gmt = *gmtime(&t);
  unsigned interval;
  bool switchNow;
  switchNow = ts.isPassed(t, interval);
  cout << "second divider " << switchNow << " to " << interval  << " " << ctime(&t);
  
  //switchNow = sts.isPassed(t, interval);
  //    cout << "second (short) divider " << switchNow << " to " << interval  << " " << ctime(&t);
  
  //    switchNow = tmin.isPassed(t, interval);
  //    cout << "minutes divider " << switchNow << " to " << interval  << " " << gmt.tm_min << ":" << gmt.tm_sec << endl;
  
  }
  }
*/

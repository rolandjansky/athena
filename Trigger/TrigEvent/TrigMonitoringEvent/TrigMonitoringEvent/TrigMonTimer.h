/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_TIMER_H
#define TRIGMON_TIMER_H

/*
  @author Rustem Ospanov
  @date July 2009 

  @brief Time stamp encoded as int and microsecond. 
   Sec has range from 0 to 3599.
   Microsec has range from 0 to 999999.
   Elapsed time is computed by passing starting time stamp to
   getElapsed() function. Only time periods that are less
   than one hour can be measured.
*/

// Framework
#include "CLIDSvc/CLASS_DEF.h"

// C/C++
#include <stdint.h>

class TrigMonTimer
{
 public:
  
  TrigMonTimer();
  TrigMonTimer(long int tv_sec, long int tv_usec);
  explicit TrigMonTimer(uint32_t encoded);
  ~TrigMonTimer() {}
  
  uint32_t getSec()      const;
  uint32_t getUSec()     const { return getMicroSec(); }
  uint32_t getMicroSec() const;
  uint32_t getEncoded()  const { return m_encoded; }
  
  double    elapsed(const TrigMonTimer &start) const { return getElapsed(start); }
  double getElapsed(const TrigMonTimer &start) const;
    
 private:
  
  uint32_t m_encoded;     // encoded timestamp
};

//
// store encoded timestamp in 32 bits
//
// ss...suu...u
//
//      seconds      [s] 12 bits, actual values: (0:    3,600)
// microseconds      [u] 20 bits, actual values: (0:1,000,000)
//
//

inline bool operator==(const TrigMonTimer &lhs, const TrigMonTimer &rhs) { 
  return lhs.getEncoded() == rhs.getEncoded();
}
inline bool operator <(const TrigMonTimer &lhs, const TrigMonTimer &rhs) { 
  return lhs.getEncoded() < rhs.getEncoded();
}

CLASS_DEF( TrigMonTimer , 255578247 , 1 )

#endif

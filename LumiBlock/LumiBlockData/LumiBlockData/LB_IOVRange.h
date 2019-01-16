/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// Header file for class LumiBlockCollection
// Author: Marjorie Shapiro <mdshapiro@lbl.gov>
/////////////////////////////////////////////////////////////////// 
#ifndef LUMIBLOCKDATA_LB_IOVRANGE_H 
#define LUMIBLOCKDATA_LB_IOVRANGE_H 

// The IOVRange class
#include "AthenaKernel/IOVRange.h"

class LB_IOVRange: public IOVRange
{
 public:
 LB_IOVRange();
 LB_IOVRange( const IOVTime& start, const IOVTime& stop );
 LB_IOVRange( const IOVRange& r);
 LB_IOVRange( const IOVRange& r, uint32_t numExpected, uint32_t numSeen);


 uint32_t getNumExpected() const {return m_NumExpected;}
 uint32_t getNumSeen() const {return m_NumSeen;}
 void setNumExpected(uint32_t num) {m_NumExpected=num;}
 void setNumSeen(uint32_t num) {m_NumSeen=num;}

 friend bool operator==(const LB_IOVRange& lhs, const LB_IOVRange& rhs);
 friend bool operator!=(const LB_IOVRange& lhs, const LB_IOVRange& rhs);

 protected:
 uint32_t m_NumExpected;
 uint32_t m_NumSeen;

};

inline bool operator==(const LB_IOVRange& lhs, const LB_IOVRange& rhs) {
  return  lhs.start()==rhs.start() && 
  lhs.stop()==rhs.stop() &&
  lhs.getNumExpected()==rhs.getNumExpected() &&
  lhs.getNumSeen()==rhs.getNumSeen();
}


inline bool operator!=(const LB_IOVRange& lhs, const LB_IOVRange& rhs) {
  return !(lhs==rhs);
}
#endif //>LUMIBLOCKDATA_LB_IOVRANGE_H 

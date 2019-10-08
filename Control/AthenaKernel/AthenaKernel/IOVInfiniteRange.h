//Dear emacs this is -*-c++-*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_IOVINFINITERANGE
#define ATHENAKERNEL_IOVINFINITERANGE

#include "GaudiKernel/EventIDRange.h"


//Unfortunatly, EventIDBase doesn't qualify for constexpr 
       
/**
 *  @class IOVInfiniteRange
 *  @brief Collection of static functions providing infinite EventIDRanges
 */   
class IOVInfiniteRange { 
public:
  /// Produces an EventIDRange that is infinite in RunLumi and invalid in Time
  static EventIDRange infiniteRunLB();

  /// Produces an EventIDRange that is inifinite in Time and invalid in RunLumi
  static EventIDRange infiniteTime();

  /// Produces an mixed EventIDRange that is infinite in Time and RunLumi
  static EventIDRange infiniteMixed();
};


//UNDEFNUM/UNDEFEVT are the maximum value of the variable in question. They denote invalidity. 
//UNDEFNUM-1 is are the largest value that is considered valid, so "infinity" in our sense. 

//We don't care about the BCID, set them to 0
//We do not use the event-number to limit IOV-Ragnes (only Run/LB). So the event-number is always invalid


inline
EventIDRange IOVInfiniteRange::infiniteRunLB() {
//                              Run                 Event                    time                   time_ns                LumiBlock               BCID
  const EventIDBase start {0                      , EventIDBase::UNDEFEVT  , EventIDBase::UNDEFNUM, EventIDBase::UNDEFNUM, 0                      ,0};
  const EventIDBase stop  {EventIDBase::UNDEFNUM-1, EventIDBase::UNDEFEVT  , EventIDBase::UNDEFNUM, EventIDBase::UNDEFNUM, EventIDBase::UNDEFNUM-1,0};
  return EventIDRange{start,stop};
}

inline
EventIDRange IOVInfiniteRange::infiniteTime() {
//                            Run                 Event                    time                   time_ns                    LumiBlock          BCID
  const EventIDBase start {EventIDBase::UNDEFNUM, EventIDBase::UNDEFEVT,  0                     , 0                      , EventIDBase::UNDEFNUM, 0};
  const EventIDBase stop  {EventIDBase::UNDEFNUM, EventIDBase::UNDEFEVT, EventIDBase::UNDEFNUM-1, EventIDBase::UNDEFNUM-1, EventIDBase::UNDEFNUM, 0};
  return EventIDRange{start, stop};
}

inline
EventIDRange IOVInfiniteRange::infiniteMixed() {
//                            Run                   Event                  time                    time_ns                   LB                      BCID
  const EventIDBase start {0                      , EventIDBase::UNDEFEVT, 0                      ,0                       , 0                      ,0};
  const EventIDBase stop  {EventIDBase::UNDEFNUM-1, EventIDBase::UNDEFEVT, EventIDBase::UNDEFNUM-1, EventIDBase::UNDEFNUM-1, EventIDBase::UNDEFNUM-1,0};
  return EventIDRange{start,stop};
}

#endif

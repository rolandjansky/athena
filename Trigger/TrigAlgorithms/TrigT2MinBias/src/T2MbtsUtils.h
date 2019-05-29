/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef T2MBTSUTILS_H
#define T2MBTSUTILS_H

#include <map>

#include "xAODTrigMinBias/TrigT2MbtsBits.h"

class MsgStream;


class T2MbtsUtils {
 public:

  T2MbtsUtils();
  int calculateMultiplicities(const xAOD::TrigT2MbtsBits *t2mbtsBits, int mode,
			      MsgStream& mlog, 
			      unsigned int msgLvl); // mode 0 --> all, 1 --> inner, 2 --> outer
 protected:
  
  /** Energy threshold in pC */
  float m_threshold;
  
  /** A time cut in ns.  Values <= 0 disable the time cut */
  float m_timeCut;
  
  /** A global time offset in ns about which the time window cuts */
  float m_globalTimeOffset; 
  
  /** Offsets with respect to the global offset of all counters. */
  float m_timeOffsets[32];
  
  /** Mean time of A side counters */
  float m_timeMean_A; 

  /** Mean time of C side counters */
  float m_timeMean_C;

  /** Time difference from A to C side */
  float m_timeDiff_A_C;
  
  /** Hit multiplicity after energy and optional time cut. */ 
  std::pair<unsigned int, unsigned int> m_mult;
};

#endif

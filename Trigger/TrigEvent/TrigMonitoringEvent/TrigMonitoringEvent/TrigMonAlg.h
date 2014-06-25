/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_ALG_H
#define TRIGMON_ALG_H

/**
  @author Rustem Ospanov
  @date July 2009 

  @brief Summary of single agorithm execution. Algorithm is identified by position
   within parent sequence. Class stores stop and start time as encoded words from 
   TrigMonTimer class, so timers are constructed on fly and returned by value. 
   Class can also store (uint16_t, float) pairs of variables.
*/

// Framework
#include "CLIDSvc/CLASS_DEF.h"

// C/C++
#include <stdint.h>
#include <set>
#include <vector>

// Local
#include "TrigMonitoringEvent/TrigMonTimer.h"

class TrigMonAlg
{
 public:
  
  TrigMonAlg();
  TrigMonAlg(unsigned int position, bool is_cached);
  ~TrigMonAlg() {}
  
  void addTimer(const TrigMonTimer &tbeg, const TrigMonTimer &tend);
  void addRoiId(unsigned int roiId);
  void addWord(unsigned int word);
  
  uint8_t getPosition() const;
  uint8_t getNRoi()     const;
  uint8_t getRoiId(unsigned int i = 0) const;

  const std::set<uint8_t> getRoiIdSet() const;

  bool isCached() const;
  bool isCalled() const; 

  const std::vector<uint8_t>&  getByte() const { return m_byte; }
  const std::vector<uint32_t>& getWord() const { return m_word; }

  const TrigMonTimer start() const;
  const TrigMonTimer stop()  const;
  
  double getTimer() const { return stop().getElapsed(start()); }
  double elapsed()  const { return stop().getElapsed(start()); }
  
 private:
  
  std::vector<uint8_t>      m_byte;     // Encoded bytes: position, caching state, rois
  std::vector<uint32_t>     m_word;     // Encoded words: first two entries are timers)
};

std::string str(const TrigMonAlg &);

//
// Note: getPosition() returns a position at which this algorithm is executed
// by a parent sequence. TrigMonAlg is held by TrigMonSeq which uniquely
// identifies the parent sequence. There should be no more than 128 algorithms
// in a sequence since top bit stores cached state of this algorithm call.
//

//
// m_byte[0] = encoded position and cache bit
// m_byte[i] = roi id, i = 1,..
//
// m_word[0] = encoded TrigMonTimer for start time
// m_word[1] = encoded TrigMonTimer for stop time
// m_word[i] = free, i =2, ...
//
// TODO: m_byte and m_word should really be merged together
//

CLASS_DEF( TrigMonAlg , 254413856 , 1 )

#endif

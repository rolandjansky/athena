/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_ROB_H
#define TRIGMON_ROB_H

/*
  @author Rustem Ospanov
  @date July 2009 

  @brief Summary of single request for ROB data.
   This is a reduced copy of ROBDataMonitorStruct.
*/

// Framework
#include "CLIDSvc/CLASS_DEF.h"

// C/C++
#include <stdint.h>
#include <iostream>
#include <vector>

// Local
#include "TrigMonitoringEvent/TrigMonROBData.h"
#include "TrigMonitoringEvent/TrigMonROBSum.h"
#include "TrigMonitoringEvent/TrigMonTimer.h"

class TrigMonROB
{
 public:
  
  TrigMonROB();
  TrigMonROB(uint32_t requestor_id,
	     long int beg_sec, long int beg_usec, 
	     long int end_sec, long int end_usec);
  ~TrigMonROB() {} 
  
  void addData(const TrigMonROBData &data) { m_data.push_back(data); }
  void addWord(const uint32_t word) { m_word.push_back(word); }

  std::vector<TrigMonROBData>& getData() { return m_data; }
  std::vector<uint32_t>&       getWord() { return m_word; }

  uint32_t getRequestorId() const { return m_word[0]; }
  double   getTimer()       const { return elapsed(); }
  double   elapsed()        const { return stop().getElapsed(start()); }
  
  const TrigMonTimer start() const { return TrigMonTimer(getStart()); }
  const TrigMonTimer stop()  const { return TrigMonTimer(getStop()); }

  const std::vector<TrigMonROBData>& getData() const { return m_data; }
  const std::vector<TrigMonROBSum>   getSum()  const;

  uint32_t getStart() const { return m_word[1]; }
  uint32_t getStop()  const { return m_word[2]; }
  
  const std::vector<uint32_t>& getWord() const { return m_word; }

  void print(std::ostream &os = std::cout) const;

 private:
  
  std::vector<uint32_t>        m_word;         // Data payload (see below)
  std::vector<TrigMonROBData>  m_data;         // Full data for requested robs
};

//
// m_word[0] = Hash id of requesting algorithm name
// m_word[1] = Encoded TrigMonTimer for start time
// m_word[2] = Encoded TrigMonTimer for stop time
// m_word[i] = Encoded words for TrigMonROBSum, i = 3,...
//

std::string str(const TrigMonROB &);

//
// Inlined comparison operators
//
inline bool operator==(const TrigMonROB &lhs, const TrigMonROB &rhs) {
  return lhs.getRequestorId() == rhs.getRequestorId();
}
inline bool operator <(const TrigMonROB &lhs, const TrigMonROB &rhs) {
  return lhs.getRequestorId() < rhs.getRequestorId();
}

inline bool operator==(const TrigMonROB &rob, unsigned int id) {
  return rob.getRequestorId() == id;
}
inline bool operator==(unsigned int id, const TrigMonROB &rob) {
  return rob.getRequestorId() == id;
}

CLASS_DEF( TrigMonROB , 157550435 , 1 )

#endif

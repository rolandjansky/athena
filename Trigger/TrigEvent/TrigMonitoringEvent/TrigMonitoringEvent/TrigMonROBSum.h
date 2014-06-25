/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_ROBSUM_H
#define TRIGMON_ROBSUM_H

/*
  @author Rustem Ospanov
  @date July 2009 

  @brief Sum of ROB data items: number of robs, size and history state.
*/

// Framework
#include "CLIDSvc/CLASS_DEF.h"

// C/C++
#include <stdint.h>

// Local
#include "TrigMonitoringEvent/TrigMonROBData.h"

class TrigMonROBSum
{
 public:
  
  TrigMonROBSum();
  explicit TrigMonROBSum(uint32_t word);
  ~TrigMonROBSum() {}

  bool setHistory(uint32_t hist);
  bool setNROB(uint32_t nrob);
  bool setSize(uint32_t size);

  bool addROB(const TrigMonROBData &data, int subdet = -1);

  uint32_t getWord()   const { return m_word; }
  uint32_t getSize()   const;
  uint32_t getNROB()   const;
  uint32_t getSubDet() const;

  TrigMonROBData::History getHistory() const;

  void print(std::ostream &os = std::cout) const;
  
 private:
  
  uint32_t    m_word; // Number of ROBs, size and history
};

std::string str(const TrigMonROBSum &);

//
// m_word stores history and state enums
//
//  4 low bits: history
//  8 mid bits: number of robs or subdetector id
// 20 top bits: size of robs
//

CLASS_DEF( TrigMonROBSum , 136379655 , 1 )

#endif

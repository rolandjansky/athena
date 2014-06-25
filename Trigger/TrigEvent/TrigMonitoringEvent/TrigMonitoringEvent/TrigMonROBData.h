/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_ROBDATA_H
#define TRIGMON_ROBDATA_H

/*
  @author Rustem Ospanov
  @date July 2009 

  @brief Summary of ROB data unit.
   This is class can have two versions:
     a) summary of multiple ROBDataStruct structures,
        where rob ids with same status and history are added together;
     b) rob ids are erased to reduce memory footprint.
*/

// Framework
#include "CLIDSvc/CLASS_DEF.h"

// C/C++
#include <stdint.h>

class TrigMonROBData
{
 public:
  
  enum History { 
    kUNCLASSIFIED = 0, // ROB was requested but never arrived at processor. History unknown. 
    kRETRIEVED    = 1, // ROB was retrieved from ROS by DataCollector
    kCACHED       = 2, // ROB was found already in the internal cache of the ROBDataProviderSvc
    kIGNORED      = 4, // ROB was on the "ignore" list and therefore not retrieved 
    kDISABLED     = 8  // ROB was disabled in OKS and therefore not retrieved
  };
  enum Status {
    kUNKNOWN  = 0x0,    // ROB status was unknown (default)
    kPREFETCH = 0x40,   // ROB was prefetched
    kOK       = 0x80    // ROB is StatusOk
  };
  
  TrigMonROBData();
  TrigMonROBData(uint32_t rob_id, uint32_t rob_size);
  ~TrigMonROBData() {}
  
  void setHistory(History hist);
  void setStatusOK();
  void setStatusPREFETCH();
  
  bool isUnclassified()      const { return getHistory() == kUNCLASSIFIED; }
  bool isCached()            const { return getHistory() == kCACHED; }
  bool isRetrieved()         const { return getHistory() == kRETRIEVED; }
  bool isIgnored()           const { return getHistory() == kIGNORED; }
  bool isDisabled()          const { return getHistory() == kDISABLED; }
  bool isStatusOk()          const;
  bool isStatusPrefetched()  const;
  
  uint32_t getROBId()        const { return m_rob_id; }
  uint32_t getROBSize()      const;
  uint32_t getEncodedState() const;
  History  getHistory()      const;
  Status   getStatus()       const;

  void print(std::ostream &os = std::cout) const;
  
 private:
  
  uint32_t    m_rob_id;        // ROB source id
  uint32_t    m_word;          // Size, history enum and status enum
};

std::string str(const TrigMonROBData &);

//
// m_word stores history and state enums
//
//  8 low bits: sssshhhh
//              history  [h] 4 low bits
//              state    [s] 4 top bits
// 24 top bits: rob data size
//

namespace Trig
{
  TrigMonROBData::History getROBHistory(unsigned int val);
  TrigMonROBData::Status  getROBStatus (unsigned int val);
  
  std::string AsStringROBHistory(TrigMonROBData::History hist);
  std::string AsStringROBStatus (TrigMonROBData::Status  stat);
}

CLASS_DEF( TrigMonROBData , 174340367 , 1 )

#endif

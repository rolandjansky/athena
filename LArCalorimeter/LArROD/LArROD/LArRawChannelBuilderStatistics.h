/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @class LArRawChannelBuilderStatistics
 * @author Rolf Seuster
 * @brief Returns various counters from the LArRawChannel building
 * 
 */

#ifndef LARROD_LARRAWCHANNELBUILDERSTATISTICS_H
#define LARROD_LARRAWCHANNELBUILDERSTATISTICS_H

#include "GaudiKernel/Kernel.h"  // for ulonglong , unsigned 64bit long int

#include <vector>
#include <string>

class LArRawChannelBuilderStatistics
{
 public:
  LArRawChannelBuilderStatistics(unsigned int nerr, unsigned short bitpat);
  ~LArRawChannelBuilderStatistics() {};
  
  void incrementErrorCount(unsigned int nerr);
  ulonglong returnErrorCount(unsigned int nerr);
  ulonglong returnErrorCountPerEvent(unsigned int nerr);
  ulonglong returnErrorCountSqPerEvent(unsigned int nerr);
  unsigned long returnEvents();
  
  void setErrorString(unsigned int nerr, const std::string& s);
  std::string returnErrorString(unsigned int nerr);
  
  unsigned int returnBitPattern();
  unsigned int returnMaxErrors();
  
  void resetErrorcountPerEvent();
  
 private:  
  
  // store information about errors in this tool
  const unsigned int                    m_N_errors;
  std::vector<ulonglong> /* 64 bits */  m_error_count;
  std::vector<std::string>              m_error_string;
  std::vector<unsigned long>            m_errors_this_event;
  std::vector<ulonglong> /* 64 bits */  m_errors_per_event;
  std::vector<ulonglong> /* 64 bits */  m_errorsSq_per_event;
  unsigned long                         m_events;
  
  // bit pattern to identify that this tool build this RawChannel
  const unsigned int                    m_bit_pattern;
  const std::string                     m_s;
  
  bool m_first;
};

#endif

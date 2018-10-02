/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTimeAlgs/TrigTimeStamp.h"

double TrigTimeStamp::millisecondsSince() const { 
  return std::chrono::duration_cast<std::chrono::microseconds>( std::chrono::high_resolution_clock::now() - m_stamp ).count();
}

double TrigTimeStamp::millisecondsDifference(const TrigTimeStamp& other) const {
  return std::chrono::duration_cast<std::chrono::microseconds>( other.get() - m_stamp ).count();
}

std::pair<uint32_t,uint32_t> TrigTimeStamp::secondsAndMillisecondsSinceEpoch() const {
  uint64_t millisecondsSinceEpoch = std::chrono::duration_cast<std::chrono::milliseconds>( m_stamp.time_since_epoch() ).count();
  uint64_t secondsSinceEpoch = millisecondsSinceEpoch / 1000ull;
  millisecondsSinceEpoch -= (secondsSinceEpoch * 1000ull);
  return std::make_pair(static_cast<uint32_t>(secondsSinceEpoch), static_cast<uint32_t>(millisecondsSinceEpoch));
}

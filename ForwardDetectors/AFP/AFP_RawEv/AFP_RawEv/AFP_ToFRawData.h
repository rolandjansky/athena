/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_TOFRAWDATA_H
#define AFP_TOFRAWDATA_H

#include "AFP_RawDataCommonHead.h"

#include <stdint.h>

/// @brief Class representing data record for time-of-flight detectors.
///
/// Documentation of the format is available in the twiki page:
/// https://twiki.cern.ch/twiki/bin/view/Atlas/AFPHPTDC#Data_Encoding
class AFP_ToFRawData : public AFP_RawDataCommonHead
{
public:
  /// @copydoc AFP_ToFRawData::m_header
  uint16_t header() const {return m_header;}
  void setHeader(const uint16_t header) {m_header = header;}

  /// @copydoc AFP_ToFRawData::m_edge
  uint16_t edge() const {return m_edge;}
  void setEdge(const uint16_t edge) {m_edge = edge;}

  /// @copydoc AFP_ToFRawData::m_channel
  uint16_t channel() const {return m_channel;}
  void setChannel(const uint16_t channel) {m_channel = channel;}
  
  /// @copydoc AFP_ToFRawData::m_time
  uint32_t time() const {return m_time;}
  void setTime(const uint32_t time) {m_time = time;}

  /// @copydoc AFP_ToFRawData::m_pulseLength
  uint32_t pulseLength() const {return m_pulseLength;}
  void setPulseLength(const uint32_t pulseLength) {m_pulseLength = pulseLength;}

private:
  /// @brief Value of 9-11 most significant bits
  ///
  /// In the following record: `xxxx xxxx hhhx xxxx xxxx xxxx xxxx xxxx`
  /// it means bits marked with `h`. Contains information about time-of-flight data type or error:
  /// - value 0 means TDC header
  /// - value 2 means hit measurement
  /// - value 3 is per event error 
  uint16_t m_header;

  /// @brief Value of 12 most significant bit
  ///
  /// In the following record: `xxxx xxxx xxxE xxxx xxxx xxxx xxxx xxxx`
  /// it means bits marked with `E`. Contains information about signal
  /// beginning or end.
  uint16_t m_edge;

  /// @brief Value of 13-17 most significant bits
  ///
  /// In the following record: `xxxx xxxx xxxx CCCC xxxx xxxx xxxx xxxx`
  /// it means bits marked with `C`. Contains information about
  /// channel in HPTDC of the time measurement.
  uint16_t m_channel;
  
  /// @brief Value of 10 least significant bits
  ///
  /// In the following record: `xxxx xxxx xxxx xxxx xxxx xxtt tttt tttt`
  /// it means bits marked with `t`. Contains information about time.
  uint32_t m_time;

  /// @brief Value of 11-16 least significant bits
  ///
  /// In the following record: `xxxx xxxx xxxx xxxx LLLL LLxx xxxx xxxx`
  /// it means bits marked with `L`. Contains information how long
  /// signal from the bar was above threshold.
  uint32_t m_pulseLength;
};
#endif

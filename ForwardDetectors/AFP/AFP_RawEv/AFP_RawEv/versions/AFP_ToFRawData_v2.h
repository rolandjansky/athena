/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_TOFRAWDATA_V2_H
#define AFP_TOFRAWDATA_V2_H

#include "AFP_RawEv/AFP_RawDataCommonHead.h"

// to use messaging macros
#include "AthenaBaseComps/AthMessaging.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"

#include <stdint.h>

/// @brief Class representing data record for time-of-flight detectors.
///
/// Documentation of the format is available in the twiki page:
/// https://twiki.cern.ch/twiki/bin/view/Atlas/AFPHPTDC#Data_Encoding
class AFP_ToFRawData_v2 : public AFP_RawDataCommonHead, public AthMessaging
{
public:

  // these constructors are deleted by AthMessaging
  AFP_ToFRawData_v2();
  AFP_ToFRawData_v2(const AFP_ToFRawData_v2& copy);

  /// @copydoc AFP_ToFRawData_v2::m_header
  uint16_t header() const {return m_header;}
  void setHeader(const uint16_t header) {m_header = header;}

  /// @copydoc AFP_ToFRawData_v2::m_edge
  uint16_t edge() const {return m_edge;}
  void setEdge(const uint16_t edge) {m_edge = edge;}

  /// @copydoc AFP_ToFRawData_v2::m_channel
  uint16_t channel() const {return m_channel;}
  void setChannel(const uint16_t channel) {m_channel = channel;}
  void setTrigger() {m_channel=m_triggerChannel;}
  bool isTrigger() const {return m_channel==m_triggerChannel;}
  
  /// @copydoc AFP_ToFRawData_v2::m_time_or_delayedTrigger
  uint32_t time() const;
  void setTime(const uint32_t time);
  
  /// @copydoc AFP_ToFRawData_v2::m_pulseLength_or_triggerPattern
  uint32_t pulseLength() const;
  void setPulseLength(const uint32_t pulseLength);
  
  /// @copydoc AFP_ToFRawData_v2::m_time_or_delayedTrigger
  uint16_t delayedTrigger() const;
  void setDelayedTrigger(const uint32_t delayedTrigger);
  
  /// @copydoc AFP_ToFRawData_v2::m_pulseLength_or_triggerPattern
  uint32_t triggerPattern() const;
  void setTriggerPattern(const uint32_t triggerPattern);

private:
  /// @brief Value of 9-11 (HPTDC) or 9 (picoTDC) most significant bit(s)
  ///
  /// In the following records:
  /// HPTDC 2017: `xxxx xxxx hhhx xxxx xxxx xxxx xxxx xxxx`
  /// picoTDC:    `xxxx xxxx hxxx xxxx xxxx xxxx xxxx xxxx`
  /// it means bits marked with `h`. Contains information about time-of-flight data type or error:
  /// - value 0 means HPTDC header
  /// - value 1 meas picoTDC header
  /// - value 2 means hit measurement (only HPTDC)
  /// - value 3 is per event error (only HPTDC)
  uint16_t m_header;

  /// @brief Value of 12 most significant bit
  ///
  /// In the following record: `xxxx xxxx xxxE xxxx xxxx xxxx xxxx xxxx`
  /// it means bits marked with `E`. Contains information about signal
  /// beginning or end. Always set to 0 for picoTDC.
  uint16_t m_edge;

  /// @brief Value of 13-17 (HPTDC) or 14-19 (picoTDC) most significant bits
  ///
  /// In the following records:
  /// HPTDC 2017:    `xxxx xxxx xxxx CCCC xxxx xxxx xxxx xxxx`
  /// picoTDC #1+#2: `xxxx xxxx xxxx xCCC CCCx xxxx xxxx xxxx`
  /// it means bits marked with `C`. Contains information about
  /// channel of the time measurement.
  uint16_t m_channel;
  
  /// @brief Special value for channel number, denoting the entry is actually a trigger word
  ///
  /// Data cannot have this channel number as there are no more than 6 bit, meaning maximum channel number is 63
  const uint16_t m_triggerChannel = 65535;

  /// @brief Value of 10 (HPTDC) or 13 (picoTDC) least significant bits for time; value of 14-16 (picoTDC) most significant bits for delayed trigger
  ///
  /// In the following records:
  /// HPTDC 2017: `xxxx xxxx xxxx xxxx xxxx xxtt tttt tttt`
  /// picoTDC #1: `xxxx xxxx xxxx xxxx xxxt tttt tttt tttt`
  /// Trigger:    `xxxx xxxx xxxx xTTT xxxx xxxx xxxx xxxx`
  /// it means bits marked with `t` or `T`. Contains information about time (`t`) or about delayed trigger (`T`).
  uint32_t m_time_or_delayedTrigger;

  /// @brief Value of 11-16 (HPTDC) or 13 (picoTDC) least significant bits for signal lenght; value of 16 (picoTDC) least significant bits for trigger pattern
  ///
  /// In the following record: 
  /// HPTDC 2017: `xxxx xxxx xxxx xxxx LLLL LLxx xxxx xxxx`
  /// picoTDC #2: `xxxx xxxx xxxx xxxx xxxx LLLL LLLL LLLL`
  /// Trigger:    `xxxx xxxx xxxx xxxx PPPP PPPP PPPP PPPP`
  /// it means bits marked with `L` or `P`. Contains information how long
  /// signal from the bar was above threshold (`L`) or information about the trigger pattern (`P`).
  uint32_t m_pulseLength_or_triggerPattern;
  
};
#endif

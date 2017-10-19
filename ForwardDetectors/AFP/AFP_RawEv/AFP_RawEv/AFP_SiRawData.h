/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_SIRAWDATA_H
#define AFP_SIRAWDATA_H

#include "AFP_RawDataCommonHead.h"

#include <stdint.h>

/// @brief Class representing data record for silicon detectors.
///
/// Documentation of the format is in presentation:
/// https://indico.cern.ch/login/?next=%2Fevent%2F493415%2Fcontributions%2F1172054%2Fattachments%2F1222236%2F1787442%2Fbytestream.pdf
class AFP_SiRawData : public AFP_RawDataCommonHead
{
public:
  /// @copydoc AFP_SiRawData::m_column
  uint16_t column() const {return m_column;}
  void setColumn (const uint16_t column) {m_column = column;}

  /// @copydoc AFP_SiRawData::m_row
  uint32_t row() const {return m_row;}
  void setRow (const uint32_t row) {m_row = row;}

  /// @copydoc AFP_SiRawData::m_timeOverThreshold
  uint16_t timeOverThreshold() const {return m_timeOverThreshold;}
  void setTimeOverThreshold (const uint16_t timeOverThreshold) {m_timeOverThreshold = timeOverThreshold;}
  
private:
  /// @brief Value of 9-15 most significant bits
  ///
  /// In the following record: `xxxx xxxx CCCC CCCx xxxx xxxx xxxx xxxx`
  /// it means bits marked with `C`. Contains column number of the fired
  /// pixel.
  uint16_t m_column;

  /// @brief Value of 16-24 most significant bits
  ///
  /// In the following record: `xxxx xxxx xxxx xxxR RRRR RRRR xxxx xxxx`
  /// it means bits marked with `R`. Contains row number of the fired
  /// pixel.
  uint32_t m_row;

  /// @brief Value of eight least significant bits
  ///
  /// In the following record: `xxxx xxxx xxxx xxxx xxxx xxxx TTTT TTTT`
  /// it means bits marked with `T`. Contains information about signal
  /// time over threshold.
  uint16_t m_timeOverThreshold;
};
#endif

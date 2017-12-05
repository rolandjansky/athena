/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_RAWDATACOMMONHEAD_H
#define AFP_RAWDATACOMMONHEAD_H

#include <stdint.h>

/// @brief Base class representing first 8 bits in a record in bytestream 
///
/// The class is meant to be used as a base for classes representing
/// records in AFP bytestream. The format of the bytestream is
/// documented in presentation:
/// https://indico.cern.ch/login/?next=%2Fevent%2F493415%2Fcontributions%2F1172054%2Fattachments%2F1222236%2F1787442%2Fbytestream.pdf
class AFP_RawDataCommonHead
{
public:
  /// @copydoc AFP_RawDataCommonHead::m_hitDiscConfig
  uint16_t hitDiscConfig() const {return m_hitDiscConfig;};
  void setHitDiscConfig(const uint16_t hitDiscConfig) {m_hitDiscConfig = hitDiscConfig;}

  uint16_t link() const {return m_link;};
  void setLink(const uint16_t outlink) {m_link = outlink;}

private:
  /// @brief Value of third and forth most significant bits
  ///
  /// In the following record: `xxdd xxxx xxxx xxxx xxxx xxxx xxxx xxxx`
  /// it means bits marked with `d`. 
  /// - values 0-2 are used by silicon detectors
  /// - value 3 represent time-of-flight detector
  uint16_t m_hitDiscConfig;

  /// @brief Values of the 5-8 most significant bits
  ///
  /// In the following record: `xxxx LLLL xxxx xxxx xxxx xxxx xxxx xxxx`
  /// it means bits marked with `L`.
  uint16_t m_link;
};
#endif

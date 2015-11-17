/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_ZDCCALOUSERHEADER_H
#define TRIGT1CALOBYTESTREAM_ZDCCALOUSERHEADER_H

#include <cstdint>


//namespace LVL1BS {

/** L1Calo User Header class.
 *
 *  The User Header is the first word of the ROD data and contains
 *  Triggered slice offsets for all the sub-detector types.
 *
 *  @author alexander.mazurov@cern.ch
 */


class ZdcCaloUserHeader {
private:
  uint32_t m_header;

public:

  /// Constructor - default just sets word ID and number of header words
  ZdcCaloUserHeader(uint32_t header = 0xf0000001);
  uint8_t length() const;
  uint8_t ppFadc() const;
  uint8_t lut() const;
  uint8_t cp() const;
  uint8_t jep() const;
  uint8_t ppLowerBound() const;

  bool isValid() const;

  static bool isValid(uint32_t word);
};



//} // end namespace

#endif

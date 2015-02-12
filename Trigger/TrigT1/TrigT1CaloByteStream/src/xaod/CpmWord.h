/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_CPMWORD_H
#define TRIGT1CALOBYTESTREAM_CPMWORD_H

#include <cstdint>

namespace LVL1BS {

/** L1Calo User Header class.
 *
 *  The User Header is the first word of the ROD data and contains
 *  Triggered slice offsets for all the sub-detector types.
 *
 *  @author alexander.mazurov@cern.ch
 */

class CpmWord {
private:
  uint32_t m_word;
public:
  /// Constructor - default just sets word ID and number of header words
  CpmWord(uint32_t header = 0x0);
  uint8_t tower1Et() const;
  uint8_t p1() const;
  uint8_t tower0Et() const;
  uint8_t p0() const;
  uint8_t linkDown() const;
  uint8_t ttPair() const;
  uint8_t serialiser() const;
  bool isValid() const;

  static bool isValid(uint32_t word);
};



} // end namespace

#endif

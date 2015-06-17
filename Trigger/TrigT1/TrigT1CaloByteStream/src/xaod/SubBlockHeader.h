/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_SUBBLOCKHEADER_H
#define TRIGT1CALOBYTESTREAM_SUBBLOCKHEADER_H

#include <cstdint>

namespace LVL1BS {

/** L1Calo User Header class.
 *
 *  The User Header is the first word of the ROD data and contains
 *  Triggered slice offsets for all the sub-detector types.
 *
 *  @author alexander.mazurov@cern.ch
 */

class SubBlockHeader {
private:
  uint32_t m_header;
public:
  /// Constructor - default just sets word ID and number of header words
  SubBlockHeader(uint32_t header = 0x0);
  uint8_t type() const;
  uint8_t version() const;
  uint8_t format() const;
  uint8_t seqNum() const;
  uint8_t crate() const;
  uint8_t module() const;
  uint8_t nSlice2() const;
  uint8_t nSlice1() const;
  uint8_t compVer() const { return seqNum();}

  bool isPpmBlock() const;

  bool isSubBlockHeader() const;
  static bool isSubBlockHeader(uint32_t word);
};



} // end namespace

#endif

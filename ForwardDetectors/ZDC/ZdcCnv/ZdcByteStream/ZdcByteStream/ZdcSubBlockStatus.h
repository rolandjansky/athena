/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_ZDCSUBBLOCKSTATUS_H
#define TRIGT1CALOBYTESTREAM_ZDCSUBBLOCKSTATUS_H

#include <cstdint>

//namespace LVL1BS {

/** L1Calo User Header class.
 *
 *  The User Header is the first word of the ROD data and contains
 *  Triggered slice offsets for all the sub-detector types.
 *
 *  @author alexander.mazurov@cern.ch
 */

class ZdcSubBlockStatus {
private:
  uint32_t m_status;
public:
  /// Constructor - default just sets word ID and number of header words
  ZdcSubBlockStatus(uint32_t status = 0x0);
  uint8_t timeout() const;
  uint8_t link() const;
  uint8_t specific() const;
  uint8_t fifo() const;
  uint8_t bcn() const;
  uint8_t protocol() const;
  uint8_t parity() const;
  uint8_t bcLowBits() const;
};



//} // end namespace

#endif

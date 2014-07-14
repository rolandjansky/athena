/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_CMMSUBBLOCK_H
#define TRIGT1CALOBYTESTREAM_CMMSUBBLOCK_H

#include <stdint.h>

#include "L1CaloSubBlock.h"

namespace LVL1BS {

/** Sub-Block class for CMM data.
 *
 *  Based on "ATLAS Level-1 Calorimeter Trigger Read-out Driver"
 *           Version 1.06d
 *
 *  @author Peter Faulkner
 */

class CmmSubBlock : public L1CaloSubBlock {

 public:
   enum CmmFirmwareCode { CMM_CP = 0, CMM_JET = 1, CMM_ENERGY = 2,
                          CMM_UNKNOWN = 3 };
   enum CmmSummingCode  { CRATE = 0, SYSTEM = 1 };
   enum CmmPositions    { LEFT = 0, RIGHT = 1 };

   CmmSubBlock();
   ~CmmSubBlock();

   /// Store CMM header
   void setCmmHeader(int version, int format, int slice, int crate,
                     int summing, int firmware, int position, int timeslices);

   //   Return CMM specific header data
   int cmmSumming()  const;
   int cmmFirmware() const;
   int cmmPosition() const;
   int timeslices()  const;

   /// CMM differentiation (CMM_CP, CMM_JET, or CMM_ENERGY)
   static CmmFirmwareCode cmmType(uint32_t word);
   /// Determine if header word corresponds to CMM
   static bool cmmBlock(uint32_t word);

 private:
   /// CMM header word ID
   static const int      s_wordIdVal        = 0xe;
   //  CMM fields packed in module field
   static const int      s_cmmSummingBit    = 3;
   static const int      s_cmmFirmwareBit   = 1;
   static const int      s_cmmPositionBit   = 0;
   static const uint32_t s_cmmSummingMask   = 0x1;
   static const uint32_t s_cmmFirmwareMask  = 0x3;
   static const uint32_t s_cmmPositionMask  = 0x1;
   /// Needed for neutral format
   static const int      s_glinkBitsPerSlice = 35;

};

inline int CmmSubBlock::cmmSumming() const
{
  return (module() >> s_cmmSummingBit) & s_cmmSummingMask;
}

inline int CmmSubBlock::cmmFirmware() const
{
  return (module() >> s_cmmFirmwareBit) & s_cmmFirmwareMask;
}

inline int CmmSubBlock::cmmPosition() const
{
  return (module() >> s_cmmPositionBit) & s_cmmPositionMask;
}

} // end namespace

#endif

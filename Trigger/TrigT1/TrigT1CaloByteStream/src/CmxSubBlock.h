/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_CMXSUBBLOCK_H
#define TRIGT1CALOBYTESTREAM_CMXSUBBLOCK_H

#include <stdint.h>

#include "L1CaloSubBlock.h"

namespace LVL1BS {

/** Sub-Block class for CMX data post LS1.
 *
 *  Based on "ATLAS Level-1 Calorimeter Trigger Read-out Driver"
 *           Version X.xxx                                         <<== CHECK
 *
 *  @author Peter Faulkner
 */

class CmxSubBlock : public L1CaloSubBlock {

 public:
   enum CmxFirmwareCode { CMX_CP = 0, CMX_JET = 1, CMX_ENERGY = 2,        // <<== CHECK
                          CMX_UNKNOWN = 3 };
   enum CmxSummingCode  { CRATE = 0, SYSTEM = 1 };
   enum CmxPositions    { LEFT = 0, RIGHT = 1 };

   CmxSubBlock();
   ~CmxSubBlock();

   /// Store CMX header
   void setCmxHeader(int version, int format, int slice, int crate,
                     int summing, int firmware, int position, int timeslices);

   //   Return CMX specific header data
   int cmxSumming()  const;
   int cmxFirmware() const;
   int cmxPosition() const;
   int timeslices()  const;

   /// CMX differentiation (CMX_CP, CMX_JET, or CMX_ENERGY)
   static CmxFirmwareCode cmxType(uint32_t word);
   /// Determine if header word corresponds to CMX
   static bool cmxBlock(uint32_t word);

 private:
   /// CMX header word ID
   static const int      s_wordIdVal        = 0xe;
   //  CMX fields packed in module field
   static const int      s_cmxSummingBit    = 3;
   static const int      s_cmxFirmwareBit   = 1;
   static const int      s_cmxPositionBit   = 0;
   static const uint32_t s_cmxSummingMask   = 0x1;
   static const uint32_t s_cmxFirmwareMask  = 0x3;
   static const uint32_t s_cmxPositionMask  = 0x1;
   /// Needed for neutral format
   static const int      s_glinkBitsPerSlice = 97;

};

inline int CmxSubBlock::cmxSumming() const
{
  return (module() >> s_cmxSummingBit) & s_cmxSummingMask;
}

inline int CmxSubBlock::cmxFirmware() const
{
  return (module() >> s_cmxFirmwareBit) & s_cmxFirmwareMask;
}

inline int CmxSubBlock::cmxPosition() const
{
  return (module() >> s_cmxPositionBit) & s_cmxPositionMask;
}

} // end namespace

#endif

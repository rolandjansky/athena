/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_CMXENERGYSUBBLOCK_H
#define TRIGT1CALOBYTESTREAM_CMXENERGYSUBBLOCK_H

#include <stdint.h>
#include <vector>

#include "CmxSubBlock.h"

namespace LVL1BS {

/** Sub-Block class for CMX-Energy data post LS1.
 *
 *  Based on "ATLAS Level-1 Calorimeter Trigger Read-out Driver"
 *           Version X.xxx                                          //<<== CHECK
 *
 *  @author Peter Faulkner
 */

class CmxEnergySubBlock : public CmxSubBlock {

 public:
   enum WordIdType { MODULE_ID, CRATE_SYSTEM_ID };
   enum EnergyType { ENERGY_EX, ENERGY_EY, ENERGY_ET, MAX_ENERGY_TYPE };
   enum HitsType   { MISSING_ET_SIG, MISSING_ET, SUM_ET, MAX_HITS_TYPE };
   enum SourceType { REMOTE, LOCAL, TOTAL, MAX_SOURCE_TYPE };
   enum SumType    { STANDARD, RESTRICTED_WEIGHTED, MAX_SUM_TYPE };

   CmxEnergySubBlock();
   ~CmxEnergySubBlock();

   /// Clear all data
   // cppcheck-suppress duplInheritedMember
   void clear();

   /// Return energy subsum for given JEM and energy type
   unsigned int energy(int slice, int jem, EnergyType eType) const;
   /// Return energy subsum error for given JEM and energy type
   int error(int slice, int jem, EnergyType eType) const;
   /// Return energy subsum for given source, sum type and energy type
   unsigned int energy(int slice, SourceType source, SumType sType,
                                  EnergyType eType) const;
   /// Return energy subsum error for given source, sum type and energy type
   int error(int slice, SourceType source, SumType sType,
                                           EnergyType eType) const;
   /// Return hits map for given hits type and sum type
   unsigned int hits(int slice, HitsType hType, SumType sType) const;

   /// Store energy subsums and errors for given JEM
   void setSubsums(int slice, int jem,
                   unsigned int ex, unsigned int ey, unsigned int et,
                   int exError, int eyError, int etError);
   /// Store energy subsums and errors for given source and sum type
   void setSubsums(int slice, SourceType source, SumType sType,
                   unsigned int ex, unsigned int ey, unsigned int et,
                   int exError, int eyError, int etError);
   /// Store hits map for given hits type and sum type
   void setEtHits(int slice, HitsType hType, SumType sType, unsigned int map);

   /// Pack data
   bool pack();
   /// Unpack data
   bool unpack();

 private:
   /// Data word length
   static const int      s_wordLength       = 32;
   //  Energy subsums bit positions and masks
   static const int      s_overflowBit      = 15;
   static const int      s_errorBit         = 16;
   static const int      s_etHitsBit        = 16;
   static const int      s_energyTypeJemBit = 23;
   static const int      s_jemBit           = 25;
   static const int      s_energyTypeBit    = 24;
   static const int      s_sumTypeBit       = 26;
   static const int      s_sourceBit        = 27;
   static const int      s_wordIdBit        = 29;
   static const int      s_maxJems          = 16;
   static const int      s_maxSums          = 66;
   static const uint32_t s_energyJemMask    = 0x3fff;
   static const uint32_t s_energySumMask    = 0x7fff;
   static const uint32_t s_errorMask        = 0x1;
   static const uint32_t s_overflowMask     = 0x1;
   static const uint32_t s_etHitsMask       = 0xff;
   static const uint32_t s_jemMask          = 0xf;
   static const uint32_t s_energyTypeMask   = 0x3;
   static const uint32_t s_sumTypeMask      = 0x1;
   static const uint32_t s_sourceMask       = 0x3;
   static const uint32_t s_wordIdMask       = 0x7;
   //  Neutral format
   static const int      s_jemSumBits        = 14;
   static const int      s_jemPaddingBits    = 9;
   static const int      s_sumBitsEtCrate    = 14;
   static const int      s_sumBitsEtSys      = 15;
   static const int      s_sumBitsExEy       = 15;
   static const int      s_bunchCrossingBits = 12;
   static const int      s_etHitMapsBits     = 8;
   static const int      s_paddingBits       = 43;

   int  index(int slice, int pos) const;
   void resize();

   /// Pack neutral data
   bool packNeutral();
   /// Pack uncompressed data
   bool packUncompressed();
   /// Unpack neutral data
   bool unpackNeutral();
   /// Unpack uncompressed data
   bool unpackUncompressed();

   /// Energy subsums data
   std::vector<uint32_t> m_sumsData;

};

} // end namespace

#endif

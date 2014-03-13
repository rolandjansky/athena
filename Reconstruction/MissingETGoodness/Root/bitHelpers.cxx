/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MissingETGoodness/bitHelpers.h"
#include "TROOT.h"

namespace METGoodiesHelpers {

   static ULong64_t ulong_one(1);
   static ULong64_t veroffset(53);
   static ULong64_t versionMask=((ulong_one<<10)-1);
   static ULong64_t bitsMask=((ulong_one<<54)-1);

   bool SplitBits(const ULong64_t& CombinedBits, ULong64_t& version, ULong64_t& CutBits)  {
      CutBits=( CombinedBits&bitsMask);
      version=((CombinedBits>>53)&versionMask);
      return 1;
   }
   
   bool CombineBits(ULong64_t& CombinedBits,const ULong64_t& version,const ULong64_t& CutBits) {
      CombinedBits=0;
      ULong64_t tmp_version;
      tmp_version=version&versionMask;
      if (tmp_version == version){
         ULong64_t shiftedversion=version<<veroffset;         
         ULong64_t tmp_bits=CutBits&bitsMask;
         if (CutBits == tmp_bits){
            CombinedBits=tmp_bits+shiftedversion;
            return 1;
         }
      }
      return 0;
   }
}

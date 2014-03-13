/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * Namespace  : bitHelpers                                                        *
 *                                                                                *
 * Utility classes for working with the the bit patterns containg cut results     *
 * The first 54 bits are the cut bits, the last 10 bits are the version number    *
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Johan Lundberg <johan.lundberg@cern.ch> - CERN, Switzerland               *
 **********************************************************************************/


#ifndef __METGoodiesbitHelpers__
#define __METGoodiesbitHelpers__

#include <TH1F.h>

namespace METGoodiesHelpers {

/**********************************************************************************
 * input: 
 *       bitset containing combined METGooness cut-version number and cut evaluation bits 
 * output:
 *       bits and version
 **********************************************************************************/   
   bool SplitBits(const ULong64_t& CombinedBits, ULong64_t& version, ULong64_t& CutBits) ;

/**********************************************************************************
 * output:
 *       bitset containing combined METGooness cut-version number and cut evaluation bits 
 * input: 
 *       bits and version
 **********************************************************************************/   
   bool CombineBits(ULong64_t& CombinedBits,const ULong64_t& version,const ULong64_t& CutBits);

}

#endif


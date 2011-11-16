/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRecConditions/LArBadChannelEnum.h"

// DO NOT USE THESE CONSTANTS!
///const LArBadChannelEnum::BitWord LArBadChannelEnum::HighGainMask=0x3ff;
///const LArBadChannelEnum::BitWord LArBadChannelEnum::MediumGainMask=0x1c7f;
///const LArBadChannelEnum::BitWord LArBadChannelEnum::LowGainMask=0xe07f;
// Instead, use the following methods:
// LArBadChanBitPacking::highGainMask()   
// LArBadChanBitPacking::mediumGainMask()
// LArBadChanBitPacking::lowGainMask()


//Bit-pattern as defined in LArBadChannelEnum
//     09876 543 210 987 6543210
// HG: 00000 000 000 111 1111111
// MG: 00000 000 111 000 1111111
// LG: 00000 111 000 000 1111111

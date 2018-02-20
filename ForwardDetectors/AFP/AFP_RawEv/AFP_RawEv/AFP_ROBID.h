/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_RAWEV_AFP_ROBID_H
#define AFP_RAWEV_AFP_ROBID_H

#include <stdint.h>


/// This class contains static members encoding A and C side robs
class AFP_ROBID
{
public:
  static constexpr unsigned int sideA = 0x00850001; ///< ROB ID of stations on A side
  static constexpr unsigned int sideC = 0x00850002; ///< ROB ID of stations on C side
};

#endif

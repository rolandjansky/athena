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
  static const uint32_t sideA; ///< ROB ID of stations on A side
  static const uint32_t sideC; ///< ROB ID of stations on C side
};

#endif

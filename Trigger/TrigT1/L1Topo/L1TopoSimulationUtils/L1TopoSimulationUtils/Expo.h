/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EXPO_H
#define EXPO_H

#include <string>
#include <vector>
#include <unordered_map>
#include "L1TopoSimulationUtils/L1TopoDataTypes.h"

namespace TSU {
  struct Expo{
    static const std::unordered_map<int,std::string> E;
  };
}
#endif

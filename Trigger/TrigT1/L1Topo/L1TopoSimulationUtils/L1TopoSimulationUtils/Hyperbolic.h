/*********************************
 * Hyperbolic.h
 * Created by Ignacio Aracena June 2015
 * <ignacio.aracena@cern.ch> 
 * Copyright (c) 2015 Ignacio Aracena. All rights reserved.
 *
 * @brief Lookup table for hyperbolic functions.
 *
**********************************/

#ifndef L1TopoSimulationUtils_Hyperbolic
#define L1TopoSimulationUtils_Hyperbolic

#include <string>
#include <vector>
#include "L1TopoSimulationUtils/L1TopoDataTypes.h"

namespace TSU {
   struct Hyperbolic{
       static const std::vector<std::string> Cosh;
   };
}
#endif

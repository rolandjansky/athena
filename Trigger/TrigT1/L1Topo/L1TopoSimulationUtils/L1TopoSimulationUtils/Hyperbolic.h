/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * Hyperbolic.h
 * Created by Ignacio Aracena June 2015
 * <ignacio.aracena@cern.ch> 
 *
 * @brief Lookup table for hyperbolic functions.
 *
**********************************/

#ifndef L1TopoSimulationUtils_Hyperbolic
#define L1TopoSimulationUtils_Hyperbolic

#include <string>
#include <vector>
#include <unordered_map>
#include "L1TopoSimulationUtils/L1TopoDataTypes.h"

namespace TSU {
   struct Hyperbolic{
     static const std::unordered_map<unsigned,std::string> Coshleg;
     static const std::unordered_map<unsigned,std::string> Cosh;
   };
}
#endif

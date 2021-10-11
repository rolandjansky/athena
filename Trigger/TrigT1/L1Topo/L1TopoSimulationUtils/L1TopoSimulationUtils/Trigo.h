/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * Trigo.h
 * Created by Ignacio Aracena June 2015 
 * <ignacio.aracena@cern.ch> 
 *
 * @brief Lookup table for trigonometric functions
 *
**********************************/

#ifndef L1TopoSimulationUtils_Trigo
#define L1TopoSimulationUtils_Trigo

#include <string>
#include <vector>
#include "L1TopoDataTypes.h"

namespace TSU {
   struct Trigo{
     static const std::vector<std::string> Cosleg;
     static const std::vector<std::string> Sinleg;
     static const std::vector<std::string> Cos;
     static const std::vector<std::string> Sin;
     static int atan2leg(TSU::L1TopoDataTypes<16,0> x, TSU::L1TopoDataTypes<16,0> y);
     static int atan2(TSU::L1TopoDataTypes<16,0> x, TSU::L1TopoDataTypes<16,0> y);
   };
}
#endif

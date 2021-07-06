/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOFEXPERF_GTOWERHELPERS_H
#define TRIGT1CALOFEXPERF_GTOWERHELPERS_H

#include <cmath>
#include <string>
#include <ostream>

namespace LVL1
{
  namespace gFEX
  {
    enum class FPGA
    {
      A,
      B,
      C,
      N_FPGAS
    };
    /// Get the FPGA code from the tower eta
    FPGA getFPGA(float eta);

    /// Get the name of an FPGA
    std::string FPGAToString(FPGA fpga);

    /// Get the enum from the name of an FPGA. Returning N_FPGA means that the string wasn't correct
    FPGA FPGAFromString(std::string name);

    /// Get the GCaloTower areas from their eta bins
    float towerArea(float eta);

  } // namespace gFEX
} // namespace LVL1

std::ostream &operator<<(std::ostream &os, LVL1::gFEX::FPGA fpga);
#endif //> !TRIGT1CALOFEXPERF_GTOWERHELPERS_H
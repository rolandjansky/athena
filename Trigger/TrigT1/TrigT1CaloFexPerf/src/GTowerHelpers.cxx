/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "GTowerHelpers.h"
#include <algorithm>
#include <cctype>

namespace LVL1
{
  namespace gFEX
  {

    /// Get the FPGA code from the tower eta
    FPGA getFPGA(float eta)
    {
      if (eta > -2.5 && eta <= 0)
        return FPGA::A;
      else if (eta > 0 && eta <= 2.5)
        return FPGA::B;
      else if (std::abs(eta) > 2.5)
        return FPGA::C;
      else
        // Given the conditions above this should never trigger...
        return FPGA::N_FPGAS;
    }

    /// Get the name of an FPGA
    std::string FPGAToString(FPGA fpga)
    {
      switch (fpga)
      {
      case FPGA::A:
        return "A";
      case FPGA::B:
        return "B";
      case FPGA::C:
        return "C";
      default:
        return "NOTANFPGA";
      }
    }

    /// Get the enum from the name of an FPGA. Returning N_FPGA means that the string wasn't correct
    FPGA FPGAFromString(std::string name)
    {
      // Convert the string to uppercase
      std::transform(name.begin(), name.end(), name.begin(), toupper);
      if (name == "A")
        return FPGA::A;
      else if (name == "B")
        return FPGA::B;
      else if (name == "C")
        return FPGA::C;
      else
        return FPGA::N_FPGAS;
    }

    float towerArea(float eta)
    {
      eta = std::abs(eta);
      if (eta < 2.4)
        return 1;
      else if (eta < 2.5)
        return 0.5;
      else if (eta < 3.1)
        return 1;
      else
        return 4;
    }
  } // namespace gFEX
} // namespace LVL1

std::ostream &operator<<(std::ostream &os, LVL1::gFEX::FPGA fpga)
{
  return os << LVL1::gFEX::FPGAToString(fpga);
}
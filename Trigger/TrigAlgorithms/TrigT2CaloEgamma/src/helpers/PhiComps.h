/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGT2CALOEGAMMA_PHICOMPS_H
#define TRIGT2CALOEGAMMA_PHICOMPS_H


#include <cmath> // for M_PI

namespace Ringer{
  /**
   * Helper function to identify wrap around cases.
   * if reverse_wrap = true, then we check for reverse wrap_arround.
   */
  inline bool check_wrap_around(const double phi_center, const bool reverse_wrap) {
    if (reverse_wrap) return (phi_center < (1.0 - M_PI)); // 1.0 is the PI_THRESHOLD
    else return (phi_center > (M_PI - 1.0));
  }
  
  /**
   * Helper function to fix wrap around cases.
   * if reverse_wrap = true, then we fix reverse wrap_arround cases.
   */
  inline double fix_wrap_around(const double phi_value, const bool reverse_wrap) {
    if (reverse_wrap) {
      if (phi_value > 0.)  return (phi_value - 2*M_PI);
    } else {
      if (phi_value < 0.) return (phi_value + 2*M_PI);
    }
    return phi_value;
  }

}

#endif

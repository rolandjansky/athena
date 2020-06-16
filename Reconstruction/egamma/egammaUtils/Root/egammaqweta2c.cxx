/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaUtils/egammaqweta2c.h"
#include "xAODCaloEvent/CaloCluster.h"

#include <cmath>

namespace {
constexpr float P0A[3] = {0.0045, 0.005375, -0.0562};
constexpr float P1A[3] = {-0.0016, -0.0215, 0.114};
constexpr float P2A[3] = {-0.0866, 0.0215, -0.053};

constexpr float P0B[3] = {0.0039, 0.005075, -0.0324};
constexpr float P1B[3] = {0.00816, -0.0203, 0.0653};
constexpr float P2B[3] = {-0.145, 0.0203, -0.0286};

constexpr float P0C[3] = {0.0047, 0.0035, 0.0};
constexpr float P1C[3] = {-0.0184, -0.0139, 0.0};
constexpr float P2C[3] = {0.0180, 0.0137, 0.0};
} // namespace

float egammaqweta2c::Correct(const float eta, const float etacell, const float weta2) {

  const float aeta = fabs(eta);
  const float etarel = RelPosition(eta, etacell);

  if (aeta < 0.8) {
    if (etarel < 0.1) {
      return (weta2 - (P0A[0] + P1A[0] * etarel + P2A[0] * etarel * etarel));
    }
    if (etarel < 0.9) {
      return (weta2 - (P0A[1] + P1A[1] * etarel + P2A[1] * etarel * etarel));
    }
    return (weta2 - (P0A[2] + P1A[2] * etarel + P2A[2] * etarel * etarel));
  }

  if (aeta < 1.5) {
    if (etarel < 0.1) {
      return (weta2 - (P0B[0] + P1B[0] * etarel + P2B[0] * etarel * etarel));
    }
    if (etarel < 0.9) {
      return (weta2 - (P0B[1] + P1B[1] * etarel + P2B[1] * etarel * etarel));
    }
    return (weta2 - (P0B[2] + P1B[2] * etarel + P2B[2] * etarel * etarel));
  }

  if (aeta < 1.8) {
    if (etarel < 0.1) {
      return (weta2 - (P0B[0] + P1B[0] * etarel + P2B[0] * etarel * etarel));
    }
    if (etarel < 0.9) {
      return (weta2 - (P0B[1] + P1B[1] * etarel + P2B[1] * etarel * etarel));
    }
    return (weta2 - (P0B[2] + P1B[2] * etarel + P2B[2] * etarel * etarel));
  }

  if (aeta < 2.0) {
    return (weta2 - (P0C[0] + P1C[0] * etarel + P2C[0] * etarel * etarel));
  }

  if (aeta < 2.5) {
    return (weta2 - (P0C[1] + P1C[1] * etarel + P2C[1] * etarel * etarel));
  }
  return weta2;
}

double egammaqweta2c::RelPosition(float eta, float etacell) {

  if (eta == -999.)
    return -999;
  // position within cell (-cellsize/2 - cellsize/2)
  const double x = fabs(eta - etacell - 0.025 / 2.);
  const double g = 0.025;
  return fmod(x, g) / (double)0.025;
}


/*
 *   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOFEXSIM_PUFIT_H
#define TRIGT1CALOFEXSIM_PUFIT_H
/*
 * Class   : Pufit
 *
 * Author  : Myers, Ava (amyers@cern.ch); Burr, Jon (jon.burr@cern.ch0
 *
 *Implements the Pufit algorithm for MET reconstruction and pileup subtraction.
 */
#include "xAODTrigL1Calo/JGTowerContainer.h"
#include <array>


/**
 *@brief Namespace for "global" variables in PUfit
 */
namespace PUfitVar{
  constexpr float maxEta = 4.8;
  constexpr float caloResSqrtTerm = 15.81;
  constexpr float caloResFloor = 50;
  constexpr float nSigma = 5.0;
  constexpr float constraintWeight = 1.;
  constexpr float trimFactor = 0.9;
  /// Fewer eta bins than offline, but ensures that they approximately match the
  /// actual L1 towers
  constexpr std::size_t nEtaBins = 12;
  constexpr std::size_t nPhiBins = 8;
  constexpr std::size_t nTowers = nEtaBins*nPhiBins;
}

/// Run the pufit calculation
std::array<float, 3> Run_PUfit(
    const xAOD::JGTowerContainer* towers,
    float inputSigma = PUfitVar::nSigma,
    bool  useNegTowers=false);

#endif

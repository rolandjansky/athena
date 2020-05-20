/*
 *   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloFexSim/Pufit.h"
#include "TrigEFMissingET/METComponent.h"
#include "TrigEFMissingET/SignedKinematics.h"
#include "TrigEFMissingET/PufitGrid.h"
#include "TrigEFMissingET/PufitUtils.h"

std::array<float, 3> Run_PUfit(
    const xAOD::JGTowerContainer* towers, float inputSigma, bool useNegTowers)
{
  using namespace HLT::MET;
  PufitGrid::GridParameters params{
    PUfitVar::maxEta, PUfitVar::nEtaBins, PUfitVar::nPhiBins};
  PufitGrid grid(params);

  // Start by filling the grid with the towers
  for (const xAOD::JGTower* tower : *towers)
    // Conditionally skip negative towers
    if (useNegTowers || tower->et() > 0)
      grid += SignedKinematics::fromEtEtaPhi(
          tower->et(), tower->eta(), tower->phi() );

  // Then calculate mean and variance
  std::pair<double, double> mav = PufitUtils::trimmedMeanAndVariance(
      grid, PUfitVar::trimFactor);
  // Calculate the threshold
  double threshold = mav.first + inputSigma*sqrt(mav.second);

  // Apply the masks, store the masked towers and calculate the pileup
  // quantities
  PufitUtils::CovarianceSum pileupSum;
  std::vector<SignedKinematics> masked;
  for (PufitGrid::Tower& tower : grid) {
    if (tower.sumEt() > threshold) {
      tower.mask(true);
      masked.push_back(tower);
    }
    else {
      double sigma = 
        PUfitVar::caloResFloor*PUfitVar::caloResFloor + 
        tower.kinematics().absPt()*PUfitVar::caloResSqrtTerm*PUfitVar::caloResSqrtTerm;
      pileupSum.add(tower, sigma);
    }
  }

  // Now derive the corrections
  std::vector<SignedKinematics> corrections = PufitUtils::pufit(
        pileupSum.sum,
        pileupSum.covariance,
        mav.first,
        mav.second,
        masked,
        PUfitVar::constraintWeight);

  // Sum over the masked towers
  METComponent sum = grid.sum(PufitGrid::SumStrategy::Masked);
  // Now add the corrections - the function above returned them with the right
  // sign for this to work
  for (const SignedKinematics& kin : corrections)
    sum += kin;
  return {sum.met(), sum.mpx, sum.mpy};
}

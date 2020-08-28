/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @file TCPufitFex.cxx
 *
 * Implementation of the pufit fex class
 * @author Jon Burr
 *****************************************************************************/

#include "TCPufitFex.h"
#include "TrigEFMissingET/METComponent.h"
#include "TrigEFMissingET/PufitGrid.h"
#include "TrigEFMissingET/PufitUtils.h"

namespace HLT
{
  namespace MET
  {
    TCPufitFex::TCPufitFex(const std::string &name, ISvcLocator *pSvcLocator) : FexBase(name, pSvcLocator)
    {
    }

    StatusCode TCPufitFex::initialize()
    {
      CHECK(m_clusterKey.initialize());
      return initializeBase({"AllTowers", "UncorrSelTowers"});
    }

    StatusCode TCPufitFex::fillMET(
        xAOD::TrigMissingET &met,
        const EventContext &context,
        MonGroupBuilder &) const
    {
      // Retrieve the inputs
      auto clusters = SG::makeHandle(m_clusterKey, context);
      if (!clusters.isValid())
      {
        ATH_MSG_ERROR("Failed to retrieve " << m_clusterKey);
        return StatusCode::FAILURE;
      }

      // Create the pufit grids
      PufitGridSet gridset(m_maxEta, m_nEtaBins, m_nPhiBins);

      // Fill the grids from the input clusters
      for (const xAOD::CaloCluster *iclus : *clusters)
        gridset += SignedKinematics::fromEnergyEtaPhi(
            iclus->e(), iclus->eta(), iclus->phi());

      // We calculate the mean and variance from the 'nominal' grid
      double mean;
      double variance;
      PufitUtils::trimmedMeanAndVariance(
          gridset[NoDisplacement], m_trimFraction, mean, variance);
      // Calculate the threshold
      float threshold = mean + m_nSigma * sqrt(variance);

      // Apply the masks
      for (PufitGrid &grid : gridset.grids)
        for (PufitGrid::Tower &tower : grid)
          tower.mask(tower.sumEt() > threshold);

      // Select the right grid
      GridDisplacement idx = PufitUtils::selectGrid(gridset);
      const PufitGrid &grid = gridset[idx];

      // Build the pileup sum and prepare the vector of corrections
      PufitUtils::CovarianceSum pileupSum;
      std::vector<SignedKinematics> masked;
      for (const PufitGrid::Tower &tower : grid)
      {
        if (!tower.masked())
        {
          float sigma =
              m_caloNoise * m_caloNoise +
              tower.kinematics().absPt() * m_caloStoch * m_caloStoch;
          pileupSum.add(tower, sigma);
        }
        else
          masked.push_back(tower);
      }
      // Perform the fit
      std::vector<SignedKinematics> corrections = PufitUtils::pufit(
          pileupSum.sum,
          pileupSum.covariance,
          mean,
          variance,
          masked,
          m_constraintImportance);

      // Save the sum over all towers to the corresponding component
      grid.sum(PufitGrid::SumStrategy::All).fillMETComponent(0, met);
      // Get the sum over the selected towers
      METComponent sum = grid.sum(PufitGrid::SumStrategy::Masked);
      // Save this uncorrected sum to the corresponding component
      sum.fillMETComponent(1, met);
      // Apply the corrections
      for (const SignedKinematics &kin : corrections)
        sum += kin;
      sum.fillMET(met);
      return StatusCode::SUCCESS;
    }
  } // namespace MET
} // namespace HLT

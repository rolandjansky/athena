/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */
/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @file PUSplitPufitFex.cxx
 *
 * Implementation of pileup-split pufit fex class
 * @author Jon Burr
 *****************************************************************************/

#include "PUSplitPufitFex.h"
#include "TrigEFMissingET/METComponent.h"
#include "TrigEFMissingET/PUSplitGrid.h"
#include "TrigEFMissingET/PufitUtils.h"
#include "StoreGate/DecorKeyHelpers.h"
#include "StoreGate/ReadDecorHandle.h"

namespace HLT
{
  namespace MET
  {
    double PUSplitPufitFex::getSigma(const SignedKinematics &kin) const
    {
      return m_caloNoise * m_caloNoise + kin.absPt() * m_caloStoch * m_caloStoch;
    }

    PUSplitPufitFex::PUSplitPufitFex(const std::string &name, ISvcLocator *pSvcLocator)
        : FexBase(name, pSvcLocator)
    {
    }

    StatusCode PUSplitPufitFex::initialize()
    {
      CHECK(m_inputKey.initialize());

      // Update the decor keys if necessary
      if (m_inputCategoryKey.key().find(".") == std::string::npos)
        m_inputCategoryKey = m_inputKey.key() + "." + m_inputCategoryKey.key();
      else if (SG::contKeyFromKey(m_inputCategoryKey.key()) != m_inputKey.key())
      {
        ATH_MSG_ERROR("Input category key does not match the input key!");
        return StatusCode::FAILURE;
      }
      CHECK(m_inputCategoryKey.initialize());
      return initializeBase({"NeutralForward", "ChargedHS", "ChargedPU", "UncorrSelNF"});
    }

    StatusCode PUSplitPufitFex::fillMET(
        xAOD::TrigMissingET &met,
        const EventContext &context,
        MonGroupBuilder &) const
    {
      // Retrieve the input
      auto input = SG::makeHandle(m_inputKey, context);
      if (!input.isValid())
      {
        ATH_MSG_ERROR("Failed to retrieve " << m_inputKey);
        return StatusCode::FAILURE;
      }
      auto categoryAcc = SG::makeHandle<int>(m_inputCategoryKey, context);

      // Create the gridset
      PUSplitGridSet gridset(m_maxEta, m_nEtaBins, m_nPhiBins);

      // Fill the grids from the input objects
      for (const xAOD::IParticle *ipart : *input)
      {
        PUClassification::PUClassification type = PUClassification::PUClassification(categoryAcc(*ipart));
        SignedKinematics kin = SignedKinematics::fromEnergyEtaPhi(
            ipart->e(), ipart->eta(), ipart->phi());
        switch (type)
        {
        case PUClassification::NeutralForward:
          gridset.get<PUClassification::NeutralForward>() += kin;
          break;
        case PUClassification::ChargedHS:
          gridset.get<PUClassification::ChargedHS>() += kin;
          break;
        case PUClassification::ChargedPU:
          gridset.get<PUClassification::ChargedPU>() += kin;
          break;
        default:
          ATH_MSG_ERROR("Invalid PU category: " << type);
          return StatusCode::FAILURE;
        }
      }

      // Calculate the mean and variance from the 'nominal' grid
      double mean;
      double variance;
      PufitUtils::trimmedMeanAndVariance(
          gridset[NoDisplacement], m_neutralThresholdMode, m_trimFraction, mean, variance);
      // Calculate the threshold
      double threshold = mean + m_nSigma * sqrt(variance);

      // Mask towers above the threshold
      std::size_t count = 0;
      for (PUSplitGrid &grid : gridset.grids)
        for (PUSplitGrid::Tower &tower : grid)
          if (tower.sumEt(m_neutralThresholdMode) > threshold)
          {
            tower.mask(true);
            ++count;
          }

      // Select the right grid
      GridDisplacement displacement = PufitUtils::selectGrid(
          gridset, m_neutralThresholdMode);
      const PUSplitGrid &grid = gridset[displacement];

      // Build the background sum, the masked tower kinematics (used for their
      // directions), the list of the expected pileup values in each masked tower
      // and the variances on each of those expected values
      PufitUtils::CovarianceSum pileupSum;
      std::vector<SignedKinematics> masked;
      masked.reserve(count);
      std::vector<double> means;
      means.reserve(count);
      std::vector<double> variances(count, variance);

      for (const PUSplitGrid::Tower &tower : grid)
      {
        const SignedKinematics &cPUKin = tower.get<PUClassification::ChargedPU>();
        if (!tower.masked())
        {
          // If the tower is masked, then add its neutral component to the background sum
          const SignedKinematics &kin = tower.get<PUClassification::NeutralForward>();
          pileupSum.add(kin, getSigma(kin));
        }
        else
        {
          masked.push_back(tower.kinematics(PUClassification::NFcHS));
          // Add the expected pileup contribution. Optionally, exclude the cPU
          // component from this as this is already accounted
          if (m_subtractCPUFromMean)
            means.push_back(mean - cPUKin.pt());
          else
            means.push_back(mean);
        }
        // cPU elements always count as background
        // (There's a possible TODO here - maybe this should only be done when subtractCPUFromMean is true?)
        pileupSum.add(cPUKin, getSigma(cPUKin));
      }

      if (msgLvl(MSG::VERBOSE))
      {
        ATH_MSG_VERBOSE("Pileup sum = " << pileupSum.sum);
        ATH_MSG_VERBOSE("Pileup covariance = " << pileupSum.covariance);
        ATH_MSG_VERBOSE("Mean background energy = " << mean);
        ATH_MSG_VERBOSE("Background energy variance = " << variance);
        ATH_MSG_VERBOSE("Constraint importance = " << m_constraintImportance);
        ATH_MSG_VERBOSE("Masked tower directions (sin, cos): ");
        for (const SignedKinematics &kin : masked)
          ATH_MSG_VERBOSE("  (" << kin.sinPhi() << ", " << kin.cosPhi() << ")");
      }
      ATH_MSG_DEBUG("Calculate corrections");

      std::vector<SignedKinematics> corrections = PufitUtils::pufit(
          pileupSum.sum,
          pileupSum.covariance,
          means,
          variances,
          masked,
          m_constraintImportance);

      // Fill components
      grid.get<PUClassification::NeutralForward>().sum(PufitGrid::SumStrategy::All).fillMETComponent(0, met);
      grid.get<PUClassification::ChargedHS>().sum(PufitGrid::SumStrategy::All).fillMETComponent(1, met);
      grid.get<PUClassification::ChargedPU>().sum(PufitGrid::SumStrategy::All).fillMETComponent(2, met);
      grid.get<PUClassification::NeutralForward>().sum(PufitGrid::SumStrategy::Masked).fillMETComponent(3, met);

      // Now build the final sum
      METComponent sum;
      // Add the NF components of all masked towers
      sum += grid.get<PUClassification::NeutralForward>().sum(PufitGrid::SumStrategy::Masked);
      // Add the cHS components of all towers
      sum += grid.get<PUClassification::ChargedHS>().sum(PufitGrid::SumStrategy::All);
      ATH_MSG_DEBUG("Before corrections: " << sum);
      // Add the corrections
      for (const SignedKinematics &kin : corrections)
      {
        ATH_MSG_DEBUG("Correction: (px, py, pz, et) = (" << kin.px() << ", " << kin.py() << ", " << kin.pz() << ", " << kin.et() << ")");
        sum += kin;
      }
      ATH_MSG_DEBUG("Final MET: " << sum);
      sum.fillMET(met);

      return StatusCode::SUCCESS;
    }
  } // namespace MET
} // namespace HLT
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "TCPufitFex.h"
#include "TrigEFMissingET/METComponent.h"
#include "TrigEFMissingET/SignedKinematics.h"
#include "TrigEFMissingET/PufitUtils.h"
#include "TrigEFMissingET/PufitGrid.h"
#include "TrigInterfaces/HLTCheck.h"
#include "xAODCaloEvent/CaloClusterContainer.h"


namespace HLT { namespace MET {
  TCPufitFex::TCPufitFex(const std::string& name, ISvcLocator* pSvcLocator) :
    FexBase(name, pSvcLocator)
  {
    declareProperty("NSigma", m_nSigma=5,
        "Set the threshold at mean + NSigma*variance");
    declareProperty("MaxEta", m_maxEta=5, "The eta range to use");
    declareProperty("NEtaBins", m_nEtaBins=14, "The number of eta bins");
    declareProperty("NPhiBins", m_nPhiBins=8, "The number of phi bins");
    declareProperty("TrimFraction", m_trimFraction=0.9,
        "The fraction of bins to use when calculating the mean and variance");
    declareProperty("CaloNoise", m_caloNoise=50,
        "The noise term in the calorimeter noise estimate [MeV]");
    declareProperty("CaloStochastic", m_caloStoch=15.81,
        "The stochastic term in the calorimeter noise estimate [MeV^1/2]");
    declareProperty("ConstraintImportance", m_constraintImportance=1,
        "The relative importance of the two constraints");
  }

  TCPufitFex::~TCPufitFex() {}

  HLT::ErrorCode TCPufitFex::hltInitialize()
  {
    // Save only an uncorrected component
    std::vector<std::string> componentNames{"Uncorrected"};
    return initializeBase(componentNames);
  }

  HLT::ErrorCode TCPufitFex::hltFinalize()
  {
    return HLT::OK;
  }

  HLT::ErrorCode TCPufitFex::fillMET(
      xAOD::TrigMissingET& met,
      const std::vector<std::vector<HLT::TriggerElement*>>& tes_in)
  {
    const xAOD::CaloClusterContainer* clusters = nullptr;
    if (tes_in.size() < 1 || tes_in.at(0).size() != 1) {
      // For some reason the TC Fex also receives the jet TEs too...
      ATH_MSG_ERROR("Unexpected number of input trigger elements");
      return HLT::NAV_ERROR;
    }
    HLT_CHECK( getFeature(tes_in.at(0).at(0), clusters) );

    // Get the grids
    PufitGridSet gridset(m_maxEta, m_nEtaBins, m_nPhiBins);

    // Now fill the grid
    for (const xAOD::CaloCluster* iclus : *clusters)
      gridset += SignedKinematics::fromEnergyEtaPhi(iclus->e(), iclus->eta(), iclus->phi() );

    // We get the mean and variance from the 'nominal' grid
    std::pair<double, double> mav = PufitUtils::trimmedMeanAndVariance(
        gridset[PufitGridSet::NoDisplacement], m_trimFraction);
    // Calculate the threshold
    double threshold = mav.first + m_nSigma*sqrt(mav.second);
    ATH_MSG_DEBUG("trimmed mean: " << mav.first);
    ATH_MSG_DEBUG("trimmed variance: " << mav.second);
    ATH_MSG_DEBUG("Calculated threshold at " << threshold);

    // Apply the masks
    for (PufitGrid& grid : gridset.grids)
      for (PufitGrid::Tower& tower : grid)
        tower.mask(tower.sumEt() > threshold);

    // Select the right grid.
    PufitGridSet::GridDisplacement displacement = PufitUtils::selectGrid(gridset);
    const PufitGrid& grid = gridset[displacement];
    ATH_MSG_DEBUG("Selected grid " << displacement);

    // Build the pileup sum and prepare the vector of corrections
    PufitUtils::CovarianceSum pileupSum;
    std::vector<SignedKinematics> masked;
    for (const PufitGrid::Tower& tower : grid) {
      if (!tower.masked() ) {
        double sigma = 
          m_caloNoise*m_caloNoise + 
          tower.kinematics().absPt()*m_caloStoch*m_caloStoch;
        pileupSum.add(tower, sigma);
      }
      else
        masked.push_back(tower);
    }
    ATH_MSG_DEBUG("Pileup sum: " << std::endl << pileupSum.sum);
    ATH_MSG_DEBUG("Pileup covariance: " << std::endl << pileupSum.covariance);
    ATH_MSG_DEBUG("Number of corrections: " << masked.size() );
    std::vector<SignedKinematics> corrections = PufitUtils::pufit(
        pileupSum.sum,
        pileupSum.covariance,
        mav.first,
        mav.second,
        masked,
        m_constraintImportance);

    if (msgLvl(MSG::DEBUG) ) {
      ATH_MSG_DEBUG("derived corrections");
      for (const SignedKinematics& kin : corrections)
        ATH_MSG_DEBUG(kin.et() );
    }

    METComponent sum = grid.sum(PufitGrid::SumStrategy::Masked);
    // Now add the corrections
    for (const SignedKinematics& kin : corrections)
      sum += kin;
    // And save the output
    sum.fillMET(met);
    // Get the uncorrected value
    METComponent uncorrected = grid.sum(PufitGrid::SumStrategy::All);
    // Save this to the right component
    uncorrected.fillMETComponent(0, met);
    return HLT::OK;
  }
} } //> end namespace HLT::MET

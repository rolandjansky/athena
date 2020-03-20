/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "CellPufitFex.h"
#include "TrigEFMissingET/PufitUtils.h"
#include "TrigEFMissingET/METComponent.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "TrigEFMissingET/StatusFlags.h"
#include "TrigEFMissingET/SignedKinematics.h"
#include "TrigInterfaces/HLTCheck.h"

namespace HLT { namespace MET {

  CellPufitFex::CellPufitFex(const std::string& name, ISvcLocator* pSvcLocator) :
    FexBase(name, pSvcLocator),
    m_trigData("TrigDataAccess/TrigDataAccess")
  {
    declareProperty("TrigDataAccess", m_trigData,
        "The tool used to access the calorimeter cells");
    declareProperty("CaloNoiseTool", m_noiseTool,
        "The tool used to access calorimeter noise levels");
    declareProperty("AbsoluteNoiseThreshold", m_absNoiseThreshold=2.0,
        "The threshold on the cell absolute energy");
    declareProperty("NegativeNoiseThreshold", m_negNoiseThreshold=5.0,
        "The maximum negative cell energy");
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

  HLT::ErrorCode CellPufitFex::hltInitialize()
  {
    if (!m_trigData.retrieve().isSuccess() ) {
      ATH_MSG_ERROR("Failed to retrieve TrigDataAccess");
      return HLT::ERROR;
    }
    if (!m_noiseTool.retrieve().isSuccess() ) {
      ATH_MSG_ERROR("Failed to retrieve Calo noise tool");
      return HLT::ERROR;
    }
    // Save a component that was the uncorrected MET
    std::vector<std::string> componentNames{"Uncorrected"};
    return initializeBase(componentNames);
  }

  HLT::ErrorCode CellPufitFex::hltFinalize()
  {
    return HLT::OK;
  }

  HLT::ErrorCode CellPufitFex::fillMET(
      xAOD::TrigMissingET& met,
      const std::vector<std::vector<HLT::TriggerElement*>>& tes_in)
  {
    // Verify the inputs - we only use cells which we don't get from a trigger
    // element
    if (tes_in.size() != 0) {
      ATH_MSG_ERROR("Unexpected number " << tes_in.size() << " of TEs received!");
      return HLT::NAV_ERROR;
    }

    // Create the pufit grids to hold the cell contributions
    PufitGridSet gridset(m_maxEta, m_nEtaBins, m_nPhiBins);

    // Also keep track of whether there were any errors from the cell retrieval
    bool cellErrors = false;

    // Load the cells into it
    // First go through the LAr calorimeters
    for (DETID detectorID : {TTEM, TTHEC, FCALEM, FCALHAD}) {
      LArTT_Selector<LArCellCont>::const_iterator itr;
      LArTT_Selector<LArCellCont>::const_iterator end;
      // retrieve the cells
      if (!m_trigData->LoadFullCollections(itr, end, detectorID).isSuccess() ) {
        // Needs a proper message
        ATH_MSG_ERROR("Failed to retrieve cells!");
        return HLT::ERROR;
      }
      cellErrors |= m_trigData->report_error();;
      for (; itr != end; ++itr)
        HLT_CHECK(processCell(gridset, *itr) );
    }

    // Now do the same for Tile
    for (std::size_t ii = 0; ii < m_trigData->TileFullContSize(); ++ii) {
      TileCellCollection::const_iterator itr;
      TileCellCollection::const_iterator end;
      if (!m_trigData->LoadFullCollections(itr, end, ii).isSuccess() ) {
        // Needs a proper message
        ATH_MSG_ERROR("Failed to retrieve cells!");
        return HLT::ERROR;
      }
      cellErrors |= m_trigData->report_error();
      for (; itr != end; ++itr)
        HLT_CHECK( processCell(gridset, *itr) );
    }

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
    // Get the uncorrected value
    METComponent uncorrected = grid.sum(PufitGrid::SumStrategy::All);
   
    if (cellErrors) {
      sum.status |= StatusFlag::BSConvError;
      uncorrected.status |= StatusFlag::BSConvError;
    }

    sum.fillMET(met);
    uncorrected.fillMETComponent(0, met);
    return HLT::OK;
  }
  HLT::ErrorCode CellPufitFex::processCell(
      PufitGridSet& gridset,
      const LArCell* cell)
  {
    return processCell(gridset, cell, false);
  }

  HLT::ErrorCode CellPufitFex::processCell(
      PufitGridSet& gridset,
      const TileCell* cell)
  {
    return processCell(gridset, cell, true);
  }

  HLT::ErrorCode CellPufitFex::processCell(
      PufitGridSet& gridset,
      const CaloCell* cell,
      bool isTile)
  {
    // Make the noise selection. This part looks different between tile and LAr
    // but the two functions do pretty similar things, although without
    // identical results.

    // The noise selection logic is to reject a cell if
    // |E| < m_absNoiseThreshold* sigma or 
    // E < -m_negNoiseThreshold * sigma
    if (isTile) {
      if (!m_noiseTool->isEOverNSigma(
            cell, m_absNoiseThreshold,
            ICalorimeterNoiseTool::MAXSYMMETRYHANDLING,
            ICalorimeterNoiseTool::TOTALNOISE) )
        return HLT::OK;
      if (cell->e() < 0 && m_noiseTool->isEOverNSigma(
            cell, m_negNoiseThreshold,
            ICalorimeterNoiseTool::MAXSYMMETRYHANDLING,
            ICalorimeterNoiseTool::TOTALNOISE) )
        return HLT::OK;
    }
    else {
      double noise = m_noiseTool->getNoise(cell, ICalorimeterNoiseTool::TOTALNOISE);
      if (fabs(cell->e()) < m_absNoiseThreshold*noise ||
          cell->e() < -m_negNoiseThreshold*noise)
        return HLT::OK;
    }

    // Add the cell to the gridset
    const CaloDetDescrElement* dde = cell->caloDDE();
    gridset += SignedKinematics::fromEnergyEtaPhi(
        cell->e(), dde->eta(), dde->phi() );
    return HLT::OK;
  }

} } //> end namespace HLT::MET

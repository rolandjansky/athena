/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CaloRec/src/CaloThinCellsBySamplingAlg.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2019
 * @brief Thin calorimeter cells by sampling.
 */


#include "CaloThinCellsBySamplingAlg.h"
#include "CaloUtils/CaloCellList.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "StoreGate/ThinningHandle.h"
#include "StoreGate/ReadHandle.h"


/**
 * @brief Gaudi initialize method.
 */
StatusCode CaloThinCellsBySamplingAlg::initialize()
{
  ATH_CHECK( m_cells.initialize (m_streamName) );
  ATH_CHECK( detStore()->retrieve (m_caloID) );

  if (!m_samplingNames.empty()) {
    ATH_CHECK( decodeSamplings() );
  }
  return StatusCode::SUCCESS;
}


/**
 * @brief Execute the algorithm.
 * @param ctx Current event context.
 */
StatusCode CaloThinCellsBySamplingAlg::execute (const EventContext& ctx) const
{
  SG::ThinningHandle<CaloCellContainer> cells (m_cells, ctx);
  cells.thinAll();

  for (IdentifierHash cellHash : m_cellHashes) {
    int index = cells->findIndex (cellHash);
    if (index >= 0) {
      cells.keep (index);
    }
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Decode the provided list of calorimeter samplings.
 */
StatusCode CaloThinCellsBySamplingAlg::decodeSamplings()
{
  bool acceptedSamplings[CaloSampling::getNumberOfSamplings()] = {false};
  for (const std::string& name : m_samplingNames) {
    CaloSampling::CaloSample samp = CaloSampling::getSampling (name);
    if (samp == CaloSampling::Unknown) {
      ATH_MSG_ERROR (  "Calorimeter sampling " << name
                       << " is not a valid Calorimeter sampling name and will be ignored! " );
    }
    else {
      acceptedSamplings[static_cast<int> (samp)] = true;
    }
  }

  size_t hashMax = m_caloID->calo_cell_hash_max();
  for (size_t cellHash = 0; cellHash < hashMax; ++cellHash) {
    if (acceptedSamplings[m_caloID->calo_sample (cellHash)]) {
      m_cellHashes.emplace_back(cellHash);
    }
  }

  return StatusCode::SUCCESS;
}

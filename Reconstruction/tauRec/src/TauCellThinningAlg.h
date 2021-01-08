/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
*/

#ifndef TAUREC_TAUCELLTHINNING_H
#define TAUREC_TAUCELLTHINNING_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloClusterCellLinkContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ThinningHandleKey.h"

/**
 * @brief Thin calorimeter cells not associated with tau clusters
 *
 * Thinning algorithm to keep calorimeter cells from tau seed-jet topoclusters
 * Based on CaloThinCellsByClusterAlg from CaloRec package
 */
class TauCellThinningAlg : public AthReentrantAlgorithm
{
  using AthReentrantAlgorithm::AthReentrantAlgorithm;

 public:

  /**
   * @brief Gaudi initialize method.
   */
  virtual StatusCode initialize() override;

  /**
   * @brief Execute the algorithm.
   * @param ctx Current event context.
   */
  virtual StatusCode execute(const EventContext& ctx) const override;


 private:

  // Name of the stream being thinned
  StringProperty m_streamName
    { this, "StreamName", "StreamAOD", "Name of the stream being thinned" };

  // Cell container to thin
  SG::ThinningHandleKey<CaloCellContainer> m_cells
    { this, "Cells", "AllCalo", "Cell container to thin" };

  // Cluster cell link container to thin
  SG::ThinningHandleKey<CaloClusterCellLinkContainer> m_cellLinks
    { this, "CellLinks", "CaloCalTopoClusters_links", "Cell container to thin" };

  // Tau container
  SG::ReadHandleKey<xAOD::TauJetContainer> m_taus
    { this, "Taus", "TauJets", "Container of taus for which cells should be saved" };
};

#endif // not TAUREC_TAUCELLTHINNING_H

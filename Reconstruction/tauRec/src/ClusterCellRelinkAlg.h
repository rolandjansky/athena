/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
*/

#ifndef TAUREC_CLUSTERCELLRELINK_H
#define TAUREC_CLUSTERCELLRELINK_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloClusterCellLinkContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

/**
 * @brief Algorithm that rebuilds clusters using cells from a different container.
 * Only the cell links are updated, other cluster properties are not recomputed.
 * The CaloClusterCellLinkContainer pointing to the new CaloCellContainer is also created.
 */
class ClusterCellRelinkAlg : public AthReentrantAlgorithm
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

  // Cell container the output clusters should point to
  SG::ReadHandleKey<CaloCellContainer> m_cells
    { this, "Cells", "", "Cell container to point to." };
  
  // Input clusters
  SG::ReadHandleKey<xAOD::CaloClusterContainer> m_clustersInput
    { this, "ClustersInput", "", "Input cluster container." };
  
  // Output clusters
  SG::WriteHandleKey<xAOD::CaloClusterContainer> m_clustersOutput
    { this, "ClustersOutput", "", "Output cluster container." };

  // Cell link container for output clusters
  SG::WriteHandleKey<CaloClusterCellLinkContainer> m_cellLinksOutput
    { this,"CellLinksOutput", "", "Output CellLinks container" };

};

#endif // not TAUREC_CLUSTERCELLRELINK_H

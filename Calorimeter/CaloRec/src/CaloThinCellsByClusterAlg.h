// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CaloRec/src/CaloThinCellsByClusterAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2019
 * @brief Thin calorimeter cells not associated with clusters.
 */


#ifndef CALOREC_CALOTHINCELLSBYCLUSTER_H
#define CALOREC_CALOTHINCELLSBYCLUSTER_H


#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "CaloEvent/CaloCellContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ThinningHandleKey.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include <vector>


/**
 * @brief Thin calorimeter cells not associated with clusters.
 *
 * Thinning algorithm to keep calorimeter cells used by clusters in a collection.
 * Optionally can also save all cells in particular samplings within
 * rectangular windows around the clusters matching the cluster size.
 */
class CaloThinCellsByClusterAlg : public AthReentrantAlgorithm
{
public:
  using AthReentrantAlgorithm::AthReentrantAlgorithm;


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
  /**
   * @brief Decode the provided list of calorimeter samplings.
   */
  StatusCode decodeSamplings();


  /// Name of the stream being thinned.
  StringProperty m_streamName
  { this, "StreamName", "", "Name of the stream being thinned" };

  /// Cell container to thin.
  SG::ThinningHandleKey<CaloCellContainer> m_cells
  { this, "Cells", "AllCalo", "Cell container to thin." };

  /// Clusters to read.
  SG::ReadHandleKey<xAOD::CaloClusterContainer> m_clusters
  { this, "Clusters", "", "Container of clusters for which cells should be saved." };

  StringArrayProperty m_samplingNames
  { this, "SamplingCellsName", {},
    "List of names of samplings for which all cluster cells in a rectangular window should be saved" };

  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey{this,"CaloDetDescrManager", "CaloDetDescrManager"};      

  /// Decoded list of samplings.
  std::vector<int> m_validSamplings;
};


#endif // not CALOREC_CALOTHINCELLSBYCLUSTER_H

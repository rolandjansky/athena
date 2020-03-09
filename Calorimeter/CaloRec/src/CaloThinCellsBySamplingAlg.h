// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CaloRec/src/CaloThinCellsBySamplingAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2019
 * @brief Thin calorimeter cells by sampling.
 */


#ifndef CALOREC_CALOTHINCELLSBYSAMPLINGALG_H
#define CALOREC_CALOTHINCELLSBYSAMPLINGALG_H


#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "CaloEvent/CaloCellContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "Identifier/IdentifierHash.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ThinningHandleKey.h"
#include <vector>


class CaloCell_ID;


/**
 * @brief Thin calorimeter cells by sampling.
 */
class CaloThinCellsBySamplingAlg : public AthReentrantAlgorithm
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

  StringArrayProperty m_samplingNames
  { this, "SamplingCellsName", {},
    "List of names of samplings for which cells should be saved" };

  /// Decoded list of cells to save
  std::vector<IdentifierHash> m_cellHashes;

  /// Calo ID helper.
  const CaloCell_ID* m_caloID;
};


#endif // not CALOREC_CALOTHINCELLSBYSAMPLINGALG_H

// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CaloRec/src/CaloCellContainerAliasAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2019
 * @brief Add an alias to a cell container.
 */


#ifndef CALOREC_CALOCELLCONTAINERALIASALG_H
#define CALOREC_CALOCELLCONTAINERALIASALG_H


#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "CaloEvent/CaloCellContainer.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"


/**
 * @brief Add an alias to a cell container.
 */
class CaloCellContainerAliasAlg : public AthReentrantAlgorithm
{
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
  SG::ReadHandleKey<CaloCellContainer> m_cells
  { this, "Cells", "AllCalo", "Cell container to alias" };

  SG::WriteHandleKey<CaloCellContainer> m_alias
  { this, "Alias", "", "Alias to make for the cell container" };
};


#endif // not CALOREC_CALOCELLCONTAINERALIASALG_H

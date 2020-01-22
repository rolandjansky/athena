/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CaloRec/src/CaloCellContainerAliasAlg.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2019
 * @brief Add an alias to a cell container.
 */


#include "CaloCellContainerAliasAlg.h"
#include "StoreGate/ReadHandle.h"


/**
 * @brief Gaudi initialize method.
 */
StatusCode CaloCellContainerAliasAlg::initialize()
{
  ATH_CHECK( m_cells.initialize() );
  ATH_CHECK( m_alias.initialize() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Execute the algorithm.
 * @param ctx Current event context.
 */
StatusCode CaloCellContainerAliasAlg::execute (const EventContext& ctx) const
{
  SG::ReadHandle<CaloCellContainer> cells (m_cells, ctx);
  ATH_CHECK( cells.alias (m_alias) );
  return StatusCode::SUCCESS;
}



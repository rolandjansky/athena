/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDAnalysis/src/TileGapSelectionAlg.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2011
 * @brief Select tile gap cells from a cell container into a new container.
 */


#include "TileGapSelectionAlg.h"
#include "D3PDMakerInterfaces/IObjGetterTool.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloSamplingHelper.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "AthContainers/OwnershipPolicy.h"
#include "AthContainers/ConstDataVector.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi algorithm constructor.
 * @param name The algorithm name.
 * @param svcloc The service locator.
 */
TileGapSelectionAlg::TileGapSelectionAlg (const std::string& name,
                                          ISvcLocator* svcloc)
  : AthAlgorithm (name, svcloc),
    m_getter (this)
{
  declareProperty ("Getter", m_getter,
                   "Getter instance for the input calorimeter cells.");
  declareProperty ("OutputCellContainerName", m_outputCellContainerName,
                   "StoreGate key for the output cell container.");
}


/**
 * @brief Standard Gaudi @c initialize method.
 */
StatusCode TileGapSelectionAlg::initialize()
{
  CHECK( AthAlgorithm::initialize() );
  CHECK( m_getter.retrieve() );
  CHECK( m_getter->configureD3PD<CaloCellContainer>() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Standard Gaudi @c execute method.
 */
StatusCode TileGapSelectionAlg::execute()
{
  // Get the input container.
  const CaloCellContainer* ccc = m_getter->get<CaloCellContainer>();
  if (!ccc) {
    REPORT_MESSAGE(MSG::WARNING) << "Can't find input CaloCellContainer.";
    return StatusCode::SUCCESS;
  }

  // Make the output container.
  ConstDataVector<CaloCellContainer>* gapcells =
    new ConstDataVector<CaloCellContainer> (SG::VIEW_ELEMENTS);
  CHECK( evtStore()->record (gapcells, m_outputCellContainerName) );

  CaloCellContainer::const_iterator f_cell =
    ccc->beginConstCalo(CaloCell_ID::TILE);
  CaloCellContainer::const_iterator l_cell =
    ccc->endConstCalo(CaloCell_ID::TILE);
  for ( ; f_cell!=l_cell; ++f_cell)
  {
    const CaloCell* cell = *f_cell; 
    if (CaloSampling::TileGap3 ==  CaloSamplingHelper::getSampling( *cell ))
      gapcells->push_back (cell);
  }

  return StatusCode::SUCCESS;
}


} // namespace D3PD

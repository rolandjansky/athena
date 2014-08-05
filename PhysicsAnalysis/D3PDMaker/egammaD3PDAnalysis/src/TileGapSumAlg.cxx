// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDAnalysis/src/TileGapSumAlg.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2011
 * @brief Sum tile gap cells in a rectangular region around clusters.
 */


#include "TileGapSumAlg.h"
#include "D3PDMakerInterfaces/ICollectionGetterTool.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "CaloEvent/CaloCell.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi algorithm constructor.
 * @param name The algorithm name.
 * @param svcloc The service locator.
 */
TileGapSumAlg::TileGapSumAlg (const std::string& name,
                              ISvcLocator* svcloc)
  : AthAlgorithm (name, svcloc),
    m_clusterGetter (this),
    m_cellGetter (this)
{
  declareProperty ("AuxPrefix", m_auxPrefix,
                   "Prefix to add to aux data items.");

  declareProperty ("AllowMissing", m_allowMissing = false,
                   "If true, don't complain if input objects are missing.");

  declareProperty ("ClusterGetter", m_clusterGetter,
                   "Getter instance for the input cluster objects.");

  declareProperty ("CellGetter", m_cellGetter,
                   "Getter instance for the input cell objects.  "
                   "Should have already been selected for only tile gap cells.");
}


/**
 * @brief Standard Gaudi @c initialize method.
 */
StatusCode TileGapSumAlg::initialize()
{
  CHECK( AthAlgorithm::initialize() );
  CHECK( m_clusterGetter.retrieve() );
  CHECK( m_clusterGetter->configureD3PD<xAOD::CaloCluster>() );
  CHECK( m_cellGetter.retrieve() );
  CHECK( m_cellGetter->configureD3PD<CaloCell>() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Standard Gaudi @c execute method.
 */
StatusCode TileGapSumAlg::execute()
{
  CHECK( m_clusterGetter->reset (m_allowMissing) );

  xAOD::CaloCluster::Decorator<float> tileGapSum (m_auxPrefix + "tileGapSum");

  while (const xAOD::CaloCluster* clust =
         m_clusterGetter->next<xAOD::CaloCluster>())
  {
    static CaloPhiRange range;

    // FIXME: Constants copied from gap correction code --- would be better
    // not to hard code these!
    double deta = 0.2;
    double dphi = 2*M_PI/64;

    double etacl = clust->eta();
    double phicl = range.fix (clust->phi());

    CHECK( m_cellGetter->reset (m_allowMissing) );
    tileGapSum(*clust) = 0;
    while (const CaloCell* cell = m_cellGetter->next<CaloCell>())
    {
      if (std::abs (etacl - cell->eta()) <= deta &&
          std::abs (range.fix (phicl - cell->phi())) <= dphi)
      {
        tileGapSum(*clust) += cell->energy();
      }
      m_cellGetter->releaseElement (cell);
    }

    m_clusterGetter->releaseElement (clust);
  }

  return StatusCode::SUCCESS;
}


} // namespace D3PD

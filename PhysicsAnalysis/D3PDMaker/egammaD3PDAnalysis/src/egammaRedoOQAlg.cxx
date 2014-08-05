/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDAnalysis/src/egammaRedoOQAlg.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2010
 * @brief Recalculate OQ flags for Electron objects.
 *        This is a template class, and can't be used directly as an Algorithm.
 *        Derive a non-templated class from this to actually use.
 */


#include "egammaRedoOQAlg.h"
#include "clusterHasCells.h"
#include "D3PDMakerInterfaces/ICollectionGetterTool.h"
#include "egammaInterfaces/IegammaBaseTool.h"
#include "xAODEgamma/Egamma.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi algorithm constructor.
 * @param name The algorithm name.
 * @param svcloc The service locator.
 */
egammaRedoOQAlgBase::egammaRedoOQAlgBase (const std::string& name,
                                          ISvcLocator* svcloc)
  : AthAlgorithm (name, svcloc),
    m_getter (this),
    m_egammaOQFlagsBuilder( "egammaOQFlagsBuilder" )
{
  declareProperty ("OutputKey", m_outputkey,
                   "SG key for output container.");

  declareProperty ("Getter", m_getter,
                   "Getter instance for the input egamma objects.");

  declareProperty ("egammaOQFlagsBuilder",
                   m_egammaOQFlagsBuilder,
                   "Tool for calculating the OQ flag");
}


/**
 * @brief Standard Gaudi @c initialize method.
 */
StatusCode egammaRedoOQAlgBase::initialize()
{
  CHECK( AthAlgorithm::initialize() );
  CHECK( m_getter.retrieve() );
  CHECK( m_getter->configureD3PD<xAOD::Egamma>() );
  CHECK( m_egammaOQFlagsBuilder.retrieve() );

  return StatusCode::SUCCESS;
}


/**
 * @brief Standard Gaudi execute method.
 */
StatusCode egammaRedoOQAlgBase::execute()
{
  CHECK (m_getter->reset() );
  while (const xAOD::Egamma* eg = m_getter->next<xAOD::Egamma>()) {
    xAOD::Egamma* neweg = this->cloneEG(eg);
    // This only works if the cluster has cells available.
    if (clusterHasCells (neweg->caloCluster()))
      CHECK( m_egammaOQFlagsBuilder->execute (neweg) );
  }
  CHECK (this->commit());

  return StatusCode::SUCCESS;
}


} // namespace D3PD

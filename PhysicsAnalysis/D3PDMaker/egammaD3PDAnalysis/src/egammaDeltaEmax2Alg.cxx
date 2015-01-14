/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDAnalysis/src/egammaDeltaEmax2.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2009
 * @brief Calculate deltaEmax2 for egamma objects and store as UD.
 */


#include "egammaDeltaEmax2Alg.h"
#include "D3PDMakerInterfaces/ICollectionGetterTool.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "AthenaKernel/errorcheck.h"
#include <cmath>


namespace D3PD {


egammaDeltaEmax2Alg::egammaDeltaEmax2Alg (const std::string& name,
                                          ISvcLocator* svcloc)
  : AthAlgorithm (name, svcloc),
    m_getter (this)
{
  declareProperty ("AuxPrefix", m_auxPrefix,
                   "Prefix to add to aux data items.");
  declareProperty ("Getter", m_getter,
                   "Getter instance for the input egamma objects.");
  declareProperty ("AllowMissing", m_allowMissing = false,
                   "If true, don't complain if input objects are missing.");
  declareProperty ("HighLum", m_highLum = false,
                   "True if we should use the high-lum definition "
                   "of deltaEmax2.");
}


/**
 * @brief Standard Gaudi @c initialize method.
 */
StatusCode egammaDeltaEmax2Alg::initialize()
{
  CHECK( AthAlgorithm::initialize() );
  CHECK( m_getter.retrieve() );
  CHECK( m_getter->configureD3PD<xAOD::Egamma>() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Standard Gaudi @c execute method.
 */
StatusCode egammaDeltaEmax2Alg::execute()
{
  xAOD::Egamma::Decorator<float> deltaemax2 (m_auxPrefix + "deltaEmax2");

  CHECK( m_getter->reset (m_allowMissing) );
  while (const xAOD::Egamma* g = m_getter->next<xAOD::Egamma>())
  {
    deltaemax2(*g) = -999;
    const xAOD::CaloCluster*   cluster  = g->caloCluster(); 
    if (cluster) {
      double eta2   = std::fabs(cluster->etaBE(2));
      // Protect against soft E topo clusters that don't have samp info.
      if (eta2 > 900)
        eta2   = std::fabs(cluster->eta());
      double et     = cluster->e()/std::cosh(eta2);
      float emax2 = 0;
      emax2 = g->showerShapeValue (xAOD::EgammaParameters::e2tsts1);

      if (m_highLum)
        deltaemax2(*g) = emax2/(1000.+0.0049*et);
      else
        deltaemax2(*g) = emax2/(1000.+0.009*et);
    }

    m_getter->releaseElement (g);
  }

  return StatusCode::SUCCESS;
}


} // namespace D3PD

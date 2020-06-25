/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file egammaD3PDAnalysis/src/egammaShowerDepthAlg.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2010
 * @brief Store in UserData the shower depth for an egamma object.
 */


#include "egammaShowerDepthAlg.h"
#include "D3PDMakerInterfaces/ICollectionGetterTool.h"
#include "xAODEgamma/Egamma.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/ThreadLocalContext.h"


namespace D3PD {


/**
 * @brief Standard Gaudi @c initialize method.
 */
StatusCode egammaShowerDepthAlg::initialize()
{
  CHECK( base_class::initialize() );
  CHECK( m_getter.retrieve() );
  CHECK( m_getter->configureD3PD<xAOD::Egamma>() );

  return StatusCode::SUCCESS;
}


/**
 * @brief Standard Gaudi @c execute method.
 */
StatusCode egammaShowerDepthAlg::execute()
{
  Context myctx = context (Gaudi::Hive::currentContext());

  xAOD::Egamma::Decorator<float> depth (m_auxPrefix + "calibHitsShowerDepth");

  // FIXME: Using the adjusted eta value isn't implemented.
  if (!m_use_raw_eta(myctx)) {
    REPORT_MESSAGE (MSG::WARNING) << "use_raw_eta==false is not implemented.";
  }

  CHECK( m_getter->reset (m_allowMissing) );
  while (const xAOD::Egamma* eg = m_getter->next<xAOD::Egamma>())
  {
    const xAOD::CaloCluster* cl = eg->caloCluster();
    depth(*eg) = -999;
    if (cl)
      depth(*eg) = m_depthCalc.depth (std::abs (cl->etaBE(2)),
                                      m_eta_start_crack(myctx),
                                      m_eta_end_crack(myctx),
                                      m_sampling_depth(myctx),
                                      m_etamax(myctx),
                                      cl, msg());
  }

  return StatusCode::SUCCESS;
}


} // namespace D3PD

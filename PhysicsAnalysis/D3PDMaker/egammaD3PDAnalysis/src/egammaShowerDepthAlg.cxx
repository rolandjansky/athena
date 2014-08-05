/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
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


namespace D3PD {


/**
 * @brief Standard Gaudi algorithm constructor.
 * @param name The algorithm name.
 * @param svcloc The service locator.
 */
egammaShowerDepthAlg::egammaShowerDepthAlg (const std::string& name,
                                            ISvcLocator* svcloc)
  : AthAlgorithm (name, svcloc),
    m_getter (this),
    m_depthCalc (m_sampling_depth, m_eta_start_crack, m_eta_end_crack, m_etamax)
{
  declareProperty ("AuxPrefix", m_auxPrefix,
                   "Prefix to add to aux data items.");
  declareProperty ("Getter", m_getter,
                   "Getter instance for the input egamma objects.");
  declareProperty ("AllowMissing", m_allowMissing = false,
                   "If true, don't complain if input objects are missing.");

  declareConstant ("sampling_depth"  , m_sampling_depth);
  declareConstant ("eta_start_crack" , m_eta_start_crack);
  declareConstant ("eta_end_crack"   , m_eta_end_crack);
  declareConstant ("etamax"          , m_etamax);
  declareConstant ("use_raw_eta"     , m_use_raw_eta);

  finish_ctor ("D3PD::egammaShowerDepthAlg");
}


/**
 * @brief Standard Gaudi @c initialize method.
 */
StatusCode egammaShowerDepthAlg::initialize()
{
  CHECK( AthAlgorithm::initialize() );
  CHECK( CaloRec::ToolWithConstantsMixin::initialize() );
  CHECK( m_getter.retrieve() );
  CHECK( m_getter->configureD3PD<xAOD::Egamma>() );

  return StatusCode::SUCCESS;
}


/**
 * @brief Method to set a property value.
 * @param p The property name/value to set.
 * @return Gaudi status code.
 *
 * Required by @c ToolWithConstantsMixin.
 */
StatusCode egammaShowerDepthAlg::setProperty (const Property& p)
{
  CHECK (AthAlgorithm::setProperty (p));
  CHECK (CaloRec::ToolWithConstantsMixin::setProperty (p));
  return StatusCode::SUCCESS;
}
StatusCode egammaShowerDepthAlg::setProperty (const std::string& propname,
                                              const std::string& value)
{
  CHECK (AthAlgorithm::setProperty (propname, value));
  CHECK (CaloRec::ToolWithConstantsMixin::setProperty (propname, value));
  return StatusCode::SUCCESS;
}


/**
 * @brief Standard Gaudi @c execute method.
 */
StatusCode egammaShowerDepthAlg::execute()
{
  xAOD::Egamma::Decorator<float> depth (m_auxPrefix + "calibHitsShowerDepth");

  // FIXME: Using the adjusted eta value isn't implemented.
  if (!m_use_raw_eta) {
    REPORT_MESSAGE (MSG::WARNING) << "use_raw_eta==false is not implemented.";
  }

  CHECK( m_getter->reset (m_allowMissing) );
  while (const xAOD::Egamma* eg = m_getter->next<xAOD::Egamma>())
  {
    const xAOD::CaloCluster* cl = eg->caloCluster();
    depth(*eg) = -999;
    if (cl)
      depth(*eg) = m_depthCalc.depth (std::abs (cl->etaBE(2)), cl, msg());
  }

  return StatusCode::SUCCESS;
}


} // namespace D3PD

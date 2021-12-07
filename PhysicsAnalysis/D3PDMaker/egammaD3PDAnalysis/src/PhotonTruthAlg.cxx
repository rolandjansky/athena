/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDAnalysis/src/PhotonTruthAlg.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2009
 * @brief Analyze @c GenParticle's matching photons and make UD decorations.
 */


#include "PhotonTruthAlg.h"
#include "PhotonTruthTool.h"
#include "D3PDMakerInterfaces/ICollectionGetterTool.h"
#include "xAODEgamma/Photon.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


PhotonTruthAlg::PhotonTruthAlg (const std::string& name,
                                ISvcLocator* svcloc)
  : AthAlgorithm (name, svcloc),
    m_truthTool ("D3PD::PhotonTruthTool", this),
    m_photonGetter (this)
{
  declareProperty ("AuxPrefix", m_auxPrefix,
                   "Prefix to add to aux data items.");
  declareProperty ("TruthTool", m_truthTool,
                   "Photon truth analysis tool instance.");
  declareProperty ("PhotonGetter", m_photonGetter,
                   "Getter instance for the input photon objects.");
  declareProperty ("AllowMissing", m_allowMissing = false,
                   "If true, don't complain if input objects are missing.");
}


/**
 * @brief Standard Gaudi @c initialize method.
 */
StatusCode PhotonTruthAlg::initialize()
{
  CHECK( AthAlgorithm::initialize() );
  CHECK( m_truthTool.retrieve() );
  CHECK( m_photonGetter.retrieve() );
  CHECK( m_photonGetter->configureD3PD<xAOD::Photon>() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Standard Gaudi @c execute method.
 */
StatusCode PhotonTruthAlg::execute()
{
#define DECOR(TYPE,N) xAOD::Photon::Decorator<TYPE> N (m_auxPrefix + #N)
  DECOR(bool,  truth_isConv);
  DECOR(float, truth_Rconv);
  DECOR(float, truth_zconv);
  DECOR(bool,  truth_isPhotonFromHardProc);
  DECOR(bool,  truth_isFromHardProc);
  DECOR(bool,  truth_isBrem);
#undef DECOR

  CHECK( m_photonGetter->reset (m_allowMissing) );
  while (const xAOD::Photon* g =
         m_photonGetter->next<xAOD::Photon>())
  {
    const xAOD::TruthParticle* p = m_truthTool->toTruthParticle (*g);

    truth_isConv(*g) = m_truthTool->getMCConv (p,
                                               truth_Rconv(*g),
                                               truth_zconv(*g));

    truth_isPhotonFromHardProc(*g) = m_truthTool->isPromptPhotonMC(p);
    truth_isFromHardProc(*g) = m_truthTool->isPromptParticleMC(p);

    truth_isBrem(*g)  = !truth_isPhotonFromHardProc(*g) && m_truthTool->isQuarkBremMC(p) ;

    m_photonGetter->releaseElement (g);
  }

  return StatusCode::SUCCESS;
}


} // namespace D3PD

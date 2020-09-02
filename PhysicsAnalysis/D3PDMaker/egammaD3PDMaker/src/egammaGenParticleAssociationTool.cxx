/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDMaker/src/egammaGenParticleAssociationTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Associate from an electron/photon to a matched GenParticle via
 *        classification tool.
 */


#include "egammaGenParticleAssociationTool.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODTruth/TruthParticle.h"
#include "FourMomUtils/P4Helpers.h"
#include "AthenaKernel/errorcheck.h"
#include "AtlasHepMC/GenEvent.h"
#include <cstdlib>


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
egammaGenParticleAssociationTool::egammaGenParticleAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent),
      m_classifier ("MCTruthClassifier")
{
  declareProperty ("Classifier", m_classifier, "Classifier tool instance.");
  declareProperty ("DRVar", m_drvar,
                   "If not empty, the variable name to use for DR.");

  m_dr = 0;
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode egammaGenParticleAssociationTool::initialize()
{
  CHECK( Base::initialize() );
  CHECK( m_classifier.retrieve() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Create any needed tuple variables.
 */
StatusCode egammaGenParticleAssociationTool::book ()
{
  if (!m_drvar.empty())
    CHECK( addVariable (m_drvar, m_dr) );
  return StatusCode::SUCCESS;
}


/**
 * @brief Return the target object.
 * @param p The source object for the association.
 *
 * Return the target of the association, or 0.
 */
const xAOD::TruthParticle*
egammaGenParticleAssociationTool::get (const xAOD::Egamma& p)
{
  IMCTruthClassifier::Cache cache;
  IMCTruthClassifier::Info info;
  info.extrapolationCache = &cache;
  if (const xAOD::Electron* q =
      dynamic_cast<const xAOD::Electron*> (&p))
  {
    m_classifier->particleTruthClassifier (q, &info);
  }
  else if (const xAOD::Photon* q =
           dynamic_cast<const xAOD::Photon*> (&p))
  {
    m_classifier->particleTruthClassifier (q, &info);
  }
  else
    std::abort();

  const xAOD::TruthParticle* out = info.genPart;

  if (!m_drvar.empty()) {
    if (out)
      *m_dr = p.p4().DeltaR (out->p4());
    else
      *m_dr = -999;
  }

  return out;
}


} // namespace D3PD

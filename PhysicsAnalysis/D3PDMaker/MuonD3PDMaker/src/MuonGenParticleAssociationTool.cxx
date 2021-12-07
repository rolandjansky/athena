/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonD3PDMaker/src/MuonGenParticleAssociationTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2010
 * @brief Associate from a muon to a matched GenParticle via
 *        classification tool.
 */


#include "MuonGenParticleAssociationTool.h"
#include "xAODMuon/Muon.h"
#include "xAODTruth/TruthParticle.h"
#include "FourMomUtils/P4Helpers.h"
#include "AthenaKernel/errorcheck.h"
#include <cstdlib>


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
MuonGenParticleAssociationTool::MuonGenParticleAssociationTool
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
StatusCode MuonGenParticleAssociationTool::initialize()
{
  CHECK( Base::initialize() );
  CHECK( m_classifier.retrieve() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Create any needed tuple variables.
 */
StatusCode MuonGenParticleAssociationTool::book ()
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
MuonGenParticleAssociationTool::get (const xAOD::Muon& p)
{
  IMCTruthClassifier::Info info;
  const xAOD::TruthParticle* out = 0;
  if (m_classifier->particleTruthClassifier (&p, &info).first !=
      MCTruthPartClassifier::Unknown)
  {
    out = info.genPart;
  }

  if (!m_drvar.empty()) {
    if (out)
      *m_dr = p.p4().DeltaR(out->p4());
    else
      *m_dr = -999;
  }

  return out;
}


} // namespace D3PD

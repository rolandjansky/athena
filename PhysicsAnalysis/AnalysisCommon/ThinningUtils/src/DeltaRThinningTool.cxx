/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
// package includes
#include "DeltaRThinningTool.h"

DeltaRThinningTool::DeltaRThinningTool(
    const std::string& type,
    const std::string& name,
    const IInterface* parent) :
  ::AthAlgTool(type, name, parent),
  m_thinningSvc("ThinningSvc", name)
{
  declareInterface<IForwardParentThinningTool>(this);
  declareProperty("ThinningService", m_thinningSvc, "The thinning service.");
  declareProperty("SGKey", m_sgKey, "The name of the child container to thin.");
  declareProperty("ConeSize", m_coneSize, "The size of the cone to use.");
  declareProperty("ApplyAnd", m_and = false,
      "Whether or not to use the AND operator when thinning.");
}

StatusCode DeltaRThinningTool::initialize()
{
  ATH_MSG_INFO("initialize() " << name() );
  ATH_CHECK( m_thinningSvc.retrieve() );
  // Verify input options.
  if (m_sgKey.empty() ) {
    ATH_MSG_ERROR("SGKey not set!");
    return StatusCode::FAILURE;
  }
  if (m_coneSize <= 0) {
    ATH_MSG_ERROR("Invalid ConeSize: " << m_coneSize);
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode DeltaRThinningTool::forwardParentThinning(
    const xAOD::IParticleContainer* parent,
    const std::vector<bool>& mask) const
{
  const xAOD::IParticleContainer* child(nullptr);
  ATH_CHECK( evtStore()->retrieve(child, m_sgKey) );
  // Start with all child particles marked for removal.
  std::vector<bool> childMask(child->size(), false);
  for (std::size_t ii = 0; ii < mask.size(); ++ii) {
    if (!mask.at(ii) )
      continue;
    TLorentzVector parentP4 = parent->at(ii)->p4();
    for (std::size_t jj = 0; jj < childMask.size(); ++jj) {
      if (childMask.at(jj) )
        continue;
      if (parentP4.DeltaR(child->at(jj)->p4() ) < m_coneSize)
        childMask.at(jj) = true;
    }
  }
  // Thin the child container.
  ATH_CHECK( m_thinningSvc->filter(*child, childMask,
        m_and ? IThinningSvc::Operator::And : IThinningSvc::Operator::Or) );
  return StatusCode::SUCCESS;
}


/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak



//
// includes
//

#include <PMGTools/PMGDecayProductsSelectionTool.h>

#include <xAODTruth/xAODTruthHelpers.h>
#include <xAODBase/IParticle.h>

#include <sstream>

//
// method implementations
//

namespace PMGTools
{
  PMGDecayProductsSelectionTool :: 
  PMGDecayProductsSelectionTool (const std::string& name)
    : AsgTool (name)
  {
    declareProperty ("requiredParentPDGIDs", m_requiredParentPDGIDs, "required parent particle PDG IDs (positive only)");
    declareProperty ("allowedIntermediatePDGIDs", m_allowedIntermediatePDGIDs, "allowed intermediate particles PDG IDs (positive only)");
  }



  StatusCode PMGDecayProductsSelectionTool ::
  initialize ()
  {
    if (m_requiredParentPDGIDs.empty())
    {
      ATH_MSG_ERROR ("required parent particles need to be set");
      return StatusCode::FAILURE;
    }

    m_truthParticleIndex = m_accept.addCut ("truthParticle", "has truth particle cut");

    std::ostringstream particles;
    for (int pdgId : m_requiredParentPDGIDs)
    {
      particles << " " << pdgId;
    }
    ATH_MSG_DEBUG ("Performing required parent particle selection with parent PDG IDs:" << particles.str());
    m_requiredParentIndex = m_accept.addCut ("requiredParents", "required parent particles cut");

    if (!m_allowedIntermediatePDGIDs.empty())
    {
      std::ostringstream particles;
      for (int pdgId : m_allowedIntermediatePDGIDs)
      {
        particles << " " << pdgId;
      }
      ATH_MSG_DEBUG ("Performing allowed intermediate particle selection with allowed PDG IDs:" << particles.str());
    }

    m_parentsAccessor = std::make_unique<const SG::AuxElement::Accessor<std::vector<ElementLink<xAOD::TruthParticleContainer>>>>("parentLinks");

    return StatusCode::SUCCESS;
  }



  const asg::AcceptInfo& PMGDecayProductsSelectionTool ::
  getAcceptInfo () const
  {
    return m_accept;
  }



  asg::AcceptData PMGDecayProductsSelectionTool ::
  accept (const xAOD::IParticle *particle) const
  {
    asg::AcceptData acceptData (&m_accept);

    // Check if xAOD::TruthParticle or if not if it has the TruthParticleLink
    const xAOD::TruthParticle *truthParticle
      = dynamic_cast<const xAOD::TruthParticle *> (particle);
    if (truthParticle == nullptr)
    {
      // need to find the truth particle
      truthParticle = xAOD::TruthHelpers::getTruthParticle(*particle);
      if (truthParticle == nullptr)
      {
        acceptData.setCutResult (m_truthParticleIndex, false);
        return acceptData;
      }
    }
    acceptData.setCutResult (m_truthParticleIndex, true);

    return hasRequiredInitialParent(truthParticle, acceptData);
  }



  asg::AcceptData PMGDecayProductsSelectionTool ::
  hasRequiredInitialParent (const xAOD::TruthParticle *truthParticle, asg::AcceptData& acceptData) const
  {
    size_t nParents = getNParents (truthParticle);
    for (size_t i = 0; i < nParents; i++)
    {
      const xAOD::TruthParticle *parent = getParent(truthParticle, i);
      if (parent)
      {
        if (std::find(m_requiredParentPDGIDs.begin(), m_requiredParentPDGIDs.end(), std::abs(parent->pdgId())) != m_requiredParentPDGIDs.end())
        {
          acceptData.setCutResult (m_requiredParentIndex, true);
          return acceptData;
        }
        else if (m_allowedIntermediatePDGIDs.empty() || std::find(m_allowedIntermediatePDGIDs.begin(), m_allowedIntermediatePDGIDs.end(), std::abs(parent->pdgId())) != m_allowedIntermediatePDGIDs.end())
        {
          return hasRequiredInitialParent(parent, acceptData);
        }
        else
        {
          ATH_MSG_VERBOSE("Removing particle as parent is not allowed: " << parent->pdgId());
          return acceptData;
        }
      }
      else
      {
        ATH_MSG_WARNING("Particle parent is not valid");
        return acceptData;
      }
    }

    if (std::find(m_requiredParentPDGIDs.begin(), m_requiredParentPDGIDs.end(), std::abs(truthParticle->pdgId())) != m_requiredParentPDGIDs.end())
    {
      acceptData.setCutResult (m_requiredParentIndex, true);
      return acceptData;
    }

    return acceptData;
  }



  size_t PMGDecayProductsSelectionTool ::
  getNParents (const xAOD::TruthParticle *truthParticle) const
  {
    if (m_parentsAccessor->isAvailable (*truthParticle))
    {
      return (*m_parentsAccessor)(*truthParticle).size();
    }
    else
    {
      return truthParticle->nParents();
    }
  }



  const xAOD::TruthParticle* PMGDecayProductsSelectionTool ::
  getParent (const xAOD::TruthParticle *truthParticle,
              size_t index) const
  {
    if (m_parentsAccessor->isAvailable (*truthParticle))
    {
      ElementLink<xAOD::TruthParticleContainer> parentElementLink
        = (*m_parentsAccessor)(*truthParticle).at(index);
      return parentElementLink.isValid() ? *parentElementLink : nullptr;
    }
    else
    {
      return truthParticle->parent(index);
    }
  }
}

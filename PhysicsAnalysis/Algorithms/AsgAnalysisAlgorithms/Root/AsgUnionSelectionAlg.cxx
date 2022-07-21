/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak

//
// includes
//

#include <AsgAnalysisAlgorithms/AsgUnionSelectionAlg.h>

#include <xAODBase/IParticle.h>

//
// method implementations
//

namespace CP
{
  AsgUnionSelectionAlg ::
  AsgUnionSelectionAlg (const std::string& name, 
                        ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
  {
    declareProperty ("selectionDecoration", m_selectionDecoration, "the decoration for the union selection");
  }



  StatusCode AsgUnionSelectionAlg ::
  initialize ()
  {
    if (m_selectionDecoration.empty())
    {
      ANA_MSG_ERROR("Selection decoration can not be empty.");
      return StatusCode::FAILURE;
    }

    ANA_CHECK (makeSelectionWriteAccessor (m_selectionDecoration, m_selectionAccessor));

    ANA_CHECK (m_particlesHandle.initialize(m_systematicsList));
    ANA_CHECK (m_preselection.initialize (m_systematicsList, m_particlesHandle));
    ANA_CHECK (m_systematicsList.initialize());

    return StatusCode::SUCCESS;
  }



  StatusCode AsgUnionSelectionAlg ::
  execute ()
  {
    std::vector<bool> selections;

    // first loop through systematics and define if particle passes each of them
    for (const auto& sys : m_systematicsList.systematicsVector())
    {
      const xAOD::IParticleContainer *particles{};
      ANA_CHECK (m_particlesHandle.retrieve (particles, sys));

      if (selections.empty())
      {
        selections.resize(particles->size(), false);
      }
      else if (selections.size() != particles->size())
      {
        ANA_MSG_ERROR("All input containers should have the same size.");
        return StatusCode::FAILURE;
      }

      for (size_t i{}; i < particles->size(); i++)
      {
        selections[i] = selections[i] || m_preselection.getBool (*particles->at(i), sys);
      }
    }

    // Now decorate the selection decoration
    // Looping over systematics is needed to ensure all containers are processed
    for (const auto& sys : m_systematicsList.systematicsVector())
    {
      const xAOD::IParticleContainer *particles{};
      ANA_CHECK (m_particlesHandle.retrieve (particles, sys));

      for (size_t i{}; i < particles->size(); i++)
      {
        m_selectionAccessor->setBool (*particles->at(i), selections[i]);
      }
    }

    return StatusCode::SUCCESS;
  }

} // namespace CP

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak <tadej@cern.ch>


#include <AsgAnalysisAlgorithms/AsgPriorityDecorationAlg.h>


namespace CP
{

AsgPriorityDecorationAlg::AsgPriorityDecorationAlg(const std::string &name,
                                                   ISvcLocator *pSvcLocator)
    : AnaAlgorithm(name, pSvcLocator)
{
  declareProperty ("priorityDecoration", m_priorityDecoration, "the decoration for the priority");
  declareProperty ("priorities", m_priorities, "priorities to use with the highest one first");
}



StatusCode AsgPriorityDecorationAlg::initialize()
{
  if (m_priorityDecoration.empty())
  {
    ANA_MSG_ERROR ("Priority decoration name should not be empty.");
    return StatusCode::FAILURE;
  }

  if (m_priorities.size() != m_preselections.size())
  {
    ANA_MSG_ERROR ("Preselections and priority values need to be of the same size.");
    return StatusCode::FAILURE;
  }

  if (!std::is_sorted(std::rbegin(m_priorities), std::rend(m_priorities)))
  {
    ANA_MSG_ERROR ("Priorities need to be provided in reverse order.");
    return StatusCode::FAILURE;
  }

  m_priorityDecorator = std::make_unique<SG::AuxElement::Decorator<char> > (m_priorityDecoration);

  ANA_CHECK (m_particlesHandle.initialize (m_systematicsList));
  ANA_CHECK (m_preselections.initialize (m_systematicsList, m_particlesHandle));
  ANA_CHECK (m_systematicsList.initialize());

  return StatusCode::SUCCESS;
}



StatusCode AsgPriorityDecorationAlg::execute()
{
  for (const auto& sys : m_systematicsList.systematicsVector())
  {
    const xAOD::IParticleContainer *particles{};
    ANA_CHECK(m_particlesHandle.retrieve(particles, sys));

    for (const xAOD::IParticle *particle : *particles)
    {
      bool passed{};
      for (size_t i{}; i < m_preselections.size(); i++)
      {
        if (m_preselections.at(i).getBool (*particle, sys))
        {
          (*m_priorityDecorator)(*particle) = m_priorities[i];
          passed = true;
          break;
        }
      }

      // decorate default priority
      if (!passed)
      {
        (*m_priorityDecorator)(*particle) = 0;
      }
    }
  }
  return StatusCode::SUCCESS;
}

} // namespace CP

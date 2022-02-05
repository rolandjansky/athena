/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak <tadej@cern.ch>


#include <AsgAnalysisAlgorithms/AsgClassificationDecorationAlg.h>


namespace CP
{

AsgClassificationDecorationAlg::AsgClassificationDecorationAlg(const std::string &name,
                                               ISvcLocator *pSvcLocator)
    : AnaAlgorithm(name, pSvcLocator)
{
  declareProperty ("decoration", m_classificationDecoration, "the decoration for classification");
  declareProperty ("tool", m_tool, "classification tool");
}



StatusCode AsgClassificationDecorationAlg::initialize()
{
  if (m_classificationDecoration.empty())
  {
    ANA_MSG_ERROR ("Classification decoration name should not be empty.");
    return StatusCode::FAILURE;
  }

  m_classificationDecorator = std::make_unique<SG::AuxElement::Decorator<unsigned int> > (m_classificationDecoration);

  ANA_CHECK (m_particlesHandle.initialize (m_systematicsList));
  ANA_CHECK(m_systematicsList.initialize());

  ANA_CHECK(m_tool->initialize());

  return StatusCode::SUCCESS;
}



StatusCode AsgClassificationDecorationAlg::execute()
{
  for (const auto& sys : m_systematicsList.systematicsVector())
  {
    const xAOD::IParticleContainer *particles{};
    ANA_CHECK(m_particlesHandle.retrieve(particles, sys));

    for (const xAOD::IParticle *particle : *particles)
    {
      unsigned int classification{};
      ANA_CHECK(m_tool->classify(*particle, classification));
      (*m_classificationDecorator)(*particle) = classification;
    }
  }
  return StatusCode::SUCCESS;
}

} // namespace CP

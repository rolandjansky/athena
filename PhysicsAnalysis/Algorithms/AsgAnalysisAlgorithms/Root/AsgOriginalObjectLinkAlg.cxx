/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak


//
// includes
//

#include <AsgAnalysisAlgorithms/AsgOriginalObjectLinkAlg.h>

//
// method implementations
//

namespace CP
{
  AsgOriginalObjectLinkAlg ::
  AsgOriginalObjectLinkAlg (const std::string& name, 
                            ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
  {
    declareProperty ("baseContainerName", m_baseContainerName, "base particle container name");
  }



  StatusCode AsgOriginalObjectLinkAlg ::
  initialize ()
  {
    if (m_baseContainerName.empty())
    {
      ANA_MSG_ERROR ("Base particle container name should not be empty.");
      return StatusCode::FAILURE;
    }

    m_systematicsList.addHandle (m_particleHandle);
    ANA_CHECK (m_systematicsList.initialize());
    return StatusCode::SUCCESS;
  }



  StatusCode AsgOriginalObjectLinkAlg ::
  execute ()
  {
    return m_systematicsList.foreach ([&] (const CP::SystematicSet& sys) -> StatusCode {
      xAOD::IParticleContainer *particles = nullptr;
      ANA_CHECK (m_particleHandle.getCopy (particles, sys));

      const xAOD::IParticleContainer *baseParticles = nullptr;
      ANA_CHECK (evtStore()->retrieve(baseParticles, m_baseContainerName));

      if (!xAOD::setOriginalObjectLink (*baseParticles, *particles))
      {
        ATH_MSG_ERROR ("Cannot set original object links for container named " << m_baseContainerName);
        return StatusCode::FAILURE;
      }

      return StatusCode::SUCCESS;
    });
  }
}

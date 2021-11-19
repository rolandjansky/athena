/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak


//
// includes
//

#include <AsgAnalysisAlgorithms/AsgEventScaleFactorAlg.h>

//
// method implementations
//

namespace CP
{
  AsgEventScaleFactorAlg ::
  AsgEventScaleFactorAlg (const std::string& name, 
                          ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
  {}



  StatusCode AsgEventScaleFactorAlg ::
  initialize ()
  {
    if (m_scaleFactorInputDecoration.empty() || m_scaleFactorOutputDecoration.empty())
    {
      ANA_MSG_ERROR ("no scale factor decoration name set");
      return StatusCode::FAILURE;
    }

    ANA_CHECK (m_eventInfoHandle.initialize (m_systematicsList));
    ANA_CHECK (m_particleHandle.initialize (m_systematicsList));
    ANA_CHECK (m_inputSelectionDecoration.initialize (m_systematicsList, m_particleHandle, SG::AllowEmpty));
    ANA_CHECK (m_scaleFactorInputDecoration.initialize (m_systematicsList, m_particleHandle));
    ANA_CHECK (m_scaleFactorOutputDecoration.initialize (m_systematicsList, m_eventInfoHandle));
    ANA_CHECK (m_systematicsList.initialize());
    ANA_CHECK (m_preselection.initialize());

    return StatusCode::SUCCESS;
  }



  StatusCode AsgEventScaleFactorAlg ::
  execute ()
  {
    for (const auto& sys : m_systematicsList.systematicsVector())
    {
      const xAOD::EventInfo *eventInfo = nullptr;
      ANA_CHECK (m_eventInfoHandle.retrieve (eventInfo, sys));

      const xAOD::IParticleContainer *particles = nullptr;
      ANA_CHECK (m_particleHandle.retrieve (particles, sys));

      float scaleFactor = 1;
      for (const xAOD::IParticle *particle : *particles)
      {
        if (m_preselection.getBool (*particle))
        {
          if (m_inputSelectionDecoration && m_inputSelectionDecoration.get (*particle, sys) == 0)
            continue;

          scaleFactor *= m_scaleFactorInputDecoration.get (*particle, sys);
        }
      }

      m_scaleFactorOutputDecoration.set (*eventInfo, scaleFactor, sys);
    }

    return StatusCode::SUCCESS;
  }
}

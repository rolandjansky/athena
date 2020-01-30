/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak


//
// includes
//

#include <AsgAnalysisAlgorithms/AsgEventScaleFactorAlg.h>

// #include <SelectionHelpers/SelectionHelpers.h>

//
// method implementations
//

namespace CP
{
  AsgEventScaleFactorAlg ::
  AsgEventScaleFactorAlg (const std::string& name, 
                          ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
  {
  }



  StatusCode AsgEventScaleFactorAlg ::
  initialize ()
  {
    if (m_scaleFactorInputDecoration.empty() || m_scaleFactorOutputDecoration.empty())
    {
      ANA_MSG_ERROR ("no scale factor decoration name set");
      return StatusCode::FAILURE;
    }

    m_systematicsList.addHandle (m_particleHandle);
    ANA_CHECK (m_systematicsList.initialize());
    ANA_CHECK (m_preselection.initialize());

    return StatusCode::SUCCESS;
  }



  StatusCode AsgEventScaleFactorAlg ::
  execute ()
  {
    ANA_CHECK (m_scaleFactorOutputDecoration.preExecute (m_systematicsList));

    return m_systematicsList.foreach ([&] (const CP::SystematicSet& sys) -> StatusCode {
      xAOD::EventInfo *eventInfo = nullptr;
      ANA_CHECK (m_eventInfoHandle.getCopy (eventInfo, sys));

      xAOD::IParticleContainer *particles = nullptr;
      ANA_CHECK (m_particleHandle.getCopy (particles, sys));

      float scaleFactor = 1;
      for (xAOD::IParticle *particle : *particles)
      {
        if (m_preselection.getBool (*particle))
        {
          scaleFactor *= m_scaleFactorInputDecoration.get (*particle, sys);
        }
      }

      m_scaleFactorOutputDecoration.set (*eventInfo, scaleFactor, sys);

      return StatusCode::SUCCESS;
    });
  }
}

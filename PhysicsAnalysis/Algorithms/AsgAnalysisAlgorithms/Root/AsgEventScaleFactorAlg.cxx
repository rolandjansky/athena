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
    declareProperty ("efficiency", m_efficiency, "the decoration for the efficiency");
    declareProperty ("outOfValidity", m_outOfValidity, "the decoration for the out of validity status");
  }



  StatusCode AsgEventScaleFactorAlg ::
  initialize ()
  {
    if (m_efficiency.empty())
    {
      ANA_MSG_ERROR ("no efficiency decoration name set");
      return StatusCode::FAILURE;
    }

    m_systematicsList.addHandle (m_eventInfoHandle);
    m_systematicsList.addHandle (m_particleHandle);
    ANA_CHECK (m_systematicsList.initialize());

    if (!m_outOfValidity.empty())
      ANA_CHECK (makeSelectionAccessor (m_outOfValidity, m_outOfValidityAccessor));

    m_efficiencyAccessor = std::make_unique<SG::AuxElement::Accessor<float> > (m_efficiency);

    return StatusCode::SUCCESS;
  }



  StatusCode AsgEventScaleFactorAlg ::
  execute ()
  {
    return m_systematicsList.foreach ([&] (const CP::SystematicSet& sys) -> StatusCode {
      xAOD::EventInfo *eventInfo = nullptr;
      ANA_CHECK (m_eventInfoHandle.getCopy (eventInfo, sys));

      xAOD::IParticleContainer *particles = nullptr;
      ANA_CHECK (m_particleHandle.getCopy (particles, sys));

      float efficiency = 1;
      for (xAOD::IParticle *particle : *particles)
      {
        bool valid = m_outOfValidityAccessor ? m_outOfValidityAccessor->getBool(*particle) : true;
        if (valid)
        {
          efficiency *= (*m_efficiencyAccessor) (*particle);
        }
      }

      (*m_efficiencyAccessor) (*eventInfo) = efficiency;

      return StatusCode::SUCCESS;
    });
  }
}

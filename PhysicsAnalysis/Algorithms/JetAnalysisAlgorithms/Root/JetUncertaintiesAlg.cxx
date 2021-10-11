/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <JetAnalysisAlgorithms/JetUncertaintiesAlg.h>

//
// method implementations
//

namespace CP
{
  JetUncertaintiesAlg ::
  JetUncertaintiesAlg (const std::string& name, 
                     ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
    , m_uncertaintiesTool ("JetUncertaintiesTool", this)
  {
    declareProperty ("uncertaintiesTool", m_uncertaintiesTool, "the uncertainties tool we apply");
  }



  StatusCode JetUncertaintiesAlg ::
  initialize ()
  {
    ANA_CHECK (m_uncertaintiesTool.retrieve());
    ANA_CHECK (m_jetHandle.initialize (m_systematicsList));
    ANA_CHECK (m_systematicsList.addSystematics (*m_uncertaintiesTool));
    ANA_CHECK (m_systematicsList.initialize());
    ANA_CHECK (m_preselection.initialize());
    ANA_CHECK (m_outOfValidity.initialize());
    return StatusCode::SUCCESS;
  }



  StatusCode JetUncertaintiesAlg ::
  execute ()
  {
    for (const auto& sys : m_systematicsList.systematicsVector())
    {
      ANA_CHECK (m_uncertaintiesTool->applySystematicVariation (sys));
      xAOD::JetContainer *jets = nullptr;
      ANA_CHECK (m_jetHandle.getCopy (jets, sys));
      for (xAOD::Jet *jet : *jets)
      {
        if (m_preselection.getBool (*jet))
        {
          ANA_CHECK_CORRECTION (m_outOfValidity, *jet, m_uncertaintiesTool->applyCorrection (*jet));
        }
      }
    }

    return StatusCode::SUCCESS;
  }
}

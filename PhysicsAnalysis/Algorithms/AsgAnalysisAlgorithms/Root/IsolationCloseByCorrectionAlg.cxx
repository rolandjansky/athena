/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <AsgAnalysisAlgorithms/IsolationCloseByCorrectionAlg.h>

//
// method implementations
//

namespace CP
{
  IsolationCloseByCorrectionAlg ::
  IsolationCloseByCorrectionAlg (const std::string& name, 
                        ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
  {
    declareProperty ("isolationCorrectionTool", m_isolationCorrectionTool);
    declareProperty ("topoEtConeModel", m_topoEtConeModel);
  }



  StatusCode IsolationCloseByCorrectionAlg ::
  initialize ()
  {
    ANA_CHECK (m_isolationCorrectionTool.retrieve());
    if (m_electronsHandle) m_systematicsList.addHandle (m_electronsHandle);
    if (m_muonsHandle) m_systematicsList.addHandle (m_muonsHandle);
    if (m_photonsHandle) m_systematicsList.addHandle (m_photonsHandle);
    ANA_CHECK (m_systematicsList.initialize());
    ANA_CHECK (m_outOfValidity.initialize());
    return StatusCode::SUCCESS;
  }



  StatusCode IsolationCloseByCorrectionAlg ::
  execute ()
  {
    return m_systematicsList.foreach ([&] (const CP::SystematicSet& sys) -> StatusCode {
        xAOD::ElectronContainer *electrons {nullptr};
        if (m_electronsHandle)
          ANA_CHECK (m_electronsHandle.getCopy (electrons, sys));
        xAOD::MuonContainer *muons {nullptr};
        if (m_muonsHandle)
          ANA_CHECK (m_muonsHandle.getCopy (muons, sys));
        xAOD::PhotonContainer *photons {nullptr};
        if (m_photonsHandle)
          ANA_CHECK (m_photonsHandle.getCopy (photons, sys));

        ANA_CHECK_CORRECTION_EVENT
          (m_outOfValidity, m_isolationCorrectionTool->getCloseByIsoCorrection
           (electrons, muons, photons, m_topoEtConeModel));

        return StatusCode::SUCCESS;
      });
  }
}

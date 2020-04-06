/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <AsgAnalysisAlgorithms/OverlapRemovalAlg.h>

//
// method implementations
//

namespace CP
{
  OverlapRemovalAlg ::
  OverlapRemovalAlg (const std::string& name, 
                        ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
  {
    declareProperty ("overlapTool", m_overlapTool);
  }



  StatusCode OverlapRemovalAlg ::
  initialize ()
  {
    ANA_CHECK (m_overlapTool.retrieve());
    if (m_electronsHandle) m_systematicsList.addHandle (m_electronsHandle);
    if (m_muonsHandle) m_systematicsList.addHandle (m_muonsHandle);
    if (m_jetsHandle) m_systematicsList.addHandle (m_jetsHandle);
    if (m_tausHandle) m_systematicsList.addHandle (m_tausHandle);
    if (m_photonsHandle) m_systematicsList.addHandle (m_photonsHandle);
    if (m_fatJetsHandle) m_systematicsList.addHandle (m_fatJetsHandle);
    ANA_CHECK (m_systematicsList.initialize());
    return StatusCode::SUCCESS;
  }



  StatusCode OverlapRemovalAlg ::
  execute ()
  {
    return m_systematicsList.foreach ([&] (const CP::SystematicSet& sys) -> StatusCode {
        xAOD::ElectronContainer *electrons {nullptr};
        if (m_electronsHandle)
          ANA_CHECK (m_electronsHandle.getCopy (electrons, sys));
        xAOD::MuonContainer *muons {nullptr};
        if (m_muonsHandle)
          ANA_CHECK (m_muonsHandle.getCopy (muons, sys));
        xAOD::JetContainer *jets {nullptr};
        if (m_jetsHandle)
          ANA_CHECK (m_jetsHandle.getCopy (jets, sys));
        xAOD::TauJetContainer *taus {nullptr};
        if (m_tausHandle)
          ANA_CHECK (m_tausHandle.getCopy (taus, sys));
        xAOD::PhotonContainer *photons {nullptr};
        if (m_photonsHandle)
          ANA_CHECK (m_photonsHandle.getCopy (photons, sys));
        xAOD::JetContainer *fatJets {nullptr};
        if (m_fatJetsHandle)
          ANA_CHECK (m_fatJetsHandle.getCopy (fatJets, sys));

        ATH_CHECK (m_overlapTool->removeOverlaps (electrons, muons, jets, taus,
                                                  photons, fatJets));

        return StatusCode::SUCCESS;
      });
  }
}

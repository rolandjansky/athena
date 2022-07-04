/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
    declareProperty ("OutputLabel", m_overlapRemovalDecoration, "the decoration for the overlap removal tool output");
  }



  StatusCode OverlapRemovalAlg ::
  initialize ()
  {
    if (m_overlapRemovalDecoration.empty())
    {
      ANA_MSG_ERROR ("no overlap removal decoration name set");
      return StatusCode::FAILURE;
    }

    m_overlapRemovalAccessor = std::make_unique<SG::AuxElement::Accessor<char> > (m_overlapRemovalDecoration);

    ANA_CHECK (m_overlapTool.retrieve());
    ANA_CHECK (m_electronsHandle.initialize (m_systematicsList, SG::AllowEmpty));
    ANA_CHECK (m_electronsSelectionHandle.initialize (m_systematicsList, m_electronsHandle, SG::AllowEmpty));
    ANA_CHECK (m_muonsHandle.initialize (m_systematicsList, SG::AllowEmpty));
    ANA_CHECK (m_muonsSelectionHandle.initialize (m_systematicsList, m_muonsHandle, SG::AllowEmpty));
    ANA_CHECK (m_jetsHandle.initialize (m_systematicsList, SG::AllowEmpty));
    ANA_CHECK (m_jetsSelectionHandle.initialize (m_systematicsList, m_jetsHandle, SG::AllowEmpty));
    ANA_CHECK (m_tausHandle.initialize (m_systematicsList, SG::AllowEmpty));
    ANA_CHECK (m_tausSelectionHandle.initialize (m_systematicsList, m_tausHandle, SG::AllowEmpty));
    ANA_CHECK (m_photonsHandle.initialize (m_systematicsList, SG::AllowEmpty));
    ANA_CHECK (m_photonsSelectionHandle.initialize (m_systematicsList, m_photonsHandle, SG::AllowEmpty));
    ANA_CHECK (m_fatJetsHandle.initialize (m_systematicsList, SG::AllowEmpty));
    ANA_CHECK (m_fatJetsSelectionHandle.initialize (m_systematicsList, m_fatJetsHandle, SG::AllowEmpty));
    ANA_CHECK (m_systematicsList.initialize());
    return StatusCode::SUCCESS;
  }



  StatusCode OverlapRemovalAlg ::
  execute ()
  {
    for (const auto& sys : m_systematicsList.systematicsVector())
    {
      std::unordered_map<const xAOD::IParticleContainer *, const SysWriteSelectionHandle *> decorationsMap;

      xAOD::ElectronContainer *electrons {nullptr};
      if (m_electronsHandle)
      {
        ANA_CHECK (m_electronsHandle.getCopy (electrons, sys));
        if (m_electronsSelectionHandle)
          decorationsMap.emplace(electrons, &m_electronsSelectionHandle);
      }
      xAOD::MuonContainer *muons {nullptr};
      if (m_muonsHandle)
      {
        ANA_CHECK (m_muonsHandle.getCopy (muons, sys));
        if (m_muonsSelectionHandle)
          decorationsMap.emplace(muons, &m_muonsSelectionHandle);
      }
      xAOD::JetContainer *jets {nullptr};
      if (m_jetsHandle)
      {
        ANA_CHECK (m_jetsHandle.getCopy (jets, sys));
        if (m_jetsSelectionHandle)
          decorationsMap.emplace(jets, &m_jetsSelectionHandle);
      }
      xAOD::TauJetContainer *taus {nullptr};
      if (m_tausHandle)
      {
        ANA_CHECK (m_tausHandle.getCopy (taus, sys));
        if (m_tausSelectionHandle)
          decorationsMap.emplace(taus, &m_tausSelectionHandle);
      }
      xAOD::PhotonContainer *photons {nullptr};
      if (m_photonsHandle)
      {
        ANA_CHECK (m_photonsHandle.getCopy (photons, sys));
        if (m_photonsSelectionHandle)
          decorationsMap.emplace(photons, &m_photonsSelectionHandle);
      }
      xAOD::JetContainer *fatJets {nullptr};
      if (m_fatJetsHandle)
      {
        ANA_CHECK (m_fatJetsHandle.getCopy (fatJets, sys));
        if (m_fatJetsSelectionHandle)
          decorationsMap.emplace(fatJets, &m_fatJetsSelectionHandle);
      }

      ATH_CHECK (m_overlapTool->removeOverlaps (electrons, muons, jets, taus,
                                                photons, fatJets));

      // Re-decorate if needed
      for (const auto &pair : decorationsMap)
      {
        for (const xAOD::IParticle *particle : *(pair.first))
        {
          (*(pair.second)).setBool
            (*particle, (*m_overlapRemovalAccessor) (*particle), sys);
        }
      }
    }

    return StatusCode::SUCCESS;
  }
}

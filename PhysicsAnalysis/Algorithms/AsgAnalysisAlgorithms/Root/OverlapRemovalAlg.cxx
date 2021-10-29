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
    ANA_CHECK (m_electronsDecoration.initialize (m_systematicsList, m_electronsHandle, SG::AllowEmpty));
    ANA_CHECK (m_muonsHandle.initialize (m_systematicsList, SG::AllowEmpty));
    ANA_CHECK (m_muonsDecoration.initialize (m_systematicsList, m_muonsHandle, SG::AllowEmpty));
    ANA_CHECK (m_jetsHandle.initialize (m_systematicsList, SG::AllowEmpty));
    ANA_CHECK (m_jetsDecoration.initialize (m_systematicsList, m_jetsHandle, SG::AllowEmpty));
    ANA_CHECK (m_tausHandle.initialize (m_systematicsList, SG::AllowEmpty));
    ANA_CHECK (m_tausDecoration.initialize (m_systematicsList, m_tausHandle, SG::AllowEmpty));
    ANA_CHECK (m_photonsHandle.initialize (m_systematicsList, SG::AllowEmpty));
    ANA_CHECK (m_photonsDecoration.initialize (m_systematicsList, m_photonsHandle, SG::AllowEmpty));
    ANA_CHECK (m_fatJetsHandle.initialize (m_systematicsList, SG::AllowEmpty));
    ANA_CHECK (m_fatJetsDecoration.initialize (m_systematicsList, m_fatJetsHandle, SG::AllowEmpty));
    ANA_CHECK (m_systematicsList.initialize());
    return StatusCode::SUCCESS;
  }



  StatusCode OverlapRemovalAlg ::
  execute ()
  {
    for (const auto& sys : m_systematicsList.systematicsVector())
    {
      std::unordered_map<const xAOD::IParticleContainer *, const SysWriteDecorHandle<char> *> decorationsMap;

      xAOD::ElectronContainer *electrons {nullptr};
      if (m_electronsHandle)
      {
        ANA_CHECK (m_electronsHandle.getCopy (electrons, sys));
        if (m_electronsDecoration)
          decorationsMap.emplace(electrons, &m_electronsDecoration);
      }
      xAOD::MuonContainer *muons {nullptr};
      if (m_muonsHandle)
      {
        ANA_CHECK (m_muonsHandle.getCopy (muons, sys));
        if (m_muonsDecoration)
          decorationsMap.emplace(muons, &m_muonsDecoration);
      }
      xAOD::JetContainer *jets {nullptr};
      if (m_jetsHandle)
      {
        ANA_CHECK (m_jetsHandle.getCopy (jets, sys));
        if (m_jetsDecoration)
          decorationsMap.emplace(jets, &m_jetsDecoration);
      }
      xAOD::TauJetContainer *taus {nullptr};
      if (m_tausHandle)
      {
        ANA_CHECK (m_tausHandle.getCopy (taus, sys));
        if (m_tausDecoration)
          decorationsMap.emplace(taus, &m_tausDecoration);
      }
      xAOD::PhotonContainer *photons {nullptr};
      if (m_photonsHandle)
      {
        ANA_CHECK (m_photonsHandle.getCopy (photons, sys));
        if (m_photonsDecoration)
          decorationsMap.emplace(photons, &m_photonsDecoration);
      }
      xAOD::JetContainer *fatJets {nullptr};
      if (m_fatJetsHandle)
      {
        ANA_CHECK (m_fatJetsHandle.getCopy (fatJets, sys));
        if (m_fatJetsDecoration)
          decorationsMap.emplace(fatJets, &m_fatJetsDecoration);
      }

      ATH_CHECK (m_overlapTool->removeOverlaps (electrons, muons, jets, taus,
                                                photons, fatJets));

      // Re-decorate if needed
      for (const auto &pair : decorationsMap)
      {
        for (const xAOD::IParticle *particle : *(pair.first))
        {
          (*(pair.second)).set (*particle, (*m_overlapRemovalAccessor) (*particle), sys);
        }
      }
    }

    return StatusCode::SUCCESS;
  }
}

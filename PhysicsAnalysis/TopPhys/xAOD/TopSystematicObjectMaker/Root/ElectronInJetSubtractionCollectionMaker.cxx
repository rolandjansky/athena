/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

// $Id: ElectronInJetSubtractionCollectionMaker.cxx 806051 2017-06-07 00:32:41Z tpelzer $
#include "TopSystematicObjectMaker/ElectronInJetSubtractionCollectionMaker.h"
#include "TopConfiguration/TopConfig.h"
#include "TopEvent/EventTools.h"

#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODCore/ShallowCopy.h"

namespace top {
  ElectronInJetSubtractionCollectionMaker::ElectronInJetSubtractionCollectionMaker(const std::string& name) :
    asg::AsgTool(name),
    m_config(nullptr),
    m_doLooseCuts(false) {
    declareProperty("config", m_config);
  }

  StatusCode ElectronInJetSubtractionCollectionMaker::initialize() {
    if (!m_config->isMC()) m_doLooseCuts = true;

    if (m_config->doOverlapRemovalOnLooseLeptonDef()) m_doLooseCuts = true;

    if (m_config->doLooseEvents()) m_doLooseCuts = true;

    return StatusCode::SUCCESS;
  }

  StatusCode ElectronInJetSubtractionCollectionMaker::execute(bool executeNominal) {
    // Loop over nominal, all electron syst and all jet syst
    // For each:
    //  Create a shallow electron and a shallow jet collection
    //  Run the subtraction algorithm:
    //    - Modify some jet 4-momentum
    //    - Veto some electrons
    //  Place modified shallow collections into TStore

    for (auto currentSystematic : *m_config->systHashElectronInJetSubtraction()) {
      ///-- if executeNominal, skip other systematics (and vice-versa) --///
      if (executeNominal && !m_config->isSystNominal(m_config->systematicName(currentSystematic))) continue;
      if (!executeNominal && m_config->isSystNominal(m_config->systematicName(currentSystematic))) continue;

      // Get the Electrons
      const xAOD::ElectronContainer* electrons(nullptr);
      top::check(evtStore()->retrieve(electrons, m_config->sgKeyElectronsStandAlone(
                                        currentSystematic)),
                 "ElectronInJetSubtractionCollectionMaker::execute() Failed to retrieve electrons");

      // Get the Jets
      const xAOD::JetContainer* jets(nullptr);
      top::check(evtStore()->retrieve(jets, m_config->sgKeyJetsStandAlone(
                                        currentSystematic)),
                 "ElectronInJetSubtractionCollectionMaker::execute() Failed to retrieve jets");

      // Shallow copy the electrons
      std::pair< xAOD::ElectronContainer*, xAOD::ShallowAuxContainer* > shallow_electrons = xAOD::shallowCopyContainer(
        *electrons);
      // Shallow copy the jets
      std::pair< xAOD::JetContainer*, xAOD::ShallowAuxContainer* > shallow_jets = xAOD::shallowCopyContainer(*jets);

      // Apply the Electron In Jet Subtraction
      // The actual calculation is performed in another package
      m_subtractionAlgorithm.apply(shallow_electrons.first, shallow_jets.first);

      // Save the modified electrons to TStore
      std::string outputElectronsSGKey = m_config->sgKeyElectrons(currentSystematic);
      std::string outputElectronsSGKeyAux = outputElectronsSGKey + "Aux.";

      StatusCode saveElectrons = evtStore()->tds()->record(shallow_electrons.first, outputElectronsSGKey);
      StatusCode saveElectronsAux = evtStore()->tds()->record(shallow_electrons.second, outputElectronsSGKeyAux);
      if (!saveElectrons || !saveElectronsAux) {
        return StatusCode::FAILURE;
      }

      // Save the modified jets to TStore
      std::string outputJetsSGKey = m_config->sgKeyJets(currentSystematic, false);
      std::string outputJetsSGKeyAux = outputJetsSGKey + "Aux.";

      StatusCode saveJets = evtStore()->tds()->record(shallow_jets.first, outputJetsSGKey);
      StatusCode saveJetsAux = evtStore()->tds()->record(shallow_jets.second, outputJetsSGKeyAux);
      if (!saveJets || !saveJetsAux) {
        return StatusCode::FAILURE;
      }


      // If we are using loose lepton definitions anywhere, we'd better make more jet colelctions
      // We subtract loose leptons from these jets
      // We now have loose jets! fun fun fun, we all love combinatorics

      if (m_doLooseCuts) {
        // Shallow copy the jets
        std::pair< xAOD::JetContainer*, xAOD::ShallowAuxContainer* > shallow_jetsLoose = xAOD::shallowCopyContainer(
          *jets);

        // Apply the Electron In Jet Subtraction
        // The actual calculation is performed in another package
        m_subtractionAlgorithm.apply(shallow_electrons.first, shallow_jetsLoose.first, m_doLooseCuts);

        // Save the modified jets to TStore
        std::string outputJetsLooseSGKey = m_config->sgKeyJets(currentSystematic, m_doLooseCuts);
        std::string outputJetsLooseSGKeyAux = outputJetsLooseSGKey + "Aux.";

        StatusCode saveJetsLoose = evtStore()->tds()->record(shallow_jetsLoose.first, outputJetsLooseSGKey);
        StatusCode saveJetsLooseAux =
          evtStore()->tds()->record(shallow_jetsLoose.second, outputJetsLooseSGKeyAux);
        if (!saveJetsLoose || !saveJetsLooseAux) {
          return StatusCode::FAILURE;
        }
      }
    }

    return StatusCode::SUCCESS;
  }
}

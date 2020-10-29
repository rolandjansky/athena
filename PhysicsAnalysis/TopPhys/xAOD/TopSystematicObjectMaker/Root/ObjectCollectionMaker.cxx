/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

// $Id: ObjectCollectionMaker.cxx 790414 2016-12-19 00:49:08Z tpelzer $
#include "TopSystematicObjectMaker/ObjectCollectionMaker.h"
#include "TopConfiguration/TopConfig.h"
#include "TopEvent/EventTools.h"

// EDM include(s):
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODTracking/TrackParticleContainer.h"


namespace top {
  ObjectCollectionMaker::ObjectCollectionMaker(const std::string& name) :
    asg::AsgTool(name),
    m_config(nullptr),

    m_egammaMaker(nullptr),
    m_muonMaker(nullptr),
    m_softmuonMaker(nullptr),
    m_tauMaker(nullptr),
    m_jetMaker(nullptr),
    m_metMaker(nullptr),
    m_trackSystMaker(nullptr)
  {
    declareProperty("config", m_config);
  }

  StatusCode ObjectCollectionMaker::initialize() {
    ATH_MSG_INFO(" top::ObjectCollectionMaker initialize");

    m_egammaMaker =
      std::unique_ptr<top::EgammaObjectCollectionMaker>    (new top::EgammaObjectCollectionMaker(
                                                              "top::EgammaObjectCollectionMaker"));
    m_muonMaker =
      std::unique_ptr<top::MuonObjectCollectionMaker>      (new top::MuonObjectCollectionMaker(
                                                              "top::MuonObjectCollectionMaker"));
    m_softmuonMaker =
      std::unique_ptr<top::SoftMuonObjectCollectionMaker>      (new top::SoftMuonObjectCollectionMaker(
                                                                  "top::SoftMuonObjectCollectionMaker"));
    m_tauMaker =
      std::unique_ptr<top::TauObjectCollectionMaker>       (new top::TauObjectCollectionMaker(
                                                              "top::TauObjectCollectionMaker"));
    m_jetMaker =
      std::unique_ptr<top::JetObjectCollectionMaker>       (new top::JetObjectCollectionMaker(
                                                              "top::JetObjectCollectionMaker"));
    m_metMaker =
      std::unique_ptr<top::MissingETObjectCollectionMaker> (new top::MissingETObjectCollectionMaker(
                                                              "top::MissingETObjectCollectionMaker"));
    m_ghostTrackSystMaker =
      std::unique_ptr<top::GhostTrackSystematicsMaker> (new top::GhostTrackSystematicsMaker(
                                                          "top::GhostTrackSystematicsMaker"));

    m_trackSystMaker =
      std::unique_ptr<top::TrackSystematicsMaker> (new top::TrackSystematicsMaker(
						      "top::TrackSystematicsMaker"));


    if (m_config->usePhotons() || m_config->useElectrons() || m_config->useFwdElectrons()) {
      top::check(m_egammaMaker->setProperty("config", m_config), "Failed to setProperty");
      top::check(m_egammaMaker->initialize(), "Failed to initialize");
    }

    if (m_config->useMuons()) {
      top::check(m_muonMaker->setProperty("config", m_config), "Failed to setProperty");
      top::check(m_muonMaker->initialize(), "Failed to initialize");
    }

    if (m_config->useSoftMuons()) {
      top::check(m_softmuonMaker->setProperty("config", m_config), "Failed to setProperty");
      top::check(m_softmuonMaker->initialize(), "Failed to initialize");
    }

    if (m_config->useTaus()) {
      top::check(m_tauMaker->setProperty("config", m_config), "Failed to setProperty");
      top::check(m_tauMaker->initialize(), "Failed to initialize");
    }

    if (m_config->useJets() || m_config->useLargeRJets()) {
      top::check(m_jetMaker->setProperty("config", m_config), "Failed to setProperty");
      top::check(m_jetMaker->initialize(), "Failed to initialize");
    }

    if (m_config->useJets() && m_config->useJetGhostTrack()) {
      top::check(m_ghostTrackSystMaker->setProperty("config", m_config),
                 "Failed to setProperty");

      top::check(m_ghostTrackSystMaker->initialize(),
                 "Failed to initialize");
    }

    if (m_config->useTracks()) {
      top::check(m_trackSystMaker->setProperty("config", m_config),
                 "Failed to setProperty");

      top::check(m_trackSystMaker->initialize(),
                 "Failed to initialize");
    }


    top::check(m_metMaker->setProperty("config", m_config), "Failed to setProperty");
    top::check(m_metMaker->initialize(), "Failed to initialize");

    return StatusCode::SUCCESS;
  }

  StatusCode ObjectCollectionMaker::execute(bool executeNominal) {
    if (executeNominal) {
      top::check(this->executeNominal(), "Failed to executeNominal()");
    } else {
      top::check(this->executeSystematics(), "Failed to executeSystematics()");
    }
    return StatusCode::SUCCESS;
  }

  StatusCode ObjectCollectionMaker::executeNominal() {
    if (m_config->usePhotons()) {
      top::check(m_egammaMaker->executePhotons(true), "Failed to executePhotons()");
    }
    if (m_config->useElectrons()) {
      top::check(m_egammaMaker->executeElectrons(true), "Failed to executeElectrons()");
    }
    if (m_config->useFwdElectrons()) {
      top::check(m_egammaMaker->executeFwdElectrons(true), "Failed to executeFwdElectrons()");
    }
    if (m_config->useMuons()) {
      top::check(m_muonMaker->execute(true), "Failed to executeMuons()");
    }
    if (m_config->useSoftMuons()) {
      top::check(m_softmuonMaker->execute(true), "Failed to executeSoftMuons()");
    }
    if (m_config->useTaus()) {
      top::check(m_tauMaker->execute(true), "Failed to executeTaus()");
    }
    if (m_config->useJets()) {
      top::check(m_jetMaker->executeJets(true), "Failed to executeJets()");
    }
    if (m_config->useLargeRJets()) {
      top::check(m_jetMaker->executeLargeRJets(true), "Failed to executeLargeRJets()");
    }
    if (m_config->useTrackJets()) {
      top::check(m_jetMaker->executeTrackJets(true), "Failed to executeTrackJets() ");
    }
    if (m_config->useTracks()) {
      top::check(m_trackSystMaker->execute(true), "Failed to executeTracks() ");
    }

    // This must come _AFTER_ the jets have been calibrated!
    if (m_config->useJets() && m_config->useJetGhostTrack()) {
      top::check(m_ghostTrackSystMaker->execute(true), "Failed to executeGhostTrackSystematics() ");
    }

    m_config->setNominalAvailable(true);

    return StatusCode::SUCCESS;
  }

  StatusCode ObjectCollectionMaker::executeSystematics() {
    if (m_config->isNominalAvailable() == false) {
      ATH_MSG_ERROR("ObjectCollectionMaker::executeNominal has not been called before executeSystematics");
      return StatusCode::FAILURE;
    }

    if (m_config->usePhotons()) {
      top::check(m_egammaMaker->executePhotons(false), "Failed to executePhotons()");
    }
    if (m_config->useElectrons()) {
      top::check(m_egammaMaker->executeElectrons(false), "Failed to executeElectrons()");
    }
    if (m_config->useFwdElectrons()) {
      top::check(m_egammaMaker->executeFwdElectrons(false), "Failed to executeFwdElectrons()");
    }
    if (m_config->useMuons()) {
      top::check(m_muonMaker->execute(false), "Failed to executeMuons()");
    }
    if (m_config->useSoftMuons()) {
      top::check(m_softmuonMaker->execute(false), "Failed to executeSoftMuons()");
    }
    if (m_config->useTaus()) {
      top::check(m_tauMaker->execute(false), "Failed to executeTaus()");
    }
    if (m_config->useJets()) {
      top::check(m_jetMaker->executeJets(false), "Failed to executeJets()");
    }
    if (m_config->useLargeRJets()) {
      top::check(m_jetMaker->executeLargeRJets(false), "Failed to executeLargeRJets()");
    }
    if (m_config->useTrackJets()) {
      top::check(m_jetMaker->executeTrackJets(false), "Failed to executeTrackJets() ");
    }

    // This must come _AFTER_ the jets have been calibrated!
    if (m_config->useJets() && m_config->useJetGhostTrack()) {
      top::check(m_ghostTrackSystMaker->execute(false), "Failed to executeGhostTrackSystematics() ");
    }


    if (m_config->useTracks()) {
      top::check(m_trackSystMaker->execute(false), "Failed to executeTracks() ");
    }


    return StatusCode::SUCCESS;
  }

  StatusCode ObjectCollectionMaker::recalculateMET(bool executeNominal) {
    top::check(m_metMaker->recalculateMET(executeNominal), "Failed to recalculateMET()");
    return StatusCode::SUCCESS;
  }

  StatusCode ObjectCollectionMaker::printout() {
    if (m_config->usePhotons()) {
      top::check(m_egammaMaker->printoutPhotons(), "Failed to printoutPhotons()");
    }
    if (m_config->useElectrons()) {
      top::check(m_egammaMaker->printoutElectrons(), "Failed to printoutElectrons()");
    }
    if (m_config->useFwdElectrons()) {
      top::check(m_egammaMaker->printoutFwdElectrons(), "Failed to printoutFwdElectrons()");
    }
    if (m_config->useMuons()) {
      top::check(m_muonMaker->printout(), "Failed to printoutMuons()");
    }
    if (m_config->useSoftMuons()) {
      top::check(m_softmuonMaker->printout(), "Failed to printoutSoftMuons()");
    }
    if (m_config->useTaus()) {
      top::check(m_tauMaker->printout(), "Failed to printoutTaus()");
    }
    if (m_config->useJets()) {
      top::check(m_jetMaker->printoutJets(), "Failed to printoutJets()");
    }
    if (m_config->useLargeRJets()) {
      top::check(m_jetMaker->printoutLargeRJets(), "Failed to printoutLargeRJets()");
    }

    return StatusCode::SUCCESS;
  }

  bool ObjectCollectionMaker::isTruthDxAOD() const {
    ///-- If any of the following aren't present, then assume we're running over a truth DxAOD --///
    if (m_config->usePhotons()) {
      if (!evtStore()->contains<xAOD::PhotonContainer>(m_config->sgKeyPhotons())) {
        return true;
      }
    }

    if (m_config->useElectrons()) {
      if (!evtStore()->contains<xAOD::ElectronContainer>(m_config->sgKeyElectrons())) {
        return true;
      }
    }

    if (m_config->useMuons()) {
      if (!evtStore()->contains<xAOD::MuonContainer>(m_config->sgKeyMuons())) {
        return true;
      }
    }

    if (m_config->useTaus()) {
      if (!evtStore()->contains<xAOD::TauJetContainer>(m_config->sgKeyTaus())) {
        return true;
      }
    }

    if (m_config->useJets()) {
      if (!evtStore()->contains<xAOD::JetContainer>(m_config->sgKeyJets())) {
        return true;
      }
    }

    ///-- If we have made it this far, then all reco objects are available --///
    ///-- Therefore we can't be running over a truth DxAOD --///
    return false;
  }
}

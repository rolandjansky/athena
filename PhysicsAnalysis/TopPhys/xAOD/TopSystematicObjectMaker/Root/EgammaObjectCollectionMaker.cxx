/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

// $Id: EgammaObjectCollectionMaker.cxx 811374 2017-10-24 13:04:52Z iconnell $
#include "TopSystematicObjectMaker/EgammaObjectCollectionMaker.h"
#include "TopConfiguration/TopConfig.h"
#include "TopConfiguration/TreeFilter.h"
#include "TopEvent/EventTools.h"

#include "AthContainers/AuxElement.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/PhotonAuxContainer.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"
#include "PATInterfaces/SystematicsUtil.h"

#include <list>

namespace top {
  EgammaObjectCollectionMaker::EgammaObjectCollectionMaker(const std::string& name) :
    asg::AsgTool(name),
    m_config(nullptr),

    m_specifiedSystematicsPhotons(),
    m_specifiedSystematicsElectrons(),
    m_specifiedSystematicsFwdElectrons(),
    m_recommendedSystematicsPhotons(),
    m_recommendedSystematicsElectrons(),
    m_recommendedSystematicsFwdElectrons(),

    m_calibrationTool("EgammaCalibrationAndSmearingTool"),
    m_photonFudgeTool("PhotonFudgeTool"),

    m_isolationCorr("IsolationCorrectionTool") {
    declareProperty("config", m_config);
  }

  StatusCode EgammaObjectCollectionMaker::initialize() {
    ATH_MSG_INFO(" top::EgammaObjectCollectionMaker initialize");

    top::check(m_calibrationTool.retrieve(), "Failed to retrieve egamma calibration tool");

    // These flags were for early R21 when we were asked not to calibrate egamma objects
    calibrateElectrons = true;
    calibrateFwdElectrons = true;
    calibratePhotons = true;

    if (m_config->usePhotons()) {
      for (const std::string& WP : m_config->photonIsolationWPs()) {
        m_photonIsolationTools[WP] = ToolHandle<CP::IIsolationSelectionTool>("IsolationTool_PhotonWP_" + WP);
        top::check(m_photonIsolationTools[WP].retrieve(), "Failed to retrieve photon isolation tool for WP " + WP);
      }
      top::check(m_photonFudgeTool.retrieve(), "Failed to retrieve photon shower shape fudge tool");
    }

    if (m_config->useElectrons()) {
      for (const std::string& WP : m_config->electronIsolationWPs()) {
        m_electronIsolationTools[WP] = ToolHandle<CP::IIsolationSelectionTool>("IsolationTool_ElectronWP_" + WP);
        top::check(m_electronIsolationTools[WP].retrieve(), "Failed to retrieve electron isolation tool for WP " + WP);
      }
    }

    top::check(m_isolationCorr.retrieve(), "Failed to retrieve Isolation Correction Tool");

    std::set<std::string> systPhoton;
    std::set<std::string> systElectron;
    std::set<std::string> systFwdElectron;

    const std:: string& syststr = m_config->systematics();
    std::set<std::string> syst;

    if (!m_config->isSystNominal(syststr) && !m_config->isSystAll(syststr)) {
      bool ok = m_config->getSystematicsList(syststr, syst);
      if (!ok) {
        ATH_MSG_ERROR(" top::EgammaObjectCollectionMaker could not determine systematic list");
        return StatusCode::FAILURE;
      }
      //here the idea is that if the user specifies AllXXX, we leave syst as an empty string, so that all recommended CP
      // systematics are then used
      if (!m_config->contains(syst, "AllElectrons")) {
        systElectron = syst;
      }
      if (!m_config->contains(syst, "AllPhotons")) {
        systPhoton = syst;
      }
      if (!m_config->contains(syst, "AllFwdElectrons")) {
        systFwdElectron = syst;
      }
    }

    specifiedSystematicsPhotons(systPhoton);
    specifiedSystematicsElectrons(systElectron);
    specifiedSystematicsFwdElectrons(systFwdElectron);

    if (m_config->usePhotons()) {
      m_config->systematicsPhotons(specifiedSystematicsPhotons());
    }
    if (m_config->useElectrons()) {
      m_config->systematicsElectrons(specifiedSystematicsElectrons());
    }
    if (m_config->useFwdElectrons()) {
      m_config->systematicsFwdElectrons(specifiedSystematicsFwdElectrons());
    }

    // bool to decide whether to use certain Egamma tools
    m_recomputePhotonFudge = m_config->recomputeCPvars();

    return StatusCode::SUCCESS;
  }

  StatusCode EgammaObjectCollectionMaker::executePhotons(bool executeNominal) {
    ///-- Get base photons from xAOD --///
    const xAOD::PhotonContainer* xaod(nullptr);

    top::check(evtStore()->retrieve(xaod, m_config->sgKeyPhotons()), "Failed to retrieve Photons");

    ///-- Loop over all systematics --///
    for (auto systematic : m_specifiedSystematicsPhotons) {
      ///-- if executeNominal, skip other systematics (and vice-versa) --///
      if (executeNominal && !m_config->isSystNominal(m_config->systematicName(systematic.hash()))) continue;
      if (!executeNominal && m_config->isSystNominal(m_config->systematicName(systematic.hash()))) continue;

      ///-- Tell tool which systematic to use --///
      top::check(m_calibrationTool->applySystematicVariation(systematic), "Failed to applySystematicVariation");

      ///-- Shallow copy of the xAOD --///
      std::pair< xAOD::PhotonContainer*, xAOD::ShallowAuxContainer* > shallow_xaod_copy = xAOD::shallowCopyContainer(
        *xaod);

      ///-- Loop over the xAOD Container and apply corrections--///
      for (auto photon : *(shallow_xaod_copy.first)) {
        ///-- Apply correction to object --///
        // Needs a calo cluster so carry on if no cluster
        if (!photon->caloCluster()) continue;

        if (calibratePhotons) {
          top::check(m_calibrationTool->applyCorrection(*photon),
                     "Failed to applyCorrection");
          // TODO -- revert back once the isolation corrections are fully settled in R22
          // top::check(m_isolationCorr->applyCorrection(*photon),
          //            "Failed to apply photon isolation leakage correction");

          // Only apply shower shape fudging on full simulation MC
          if (m_config->isMC() && !m_config->isAFII() && m_recomputePhotonFudge && m_config->getDerivationStream() != "PHYS") {
            if (m_photonFudgeTool->applyCorrection(*photon) == 0) { // 0: error, 1: OutOfRange (not possible), 2: OK
              // ElectronPhotonShowerShapeFudgeTool::applyCorrection can return an error for 3 reasons
              // 1) shower shapes not all found, 2) bad cluster, 3) shower shapes not all set.
              // 1 & 3 are most likely due to the smart slimming (no PhotonsDetailedCPContent), whereas 2 is an actual
              // issue.
              // Check for case 2 now:
              if (photon->caloCluster() == nullptr) {
                ATH_MSG_ERROR("Photon " << photon << " had no calo cluster - this is bad!");
                return StatusCode::FAILURE;
              } else {
                // We're now in case 1 or 3
                ATH_MSG_WARNING(
                  " Didn't find the necessary photon shower shapes variables for the ElectronPhotonShowerShapeFudgeTool! (but don't worry, you're still getting correctly ID'd photons)");
                // Keep going, but don't try to use the tool anymore
                m_recomputePhotonFudge = false;
              }
            }
          }
        }
        ///-- Isolation selection --///
        for (const auto& photonIsoWP : m_photonIsolationTools) {
          const char passIsol = (photonIsoWP.second->accept(*photon)) ? 1 : 0;
          photon->auxdecor<char>("AnalysisTop_Isol_" + photonIsoWP.first) = passIsol;
        }
      } // end photon loop

      ///-- set links to original objects- needed for MET calculation --///
      bool setLinks = xAOD::setOriginalObjectLink(*xaod, *shallow_xaod_copy.first);
      if (!setLinks) ATH_MSG_ERROR(" Cannot set original object links for photons, MET recalculation may struggle");

      ///-- Save corrected xAOD Container to StoreGate / TStore --///
      std::string outputSGKey = m_config->sgKeyPhotons(systematic.hash());
      std::string outputSGKeyAux = outputSGKey + "Aux.";

      StatusCode save = evtStore()->tds()->record(shallow_xaod_copy.first, outputSGKey);
      StatusCode saveAux = evtStore()->tds()->record(shallow_xaod_copy.second, outputSGKeyAux);

      if (!save || !saveAux) {
        return StatusCode::FAILURE;
      }
    }  // Loop over all systematics
    return StatusCode::SUCCESS;
  }

  StatusCode EgammaObjectCollectionMaker::executeElectrons(bool executeNominal) {
    const xAOD::EventInfo* eventInfo(nullptr);

    top::check(evtStore()->retrieve(eventInfo, m_config->sgKeyEventInfo()), "Failed to retrieve EventInfo");
    float beam_pos_sigma_x = eventInfo->beamPosSigmaX();
    float beam_pos_sigma_y = eventInfo->beamPosSigmaY();
    float beam_pos_sigma_xy = eventInfo->beamPosSigmaXY();

    ///-- Get base electrons from xAOD --///
    const xAOD::ElectronContainer* xaod(nullptr);
    top::check(evtStore()->retrieve(xaod, m_config->sgKeyElectrons()), "Failed to retrieve Electrons");

    ///-- Loop over all systematics --///
    for (auto systematic : m_specifiedSystematicsElectrons) {
      ///-- if executeNominal, skip other systematics (and vice-versa) --///
      if (executeNominal && !m_config->isSystNominal(m_config->systematicName(systematic.hash()))) continue;
      if (!executeNominal && m_config->isSystNominal(m_config->systematicName(systematic.hash()))) continue;

      ///-- Tell tool which systematic to use -///
      top::check(m_calibrationTool->applySystematicVariation(systematic), "Failed to applySystematicVariation");

      ///-- Shallow copy of the xAOD --///
      std::pair< xAOD::ElectronContainer*, xAOD::ShallowAuxContainer* > shallow_xaod_copy = xAOD::shallowCopyContainer(
        *xaod);

      ///-- Loop over the xAOD Container --///
      for (auto electron : *(shallow_xaod_copy.first)) {
        // Apply correction to object
        // should not affect derivations if there is no CC or track thinning
        if (electron->caloCluster() != nullptr && electron->trackParticle() != nullptr) { // derivations might remove CC
                                                                                          // and tracks for low pt
                                                                                          // electrons
          if (calibrateElectrons) {
            top::check(m_calibrationTool->applyCorrection(*electron), "Failed to applyCorrection");
            // TODO -- revert back once the isolation corrections are fully settled in R22
            // top::check(m_isolationCorr->applyCorrection(*electron), "Failed to apply leakage correction");
          }

          double d0sig = xAOD::TrackingHelpers::d0significance(electron->trackParticle(),
                                                               beam_pos_sigma_x,
                                                               beam_pos_sigma_y,
                                                               beam_pos_sigma_xy);
          electron->auxdecor<float>("d0sig") = d0sig;

          if (eventInfo->isAvailable<float>("AnalysisTop_PRIVTX_z_position")) {
            float vtx_z = eventInfo->auxdata<float>("AnalysisTop_PRIVTX_z_position");
            float delta_z0 = electron->trackParticle()->z0() + electron->trackParticle()->vz() - vtx_z;
            electron->auxdecor<float>("delta_z0") = delta_z0;
            electron->auxdecor<float>("delta_z0_sintheta") = delta_z0 * std::sin(electron->trackParticle()->theta());
          }
        }

        ///-- Isolation selection --///
        for (const auto& electronIsoWP : m_electronIsolationTools) {
          const char passIsol = (electronIsoWP.second->accept(*electron)) ? 1 : 0;
          electron->auxdecor<char>("AnalysisTop_Isol_" + electronIsoWP.first) = passIsol;
        }
      } // end electron loop

      ///-- set links to original objects- needed for MET calculation --///
      bool setLinks = xAOD::setOriginalObjectLink(*xaod, *shallow_xaod_copy.first);
      if (!setLinks) ATH_MSG_ERROR(" Cannot set original object links for electrons, MET recalculation may struggle");

      // Save corrected xAOD Container to StoreGate / TStore
      std::string outputSGKey = m_config->sgKeyElectronsStandAlone(systematic.hash());
      std::string outputSGKeyAux = outputSGKey + "Aux.";

      StatusCode save = evtStore()->tds()->record(shallow_xaod_copy.first, outputSGKey);
      StatusCode saveAux = evtStore()->tds()->record(shallow_xaod_copy.second, outputSGKeyAux);

      if (!save || !saveAux) {
        return StatusCode::FAILURE;
      }
    }  // Loop over all systematics

    return StatusCode::SUCCESS;
  }

  StatusCode EgammaObjectCollectionMaker::executeFwdElectrons(bool executeNominal) {
    const xAOD::EventInfo* eventInfo(nullptr);

    top::check(evtStore()->retrieve(eventInfo, m_config->sgKeyEventInfo()), "Failed to retrieve EventInfo");

    ///-- Get base electrons from xAOD --///
    const xAOD::ElectronContainer* xaod(nullptr);
    top::check(evtStore()->retrieve(xaod, m_config->sgKeyFwdElectrons()), "Failed to retrieve Fwd Electrons");

    ///-- Loop over all systematics --///
    for (auto systematic : m_specifiedSystematicsFwdElectrons) {
      ///-- if executeNominal, skip other systematics (and vice-versa) --///
      if (executeNominal && !m_config->isSystNominal(m_config->systematicName(systematic.hash()))) continue;
      if (!executeNominal && m_config->isSystNominal(m_config->systematicName(systematic.hash()))) continue;

      ///-- Tell tool which systematic to use -///
      top::check(m_calibrationTool->applySystematicVariation(systematic), "Failed to applySystematicVariation");

      ///-- Shallow copy of the xAOD --///
      std::pair< xAOD::ElectronContainer*, xAOD::ShallowAuxContainer* > shallow_xaod_copy = xAOD::shallowCopyContainer(
        *xaod);

      ///-- Loop over the xAOD Container --///
      for (auto electron : *(shallow_xaod_copy.first)) {
        // Apply correction to object
        // should not affect derivations if there is no CC or track thinning
        if (electron->caloCluster() != nullptr) { // derivations might remove CC for low pt electrons
          if (calibrateFwdElectrons) {
            top::check(m_calibrationTool->applyCorrection(*electron), "Failed to applyCorrection to fwd electrons");
          }
        }
      }//end of loop on electrons

      ///-- set links to original objects- needed for MET calculation --///
      bool setLinks = xAOD::setOriginalObjectLink(*xaod, *shallow_xaod_copy.first);
      if (!setLinks) ATH_MSG_ERROR(" Cannot set original object links for fwd electrons");

      // Save corrected xAOD Container to StoreGate / TStore
      std::string outputSGKey = m_config->sgKeyFwdElectronsStandAlone(systematic.hash());
      std::string outputSGKeyAux = outputSGKey + "Aux.";

      StatusCode save = evtStore()->tds()->record(shallow_xaod_copy.first, outputSGKey);
      StatusCode saveAux = evtStore()->tds()->record(shallow_xaod_copy.second, outputSGKeyAux);

      if (!save || !saveAux) {
        return StatusCode::FAILURE;
      }
    }  // Loop over all systematics

    return StatusCode::SUCCESS;
  }

  StatusCode EgammaObjectCollectionMaker::printoutPhotons() {
    // Loop over all systematics
    for (auto s : m_specifiedSystematicsPhotons) {
      const xAOD::PhotonContainer* xaod(nullptr);
      top::check(evtStore()->retrieve(xaod, m_config->sgKeyPhotons(s.hash())), "Failed to retrieve Photons");

      ATH_MSG_INFO(" Photons with sgKey = " << m_config->sgKeyPhotons(s.hash()));
      for (auto x : *xaod) {
        float ptcone30(0.);
        x->isolationValue(ptcone30, xAOD::Iso::ptcone30);
        ATH_MSG_INFO("   ph pT , eta , ptcone30 = " << x->pt() << " , " << x->eta() << " , " << ptcone30);
      }
    }

    return StatusCode::SUCCESS;
  }

  StatusCode EgammaObjectCollectionMaker::printoutElectrons() {
    // Loop over all systematics
    for (auto s : m_specifiedSystematicsElectrons) {
      const xAOD::ElectronContainer* xaod(nullptr);
      top::check(evtStore()->retrieve(xaod, m_config->sgKeyElectronsStandAlone(
                                        s.hash())), "Failed to retrieve Electrons");

      ATH_MSG_INFO(" Electrons with sgKey = " << m_config->sgKeyElectronsStandAlone(s.hash()));
      for (auto x : *xaod) {
        float ptcone30(0.);
        x->isolationValue(ptcone30, xAOD::Iso::ptcone30);
        ATH_MSG_INFO("   El pT , eta , ptcone30 = " << x->pt() << " , " << x->eta() << " , " << ptcone30);
      }
    }

    return StatusCode::SUCCESS;
  }

  StatusCode EgammaObjectCollectionMaker::printoutFwdElectrons() {
    // Loop over all systematics
    for (auto s : m_specifiedSystematicsFwdElectrons) {
      const xAOD::ElectronContainer* xaod(nullptr);
      top::check(evtStore()->retrieve(xaod, m_config->sgKeyFwdElectronsStandAlone(
                                        s.hash())), "Failed to retrieve Fwd Electrons");

      ATH_MSG_INFO(" Fwd Electrons with sgKey = " << m_config->sgKeyFwdElectronsStandAlone(s.hash()));
      for (auto x : *xaod) {
        ATH_MSG_INFO("   El pT , eta = " << x->pt() << " , " << x->eta());
      }
    }

    return StatusCode::SUCCESS;
  }

  void EgammaObjectCollectionMaker::specifiedSystematicsPhotons(const std::set<std::string>& specifiedSystematics) {
    ///-- Get the recommended systematics from the tool, in std::vector format --///
    const std::vector<CP::SystematicSet> systList = CP::make_systematics_vector(
      m_calibrationTool->recommendedSystematics());

    for (auto s : systList) {
      
      if(!m_config->getTreeFilter()->filterTree(s.name())) continue; // Applying tree filter
      m_recommendedSystematicsPhotons.push_back(s);
      if (s.name() == "") {
        m_specifiedSystematicsPhotons.push_back(s);
      }

      ///-- MC only --///
      if (m_config->isMC()) {
        ///-- Are we only doing Nominal? Did the user specify specific systematics to use? --///
        if (!m_config->isSystNominal(m_config->systematics())) {
          if (specifiedSystematics.size() == 0) {
            m_specifiedSystematicsPhotons.push_back(s);
          }
          if (specifiedSystematics.size() > 0) {
            for (auto i : specifiedSystematics) {
              TreeFilter filter(i);
              if (!filter.filterTree(s.name())) {
                m_specifiedSystematicsPhotons.push_back(s);
              }
            }
          }
        }
      }
    }
    m_recommendedSystematicsPhotons.sort();
    m_recommendedSystematicsPhotons.unique();
    m_specifiedSystematicsPhotons.sort();
    m_specifiedSystematicsPhotons.unique();
  }

  void EgammaObjectCollectionMaker::specifiedSystematicsElectrons(const std::set<std::string>& specifiedSystematics) {
    ///-- Get the recommended systematics from the tool, in std::vector format --///
    const std::vector<CP::SystematicSet> systList = CP::make_systematics_vector(
      m_calibrationTool->recommendedSystematics());

    for (auto s : systList) {
      
      if(!m_config->getTreeFilter()->filterTree(s.name())) continue; // Applying tree filter
      m_recommendedSystematicsElectrons.push_back(s);
      if (s.name() == "") {
        m_specifiedSystematicsElectrons.push_back(s);
      }

      ///-- MC only --///
      if (m_config->isMC()) {
        ///-- Are we only doing Nominal? Did the user specify specific systematics to use? --///
        if (!m_config->isSystNominal(m_config->systematics())) {
          if (specifiedSystematics.size() == 0) {
            m_specifiedSystematicsElectrons.push_back(s);
          }
          if (specifiedSystematics.size() > 0) {
            for (auto i : specifiedSystematics) {
              TreeFilter filter(i);
              if (!filter.filterTree(s.name())) {
                m_specifiedSystematicsElectrons.push_back(s);
              }
            }
          }
        }
      }
    }
    m_recommendedSystematicsElectrons.sort();
    m_recommendedSystematicsElectrons.unique();
    m_specifiedSystematicsElectrons.sort();
    m_specifiedSystematicsElectrons.unique();
  }

  void EgammaObjectCollectionMaker::specifiedSystematicsFwdElectrons(const std::set<std::string>& specifiedSystematics)
  {
    ///-- Get the recommended systematics from the tool, in std::vector format --///
    const std::vector<CP::SystematicSet> systList = CP::make_systematics_vector(
      m_calibrationTool->recommendedSystematics());

    for (auto s : systList) {
      
      if(!m_config->getTreeFilter()->filterTree(s.name())) continue; // Applying tree filter
      m_recommendedSystematicsFwdElectrons.push_back(s);
      if (s.name() == "") {
        m_specifiedSystematicsFwdElectrons.push_back(s);
      }

      ///-- MC only --///
      if (m_config->isMC()) {
        ///-- Are we only doing Nominal? Did the user specify specific systematics to use? --///
        if (!m_config->isSystNominal(m_config->systematics())) {
          if (specifiedSystematics.size() == 0) {
            m_specifiedSystematicsFwdElectrons.push_back(s);
          }
          if (specifiedSystematics.size() > 0) {
            for (auto i : specifiedSystematics) {
              TreeFilter filter(i);
              if (!filter.filterTree(s.name())) {
                m_specifiedSystematicsFwdElectrons.push_back(s);
              }
            }
          }
        }
      }
    }
    m_recommendedSystematicsFwdElectrons.sort();
    m_recommendedSystematicsFwdElectrons.unique();
    m_specifiedSystematicsFwdElectrons.sort();
    m_specifiedSystematicsFwdElectrons.unique();
  }
}

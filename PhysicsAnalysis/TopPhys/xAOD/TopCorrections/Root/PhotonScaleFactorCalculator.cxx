/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

// $Id: PhotonScaleFactorCalculator.cxx 802226 2017-04-04 16:13:10Z grancagn $
#include "TopCorrections/PhotonScaleFactorCalculator.h"

#include <string>

#include "TopConfiguration/TopConfig.h"
#include "TopEvent/EventTools.h"

#include "xAODEgamma/PhotonContainer.h"

namespace top {
  PhotonScaleFactorCalculator::PhotonScaleFactorCalculator(const std::string& name) :
    asg::AsgTool(name),
    m_config(nullptr),
    m_systNominal(CP::SystematicSet()),
    m_systEffIDUp("PH_EFF_ID_Uncertainty__1up"),
    m_systEffIDDown("PH_EFF_ID_Uncertainty__1down"),
    m_systEffIsoUp("PH_EFF_ISO_Uncertainty__1up"),
    m_systEffIsoDown("PH_EFF_ISO_Uncertainty__1down"),
    m_photonEffSF("AsgPhotonEfficiencyCorrectionTool"),
    m_photonIsoSF(),
    m_photonLooseIsoSF(),
    m_photonIsoSF_exists(false),
    m_photonLooseIsoSF_exists(false),
    m_decor_isoSF("SetMe"),
    m_decor_isoSF_loose("SetMe") {
    declareProperty("config", m_config);
  }

  StatusCode PhotonScaleFactorCalculator::initialize() {
    ATH_MSG_INFO(" top::PhotonScaleFactorCalculator initialize");
    top::check(m_photonEffSF.retrieve(),
               "Failed to retrieve photon efficiency SF calculator");
    std::ostream& msgInfo = msg(MSG::Level::INFO);
    msgInfo << "------>Systematics:\n";
    for (auto sys:m_photonEffSF->recommendedSystematics())
      msgInfo << "---> " << sys << "\n";

    // remove "FixedCut" if present
    std::string s = "FixedCut";
    std::string isoName = m_config->photonIsolation();
    m_decor_isoSF = "PH_SF_Iso_" + isoName;
    std::string::size_type i = isoName.find(s);
    if (i != std::string::npos) isoName.erase(i, s.length());
    std::string photonIsoSFName = "AsgPhotonEfficiencyCorrectionTool_IsoSF" + isoName;
    m_photonIsoSF.setName(photonIsoSFName);
    if (asg::ToolStore::contains<IAsgPhotonEfficiencyCorrectionTool>(photonIsoSFName)) {
      m_photonIsoSF_exists = true;
      top::check(m_photonIsoSF.retrieve(),
                 "Failed to retrieve photon isolation efficiency SF calculator");
      msgInfo << "------>Systematics:\n";
      for (auto sys:m_photonIsoSF->recommendedSystematics())
        msgInfo << "---> " << sys << "\n";
    }

    std::string isoNameLoose = m_config->photonIsolationLoose();
    m_decor_isoSF_loose = "PH_LOOSE_SF_Iso_" + isoNameLoose;
    i = isoNameLoose.find(s);
    if (i != std::string::npos) isoNameLoose.erase(i, s.length());
    std::string photonLooseIsoSFName = "AsgPhotonEfficiencyCorrectionTool_IsoSF" + isoNameLoose;
    m_photonLooseIsoSF.setName(photonLooseIsoSFName);
    if (asg::ToolStore::contains<IAsgPhotonEfficiencyCorrectionTool>(photonLooseIsoSFName)) {
      m_photonLooseIsoSF_exists = true;
      top::check(m_photonLooseIsoSF.retrieve(),
                 "Failed to retrieve loose photon isolation efficiency SF calculator");
    }
    return StatusCode::SUCCESS;
  }

  StatusCode PhotonScaleFactorCalculator::execute() {
    // Loop over all photon collections
    for (auto currentSystematic : *m_config->systSgKeyMapPhotons()) {
      const xAOD::PhotonContainer* photons(nullptr);

      top::check(evtStore()->retrieve(photons, currentSystematic.second),
                 "Failed to retrieve photons");

      // Loop over all photons in each collection
      for (auto photonPtr : *photons) {
        // Does the photon pass object selection?
        bool passSelection(false);
        if (photonPtr->isAvailable<char>("passPreORSelection")) {
          if (photonPtr->auxdataConst<char>("passPreORSelection") == 1) {
            passSelection = true;
          }
        }
        if (photonPtr->isAvailable<char>("passPreORSelectionLoose")) {
          if (photonPtr->auxdataConst<char>("passPreORSelectionLoose") == 1) {
            passSelection = true;
          }
        }

        if (!passSelection) continue;

        top::check(m_photonEffSF->applySystematicVariation(m_systNominal),
                   "Failed to set photon efficiency SF tool to nominal");

        double effSF(1.);
        top::check(m_photonEffSF->getEfficiencyScaleFactor(*photonPtr, effSF),
                   "Failed to get nominal photon SF");

        static SG::AuxElement::Decorator<float> ph_effID_dec("EFF_ID_SF");
        ph_effID_dec(*photonPtr) = effSF;

        double isoSF(1.), isoLooseSF(1.);
        if (m_photonIsoSF_exists) {
          top::check(m_photonIsoSF->applySystematicVariation(m_systNominal),
                     "Failed to set photon efficiency SF tool to nominal");
          top::check(m_photonIsoSF->getEfficiencyScaleFactor(*photonPtr, isoSF),
                     "Failed to get nominal photon SF");
          static SG::AuxElement::Decorator<float> ph_isoID_dec(m_decor_isoSF);
          ph_isoID_dec(*photonPtr) = isoSF;
        }
        if (m_photonLooseIsoSF_exists) {
          top::check(m_photonLooseIsoSF->applySystematicVariation(m_systNominal),
                     "Failed to set photon efficiency SF tool to nominal");
          top::check(m_photonLooseIsoSF->getEfficiencyScaleFactor(*photonPtr, isoLooseSF),
                     "Failed to get nominal photon SF");
          static SG::AuxElement::Decorator<float> ph_isoID_dec(m_decor_isoSF_loose);
          ph_isoID_dec(*photonPtr) = isoLooseSF;
        }

        // For nominal calibration, vary the SF systematics
        if (currentSystematic.first != m_config->nominalHashValue()) continue;

        double effSF_up(1.), effSF_down(1.);
        top::check(m_photonEffSF->applySystematicVariation(m_systEffIDUp),
                   "Failed to set photon efficiency SF tool to"
                   " ID up systematic");
        top::check(m_photonEffSF->getEfficiencyScaleFactor(*photonPtr, effSF_up),
                   "Failed to get photon efficiency SF:"
                   " systematic up ID SF");

        top::check(m_photonEffSF->applySystematicVariation(m_systEffIDDown),
                   "Failed to set photon efficiency SF tool to"
                   " ID up systematic");
        top::check(m_photonEffSF->getEfficiencyScaleFactor(*photonPtr, effSF_down),
                   "Failed to get photon efficiency SF:"
                   " systematic down ID SF");

        static SG::AuxElement::Decorator<float> ph_effIDUp_dec("EFF_ID_SF_UP");
        static SG::AuxElement::Decorator<float> ph_effIDDown_dec("EFF_ID_SF_DOWN");
        ph_effIDUp_dec(*photonPtr) = effSF_up;
        ph_effIDDown_dec(*photonPtr) = effSF_down;

        // isolation systematic uncertainties
        double effIsoSF_up(1.), effIsoSF_down(1.);
        double effLooseIsoSF_up(1.), effLooseIsoSF_down(1.);
        if (m_photonIsoSF_exists) {
          top::check(m_photonIsoSF->applySystematicVariation(m_systEffIsoUp),
                     "Failed to set photon efficiency SF tool to"
                     " isolation up systematic");
          top::check(m_photonIsoSF->getEfficiencyScaleFactor(*photonPtr, effIsoSF_up),
                     "Failed to get photon efficiency SF:"
                     " up systematic isolation");
          top::check(m_photonIsoSF->applySystematicVariation(m_systEffIsoDown),
                     "Failed to set photon efficiency SF tool to"
                     " isolation down systematic");
          top::check(m_photonIsoSF->getEfficiencyScaleFactor(*photonPtr, effIsoSF_down),
                     "Failed to get photon efficiency SF:"
                     " down systematic isolation");
        }
        if (m_photonLooseIsoSF_exists) {
          top::check(m_photonLooseIsoSF->applySystematicVariation(m_systEffIsoUp),
                     "Failed to set photon efficiency SF tool to"
                     " isolation up systematic");
          top::check(m_photonLooseIsoSF->getEfficiencyScaleFactor(*photonPtr, effLooseIsoSF_up),
                     "Failed to get photon efficiency SF:"
                     " up systematic isolation");
          top::check(m_photonLooseIsoSF->applySystematicVariation(m_systEffIsoDown),
                     "Failed to set photon efficiency SF tool to"
                     " isolation down systematic");
          top::check(m_photonLooseIsoSF->getEfficiencyScaleFactor(*photonPtr, effLooseIsoSF_down),
                     "Failed to get photon efficiency SF:"
                     " down systematic isolation");
        }

        static SG::AuxElement::Decorator<float> ph_effIsoUp_dec(m_decor_isoSF + "_UP");
        static SG::AuxElement::Decorator<float> ph_effIsoDown_dec(m_decor_isoSF + "_DOWN");
        static SG::AuxElement::Decorator<float> ph_effLooseIsoUp_dec(m_decor_isoSF_loose + "_UP");
        static SG::AuxElement::Decorator<float> ph_effLooseIsoDown_dec(m_decor_isoSF_loose + "_DOWN");

        ph_effIsoUp_dec(*photonPtr) = effIsoSF_up;
        ph_effIsoDown_dec(*photonPtr) = effIsoSF_down;
        ph_effLooseIsoUp_dec(*photonPtr) = effLooseIsoSF_up;
        ph_effLooseIsoDown_dec(*photonPtr) = effLooseIsoSF_down;
      }
    }
    return StatusCode::SUCCESS;
  }
}  // namespace top

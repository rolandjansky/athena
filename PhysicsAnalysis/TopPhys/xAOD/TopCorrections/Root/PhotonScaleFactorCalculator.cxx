/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  m_systEffLowPtIsoUp("PH_EFF_LOWPTISO_Uncertainty__1up"),
  m_systEffLowPtIsoDown("PH_EFF_LOWPTISO_Uncertainty__1down"),
  m_systEffTrkIsoUp("PH_EFF_TRKISO_Uncertainty__1up"),
  m_systEffTrkIsoDown("PH_EFF_TRKISO_Uncertainty__1down"),
  m_photonEffSF("AsgPhotonEfficiencyCorrectionTool"),
  m_photonIsoSF(),
  m_photonLooseIsoSF(),
  m_photonIsoSF_exists(false),
  m_photonLooseIsoSF_exists(false),
  m_decor_isoSF("SetMe"),
  m_decor_isoSF_loose("SetMe")
 {
  declareProperty("config" , m_config);
}

  StatusCode PhotonScaleFactorCalculator::initialize() {
    ATH_MSG_INFO(" top::PhotonScaleFactorCalculator initialize");
    top::check(m_photonEffSF.retrieve(),
               "Failed to retrieve photon efficiency SF calculator");
    std::cout<<"------>Systematics:"<<std::endl;
    for (auto sys:m_photonEffSF->recommendedSystematics())
      std::cout<<"---> "<<sys<<std::endl;

    // remove "FixedCut" if present
    std::string s = "FixedCut";
    std::string isoName = m_config->photonIsolation();
    m_decor_isoSF = "PH_SF_Iso_" + isoName;
    std::string::size_type i = isoName.find(s);
    if (i != std::string::npos)
      isoName.erase(i, s.length());
    std::string photonIsoSFName = "AsgPhotonEfficiencyCorrectionTool_IsoSF" + isoName;
    m_photonIsoSF.setName(photonIsoSFName);
    if (asg::ToolStore::contains<IAsgPhotonEfficiencyCorrectionTool>(photonIsoSFName)) {
      m_photonIsoSF_exists = true;
      top::check(m_photonIsoSF.retrieve(),
		 "Failed to retrieve photon isolation efficiency SF calculator");
      std::cout<<"------>Systematics:"<<std::endl;
      for (auto sys:m_photonIsoSF->recommendedSystematics())
	std::cout<<"---> "<<sys<<std::endl;
    }

    std::string isoNameLoose = m_config->photonIsolationLoose();
    m_decor_isoSF_loose = "PH_SF_Iso_" + isoNameLoose;
    i = isoNameLoose.find(s);
    if (i != std::string::npos)
      isoNameLoose.erase(i, s.length());
    std::string photonLooseIsoSFName = "AsgPhotonEfficiencyCorrectionTool_IsoSF" + isoNameLoose;
    m_photonLooseIsoSF.setName(photonLooseIsoSFName);
    if (asg::ToolStore::contains<IAsgPhotonEfficiencyCorrectionTool>(photonLooseIsoSFName)) {
      m_photonLooseIsoSF_exists = true;
      top::check(m_photonLooseIsoSF.retrieve(),
		 "Failed to retrieve loose photon isolation efficiency SF calculator");
      //std::cout<<"------>Systematics:"<<std::endl;
      //for (auto sys:m_photonIsoSF->recommendedSystematics())
      //std::cout<<"---> "<<sys<<std::endl;
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
      if (currentSystematic.first != m_config->nominalHashValue())
        continue;

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
      double effLowPtIsoSF_up(1.), effTrkIsoSF_up(1.);
      double effLowPtIsoSF_down(1.), effTrkIsoSF_down(1.);
      double effLowPtLooseIsoSF_up(1.), effTrkLooseIsoSF_up(1.);
      double effLowPtLooseIsoSF_down(1.), effTrkLooseIsoSF_down(1.);
      if (m_photonIsoSF_exists) {
	top::check(m_photonIsoSF->applySystematicVariation(m_systEffLowPtIsoUp),
		   "Failed to set photon efficiency SF tool to"
		   " Radiative Z (low ET) up systematic ");
	top::check(m_photonIsoSF->getEfficiencyScaleFactor(*photonPtr, effLowPtIsoSF_up),
		   "Failed to get photon efficiency SF:"
		   " up systematic Radiative Z (low ET)");
	top::check(m_photonIsoSF->applySystematicVariation(m_systEffLowPtIsoDown),
		   "Failed to set photon efficiency SF tool to"
		   " Radiative Z (low ET) down systematic");
	top::check(m_photonIsoSF->getEfficiencyScaleFactor(*photonPtr, effLowPtIsoSF_down),
		   "Failed to get photon efficiency SF:"
		   " down systematic Radiative Z (low ET)");

	top::check(m_photonIsoSF->applySystematicVariation(m_systEffTrkIsoUp),
		   "Failed to set photon efficiency SF tool to"
		   " Track isolation (ptcone, intermediate and high ET) up systematic");
	top::check(m_photonIsoSF->getEfficiencyScaleFactor(*photonPtr, effTrkIsoSF_up),
		   "Failed to get photon efficiency SF:"
		   " up systematic Track isolation (ptcone, intermediate and high ET)");
	top::check(m_photonIsoSF->applySystematicVariation(m_systEffTrkIsoDown),
		   "Failed to set photon efficiency SF tool to"
		   " Track isolation (ptcone, intermediate and high ET) down systematic");
	top::check(m_photonIsoSF->getEfficiencyScaleFactor(*photonPtr, effTrkIsoSF_down),
		   "Failed to get photon efficiency SF:"
		   " down systematic Track isolation (ptcone, intermediate and high ET)");
      }
      if (m_photonLooseIsoSF_exists) {
	top::check(m_photonLooseIsoSF->applySystematicVariation(m_systEffLowPtIsoUp),
		   "Failed to set photon efficiency SF tool to"
		   " Radiative Z (low ET) up systematic ");
	top::check(m_photonLooseIsoSF->getEfficiencyScaleFactor(*photonPtr, effLowPtLooseIsoSF_up),
		   "Failed to get photon efficiency SF:"
		   " up systematic Radiative Z (low ET)");
	top::check(m_photonLooseIsoSF->applySystematicVariation(m_systEffLowPtIsoDown),
		   "Failed to set photon efficiency SF tool to"
		   " Radiative Z (low ET) down systematic");
	top::check(m_photonLooseIsoSF->getEfficiencyScaleFactor(*photonPtr, effLowPtLooseIsoSF_down),
		   "Failed to get photon efficiency SF:"
		   " down systematic Radiative Z (low ET)");

	top::check(m_photonLooseIsoSF->applySystematicVariation(m_systEffTrkIsoUp),
		   "Failed to set photon efficiency SF tool to"
		   " Track isolation (ptcone, intermediate and high ET) up systematic");
	top::check(m_photonLooseIsoSF->getEfficiencyScaleFactor(*photonPtr, effTrkLooseIsoSF_up),
		   "Failed to get photon efficiency SF:"
		   " up systematic Track isolation (ptcone, intermediate and high ET)");
	top::check(m_photonLooseIsoSF->applySystematicVariation(m_systEffTrkIsoDown),
		   "Failed to set photon efficiency SF tool to"
		   " Track isolation (ptcone, intermediate and high ET) down systematic");
	top::check(m_photonLooseIsoSF->getEfficiencyScaleFactor(*photonPtr, effTrkLooseIsoSF_down),
		   "Failed to get photon efficiency SF:"
		   " down systematic Track isolation (ptcone, intermediate and high ET)");
      }

      static SG::AuxElement::Decorator<float> ph_effLowPtIsoUp_dec(m_decor_isoSF+"_LOWPT_UP");
      static SG::AuxElement::Decorator<float> ph_effLowPtIsoDown_dec(m_decor_isoSF+"_LOWPT_DOWN");
      static SG::AuxElement::Decorator<float> ph_effTrkIsoUp_dec(m_decor_isoSF+"_TRK_UP");
      static SG::AuxElement::Decorator<float> ph_effTrkIsoDown_dec(m_decor_isoSF+"_TRK_DOWN");
      static SG::AuxElement::Decorator<float> ph_effLowPtLooseIsoUp_dec(m_decor_isoSF_loose+"_LOWPT_UP");
      static SG::AuxElement::Decorator<float> ph_effLowPtLooseIsoDown_dec(m_decor_isoSF_loose+"_LOWPT_DOWN");
      static SG::AuxElement::Decorator<float> ph_effTrkLooseIsoUp_dec(m_decor_isoSF_loose+"_TRK_UP");
      static SG::AuxElement::Decorator<float> ph_effTrkLooseIsoDown_dec(m_decor_isoSF_loose+"_TRK_DOWN");
      ph_effLowPtIsoUp_dec(*photonPtr) = effLowPtIsoSF_up;
      ph_effLowPtIsoDown_dec(*photonPtr) = effLowPtIsoSF_down;
      ph_effTrkIsoUp_dec(*photonPtr) = effTrkIsoSF_up;
      ph_effTrkIsoDown_dec(*photonPtr) = effTrkIsoSF_down;
      ph_effLowPtLooseIsoUp_dec(*photonPtr) = effLowPtLooseIsoSF_up;
      ph_effLowPtLooseIsoDown_dec(*photonPtr) = effLowPtLooseIsoSF_down;
      ph_effTrkLooseIsoUp_dec(*photonPtr) = effTrkLooseIsoSF_up;
      ph_effTrkLooseIsoDown_dec(*photonPtr) = effTrkLooseIsoSF_down;
    }
  }
  return StatusCode::SUCCESS;
}
}  // namespace top

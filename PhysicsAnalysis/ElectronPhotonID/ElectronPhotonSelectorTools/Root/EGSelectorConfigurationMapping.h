/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __EGSelectorConfigurationMapping__
#define __EGSelectorConfigurationMapping__
////////////////////////////////////////////////////////////////
#include "ElectronPhotonSelectorTools/egammaPIDdefs.h"
#include <map>
#include <string>

namespace EgammaSelectors {
// These should be the current best-supported ID menus. Typically picked up by
// analyzers using the "WorkingPoint" option.
// Note: keep this conf file up to date with the PhotonIsEMTightSelectorConfig
// function in python/PhotonIsEMTightSelectorCutDefs.py

// This is the internal part , We need to  map string to latest reccomendations
const std::map<std::string, std::string> LHPointToConfFile = {
  { "VeryLooseLHElectron",
    "ElectronPhotonSelectorTools/offline/mc20_20220620/"
    "ElectronLikelihoodVeryLooseOfflineConfig2018_Smooth.conf" },
  { "LooseLHElectron",
    "ElectronPhotonSelectorTools/offline/mc20_20220620/"
    "ElectronLikelihoodLooseOfflineConfig2018_Smooth.conf" },
  { "LooseBLLHElectron",
    "ElectronPhotonSelectorTools/offline/mc20_20220620/"
    "ElectronLikelihoodLooseOfflineConfig2018_CutBL_Smooth.conf" },
  { "MediumLHElectron",
    "ElectronPhotonSelectorTools/offline/mc20_20220620/"
    "ElectronLikelihoodMediumOfflineConfig2018_Smooth.conf" },
  { "TightLHElectron",
    "ElectronPhotonSelectorTools/offline/mc20_20220620/"
    "ElectronLikelihoodTightOfflineConfig2018_Smooth.conf" },
};
const std::map<std::string, std::string> PhotonCutPointToConfFile = {
  { "LoosePhoton",
    "ElectronPhotonSelectorTools/offline/mc15_20150712/"
    "PhotonIsEMLooseSelectorCutDefs.conf" },
  { "MediumPhoton",
    "ElectronPhotonSelectorTools/offline/mc15_20160512/"
    "PhotonIsEMMediumSelectorCutDefs.conf" },
  { "TightPhoton",
    "ElectronPhotonSelectorTools/offline/20180825/"
    "PhotonIsEMTightSelectorCutDefs.conf" },
};
const std::map<std::string, std::string> ForwardLHPointToConfFile = {
  { "LooseLHForwardElectron",
    "ElectronPhotonSelectorTools/offline/mc16_20180822/FwdLHLooseConf.conf" },
  { "MediumLHForwardElectron",
    "ElectronPhotonSelectorTools/offline/mc16_20180822/FwdLHMediumConf.conf" },
  { "TightLHForwardElectron",
    "ElectronPhotonSelectorTools/offline/mc16_20180822/FwdLHTightConf.conf" },
};
const std::map<std::string, std::string> ForwardElectronCutPointToConfFile = {
  { "LooseForwardElectron",
    "ElectronPhotonSelectorTools/offline/mc15_20150812/"
    "ForwardElectronIsEMLooseSelectorCutDefs.conf" },
  { "MediumForwardElectron",
    "ElectronPhotonSelectorTools/offline/mc15_20150812/"
    "ForwardElectronIsEMMediumSelectorCutDefs.conf" },
  { "TightForwardElectron",
    "ElectronPhotonSelectorTools/offline/mc15_20150812/"
    "ForwardElectronIsEMTightSelectorCutDefs.conf" },
};
const std::map<std::string, std::string> ElectronCutPointToConfFile = {
  { "LooseElectron",
    "ElectronPhotonSelectorTools/offline/mc15_20150712/"
    "ElectronIsEMLooseSelectorCutDefs.conf" },
  { "MediumElectron",
    "ElectronPhotonSelectorTools/offline/mc15_20150712/"
    "ElectronIsEMMediumSelectorCutDefs.conf" },
  { "TightElectron",
    "ElectronPhotonSelectorTools/offline/mc15_20150712/"
    "ElectronIsEMTightSelectorCutDefs.conf" }
};
//----------------------------------------------------------
// Map enums to masks , needed for photons
const std::map<std::string, unsigned int> PhotonCutPointToMask = {
  { "LoosePhoton", egammaPID::PhotonLoose },
  { "MediumPhoton", egammaPID::PhotonMedium },
  { "TightPhoton", egammaPID::PhotonTight },
};
const std::map<std::string, std::string> ElectronDNNPointToConfFile = {
  { "LooseDNNElectron",
    "ElectronPhotonSelectorTools/offline/mc16_20210430/"
    "ElectronDNNMulticlassLoose.conf"},
  { "MediumDNNElectron",
    "ElectronPhotonSelectorTools/offline/mc16_20210430/"
    "ElectronDNNMulticlassMedium.conf"},
  { "TightDNNElectron",
    "ElectronPhotonSelectorTools/offline/mc16_20210430/"
    "ElectronDNNMulticlassTight.conf"}
};
}
////////////////////////////////////////////
#endif

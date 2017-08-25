/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __EGSelectorConfigurationMapping__
#define __EGSelectorConfigurationMapping__
////////////////////////////////////////////////////////////////
#include <map>
#include <string>
#include "EgammaAnalysisInterfaces/egammaPIDdefs.h"

namespace EgammaSelectors {
  //----------------------------------------------------------
  //This is the internal part , We need to  map string to latest reccomendations
  const std::map<std::string,std::string> LHPointToConfFile={
    {"VeryLooseLHElectron","ElectronPhotonSelectorTools/offline/mc15_20160512/ElectronLikelihoodVeryLooseOfflineConfig2016_Smooth.conf"},
    {"LooseLHElectron","ElectronPhotonSelectorTools/offline/mc15_20160512/ElectronLikelihoodLooseOfflineConfig2016_Smooth.conf"},
    {"LooseBLLHElectron","ElectronPhotonSelectorTools/offline/mc15_20160512/ElectronLikelihoodLooseOfflineConfig2016_CutBL_Smooth.conf"},
    {"MediumLHElectron","ElectronPhotonSelectorTools/offline/mc15_20160512/ElectronLikelihoodMediumOfflineConfig2016_Smooth.conf"},
    {"TightLHElectron","ElectronPhotonSelectorTools/offline/mc15_20160512/ElectronLikelihoodTightOfflineConfig2016_Smooth.conf"}
  };
  const std::map<std::string,std::string> PhotonCutPointToConfFile={
    {"LoosePhoton","ElectronPhotonSelectorTools/offline/mc15_20150712/PhotonIsEMLooseSelectorCutDefs.conf"},
    {"MediumPhoton","ElectronPhotonSelectorTools/offline/mc15_20150712/PhotonIsEMMediumSelectorCutDefs.conf"},
    {"TightPhoton","ElectronPhotonSelectorTools/offline/mc15_20150712/PhotonIsEMTightSelectorCutDefs.conf"},
  };
  const std::map<std::string,std::string> ForwardElectronCutPointToConfFile={
    {"LooseForwardElectron","ElectronPhotonSelectorTools/offline/mc15_20150812/ForwardElectronIsEMLooseSelectorCutDefs.conf"},
    {"MediumForwardElectron","ElectronPhotonSelectorTools/offline/mc15_20150812/ForwardElectronIsEMMediumSelectorCutDefs.conf"},
    {"TightForwardElectron","ElectronPhotonSelectorTools/offline/mc15_20150812/ForwardElectronIsEMTightSelectorCutDefs.conf"},
  };
  const std::map<std::string,std::string> ElectronCutPointToConfFile={
    {"LooseElectron","ElectronPhotonSelectorTools/offline/mc15_20150712/ElectronIsEMLooseSelectorCutDefs.conf"},
    {"MediumElectron","ElectronPhotonSelectorTools/offline/mc15_20150712/ElectronIsEMMediumSelectorCutDefs.conf"},
    {"TightElectron","ElectronPhotonSelectorTools/offline/mc15_20150712/ElectronIsEMTightSelectorCutDefs.conf"}
  };
  //----------------------------------------------------------
  //Map enums to masks , needed for photons
  const std::map<std::string,unsigned int> PhotonCutPointToMask={{"LoosePhoton",egammaPID::PhotonLoose},
                                                                 {"MediumPhoton",egammaPID::PhotonMedium},
                                                                 {"TightPhoton",egammaPID::PhotonTight},
  };
}
////////////////////////////////////////////
#endif

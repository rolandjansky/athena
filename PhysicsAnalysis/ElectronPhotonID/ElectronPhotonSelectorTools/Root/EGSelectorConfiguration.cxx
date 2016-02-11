/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ElectronPhotonSelectorTools/EGSelectorConfiguration.h"
#include "ElectronPhotonSelectorTools/egammaPIDdefs.h"
#include "ElectronPhotonSelectorTools/AsgPhotonIsEMSelector.h"
#include "ElectronPhotonSelectorTools/AsgForwardElectronIsEMSelector.h"
#include "ElectronPhotonSelectorTools/AsgElectronIsEMSelector.h"
#include "ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h"

/// Helper macro for ensuring consistent enum <==> name
#define DEFINE_PAIR( NAME) {EgammaSelectors::NAME, #NAME}	
/// def file with the mappings
#include "EGSelectorConfigurationMapping.def"

EGSelectorConfiguration::EGSelectorConfiguration(std::string name):
  asg::AsgTool(name){
}
  
StatusCode EGSelectorConfiguration::initialize(){
  ATH_MSG_INFO("")  ;
  ATH_MSG_INFO("Supported ID Menu names") ; 
  printMap(m_LHPointToName);
  printMap(m_PhotonCutPointToName);
  printMap(m_ForwardElectronCutPointToName);
  printMap(m_ElectronCutPointToName);
  ATH_MSG_INFO("")  ;
  return StatusCode::SUCCESS;
}

StatusCode EGSelectorConfiguration::finalize(){
  return StatusCode::SUCCESS;
}
  
std::unique_ptr<AsgElectronLikelihoodTool> 
EGSelectorConfiguration::configuredTool(const EgammaSelectors::LHidentificationPoint input) const{  
  std::unique_ptr<AsgElectronLikelihoodTool> tool = 
    configureImpl<AsgElectronLikelihoodTool>(input,m_LHPointToName,m_LHPointToConfFile,m_LHPointToMask);
  return tool;
}

std::unique_ptr<AsgPhotonIsEMSelector> 
EGSelectorConfiguration::configuredTool(const EgammaSelectors::PhotonCutidentificationPoint input) const{
  std::unique_ptr<AsgPhotonIsEMSelector> tool = 
    configureImpl<AsgPhotonIsEMSelector>(input,m_PhotonCutPointToName,m_PhotonCutPointToConfFile,m_PhotonCutPointToMask);
  return tool;
}

std::unique_ptr<AsgForwardElectronIsEMSelector> 
EGSelectorConfiguration::configuredTool(const EgammaSelectors::ForwardElectronCutidentificationPoint input) const {
  std::unique_ptr<AsgForwardElectronIsEMSelector> tool = 
    configureImpl<AsgForwardElectronIsEMSelector>(input,m_ForwardElectronCutPointToName,m_ForwardElectronCutPointToConfFile,m_ForwardElectronCutPointToMask);
  return tool;
} 

std::unique_ptr<AsgElectronIsEMSelector> 
EGSelectorConfiguration::configuredTool(const EgammaSelectors::ElectronCutidentificationPoint input) const{
  std::unique_ptr<AsgElectronIsEMSelector> tool = 
    configureImpl<AsgElectronIsEMSelector>(input,m_ElectronCutPointToName,m_ElectronCutPointToConfFile,m_ElectronCutPointToMask);
  return tool;
}



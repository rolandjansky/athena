/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef __EGSelectorConfiguration__
#define __EGSelectorConfiguration__

/**
   @class EGSelectronConfiguration
   @brief Configuration helper for egamma selectors
*/

#include <map>
#include <string>

#include "AsgTools/AsgTool.h"
#include "CxxUtils/make_unique.h"
#include "ElectronPhotonSelectorTools/IEGSelectorConfiguration.h"

class EGSelectorConfiguration : public asg::AsgTool,
				virtual public IEGSelectorConfiguration
{
  ASG_TOOL_CLASS2(EGSelectorConfiguration,IEGSelectorConfiguration, asg::IAsgTool)
  
  public: 
  /** Standard constructor */
  EGSelectorConfiguration(std::string name);
  
  /** Standard destructor */
  virtual ~EGSelectorConfiguration(){
  }
  
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  /** Recommended interface uses smart pointer */
  virtual std::unique_ptr<AsgElectronLikelihoodTool> configuredTool(const EgammaSelectors::LHidentificationPoint input)  const ;
  virtual std::unique_ptr<AsgPhotonIsEMSelector> configuredTool(const EgammaSelectors::PhotonCutidentificationPoint input)  const ;
  virtual std::unique_ptr<AsgForwardElectronIsEMSelector> configuredTool(const EgammaSelectors::ForwardElectronCutidentificationPoint input)  const ;
  virtual std::unique_ptr<AsgElectronIsEMSelector> configuredTool(const EgammaSelectors::ElectronCutidentificationPoint input)  const ;

  /** interface for naked pointer, as some user prefer to manage the memory themselves */
  virtual AsgElectronLikelihoodTool* configuredToolPtr(const EgammaSelectors::LHidentificationPoint input)  const {
    std::unique_ptr<AsgElectronLikelihoodTool> un_ptr = configuredTool(input); 
    return un_ptr.release();
  }
  virtual AsgPhotonIsEMSelector* configuredToolPtr(const EgammaSelectors::PhotonCutidentificationPoint input)  const {
    std::unique_ptr<AsgPhotonIsEMSelector> un_ptr = configuredTool(input); 
    return un_ptr.release();
  }
  virtual AsgForwardElectronIsEMSelector* configuredaToolPtr(const EgammaSelectors::ForwardElectronCutidentificationPoint input)  const {
    std::unique_ptr<AsgForwardElectronIsEMSelector> un_ptr = configuredTool(input); 
    return un_ptr.release();
  }
  virtual AsgElectronIsEMSelector* configuredToolPtr(const EgammaSelectors::ElectronCutidentificationPoint input)  const {
    std::unique_ptr<AsgElectronIsEMSelector> un_ptr = configuredTool(input); 
    return un_ptr.release();
  }

  private:

  template <typename T, typename U>
  std::unique_ptr<T> configureImpl( const U input,
				    const std::map<U,std::string>& namemap,
				    const std::map<U,std::string>& configmap,
				    const std::map<U,unsigned int>& maskmap)const{

    //Find the name
    auto name_itr=namemap.find(input);
    if(name_itr == namemap.end()){
      ATH_MSG_ERROR("could not find Name for the provided point");
      return 0;
    }
    std::string name = name_itr->second;    
    std::unique_ptr<T> tool=CxxUtils::make_unique<T> (name);

    //Find the conf file
    auto confFile_itr=configmap.find(input);
    if(confFile_itr == configmap.end()){
      ATH_MSG_ERROR("could not find config file for the provided point");
      return 0;
    }
    const std::string confFile =confFile_itr->second;
    if(!tool->setProperty("ConfigFile", confFile)){
      ATH_MSG_ERROR("could not set ConfigFile to: " << confFile);
      return 0;
    }
    
    //In case we need also the masks
    if(maskmap.size()!=0){ 
      auto mask_itr=maskmap.find(input);
      if(mask_itr==maskmap.end()){
	ATH_MSG_ERROR("could not find  egamma pid mask for the provided point");
	return 0;
      }
      const unsigned int mask= static_cast < unsigned int> (mask_itr->second);  
      if(!tool->setProperty("isEMMask",mask )){
	ATH_MSG_ERROR("could not set isEMMask to: " << mask);
	return 0;
      }
    }
    //initialize the tool
    if (!tool->initialize()){
      ATH_MSG_ERROR("could not initialize tool with name: " << name);
      return 0;
    }
    
    ATH_MSG_INFO("------------------------------------")  ;
    ATH_MSG_INFO("Initialized: " << name );
    ATH_MSG_INFO("with ConfigFile: " << *(tool->template getProperty<std::string>("ConfigFile")));
    ATH_MSG_INFO("with operating point: " << tool->getOperatingPointName());
    ATH_MSG_INFO("------------------------------------")  ;
    return tool;
  }

  template<typename T> void printMap (const T& map){
    ATH_MSG_INFO("------------------------------------")  ;
    for (auto const& i : map){
      ATH_MSG_INFO(i.second);
    }
    ATH_MSG_INFO("------------------------------------")  ;
  }
  
}; 


#endif


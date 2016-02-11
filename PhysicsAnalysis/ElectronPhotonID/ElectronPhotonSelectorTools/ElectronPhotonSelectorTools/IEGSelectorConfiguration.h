/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef __IEGSelectorConfiguration__
#define __IEGSelectorConfiguration__

/**
   @class IEGSelectronConfiguration
   @brief Configuration helper for egamma selectors
*/
#include <memory>
#include "AsgTools/IAsgTool.h"
class AsgPhotonIsEMSelector;
class AsgForwardElectronIsEMSelector;
class AsgElectronIsEMSelector;
class AsgElectronLikelihoodTool;

namespace EgammaSelectors {

  enum LHidentificationPoint{
    VeryLooseLHElectron = 0,
    LooseLHElectron = 1,
    LooseBLLHElectron = 2,
    MediumLHElectron = 3,
    TightLHElectron = 4
  };
  
  enum PhotonCutidentificationPoint{
    LoosePhoton = 0,
    MediumPhoton = 1,
    TightPhoton = 2
  };

  enum ForwardElectronCutidentificationPoint{
    LooseForwardElectron = 0,
    MediumForwardElectron = 1,
    TightForwardElectron = 2
  };

  enum ElectronCutidentificationPoint{
    LooseElectron = 0,
    MediumElectron = 1,
    TightElectron = 2
  };
}

class IEGSelectorConfiguration : virtual public asg::IAsgTool
{
  ASG_TOOL_INTERFACE(IEGSelectronConfiguration)

public:

  /**Virtual Destructor*/
  virtual ~IEGSelectorConfiguration() {};

  virtual std::unique_ptr<AsgElectronLikelihoodTool> configuredTool(const EgammaSelectors::LHidentificationPoint input)  const =0;
  virtual std::unique_ptr<AsgPhotonIsEMSelector> configuredTool(const EgammaSelectors::PhotonCutidentificationPoint input)  const =0;
  virtual std::unique_ptr<AsgForwardElectronIsEMSelector> configuredTool(const EgammaSelectors::ForwardElectronCutidentificationPoint input)  const =0;
  virtual std::unique_ptr<AsgElectronIsEMSelector> configuredTool(const EgammaSelectors::ElectronCutidentificationPoint input)  const =0;

  /** interface for naked pointer, as some user prefer to manage the memory themselves */
  virtual AsgElectronLikelihoodTool* configuredToolPtr(const EgammaSelectors::LHidentificationPoint input)  const =0;
  virtual AsgPhotonIsEMSelector* configuredToolPtr(const EgammaSelectors::PhotonCutidentificationPoint input)  const =0;
  virtual AsgForwardElectronIsEMSelector* configuredaToolPtr(const EgammaSelectors::ForwardElectronCutidentificationPoint input)  const =0;
  virtual AsgElectronIsEMSelector* configuredToolPtr(const EgammaSelectors::ElectronCutidentificationPoint input)  const =0;
  
}; // End: class definition

#endif


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAANALYSISINTERFACES__IASGFORWARDELECTRONISEMSELECTOR__
#define EGAMMAANALYSISINTERFACES__IASGFORWARDELECTRONISEMSELECTOR__

/**
   @class IAsgForwardElectronIsEMSelector
   @brief Interface to tool to select electrons

   @author Jovan Mitrevski (UCSC) Karsten Koeneke (CERN)
   @date   Dec 2011 - Fab 2012

   11-MAR-2014, convert to ASGTool
*/

// Include the interfaces
#include "PATCore/IAsgSelectionTool.h"
#include "EgammaAnalysisInterfaces/IAsgEGammaIsEMSelector.h"

// Forward declarations
namespace Root{
  class TAccept;
}
#include "xAODEgamma/ElectronFwd.h"
#include "xAODEgamma/PhotonFwd.h"
#include "xAODEgamma/EgammaFwd.h"

class IAsgForwardElectronIsEMSelector : virtual public IAsgEGammaIsEMSelector
{

  ASG_TOOL_INTERFACE(IAsgForwardElectronIsEMSelector)
  
  public:

  /// @name IAsgForwardElectronIsEMSelector methods in Addition to the IAsgEGammaIsEMSelector ones
  /// @{
  /// It adds no new mehtod to the IAsgEGammaIsEMSelector interface 
  ///@}

  /// Virtual Destructor
  virtual ~IAsgForwardElectronIsEMSelector() {};


}; // End: class definition


#endif


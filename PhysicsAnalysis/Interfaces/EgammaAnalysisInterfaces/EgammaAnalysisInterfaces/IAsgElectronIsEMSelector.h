/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef EGAMMAANALYSISINTERFACES__IASGELECTRONISEMSELECTOR__
#define EGAMMAANALYSISINTERFACES__IASGELECTRONISEMSELECTOR__

/**
   @class IAsgElectronIsEMSelector
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

class IAsgElectronIsEMSelector : virtual public IAsgEGammaIsEMSelector
{

  ASG_TOOL_INTERFACE(IAsgElectronIsEMSelector)
  
  public:

  /// @name IAsgElectronIsEMSelector methods in Addition to the IAsgEGammaIsEMSelector ones
  /// @{
  /// It adds no new methods to the IAsgEGammaIsEMSelector interface 
  ///@}

  /// Virtual Destructor
  virtual ~IAsgElectronIsEMSelector() {};



}; // End: class definition


#endif


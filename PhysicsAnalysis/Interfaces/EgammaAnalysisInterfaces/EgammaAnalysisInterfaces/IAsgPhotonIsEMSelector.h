/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAANALYSISINTERFACES__IASGPHOTONISEMSELECTOR__
#define EGAMMAANALYSISINTERFACES__IASGPHOTONISEMSELECTOR__

/**
   @class IAsgPhotonIsEMSelector
   @brief Interface to tool to select photons

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
#include "xAODEgamma/EgammaFwd.h"
#include "xAODEgamma/PhotonFwd.h"
#include "xAODEgamma/ElectronFwd.h"

class IAsgPhotonIsEMSelector : virtual public IAsgEGammaIsEMSelector
{

  ASG_TOOL_INTERFACE(IAsgPhotonIsEMSelector)

public:

  /// @name IAsgPhotonIsEMSelector methods in Addition to the IAsgEGammaIsEMSelector ones
  /// @{
  /// It adds no new methods to the IAsgEGammaISEMSelector Interface
  ///@}

  /// Virtual Destructor
  virtual ~IAsgPhotonIsEMSelector() {};
 
}; // End: class definition


#endif


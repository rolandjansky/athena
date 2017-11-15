/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef EGAMMAANALYSISINTERFACES__IASGEGAMMAISEMSELECTOR__
#define EGAMMAANALYSISINTERFACES__IASGEGAMMAISEMSELECTOR__

/**
   @class IAsgEGammaIsEMSelector
   @brief Interface to tool to select photons

   @author Jovan Mitrevski (UCSC) Karsten Koeneke (CERN)
   @date   Dec 2011 - Fab 2012

   11-MAR-2014, convert to ASGTool
*/

// Include the interfaces
#include "PATCore/IAsgSelectionTool.h"
// Forward declarations
namespace Root{
  class TAccept;
}
#include "xAODEgamma/EgammaFwd.h"
#include "xAODEgamma/PhotonFwd.h"
#include "xAODEgamma/ElectronFwd.h"

class IAsgEGammaIsEMSelector : virtual public IAsgSelectionTool
{

  ASG_TOOL_INTERFACE(IAsgEGammaIsEMSelector)

public:

  /// @name IAsgEGammaIsEMSelector methods in Addition to the IAsgSelectionTool
  /// @{

  /// Accept with reference to  IParticle  
  virtual const Root::TAccept& accept( const xAOD::IParticle& part ) const = 0;

  /// Accept with pointer to Egamma object 
  virtual const Root::TAccept& accept( const xAOD::Egamma* part) const = 0;

  ///Accept with reference to Egamma objects
  virtual const Root::TAccept& accept( const xAOD::Egamma& part) const = 0;

  /// Accept with Photon pointer
  virtual const Root::TAccept& accept( const xAOD::Photon* part ) const = 0;

  /// Accept with Photon reference
  virtual const Root::TAccept& accept( const xAOD::Photon& part ) const = 0;

  /// Accept with Electron reference
  virtual const Root::TAccept& accept( const xAOD::Electron* part ) const = 0;

  /// Accept with Electron reference 
  virtual const Root::TAccept& accept( const xAOD::Electron& part ) const = 0;

  ///Add a legacy execute method 
  virtual StatusCode execute(const xAOD::Egamma* eg) const =0;

  ///Return the egamma IsEM decision as unsigned int
  virtual unsigned int IsemValue() const =0;

  /// Report the current operating point
  virtual std::string getOperatingPointName( ) const =0;

  ///@}

  /// Virtual Destructor
  virtual ~IAsgEGammaIsEMSelector() {};

}; 


#endif


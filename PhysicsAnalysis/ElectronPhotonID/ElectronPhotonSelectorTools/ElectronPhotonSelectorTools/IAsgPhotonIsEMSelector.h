/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef __IASGPHOTONISEMSELECTOR__
#define __IASGPHOTONISEMSELECTOR__

/**
   @class IAsgPhotonIsEMSelector
   @brief Interface to tool to select photons

   @author Jovan Mitrevski (UCSC) Karsten Koeneke (CERN)
   @date   Dec 2011 - Fab 2012

   11-MAR-2014, convert to ASGTool
*/

// Include the interfaces
#include "PATCore/IAsgSelectionTool.h"
#include "ElectronPhotonSelectorTools/IAsgEGammaIsEMSelector.h"
#include "ElectronPhotonSelectorTools/egammaPIDdefs.h"

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

  /**Virtual Destructor*/
  virtual ~IAsgPhotonIsEMSelector() {};

  /** Accept with generic interface */
  virtual const Root::TAccept& accept( const xAOD::IParticle* part ) const = 0;

  /** Accept with generic interface */
  virtual const Root::TAccept& accept( const xAOD::IParticle& part ) const = 0;
  
  /** Accept with Egamma objects */
  virtual const Root::TAccept& accept( const xAOD::Egamma* part) const = 0;

  /** Accept with Egamma objects */
  virtual const Root::TAccept& accept( const xAOD::Egamma& part) const = 0;

  /** The main accept method: the actual cuts are applied here */
  virtual const Root::TAccept& accept( const xAOD::Photon* part ) const = 0;

  /** The main accept method: the actual cuts are applied here */
  virtual const Root::TAccept& accept( const xAOD::Photon& part ) const = 0;

  /** The main accept method: the actual cuts are applied here */
  virtual const Root::TAccept& accept( const xAOD::Electron* part ) const = 0;

  /** The main accept method: the actual cuts are applied here */
  virtual const Root::TAccept& accept( const xAOD::Electron& part ) const = 0;


  ////Expert methods /////////////////////////////////////
  /** The isem potentially for the trigger */
  virtual StatusCode execute(const xAOD::Egamma* eg) const =0;

  virtual unsigned int IsemValue() const =0;

  /** Method to get the operating point */
  virtual std::string getOperatingPointName( ) const =0;
 
}; // End: class definition


#endif


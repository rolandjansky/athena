/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef __IASGEGAMMAISEMSELECTOR__
#define __IASGEGAMMAISEMSELECTOR__

/**
   @class IAsgEGammaIsEMSelector
   @brief Interface to tool to select photons

   @author Jovan Mitrevski (UCSC) Karsten Koeneke (CERN)
   @date   Dec 2011 - Fab 2012

   11-MAR-2014, convert to ASGTool
*/

// Include the interfaces
#include "PATCore/IAsgSelectionTool.h"
#include "ElectronPhotonSelectorTools/egammaPIDdefs.h"

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

  /**Virtual Destructor*/
  virtual ~IAsgEGammaIsEMSelector() {};

  /** Accept with generic interface */
  virtual const Root::TAccept& accept( const xAOD::IParticle* part ) const = 0;

  /** Accept with generic interface */
  virtual const Root::TAccept& accept( const xAOD::IParticle& part ) const = 0;

  /////Egammma IsEM specific methods

  /** Accept with Egamma objects */
  virtual const Root::TAccept& accept( const xAOD::Egamma* part) const = 0;

  /** Accept with Egamma objects */
  virtual const Root::TAccept& accept( const xAOD::Egamma& part) const = 0;

  virtual unsigned int IsemValue() const =0;

  /** Method to get the operating point */
  virtual std::string getOperatingPointName( ) const =0;
 
}; // End: class definition


#endif


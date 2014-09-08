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

// Include the return object and the underlying ROOT tool
#include "PATCore/TAccept.h"
#include "ElectronPhotonSelectorTools/egammaPIDdefs.h"

// Forward declarations

#include "xAODEgamma/Photon.h"

class IAsgPhotonIsEMSelector : virtual public IAsgSelectionTool
{

  ASG_TOOL_INTERFACE(IAsgPhotonIsEMSelector)

public:

  /**Virtual Destructor*/
  virtual ~IAsgPhotonIsEMSelector() {};

  /** The main accept method: using the generic interface */
  virtual const Root::TAccept& accept( const xAOD::IParticle* part ) const = 0;

  /** The main accept method: the actual cuts are applied here */
  virtual const Root::TAccept& accept( const xAOD::Photon* part ) const = 0;


  /** The isem potentially for the trigger */
  virtual StatusCode execute(const xAOD::Photon*, double trigEtTh) const =0;

  virtual unsigned int IsemValue() const =0;

  virtual egammaPID::PID PIDName() const = 0;

  //virtual const Root::TAccept& getTAccept( ) =0; 
 
}; // End: class definition


#endif


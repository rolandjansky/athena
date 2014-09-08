/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef __IASGELECTRONISEMSELECTOR__
#define __IASGELECTRONISEMSELECTOR__

/**
   @class IAsgElectronIsEMSelector
   @brief Interface to tool to select electrons

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
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Egamma.h"

class IAsgElectronIsEMSelector : virtual public IAsgSelectionTool
{

  ASG_TOOL_INTERFACE(IAsgElectronIsEMSelector)
  
  public:

  /**Virtual Destructor*/
  virtual ~IAsgElectronIsEMSelector() {};

  /** The main accept method: using the generic interface */
  virtual const Root::TAccept& accept( const xAOD::IParticle* part ) const = 0;

  /** The main accept method: the actual cuts are applied here */
  virtual const Root::TAccept& accept( const xAOD::Electron* part ) const = 0;


  /** The main accept method: the actual cuts are applied here */
  virtual const Root::TAccept& accept( const xAOD::Electron* part, 
				       double trigEtTh, 
				       bool CaloCutsOnly) const = 0;

  /** This method is for the trigger, and implies CaloCutsOnly set to true */
  virtual const Root::TAccept& accept( const xAOD::Egamma* part, 
				       double trigEtTh) const = 0;

  /** The basic isem */
  virtual StatusCode execute(const xAOD::Electron* eg) const =0;
  
  /** The isem potentially for the trigger */
  virtual StatusCode execute(const xAOD::Electron* eg, 
			     double trigEtTh, 
			     bool CaloCutsOnly) const =0;

  /** The isem potentially for the trigger, imples CalocCutsOnly*/
  virtual StatusCode execute(const xAOD::Egamma* eg, 
			     double trigEtTh) const =0;
  
  virtual unsigned int IsemValue() const = 0;
  
  virtual egammaPID::PID PIDName() const = 0;

  //virtual const Root::TAccept& getTAccept( ) =0;



}; // End: class definition


#endif


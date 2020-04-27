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
#include "EgammaAnalysisInterfaces/IAsgEGammaIsEMSelector.h"
#include "xAODEgamma/ElectronFwd.h"
#include "xAODEgamma/PhotonFwd.h"
#include "xAODEgamma/EgammaFwd.h"

class EventContext;

class IAsgForwardElectronIsEMSelector : virtual public IAsgEGammaIsEMSelector
{

  ASG_TOOL_INTERFACE(IAsgForwardElectronIsEMSelector)
  
  public:

  /// @name IAsgForwardElectronIsEMSelector methods in Addition to the IAsgEGammaIsEMSelector ones
  /// @{
  /// It adds no new methods to the IAsgEGammaIsEMSelector interface 
  ///@}

  /// Virtual Destructor
  virtual ~IAsgForwardElectronIsEMSelector() {};
  virtual  StatusCode execute(const EventContext& ctx, const xAOD::Egamma* eg, unsigned int& isEM) const = 0;
  
  virtual asg::AcceptData accept( const xAOD::IParticle* part) const = 0 ;
  virtual asg::AcceptData accept( const EventContext& ctx, const xAOD::IParticle* part) const = 0 ;

  virtual asg::AcceptData accept( const EventContext& ctx, const xAOD::Egamma* part) const = 0 ;

  virtual asg::AcceptData accept( const EventContext& ctx, const xAOD::Photon* part ) const = 0 ;

  virtual asg::AcceptData accept( const EventContext& ctx, const xAOD::Electron* part ) const = 0 ;
  /** Method to get the operating point */
  virtual std::string getOperatingPointName() const = 0;


}; // End: class definition


#endif


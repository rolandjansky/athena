/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef EGAMMAANALYSISINTERFACES__IASGELECTRONCHARGEIDSELECTOR__
#define EGAMMAANALYSISINTERFACES__IASGELECTRONCHARGEIDSELECTOR__

/**
   @class IAsgElectronChargeIDSelectorTool
   @brief Interface to tool for ChargeID


*/

// Include the interfaces
#include "PATCore/IAsgSelectionTool.h"
#include "xAODEgamma/ElectronFwd.h"
#include "xAODEgamma/EgammaFwd.h"
#include "GaudiKernel/EventContext.h"

class IAsgElectronChargeIDSelectorTool : virtual public IAsgSelectionTool
{

  ASG_TOOL_INTERFACE(IAsgElectronChargeIDSelectorTool)

 public:

  /// @name IAsgElectronChargeIDSelectorTool  methods in Addition to the IAsgSelectionTool ones
  /// Some are there to mainly support the calls done from the online/Trigger side 
  /// @{

  /// accept with pointer to  IParticle  so as to not hide the IAsgSelectionTool one
  virtual asg::AcceptData accept( const xAOD::IParticle* part ) const = 0;
  /// accept method with pointer to electron 
  virtual asg::AcceptData accept( const xAOD::Electron* part ) const = 0;

  /// accept method with pointer to electron  when mu not in EventInfo for online 
  virtual asg::AcceptData accept( const xAOD::Electron* part, double mu ) const = 0;

  /// accept method with pointer to  egamma 
  virtual asg::AcceptData accept( const xAOD::Egamma* part ) const = 0;

  /// accept method with pointer to egammma when mu not in EventInfo for online 
  virtual asg::AcceptData accept( const xAOD::Egamma* part, double mu ) const = 0;

  /// calculate method: for pointer to IParticle
  virtual double calculate( const xAOD::IParticle* part ) const = 0;

  /// calculate method: for pointer to electron
  virtual double calculate( const xAOD::Electron* eg ) const = 0;

  /// calculate method: for pointer to egamma 
  virtual double calculate( const xAOD::Egamma* eg ) const = 0;

  /// calculate method: for pointer to electron  when mu not in EventInfo for online
  virtual double calculate( const xAOD::Electron* eg, double mu ) const = 0;

  /// calculate method: for pointer to egamma when mu not in EventInfo for online
  virtual double calculate( const xAOD::Egamma* eg, double mu ) const = 0;

  ///Method to get the operating point 
  virtual std::string getOperatingPointName( ) const =0;

  ///@}

  /// Virtual Destructor
  virtual ~IAsgElectronChargeIDSelectorTool() {};

}; // End: class definition


#endif


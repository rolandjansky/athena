/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @file ILuminosityTool.h
   @class ILuminosityTool
   @brief Provides interface for LuminosityTool
   @author E.Torrence
**/

#ifndef LUMIBLOCKCOMPS_ILuminosityTool_H
#define LUMIBLOCKCOMPS_ILuminosityTool_H

#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"

class ILuminosityTool: virtual public IAlgTool {

 public:

  // Gaudi Boilerplate
  static const InterfaceID& interfaceID() {
    static const InterfaceID ILuminosityTool( "ILuminosityTool", 1, 0);
    return ILuminosityTool;
  }

  // Standard tool methods
  virtual StatusCode initialize() = 0;
  virtual StatusCode finalize() = 0;

  // Interface

  // Instantaneous luminosity information from /TRIGGER/LUMI/LBLESTONL or LBLESTOFL
  virtual float lbAverageLuminosity() const = 0;  // In ub-1 s-1 => 10^30 cm-2 s-1
  virtual float lbAverageInteractionsPerCrossing() const = 0;  
  virtual unsigned int lbAverageValid() const = 0; // Validity flag from COOL

  // Luminosity block time (in seconds) from /TRIGGER/LUMI/LBLB
  // Multiply by lbAverageLuminosity to get integrated luminosity in the current lumi block
  virtual double lbDuration() const = 0; // Luminosity block time (in seconds)

  // Instantaneous luminosity per BCID
  // Non-zero values only filled for BCIDs in the physics bunch group, or found to have collisions by the bunch currents

  // Return vector with all BCIDs indexed by bcid number
  virtual const std::vector<float>& lbLuminosityPerBCIDVector() = 0;  // In ub-1 s-1 => 10^30 cm-2 s-1

  // Return current BCID luminosity based on EventInfo bcid number
  virtual float lbLuminosityPerBCID() = 0;

  // Return specific BCID luminosity for any BCID in the current lumi block
  virtual float lbLuminosityPerBCID(unsigned int bcid) = 0;

  // Conversion factor from OLC.  lumiPerBCID/muToLumi = interactionsPerCrossingPerBCID
  virtual float muToLumi() = 0;

  // Callback function called when any cached data is invalidated, 
  // Can be used to provide callbacks to other functions
  virtual StatusCode updateCache(IOVSVC_CALLBACK_ARGS) = 0;
 
 private:
};

#endif

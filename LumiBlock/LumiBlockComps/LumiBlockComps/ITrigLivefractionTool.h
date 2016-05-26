/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @file ITrigLivefractionTool.h
   @class ITrigLivefractionTool
   @brief Provides interface for TrigLivefractionTool, depends on ILuminosityTool
   @author E.Torrence
**/

#ifndef LUMIBLOCKCOMPS_ITrigLivefractionTool_H
#define LUMIBLOCKCOMPS_ITrigLivefractionTool_H

#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"

class ITrigLivefractionTool: virtual public IAlgTool {

 public:

  // Gaudi Boilerplate
  static const InterfaceID& interfaceID() {
    static const InterfaceID ITrigLivefractionTool( "ITrigLivefractionTool", 1, 0);
    return ITrigLivefractionTool;
  }

  // Standard tool methods
  virtual StatusCode initialize() = 0;
  virtual StatusCode finalize() = 0;

  // Interface

  // L1 livefraction information from /TRIGGER/LUMI/PerBcidDeadtime
  // In all of these functions, the highPriority boolean selects
  //  the highPriority triggers (true) or lowPriority triggers (false)

  // Return vector with all BCIDs indexed by bcid number
  virtual const std::vector<float>& l1LivefractionVector(bool highPriority = true) const = 0;

  // Return current BCID livefraction based on EventInfo bcid number
  // Defaults to high priority L1 triggers.  Use highPriority = false to get low priority triggers
  virtual float livefractionPerBCID(bool highPriority = true) const = 0;

  // Return specific BCID live fraction for any BCID in the current lumi block
  virtual float livefractionPerBCID(unsigned int bcid, bool highPriority = true) const = 0;

  // Return number of turns in this lumi block
  virtual unsigned int lhcTurnCounter() const = 0;

  // Luminosity-averaged live fraction over all physics BCIDs (high or low priority triggers)
  virtual float lbAverageLivefraction(bool highPriority=true) = 0;

  // Callback function called when any cached data is invalidated, 
  // Can be used to provide callbacks to other functions
  virtual StatusCode updateCache(IOVSVC_CALLBACK_ARGS) = 0;

 private:
};

#endif

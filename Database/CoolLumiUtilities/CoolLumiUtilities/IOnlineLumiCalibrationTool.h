/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @file IOnlineLumiCalibrationTool.h
   @class IOnlineLumiCalibrationTool
   @brief Provides interface for LumiCalibrationTool
   @author E.Torrence
**/

#ifndef COOLLUMIUTILITIES_IONLINELUMICALIBRATIONTOOL_H
#define COOLLUMIUTILITIES_IONLINELUMICALIBRATIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"

#include "CoolLumiUtilities/OnlineLumiCalibrator.h"

class IOnlineLumiCalibrationTool : virtual public IAlgTool {

 public:

  /// define interface ID (as required by Gaudi)
  static const InterfaceID& interfaceID() { 
    static const InterfaceID s_myID("IOnlineLumiCalibrationTool", 1 , 0); 
    return s_myID; 
  }

  // Standard tool methods
  virtual StatusCode initialize() = 0;

  // Interface

  // Returns -1 if channel doesn't exist
  virtual float getMuToLumi(unsigned int channel) const = 0;

  // Returns false on error
  virtual bool calibrateLumi(unsigned int channel, float raw, float& lumi) const = 0;
  virtual bool calibrateMu(unsigned int channel, float raw, float& mu) const = 0;

  // Callback function called when any cached data is invalidated,
  // Can be used to provide callbacks to other functions
  virtual StatusCode updateCache(IOVSVC_CALLBACK_ARGS) = 0;

 private:
};

#endif

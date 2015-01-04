/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @file IBunchLumisTool.h
   @class IBunchLumisTool
   @brief Provides interface for BunchLumisTool
   @author E.Torrence
**/

#ifndef COOLLUMIUTILITIES_IBUNCHLUMISTOOL_H
#define COOLLUMIUTILITIES_IBUNCHLUMISTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"

class IBunchLumisTool : virtual public IAlgTool {

 public:

  /// define interface ID (as required by Gaudi)
  static const InterfaceID& interfaceID() { 
    static const InterfaceID s_myID("IBunchLumisTool", 1 , 0); 
    return s_myID; 
  }

  // Standard tool methods
  virtual StatusCode initialize() = 0;

  // Interface

  // Set channel
  virtual void setChannel(unsigned int channel) = 0;

  // Arrays with raw luminosity per BCID
  virtual const std::vector<float>& rawLuminosity() = 0;
  virtual float rawLuminosity(unsigned int bcid) = 0;
  virtual float averageRawLuminosity() = 0;

  // Callback function called when any cached data is invalidated,
  // Can be used to provide callbacks to other functions
  virtual StatusCode updateCache(IOVSVC_CALLBACK_ARGS) = 0;

 private:
};

#endif

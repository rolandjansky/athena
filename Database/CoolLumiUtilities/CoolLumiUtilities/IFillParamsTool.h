/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @file IFillParamsTool.h
   @class IFillParamsTool
   @brief Provides interface for FillParamsTool
   @author E.Torrence
**/

#ifndef COOLLUMIUTILITIES_IFILLPARAMSTOOL_H
#define COOLLUMIUTILITIES_IFILLPARAMSTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"

class IFillParamsTool : virtual public IAlgTool {

 public:

  /// define interface ID (as required by Gaudi)
  static const InterfaceID& interfaceID() { 
    static const InterfaceID s_myID("IFillParamsTool", 1 , 0); 
    return s_myID; 
  }

  // Standard tool methods
  virtual StatusCode initialize() = 0;

  // Interface

  // Number of filled bunches (length of vectors below)
  virtual unsigned int nBeam1Bunches() const = 0;
  virtual unsigned int nBeam2Bunches() const = 0;
  virtual unsigned int nLuminousBunches() const = 0;

  // Arrays with BCID numbers for filled bunches
  virtual const std::vector<unsigned int>& beam1Bunches() const = 0;
  virtual const std::vector<unsigned int>& beam2Bunches() const = 0;
  virtual const std::vector<unsigned int>& luminousBunches() const = 0;

  // Callback function called when any cached data is invalidated,
  // Can be used to provide callbacks to other functions
  virtual StatusCode updateCache(IOVSVC_CALLBACK_ARGS) = 0;

 private:
};

#endif

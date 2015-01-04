/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @file IBunchGroupTool.h
   @class IBunchGroupTool
   @brief Provides interface for BunchGroupTool
   @author E.Torrence
**/

#ifndef COOLLUMIUTILITIES_IBUNCHGROUPTOOL_H
#define COOLLUMIUTILITIES_IBUNCHGROUPTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"

class IBunchGroupTool : virtual public IAlgTool {

 public:

  /// define interface ID (as required by Gaudi)
  static const InterfaceID& interfaceID() { 
    static const InterfaceID s_myID("IBunchGroupTool", 1 , 0); 
    return s_myID; 
  }

  // Standard tool methods
  virtual StatusCode initialize() = 0;

  // Interface

  // Number of filled bunches (length of vectors below)
  virtual unsigned int nBunchGroup0() const = 0;
  virtual unsigned int nBunchGroup1() const = 0;
  virtual unsigned int nBunchGroup2() const = 0;
  virtual unsigned int nBunchGroup3() const = 0;
  virtual unsigned int nBunchGroup4() const = 0;
  virtual unsigned int nBunchGroup5() const = 0;
  virtual unsigned int nBunchGroup6() const = 0;
  virtual unsigned int nBunchGroup7() const = 0;

  // Arrays with BCID numbers for filled bunches
  virtual const std::vector<unsigned int>& bunchGroup0() const = 0;
  virtual const std::vector<unsigned int>& bunchGroup1() const = 0;
  virtual const std::vector<unsigned int>& bunchGroup2() const = 0;
  virtual const std::vector<unsigned int>& bunchGroup3() const = 0;
  virtual const std::vector<unsigned int>& bunchGroup4() const = 0;
  virtual const std::vector<unsigned int>& bunchGroup5() const = 0;
  virtual const std::vector<unsigned int>& bunchGroup6() const = 0;
  virtual const std::vector<unsigned int>& bunchGroup7() const = 0;

  // Callback function called when any cached data is invalidated,
  // Can be used to provide callbacks to other functions
  virtual StatusCode updateCache(IOVSVC_CALLBACK_ARGS) = 0;

 private:
};

#endif

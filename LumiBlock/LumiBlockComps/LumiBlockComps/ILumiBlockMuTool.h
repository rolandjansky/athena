/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @file ILumiBlockMuTool.h
   @class ILumiBlockMuTool
   @brief Provides interface for LumiBlockMuTool
   @author E.Torrence
**/

#ifndef LUMIBLOCKCOMPS_ILumiBlockMuTool_H
#define LUMIBLOCKCOMPS_ILumiBlockMuTool_H

#include "GaudiKernel/IAlgTool.h"

class ILumiBlockMuTool: virtual public IAlgTool {

 public:

  // Gaudi Boilerplate
  static const InterfaceID& interfaceID() {
    static const InterfaceID ILumiBlockMuTool( "ILumiBlockMuTool", 1, 0);
    return ILumiBlockMuTool;
  }

  // Standard tool methods
  virtual StatusCode initialize () = 0;
  virtual StatusCode finalize() = 0;

  // Interface
  virtual float averageInteractionsPerCrossing() = 0;
  virtual float actualInteractionsPerCrossing() = 0;

 private:
};

#endif

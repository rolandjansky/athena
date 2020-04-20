/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
  DeclareInterfaceID(ILumiBlockMuTool, 1, 0);

  // Interface
  virtual float averageInteractionsPerCrossing() const = 0;
  virtual float actualInteractionsPerCrossing() const = 0;

 private:
};

#endif

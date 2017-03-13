/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEMONITORING_ITILESTUCKBITSPROBSTOOL_H
#define TILEMONITORING_ITILESTUCKBITSPROBSTOOL_H

// Gaudi include(s):
#include "GaudiKernel/IAlgTool.h"

class TTree;

class ITileStuckBitsProbsTool: virtual public IAlgTool {
 public:
 
  virtual ~ITileStuckBitsProbsTool() {}
 
  static const InterfaceID& interfaceID() {
    static const InterfaceID IID_ITileStuckBitsProbsTool("ITileStuckBitsProbsTool", 1, 0);
    return IID_ITileStuckBitsProbsTool;
  }

  virtual const uint8_t* stuckBitProb (int ros, int module, int channel, int gain) const = 0;
  virtual void saveStuckBitsProbabilities(TTree* tree) = 0;
};


#endif

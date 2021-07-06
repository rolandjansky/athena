/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_ITILECONDTOOLNOISE_H
#define TILECONDITIONS_ITILECONDTOOLNOISE_H

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"
#include "TileIdentifier/TileRawChannelUnit.h"

class ITileCondToolNoise : virtual public IAlgTool {
 public:

  virtual ~ITileCondToolNoise() {}

  static const InterfaceID& interfaceID() { 
    static const InterfaceID IID_ITileCondToolNoise("ITileCondToolNoise", 1 , 0);
    return IID_ITileCondToolNoise; 
  }
  
  virtual float getNoise(unsigned int drawerIdx, unsigned int channel, unsigned int adc, 
			                   TileRawChannelUnit::UNIT unit,
                         const EventContext &ctx) const = 0;

};

#endif

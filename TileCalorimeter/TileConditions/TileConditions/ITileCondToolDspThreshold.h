/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_ITILECONDTOOLDSPTHRESHOLD_H
#define TILECONDITIONS_ITILECONDTOOLDSPTHRESHOLD_H

#include "GaudiKernel/IAlgTool.h"



class ITileCondToolDspThreshold: virtual public IAlgTool {

  public:

    virtual ~ITileCondToolDspThreshold() {};

    static const InterfaceID& interfaceID() {
      static const InterfaceID IID_TileCondToolDspThreshold("ITileCondToolDspThreshold", 1, 0);    
      return IID_TileCondToolDspThreshold;
    };

    virtual float getMinimumAmplitudeThreshold(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const = 0;
    virtual float getMaximumAmplitudeThreshold(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const = 0;
    virtual void getAmplitudeThresholds(unsigned int drawerIdx, unsigned int channel, unsigned int adc, float& minimumThreshold, float& maximumThreshold) const = 0;
    virtual float getDspThreshold(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const = 0;

};

#endif // TILECONDITIONS_ITILECONDTOOLDSPTHRESHOLD_H

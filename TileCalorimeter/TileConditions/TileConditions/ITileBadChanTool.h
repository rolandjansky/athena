/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_ITILEBADCHANTOOL_H
#define TILECONDITIONS_ITILEBADCHANTOOL_H

#include "Identifier/HWIdentifier.h"
#include "GaudiKernel/IAlgTool.h"
#include "TileCalibBlobObjs/TileBchStatus.h"
#include <stdint.h>
#include <set>

class ITileBadChanTool: virtual public IAlgTool {
  public:

    virtual ~ITileBadChanTool() {}

    static const InterfaceID& interfaceID() {
      static const InterfaceID IID_ITileBadChanTool("ITileBadChanTool", 1, 0);
      return IID_ITileBadChanTool;
    }

    virtual const TileBchStatus& getAdcStatus(const HWIdentifier& adc_id) const = 0;
    virtual const TileBchStatus& getAdcStatus(unsigned int drawerIdx
                                             , unsigned int channel
                                             , unsigned int adc) const = 0;

    virtual const TileBchStatus& getChannelStatus(const HWIdentifier& adc_id) const = 0;
    virtual const TileBchStatus& getChannelStatus(unsigned int drawerIdx
                                                 , unsigned int channel) const = 0;

    virtual uint32_t encodeStatus(const TileBchStatus& status) const = 0;

    virtual const std::vector<float>& getTripsProbabilities(unsigned int ros) const = 0;

};

#endif

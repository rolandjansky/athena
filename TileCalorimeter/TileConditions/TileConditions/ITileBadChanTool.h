/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_ITILEBADCHANTOOL_H
#define TILECONDITIONS_ITILEBADCHANTOOL_H

#include "Identifier/HWIdentifier.h"
#include "GaudiKernel/IAlgTool.h"
#include "TileCalibBlobObjs/TileBchStatus.h"
#include <stdint.h>
#include <set>

/**
 * @class ITileBadChanTool
 * @brief An abstract inteface to get Tile channel and ADC status
 */

class ITileBadChanTool: virtual public IAlgTool {
  public:

    virtual ~ITileBadChanTool() {}

    /**
     * @brief Return Tile ADC status
     * @param adc_id Tile hardware (online) ADC identifier
     * @return Tile ADC status
     */
    virtual const TileBchStatus& getAdcStatus(const HWIdentifier& adc_id) const = 0;

    /**
     * @brief Return Tile ADC status
     * @param drawerIdx Tile drawer index in range [0..275]
     * @param channel Tile channel number in range [0..47]
     * @param adc Tile ADC number
     * @return Tile ADC status
     */
    virtual const TileBchStatus& getAdcStatus(unsigned int drawerIdx
                                             , unsigned int channel
                                             , unsigned int adc) const = 0;

    /**
     * @brief Return Tile channel status
     * @param channel_id Tile hardware (online) channel identifier
     * @return Tile channel status
     */
    virtual const TileBchStatus& getChannelStatus(const HWIdentifier& channel_id) const = 0;

    /**
     * @brief Return Tile channel status
     * @param drawerIdx Tile drawer index in range [0..275]
     * @param channel Tile channel number in range [0..47]
     * @return Tile channel status
     */
    virtual const TileBchStatus& getChannelStatus(unsigned int drawerIdx
                                                 , unsigned int channel) const = 0;

    virtual uint32_t encodeStatus(const TileBchStatus& status) const = 0;

   /**
    * @brief Return trips probabilities for all Tile drawers
    * @return Trip probabilites for all Tile drawers
    */
    virtual const std::vector<float>& getTripsProbabilities(unsigned int ros) const = 0;

   /**
    * @brief Check if Tile drawer is masked completely
    * @param frag_id Tile frag identifier
    * @return True if Tile drawer is masked comletely
    */
    virtual bool isDrawerMasked(unsigned int frag_id) const = 0;

    DeclareInterfaceID(ITileBadChanTool, 1, 0);
};

#endif // TILECONDITIONS_ITILEBADCHANTOOL_H

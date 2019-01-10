/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILEBADCHANTOOL_H
#define TILECONDITIONS_TILEBADCHANTOOL_H

// Tile includes
#include "TileConditions/ITileBadChanTool.h"
#include "TileCalibBlobObjs/TileBchStatus.h"
#include "TileConditions/TileBadChannels.h"

// Calo includes
#include "CaloConditions/ICaloBadChanTool.h"
#include "CaloConditions/CaloBadChannel.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/HWIdentifier.h"
#include "StoreGate/ReadCondHandleKey.h"

#include <map>
#include <vector>
#include <string>

class TileHWID;
class TileDetDescrManager;

/**
 * @class TileBadChanTool
 * @brief The tool to get Tile channel and ADC status
 */

class TileBadChanTool: public extends<AthAlgTool, ITileBadChanTool, ICaloBadChanTool> {

  public:

    TileBadChanTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~TileBadChanTool();

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    //===============================================================
    //=== ICaloBadChanTool methods
    //===============================================================

    /**
     * @brief ICaloBadChanTool interface.
     * @param cell_id Calo cell identifier
     */
    virtual CaloBadChannel caloStatus(Identifier cell_id) const override;

    //===============================================================
    //=== ITileBadChanTool methods
    //===============================================================

    const TileBchStatus& getAdcStatus(IdentifierHash hash_id, unsigned int adc) const;

    /**
     * @copydoc ITileBadChanTool::getAdcStatus(const HWIdentifier& adc_id) const
     */
    virtual const TileBchStatus& getAdcStatus(const HWIdentifier& adc_id) const override;

    /**
     * @copydoc ITileBadChanTool::getAdcStatus(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const
     */
    virtual const TileBchStatus& getAdcStatus(unsigned int drawerIdx
                                     , unsigned int channel
                                     , unsigned int adc) const override;

    const TileBchStatus& getChannelStatus(IdentifierHash hash_id) const;

    /**
     * @copydoc ITileBadChanTool::getChannelStatus(const HWIdentifier& channel_id) const
     */
    virtual const TileBchStatus& getChannelStatus(const HWIdentifier& channel_id) const override;

    /**
     * @copydoc ITileBadChanTool::getChannelStatus(unsigned int drawerIdx, unsigned int channel) const
     */
    virtual const TileBchStatus& getChannelStatus(unsigned int drawerIdx, unsigned int channel) const override;

    virtual uint32_t encodeStatus(const TileBchStatus& status) const override;

    /**
     * @copydoc ITileBadChanTool::getTripsProbabilities(unsigned int ros) const
     */
    virtual const std::vector<float>& getTripsProbabilities(unsigned int ros) const override;

    /**
     * @copydoc ITileBadChanTool::isDrawerMasked(unsigned int frag_id) const
     */
    virtual bool isDrawerMasked(unsigned int frag_id) const override;

  private:

    /**
     * @brief Name of TileBadChannels in condition store
     */
    SG::ReadCondHandleKey<TileBadChannels> m_badChannelsKey{this,
        "TileBadChannels", "TileBadChannels", "Input Tile bad channel status"};

    const TileDetDescrManager* m_tileMgr;

    const TileHWID* m_tileHWID;
    TileBchStatus m_defaultStatus;

    // dummy declarations just to make new gccreflex happy
    TileBadChanTool& operator=(const TileBadChanTool&);

    std::vector<float> m_defaultTripsProbs;

    std::vector<unsigned int> m_roses;
    std::vector<unsigned int> m_drawers;

};


#endif

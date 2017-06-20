/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILEBADCHANTOOL_H
#define TILECONDITIONS_TILEBADCHANTOOL_H

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "Identifier/HWIdentifier.h"
#include "Identifier/IdentifierHash.h"

// Calo includes
#include "CaloConditions/ICaloBadChanTool.h"
#include "CaloConditions/CaloBadChannel.h"

// Tile includes
#include "TileConditions/ITileBadChanTool.h"
#include "TileConditions/ITileCondProxy.h"
#include "TileConditions/TileCondIdTransforms.h"
#include "TileCalibBlobObjs/TileBchStatus.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"

#include <map>
#include <vector>
#include <string>

class TileHWID;
class TileDetDescrManager;
class IdContext;
class TileBchDecoder;
class TileCalibDrawerBch;

class TileBadChanTool: public AthAlgTool
                     , virtual public ITileBadChanTool
                     , virtual public ICaloBadChanTool {
  public:

    TileBadChanTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~TileBadChanTool();

    StatusCode initialize();
    StatusCode finalize();
    static const InterfaceID& interfaceID();

    //===============================================================
    //=== ICaloBadChanTool methods
    //===============================================================

    /** @brief ICaloBadChanTool interface.
     @param id Calo cell identifier */
    CaloBadChannel caloStatus(Identifier id) const;

    //===============================================================
    //=== ITileBadChanTool methods
    //===============================================================
    const TileBchStatus& getAdcStatus(const HWIdentifier& adc_id) const;
    const TileBchStatus& getAdcStatus(IdentifierHash hash_id, unsigned int adc) const;
    const TileBchStatus& getAdcStatus(unsigned int drawerIdx
                                     , unsigned int channel
                                     , unsigned int adc) const;

    const TileBchStatus& getChannelStatus(IdentifierHash hash_id) const;
    const TileBchStatus& getChannelStatus(const HWIdentifier& channel_id) const;
    const TileBchStatus& getChannelStatus(unsigned int drawerIdx, unsigned int channel) const;

    uint32_t encodeStatus(const TileBchStatus& status) const;

    const std::vector<float>& getTripsProbabilities(unsigned int ros) const { return m_tripsProbs[ros - 1]; };

  private:
    StatusCode recache(IOVSVC_CALLBACK_ARGS);

    const TileDetDescrManager* m_tileMgr;
    const IdContext* m_channel_context;
    ToolHandle<TileCondIdTransforms> m_tileIdTrans;

    ToolHandle<ITileCondProxy<TileCalibDrawerBch> > m_pryOnlBch;
    ToolHandle<ITileCondProxy<TileCalibDrawerBch> > m_pryOflBch;

    std::vector<const TileBchDecoder*> m_tileBchDecoder;
    std::map<HWIdentifier, TileBchStatus> m_statusMapAdc;
    std::map<HWIdentifier, TileBchStatus> m_statusMapChn;

    TileBchStatus m_defaultStatus;

    // dummy declarations just to make new gccreflex happy
    TileBadChanTool& operator=(const TileBadChanTool&);

    std::vector<std::vector<float> > m_tripsProbs;
    bool m_useOflBch;

    const TileHWID* m_tileHWID;
    std::vector<unsigned int> m_roses;
    std::vector<unsigned int> m_drawers;
};


#endif

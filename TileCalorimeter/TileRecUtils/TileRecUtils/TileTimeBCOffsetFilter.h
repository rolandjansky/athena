/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILERECUTILS_ITILETIMEBCOFFSETFILTER_H
#define TILERECUTILS_ITILETIMEBCOFFSETFILTER_H

/********************************************************************
 *
 * NAME:     TileTileBCOffsetFilter
 * PACKAGE:  TileCalorimeter/TileRecUtils
 *
 * AUTHOR :  T. Davidek, A. Solodkov
 * CREATED:  Feb 2019
 *
 * PURPOSE:  mask the time offsets by +/-1 or +/-2 bunch crossings in the given
 *           Tile DMU
 *
 *  Input:   TileRawChannelContainer
 *  Output:  TileRawChannelContainer is modified
 *  Parameters:  none
 *
 ********************************************************************/

// Tile includes
#include "TileRecUtils/ITileRawChannelTool.h"
#include "TileConditions/TileCondToolEmscale.h"
#include "TileConditions/TileCablingSvc.h"
#include "TileConditions/ITileDCSTool.h"
#include "TileConditions/ITileBadChanTool.h"
#include "TileEvent/TileDQstatus.h"

// Atlas includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include <vector>

// forward declarations
class TileHWID;
class TileRawChannel;
class TileRawChannelContainer;
class TileRawChannelCollection;
class TileCablingService;

/**
 @class TileTimeBCOffsetFilter
 @brief This tool sets bad status for channels with 25ns or 50ns timing jump
 */
class TileTimeBCOffsetFilter: public extends<AthAlgTool, ITileRawChannelTool> {

  public:

    TileTimeBCOffsetFilter(const std::string& type, const std::string& name,
                           const IInterface* parent);

    virtual ~TileTimeBCOffsetFilter() {};

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    virtual StatusCode process (TileMutableRawChannelContainer& rchCont) const override;

  private:

    int find_partner(int ros, int channel) const;
    bool drawer_ok(const int drawerIndex, std::vector<bool> & channel_time_ok,
                   std::vector<int> & bad_dmu) const;
    bool ch_masked_or_empty(int ros, int drawer, int channel, int gain,
                            const TileDQstatus* DQstatus) const;
    bool isChanDCSgood(int ros, int drawer, int channel) const;

    const TileHWID* m_tileHWID; //!< Pointer to TileHWID

    const TileCablingService* m_cabling; //!< TileCabling instance

    ServiceHandle<TileCablingSvc> m_cablingSvc{ this,
        "TileCablingSvc", "TileCablingSvc", "The Tile cabling service"};

    ToolHandle<ITileDCSTool> m_tileDCS{this, "TileDCSTool", "TileDCSTool", "Tile DCS tool"};

    ToolHandle<TileCondToolEmscale> m_tileToolEmscale{this,
        "TileCondToolEmscale", "TileCondToolEmscale", "Tile EM scale calibration tool"};

    ToolHandle<ITileBadChanTool> m_tileBadChanTool{this,
        "TileBadChanTool", "TileBadChanTool", "Tile bad channel tool"};

    // properties
    SG::ReadHandleKey<TileDQstatus> m_DQstatusKey{this, "TileDQstatus", 
                                                  "TileDQstatus", 
                                                  "TileDQstatus key"};

    float m_ene_threshold_3chan;
    float m_ene_threshold_1chan;
    float m_time_threshold_diff;
    bool m_checkDCS;

};
#endif

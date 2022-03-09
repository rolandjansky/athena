//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILEBADCHANNELSCONDALG_H
#define TILECONDITIONS_TILEBADCHANNELSCONDALG_H

// Tile includes
#include "TileConditions/TileBadChannels.h"
#include "TileConditions/ITileCondProxy.h"
#include "TileConditions/TileCondIdTransforms.h"
#include "TileConditions/TileCablingSvc.h"
#include "TileCalibBlobObjs/TileCalibDrawerBch.h"
#include "TileCalibBlobObjs/TileBchDecoder.h"

// Athena includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"

/**
 * @class TileBadChannelsCondAlg
 * @brief Condition algorithm to prepare TileBadChannels and put it into condition store
 */

class TileBadChannelsCondAlg: public AthAlgorithm {
  public:

    TileBadChannelsCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
    ~TileBadChannelsCondAlg();

    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;
    virtual StatusCode finalize() override {return StatusCode::SUCCESS;}

  private:

   /**
    * @brief Tool to provide online Tile bad channel status
    */
    ToolHandle<ITileCondProxy<TileCalibDrawerBch> > m_onlBchProxy{this, "OnlBchProxy", "",
                                                                  "Tile online bad channels proxy tool"};
   /**
    * @brief Tool to provide ofline Tile bad channel status
    *        If it is provided online and offline Tile bad channel status will be merged
    */
    ToolHandle<ITileCondProxy<TileCalibDrawerBch> > m_oflBchProxy{this, "OflBchProxy", "",
                                                                  "Tile offline bad channels proxy tool"};

   /**
    * @brief Name of output TileBadChannels
    */
    SG::WriteCondHandleKey<TileBadChannels> m_badChannelsKey{this, "TileBadChannels", "TileBadChannels",
                                                             "Output Tile bad channels status"};

   /**
    * @brief Name of Tile cabling service
    */
    ServiceHandle<TileCablingSvc> m_cablingSvc{ this,
        "TileCablingSvc", "TileCablingSvc", "The Tile cabling service" };

    ToolHandle<TileCondIdTransforms> m_tileIdTrans{this, "TileCondIdTransforms", "TileCondIdTransforms",
                                                   "Tile Id transform helper"};

    std::vector<const TileBchDecoder*> m_tileBchDecoder;


    bool m_useOflBch;

};


#endif // TILECONDITIONS_TILEBADCHANNELSCONDALG_H

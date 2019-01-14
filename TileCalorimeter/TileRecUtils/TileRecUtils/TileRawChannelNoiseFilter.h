/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#ifndef TILERAWCHANNELNOISEFILTER_H
#define TILERAWCHANNELNOISEFILTER_H

// Tile includes
#include "TileIdentifier/TileRawChannelUnit.h"
#include "TileRecUtils/ITileRawChannelTool.h"
#include "TileConditions/TileCondToolEmscale.h"
#include "TileConditions/ITileBadChanTool.h"
#include "TileConditions/TileCondToolNoiseSample.h"
#include "TileEvent/TileDQstatus.h"

// Atlas includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

// forward declarations
class TileHWID;
class TileRawChannel;
class TileRawChannelContainer;
class TileRawChannelCollection;

/**
 @class TileRawChannelNoiseFilter
 @brief This tool subtracts common-mode noise from all TileRawChannels in one container
 */
class TileRawChannelNoiseFilter: public extends<AthAlgTool, ITileRawChannelTool> {

  public:

    /** AlgTool like constructor */
    TileRawChannelNoiseFilter(const std::string& type, const std::string& name,
        const IInterface* parent);

    /** Virtual destructor */
    virtual ~TileRawChannelNoiseFilter() {};

    /** AlgTool initialize method.*/
    virtual StatusCode initialize() override;
    /** AlgTool finalize method */
    virtual StatusCode finalize() override;

    /** process the coherent noise subtruction algorithm and correct TileRawChannel amplitudes */
    virtual StatusCode process (TileMutableRawChannelContainer& rchCont) const override;


  private:

    const TileHWID* m_tileHWID; //!< Pointer to TileHWID

    ToolHandle<TileCondToolEmscale> m_tileToolEmscale{this,
        "TileCondToolEmscale", "TileCondToolEmscale", "Tile EM scale calibration tool"};

    ToolHandle<TileCondToolNoiseSample> m_tileToolNoiseSample{this,
        "TileCondToolNoiseSample", "TileCondToolNoiseSample", "Tile noise sample tool"};

    ToolHandle<ITileBadChanTool> m_tileBadChanTool{this,
        "TileBadChanTool", "TileBadChanTool", "Tile bad channel tool"};

    // properties
    SG::ReadHandleKey<TileDQstatus> m_DQstatusKey{this, "TileDQstatus", 
                                                  "TileDQstatus", 
                                                  "TileDQstatus key"};

    float m_truncationThresholdOnAbsEinSigma;
    float m_minimumNumberOfTruncatedChannels;
    bool m_useTwoGaussNoise;
    bool m_useGapCells;
    float m_maxNoiseSigma;
};

#endif // TILERAWCHANNELNOISEFILTER_H

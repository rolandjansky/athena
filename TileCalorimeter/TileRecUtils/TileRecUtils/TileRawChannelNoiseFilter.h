/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#ifndef TILERAWCHANNELNOISEFILTER_H
#define TILERAWCHANNELNOISEFILTER_H

// Atlas includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

// Tile includes
#include "TileIdentifier/TileRawChannelUnit.h"
#include "TileRecUtils/ITileRawChannelTool.h"
#include "TileConditions/TileCondToolEmscale.h"

// forward declarations
class TileHWID;
class TileRawChannel;
class TileRawChannelContainer;
class TileRawChannelCollection;
class ITileBadChanTool;
//class TileCondToolEmscale;
class TileCondToolNoiseSample;
class TileBeamInfoProvider;

/**
 @class TileRawChannelNoiseFilter
 @brief This tool subtracts common-mode noise from all TileRawChannels in one container
 */
class TileRawChannelNoiseFilter: public AthAlgTool, virtual public ITileRawChannelTool {
  public:

    /** AlgTool like constructor */
    TileRawChannelNoiseFilter(const std::string& type, const std::string& name,
        const IInterface* parent);

    /** Virtual destructor */
    virtual ~TileRawChannelNoiseFilter() {};

    /** AlgTool InterfaceID */
    static const InterfaceID& interfaceID();

    /** AlgTool initialize method.*/
    virtual StatusCode initialize();
    /** AlgTool finalize method */
    virtual StatusCode finalize();

    /** proceed the coherent noise subtruction algorithm and correct TileRawChannel amplitudes */
    virtual StatusCode process(TileRawChannelContainer *rchCnt);

  private:

    const TileHWID* m_tileHWID; //!< Pointer to TileHWID

    ToolHandle<TileCondToolEmscale> m_tileToolEmscale; //!< main Tile Calibration tool
    ToolHandle<TileCondToolNoiseSample> m_tileToolNoiseSample; //!< tool which provided noise values
    ToolHandle<ITileBadChanTool> m_tileBadChanTool;   //!< Tile Bad Channel tool
    ToolHandle<TileBeamInfoProvider> m_beamInfo; //!< Beam Info tool to get the DQ Status object

    // properties
    float m_truncationThresholdOnAbsEinSigma;
    float m_minimumNumberOfTruncatedChannels;
    bool m_useTwoGaussNoise;
    bool m_useGapCells;
    float m_maxNoiseSigma;
};

#endif // TILERAWCHANNELNOISEFILTER_H

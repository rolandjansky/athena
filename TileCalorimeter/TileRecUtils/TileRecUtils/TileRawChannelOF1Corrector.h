/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#ifndef TILERECUTILS_TILERAWCHANNELOF1CORRECTOR_H
#define TILERECUTILS_TILERAWCHANNELOF1CORRECTOR_H

// Atlas includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// Tile includes
#include "TileRecUtils/ITileRawChannelTool.h"

#include <vector>

// forward declarations
class TileRawChannel;
class TileRawChannelContainer;
class TileRawChannelCollection;
class TileHWID;
class TileID;
class TileTBID;
class TileCondToolNoiseSample;
class ITileCondToolOfc;
class TileCondToolTiming;
class TileCondToolEmscale;

/**
 @class TileRawChannelOF1Corrector
 @brief This tool correct TileRawChannels amplitudes which came from OF1 DSP if pedestal changed
 */
class TileRawChannelOF1Corrector: public AthAlgTool, virtual public ITileRawChannelTool {
  public:

    /** AlgTool like constructor */
    TileRawChannelOF1Corrector(const std::string& type, const std::string& name, const IInterface* parent);

    /** Virtual destructor */
    virtual ~TileRawChannelOF1Corrector() {};

    /** AlgTool InterfaceID */
    static const InterfaceID& interfaceID();

    /** AlgTool initialize method.*/
    virtual StatusCode initialize();
    /** AlgTool finalize method */
    virtual StatusCode finalize();

    /** Callback to handle Data-driven GeoModel initialisation */
    virtual StatusCode geoInit(IOVSVC_CALLBACK_ARGS);

    /** Correct TileRawChannel amplitudes if pedestal changed */
    virtual StatusCode process(const TileRawChannelContainer* rawChannelContainer);


  private:

    const TileHWID* m_tileHWID; //!< Pointer to TileHWID
    const TileID* m_tileID;     //!< Pointer to TileID
    const TileTBID* m_tileTBID;     //!< Pointer to TileTBID

    ToolHandle<TileCondToolNoiseSample> m_tileToolNoiseSample; //!< tool which provided noise values
    ToolHandle<ITileCondToolOfc> m_tileCondToolOfc;    
    ToolHandle<TileCondToolTiming> m_tileToolTiming;
    ToolHandle<TileCondToolEmscale> m_tileToolEms;

    std::string m_digitsContainerName;
    bool m_zeroAmplitudeWithoutDigits;
    float m_negativeAmplitudeThreshold;
    float m_positiveAmplitudeThreshold;

    float m_E1AmplitudeThreshold;
    float m_E2AmplitudeThreshold;
    float m_E3AmplitudeThreshold;
    float m_E4AmplitudeThreshold;
    float m_E4PrimeAmplitudeThreshold;
    float m_innerMBTSAmplitudeThreshold;
    float m_outerMBTSAmplitudeThreshold;
    float m_specialC10AmplitudeThreshold;

    std::vector<float> m_thresholds;
    enum CHANNEL_TYPE {NORMAL = 0, E1 = 1, E2 = 2, E3 = 3, E4 = 4, E4PRIME = 5, INNERMBTS = 6, OUTERMBTS = 7, SPECIALC10 = 8, MAX_TYPES = 9};

};

#endif // TILERECUTILS_TILERAWCHANNELOF1CORRECTOR_H

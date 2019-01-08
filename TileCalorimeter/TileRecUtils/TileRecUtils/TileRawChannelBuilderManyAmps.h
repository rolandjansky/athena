/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILERECUTILS_TILERAWCHANNELBUILDERMANYAMPS_H
#define TILERECUTILS_TILERAWCHANNELBUILDERMANYAMPS_H

/********************************************************************
 *
 * NAME:     TileRawChannelBuilderManyamps.h 
 * PACKAGE:  offline/TileCalorimeter/TileRecUtils
 *
 * AUTHOR :  X. Poveda
 * CREATED:  March 2006
 *
 * PURPOSE:  Build TileRawChannels from digits using flat filter
 *
 *  Input: TileDigitsContainer
 *  Output: TileRawChannelContainer
 *  Parameters: TileRawChannelCont - Name of output container in SG
 ********************************************************************/

// Tile includes
#include "TileRecUtils/TileRawChannelBuilder.h"
#include "TileConditions/TileCondToolNoiseSample.h"

class TileHWID;
class TileInfo;
class TileFilterManager;

class TileRawChannelBuilderManyAmps: public TileRawChannelBuilder {
  public:

    // constructor
    TileRawChannelBuilderManyAmps(const std::string& type, const std::string& name,
        const IInterface *parent);
    // destructor
    ~TileRawChannelBuilderManyAmps();

    // virtual methods
    virtual StatusCode initialize();
    virtual StatusCode finalize();

    // Inherited from TileRawChannelBuilder
    virtual TileRawChannel* rawChannel(const TileDigits* digits);

  private:

    int m_digitFilterMode; //<! 2=> start with 1 amplitude and add needed; 3=> start with 8 amplitudes and drop spurious
    int m_digitFilterLevel; //<! number of parameters for fit (3-9 for mode 2)
    int m_digitFilterTest;  //<! non-zero means call Tester (during initialization phase)

    //IChronoStatSvc* m_timeKeeper;

    TileFilterManager* m_tileFilterManagerHi; //<! TileFilterManager for high gain
    TileFilterManager* m_tileFilterManagerLo; //<! TileFilterManager for low gain

    ToolHandle<TileCondToolNoiseSample> m_tileToolNoiseSample{this,
        "TileCondToolNoiseSample", "TileCondToolNoiseSample", "Tile sample noise tool"};

};

#endif

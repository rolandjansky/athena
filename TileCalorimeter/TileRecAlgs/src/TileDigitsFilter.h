/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileDigitsFilter.h
// Author   : Alexander Solodkov
// Created  : June 2008
//
// DESCRIPTION
// 
// Copy TileDigits from input container to output container
// keeping only channels with (max-min) sample above threshold
//
// Properties (JobOption Parameters):
//
//    InputDigitsContainer      string   Name of container with TileDigits to read
//    OutputDigitsContainer     string   Name of container with TileDigits to write
//    InputRawChannelContainer  string   Name of container with TileRawChannels to read
//    OutputRawChannelContainer string   Name of container with TileRawChannels to write
//    HighGainThereshold        integer  thereshold on max-min sample in low gain
//    LowGainThereshold         integer  thereshold on max-min sample in high gain
//
//
// BUGS:
//  
// History:
//  
//  
//****************************************************************************

#ifndef TILERECALGS_TILEDIGITSFILTER_H
#define TILERECALGS_TILEDIGITSFILTER_H

// Tile includes
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileConditions/TileCablingSvc.h"

// Atlas includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

class TileHWID;

#include <string>
#include <vector>

/** 
 @class TileDigitsFilter
 @brief This algorithm copies TileDigits from input container to output container
 keeping only channels with (max-min) sample above threshold
 */
class TileDigitsFilter: public AthReentrantAlgorithm {
  public:
    // Constructor
    TileDigitsFilter(const std::string& name, ISvcLocator* pSvcLocator);

    //Destructor 
    virtual ~TileDigitsFilter();

    //Gaudi Hooks
    virtual StatusCode initialize() override; //!< initialize method
    virtual StatusCode execute(const EventContext& ctx) const override;    //!< execute method
    virtual StatusCode finalize() override;   //!< finalize method

  private:

    SG::ReadHandleKey<TileDigitsContainer> m_inputDigitsContainerKey{this,
        "InputDigitsContainer", "TileDigitsCnt", "Input Tile digits container key"};

    SG::WriteHandleKey<TileDigitsContainer> m_outputDigitsContainerKey{this,
        "OutputDigitsContainer", "TileDigitsFlt","Output Tile digits container key"};

    SG::ReadHandleKey<TileRawChannelContainer> m_inputRawChannelContainerKey{this,
        "InputRawChannelContainer", "TileRawChannelCnt", "Input Tile raw channels container key"};

    SG::WriteHandleKey<TileRawChannelContainer> m_outputRawChannelContainerKey{this,
        "OutputRawChannelContainer", "TileRawChannelFlt", "Output Tile digits container key"};

    Gaudi::Property<int> m_lowGainThreashold{this,
        "LowGainThereshold", 0, "Low gain threshold to keep digits"}; // keep all LG except zeros

    Gaudi::Property<int> m_highGainThreashold{this,
        "HighGainThereshold", 10, "High gain threshold to keep digits"}; // keep signals above ~128(106) MeV in A,BC(D) samplings

    const TileHWID* m_tileHWID;

    /**
     * @brief Name of Tile cabling service
     */
    ServiceHandle<TileCablingSvc> m_cablingSvc{ this,
        "TileCablingSvc", "TileCablingSvc", "The Tile cabling service"};

    int m_threshold[2]{};  //!< Threshold value for low and high gain
};

#endif // TILERECALGS_TILEDIGITSFILTER_H

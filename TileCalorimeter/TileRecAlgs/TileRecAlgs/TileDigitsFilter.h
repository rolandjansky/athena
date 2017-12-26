/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

// Atlas includes
#include "AthenaBaseComps/AthAlgorithm.h"
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
class TileDigitsFilter: public AthAlgorithm {
  public:
    // Constructor
    TileDigitsFilter(std::string name, ISvcLocator* pSvcLocator);

    //Destructor 
    virtual ~TileDigitsFilter();

    //Gaudi Hooks
    StatusCode initialize(); //!< initialize method
    StatusCode execute();    //!< execute method
    StatusCode finalize();   //!< finalize method

  private:
    std::string m_inputContainer;  //!< Name of the input TileDigitsContainer
    std::string m_outputContainer; //!< Name of the output TileDigitsContainer
    std::string m_inRchContainer; //!< Name of the input TileRawChannelContainer
    std::string m_outRchContainer; //!< Name of the output TileRawChannelContainer

    SG::ReadHandleKey<TileDigitsContainer> m_inputDigitsContainerKey{this,"InputDigitsContainer",
                                                                       "TileDigitsCnt", "Input Tile digits container key"};

    SG::WriteHandleKey<TileDigitsContainer> m_outputDigitsContainerKey{this,"OutputDigitsContainer",
                                                                       "TileDigitsFlt","Output Tile digits container key"};


    SG::ReadHandleKey<TileRawChannelContainer> m_inputRawChannelContainerKey{this,"InputRawChannelContainer",
                                                                             "TileRawChannelCnt", 
                                                                             "Input Tile raw channels container key"};

    SG::WriteHandleKey<TileRawChannelContainer> m_outputRawChannelContainerKey{this,"OutputRawChannelContainer",
                                                                               "TileRawChannelFlt",
                                                                               "Output Tile digits container key"};


    const TileHWID* m_tileHWID;

    int m_threshold[2];  //!< Threshold value for low and high gain
};

#endif // TILERECALGS_TILEDIGITSFILTER_H

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileHitToRawChannel.h
// Author   : UC-ATLAS TileCal group
// Created  : April 2002
//
// DESCRIPTION
// 
// To create TileRawChannel from TileHit
//
// Properties (JobOption Parameters):
//
//    TileHitContainer          string   Name of container with TileHit to read
//    TileRawChannelContainer   string   Name of CaloCellContainer to write
//    TileInfoName              string   Name of object in TDS with all parameters
//
// BUGS:
//  
// History:
//  
//  
//****************************************************************************

#ifndef TILESIMALGS_TILEHITTORAWCHANNEL_H
#define TILESIMALGS_TILEHITTORAWCHANNEL_H

// Tile includes
#include "TileEvent/TileHitContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileIdentifier/TileFragHash.h"
#include "TileIdentifier/TileRawChannelUnit.h"

// Atlas includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "CLHEP/Random/RandomEngine.h"


#include <string>
#include <vector>


class IAtRndmGenSvc;
class TileID;
class TileTBID;
class TileHWID;
class TileInfo;
class HWIdentifier;
class TileCablingService;
class TileCondToolEmscale;


/**
 @class TileHitToRawChannel
 @brief This algorithm builds TileRawChannels from TileHits. TileRawChannel amplitude, time and quality are obtianed from the TileHits. Noise is applied to the amplitude.

 TileHitToRawChannell takes as input the TileRawChanels in a TileRawChannel container and
 combine them in order to create TileCells (For barrel and ext.barrel, two RawChanels
 feed into each cell; for the Gap cells, there is only one PMT.)
 Calibrations constants are applied, and the total energy in the
 cell, the mean time of the energy deposition, and the quality
 of the measurement are returned.
 */
class TileHitToRawChannel: public AthAlgorithm {
  public:

    TileHitToRawChannel(std::string name, ISvcLocator* pSvcLocator); //!< Constructor

    virtual ~TileHitToRawChannel(); //!< Destructor                         

    StatusCode initialize(); //!< initialize method   
    StatusCode execute();    //!< execute method   
    StatusCode finalize();   //!< finalize method   

  private:

    SG::ReadHandleKey<TileHitContainer> m_hitContainerKey{this,"TileHitContainer","TileHitCnt",
                                                          "input Tile hit container key"};

    SG::WriteHandleKey<TileRawChannelContainer> m_rawChannelContainerKey{this,"TileRawChannelContainer",
                                                                         "TileRawChannelCnt",
                                                                         "Output Tile raw channel container key"};

    std::string m_infoName;             //!< name of the TileInfo object
    double m_deltaT;                    //!< if true, keep only hits in deltaT range
    bool m_calibrateEnergy;             //!< if true, amplitude is converted to pCb

    TileRawChannelUnit::UNIT m_rChUnit; //!< Units used for the TileRawChannels (ADC, pCb, etc.)(see TileInfo.h)
    TileFragHash::TYPE m_rChType; //!< Type of TileRawChannels (Digitizar, OF1, OF2, Fit, etc.)(see TileFragHash.h)

    const TileID* m_tileID;   //!< Pointer to TileID helper
    const TileTBID* m_tileTBID; //!< Pointer to TileID helper
    const TileHWID* m_tileHWID; //!< Pointer to TileHWID helper
    const TileInfo* m_tileInfo; //!< Pointer to TileInfo
    const TileCablingService* m_cabling;  //!< Pointer to the TileCablingService instance

    std::vector<HWIdentifier *> m_all_ids; //!< Vector to store all the drawer ids

    bool m_tileNoise;    //!< If true => generate noise for the TileRawChannel creation
    bool m_tileThresh;   //!< If true => apply threshold on the conversion to raw channels
    double m_threshHi; //!< Value of the mimimal amplitude required to do the conversion to raw channel in high gain (not used for low gain)
    double m_ampMaxHi; //!< Value of the maximum amplitude to be stored as a high gain channel. For larger amplitudes, the channel is converted to low gain

    IAtRndmGenSvc * m_atRndmGenSvc; //!< Random number generator engine to use
    CLHEP::HepRandomEngine * m_pHRengine;  //!< Random number service to use

    ToolHandle<TileCondToolEmscale> m_tileToolEmscale; //!< main Tile Calibration tool
};

#endif // TILESIMALGS_TILEHITTORAWCHANNEL_H

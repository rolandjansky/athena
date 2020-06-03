/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileHitToTTL1.h
// Author   : F. Merritt, UC-ATLAS TileCal group
// Created  : Feb 2003
//
// DESCRIPTION
// 
// Creates TileTTL1 Digits from TileHit
//
// Properties (JobOption Parameters):
//
//    TileHitContainer          string   Name of container with TileHit to read
//    TileTTL1Container         string   Name of container with TileTTL1Digits to write
//    TileInfoName              string   Name of object in TDS with all parameters
//
// BUGS:
//  
// History:
//  
//  
//****************************************************************************

#ifndef TILESIMALGS_TILEHITTOTTL1_H
#define TILESIMALGS_TILEHITTOTTL1_H

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "CLHEP/Random/RandomEngine.h"

#include "AthenaBaseComps/AthAlgorithm.h"

#include "TileEvent/TileHitContainer.h"
#include "TileEvent/TileTTL1Container.h"

class IAtRndmGenSvc;
class TileID;
class TileTBID;
class TileHWID;
class TileInfo;
class CaloLVL1_ID;
class TileCablingService;
class TileCondToolEmscale;
class ITileBadChanTool;

#include <string>

/**
 @class TileHitToTTL1
 @brief This algorithm builds TileTTL1 objects from TileHits.

 It simulates the Tile Level 1 Trigger Towers (TTL1), which are
 hardware sums of the Tile channels, with about 5 channels per tower.
 The towers are read out in N time slices, with N=7 as a default.
 The default running mode is 'standard' which produces N read out slices
 The 'cosmics' running mode is also possible which stores only the peak
 value of the pulse (this mode is used to simulate the Tile cosmics boards used
 during pre-beam commissioning).
 Noise can be added to every sample and threshold can be applied. Dead/half-gain PMTs
 are also accounted for in this algorithm.
 */
class TileHitToTTL1: public AthAlgorithm {
  public:

    TileHitToTTL1(std::string name, ISvcLocator* pSvcLocator); //!< Constructor

    virtual ~TileHitToTTL1(); //!< Destructor                          

    StatusCode initialize(); //!< initialize method      
    StatusCode execute();    //!< execute method   
    StatusCode finalize();   //!< finalize method   

  private:
    SG::ReadHandleKey<TileHitContainer> m_hitContainerKey;
    SG::WriteHandleKey<TileTTL1Container> m_TTL1ContainerKey;
    SG::WriteHandleKey<TileTTL1Container> m_MBTSTTL1ContainerKey;

    std::string m_infoName;          //!< name of TileInfo object in TES
    std::string m_TileTTL1Type;      //!< name of Trigger Type
    bool m_cosmicsType;              //!< if true => use dediated cosmcis TTL1

    const TileID* m_tileID;   //!< Pointer to TileID helper
    const TileTBID* m_tileTBID; //!< Pointer to TileID helper
    const TileHWID* m_tileHWID; //!< Pointer to TileHWID helper
    const TileInfo* m_tileInfo; //!< Pointer to TileInfo
    const CaloLVL1_ID* m_TT_ID; //!< Pointer to TT Identifier
    const TileCablingService* m_cabling; //!< Pointer to the TileCablingService instance

    std::vector<double> m_TTL1Shape;          //!< vector to store the pulse shape for the TT

    // Variables for the trigger towers
    int m_nSamples;     //!< number of time slices for each channel
    int m_iTrig;          //!< index of the triggering time slice

    // Variables for the MBTS
    int m_MBTSnSamples; //!< number of time slices for each chan for MBTS TTL1
    int m_MBTSiTrig;      //!< index of the triggering time slice for MBTS TTL1

    int m_lastTower;    //!< last tower (needed for the loops)

    bool m_tileNoise;   //!< If true => generate noise for the TileTTL1 creation
    bool m_tileThresh;  //!< If true => apply threshold on the conversion to TileTTL1

    CLHEP::HepRandomEngine* m_pHRengine; //!< Random number service to use

    ServiceHandle<IAtRndmGenSvc> m_rndmSvc; //!< Random number generator engine to use

    ToolHandle<TileCondToolEmscale> m_tileToolEmscale; //!< main Tile Calibration tool

    bool m_maskBadChannels;      //!< if true then bad channels are masked
    ToolHandle<ITileBadChanTool> m_tileBadChanTool; //!< Tile Bad Channel tool

};

#endif // TILESIMALGS_TILEHITTOTTL1_H

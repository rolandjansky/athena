/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileTBHitToBeamElem.h
// Author   : Alexander Solodkov
// Created  : July 2004
//
// DESCRIPTION
// 
// To create TileBeamElem container from TileHitVector with hits from ancillary detectors  
//
// Properties (JobOption Parameters):
//
//    TileTBHitVector           string   Name of TileHitVector to read
//    TileBeamElemContainer     string   Name of BeamElemContainer to write
//    TileInfoName              string   Name of object in TDS with all parameters
//
// BUGS:
//  
// History:
//  
//  
//****************************************************************************

#ifndef TILESIMALGS_TILETBHITTOBEAMELEM_H
#define TILESIMALGS_TILETBHITTOBEAMELEM_H

// Tile includes
#include "TileSimEvent/TileHitVector.h"
#include "TileEvent/TileBeamElemContainer.h"

// Atlas includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

class TileTBID;
class TileHWID;
class TileInfo;
class HWIdentifier;

#include <string>
#include <vector>

/**
 @class TileTBHitToBeamElem
 @brief This algorithm creates TileBeamElem container from TileHitVector with hits from ancillary detectors for testbeam simulation

 */
class TileTBHitToBeamElem: public AthAlgorithm {
  public:

    TileTBHitToBeamElem(std::string name, ISvcLocator* pSvcLocator); //!< Constructor

    virtual ~TileTBHitToBeamElem(); //!< Destructor                         

    StatusCode initialize();  //!< initialize method   
    StatusCode execute();     //!< execute method   
    StatusCode finalize();    //!< finalize method  

  private:

    SG::ReadHandleKey<TileHitVector> m_hitVectorKey{this,"TileTBHitVector","TileTBHits",
                                                   "input Tile hit vector key"};

    SG::WriteHandleKey<TileBeamElemContainer> m_beamElemContainerKey{this,"TileBeamElemContainer",
                                                                     "TileBeamElemCnt",
                                                                     "Output Tile beam elements container key"};


    std::string m_infoName;             //!< name of the TileInfo object in TES

    const TileTBID* m_tileTBID; //!< Pointer to TileID helper
    const TileHWID* m_tileHWID; //!< Pointer to TileHWID helper
    const TileInfo* m_tileInfo; //!< Pointer to TileInfo

};

#endif // TILESIMALGS_TILETBHITTOBEAMELEM_H

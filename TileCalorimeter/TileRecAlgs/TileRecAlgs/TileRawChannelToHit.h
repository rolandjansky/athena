/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileRawChannelToHit.h
// Author   : Alexander Solodkov
// Created  : Feb 2005
//
// DESCRIPTION
// 
//    Convert RawChannel amplitude to total energy (like in TileCell)
// but save it as TileHit 

// Properties (JobOption Parameters):
//
//    TileRawChannelContainer   string   Name of container with TileRawChannel to read
//    TileHitContainer          string   Name of TileHitContainer to write
//    TileInfoName              string   Name of object in TDS with all parameters
//
// BUGS:
//  
// History:
//   15Feb05  Created from TileRawChannelToCell
//  
//****************************************************************************

#ifndef TILERECALGS_TILERAWCHANNELTOHIT_H
#define TILERECALGS_TILERAWCHANNELTOHIT_H

// Tile includes
#include "TileEvent/TileRawChannelContainer.h"
#include "TileSimEvent/TileHitVector.h"

// Atlas includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"

// C++ STL includes
#include <string>

// Avoid pushing dependencies into clients- just fwd declare the following:

class TileID;
class TileHWID;
class TileHit;
class TileInfo;
class TileCablingService;
class TileCondToolEmscale;


class TileRawChannelToHit: public AthAlgorithm {
  public:
    // Constructor
    TileRawChannelToHit(const std::string& name, ISvcLocator* pSvcLocator);

    //Destructor 
    virtual ~TileRawChannelToHit();

    //Gaudi Hooks
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

  private:

    SG::ReadHandleKey<TileRawChannelContainer> m_rawChannelContainerKey{this,"TileRawChannelContainer",
                                                                        "TileRawChannelCnt", 
                                                                        "Input Tile raw channel container key"};

    SG::WriteHandleKey<TileHitVector> m_hitVectorKey{this,"TileHitContainer",
                                                      "TileHitVec","Output Tile hit container key"};


    std::string m_infoName;
    bool m_useSamplFract;

    const TileID* m_tileID;
    const TileHWID* m_tileHWID;
    const TileInfo* m_tileInfo;

    ToolHandle<TileCondToolEmscale> m_tileToolEmscale; //!< main Tile Calibration tool
};

#endif // TILERECALGS_TILERAWCHANNELTOHIT_H

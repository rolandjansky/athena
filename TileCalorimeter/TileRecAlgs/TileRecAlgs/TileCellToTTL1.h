/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//****************************************************************************
// Filename : TileCellToTTL1.h
// Author   : Monica Dunford
// Created  : Oct 2009
//
// DESCRIPTION
// 
// Creates TileTTL1 tower energy from TileCell
//
// Properties (JobOption Parameters):
//
//    CaloCellContainer         string   Name of container with CaloCells to read
//    TileCellTTL1Container     string   Name of container with TileTTL1Digits to write
//
// BUGS:
//  
// History:
//  
//  
//****************************************************************************

#ifndef TILERECALGS_TILECELLTOTTL1_H
#define TILERECALGS_TILECELLTOTTL1_H

// Tile includes
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileContainer.h"

// Calo includes
#include "CaloEvent/CaloCellContainer.h"

// Atlas includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"

class TileID;
class CaloLVL1_ID;
class TileCablingService;

#include <string>

class TileCellToTTL1: public AthAlgorithm {
  public:
    // Constructor
    TileCellToTTL1(std::string name, ISvcLocator* pSvcLocator);

    //Destructor 
    virtual ~TileCellToTTL1();

    //Gaudi Hooks
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

  private:


    SG::ReadHandleKey<CaloCellContainer> m_cellContainerKey{this,"CaloCellContainer",
                                                            "AllCalo", "Input Calo cell container key"};

    SG::WriteHandleKey<TileTTL1CellContainer> m_ttl1CellContainerKey{this,"TileTTL1CellContainer",
                                                                     "TileTTL1CellContainer",
                                                                     "Output Tile TTL1 cell container key"};



    const TileID* m_tileID;
    const CaloLVL1_ID* m_TT_ID;
    const TileCablingService* m_tileCablingService;

};

#endif // TILERECALGS_TILECELLTOTTL1_H


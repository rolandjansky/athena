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

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

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

    std::string m_caloCellContainer;    // name of CaloCellContainer
    std::string m_cellTTL1Container;   // name of TileCellTTL1Container

    const TileID* m_tileID;
    const CaloLVL1_ID* m_TT_ID;
    const TileCablingService* m_tileCablingService;

};

#endif // TILERECALGS_TILECELLTOTTL1_H


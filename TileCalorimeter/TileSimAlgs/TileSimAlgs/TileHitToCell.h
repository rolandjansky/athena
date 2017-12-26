/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileHitToCell.h
// Author   : UC-ATLAS TileCal group
// Created  : April 2002
//
//****************************************************************************

#ifndef TILESIMALGS_TILEHITTOCELL_H
#define TILESIMALGS_TILEHITTOCELL_H

// Tile includes
#include "TileEvent/TileHitContainer.h"

// Calo includes
#include "CaloEvent/CaloCellContainer.h"

// Atlas includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include <string>

class TileID;
class TileInfo;
class TileDetDescrManager;


/**
 @class TileHitToCell
 @brief This algorithm builds TileCells from TileHits. Hits from the PMTs in the cell are merged and calibration constants applied to obtained cell energy, time and quality.

 TileHitToCell groups some PMT's output (energy deposit)
 contained in TileHit. (For barrel and ext.barrel, two Hits
 feed into each cell; for the Gap cells, there is only one PMT.)
 Calibrations constants are applied, and the total energy in the
 cell, the mean time of the energy deposition, and the quality
 of the measurement are returned.
 Clearly different things are needed for different data, e.g.,
 full simulation vs. fast simulation vs. data.  LAr handles this by
 a strategy of having a top level algorithm that calls builder sub-algs
 and a list of cell-correction sub-algs.  It is not clear to us yet
 how we (tile) will want to tie to the database nor what level of
 complexity will exist in Tile for this task, so we will start with
 the simplest thing, TileCellBuilderAlg, which will do both the
 converstion of Hit to Cell and correctsions/scaling.
 */

class TileHitToCell: public AthAlgorithm {
  public:

    TileHitToCell(std::string name, ISvcLocator* pSvcLocator);  //!< Constructor

    virtual ~TileHitToCell(); //!< Destructor                         

    StatusCode initialize(); //!< initialize method
    StatusCode execute();    //!< execute method
    StatusCode finalize();   //!< finalize method

  private:

    SG::ReadHandleKey<TileHitContainer> m_hitContainerKey{this,"TileHitContainer","TileHitContainer",
                                                          "input Tile hit container key"};

    SG::WriteHandleKey<CaloCellContainer> m_cellContainerKey{this,"TileCellContainer",
                                                             "TileCellContainer",
                                                             "Output Tile cell container key"};


    std::string m_infoName;      //!< Name for TileInfo

    const TileID* m_tileID;    //!< Pointer to TileID helper
    const TileInfo* m_tileInfo;  //!< Pointer to TileInfo
    const TileDetDescrManager* m_tileMgr; //!< Pointer to TileDetDescrManager
};

#endif // TILESIMALGS_TILEHITTOCELL_H

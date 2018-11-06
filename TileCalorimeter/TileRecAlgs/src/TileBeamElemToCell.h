/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileCellFromRawChannel.h
// Author   : Ed Frank, Ambreesh Gupta, Frank Merritt
// Created  : Mar 2002
//
// DESCRIPTION
// 
//    TileCellFromRawChannel extracts the energy depositied in each
// cell from the PMT amplitudes contained in TileRawChannel.  (For
// barrel and extended-barrel, two PMT's feed into each cell; for the
// Gap cells, there is only one PMT.)  Calibrations constants are
// applied, and the total energy in the cell, the mean time of the
// energy deposition, and the quality of the measurement are calculated
// here. is returned.
//
//    Clearly different things are needed for different data, e.g., 
// full simulation vs. fast simulation vs. data.  LAr handles this by
// a strategy of having a top level algorithm that calls builder sub-algs
// and a list of cell-correction sub-algs.  It is not clear to us yet
// how we (tile) will want to tie to the database nor what level of
// complexity will exist in Tile for this task, so we will start with
// the simplest thing, TileCellBuilderAlg, which will do both the
// converstion of Raw to Cell and correctsions/scaling.
//
// Properties (JobOption Parameters):
//
//    TileRawChannelContainer   string   Name of container with TileRawChannel to read
//    TileCellContainer         string   Name of CaloCellContainer to write
//    TileInfoName              string   Name of object in TDS with all parameters
//
// BUGS:
//  
// History:
//   02Mar02  Created from TileCellBuilderAlg.
//  
//****************************************************************************

#ifndef TILERECALGS_TILEBEAMELEMTOCELL_H
#define TILERECALGS_TILEBEAMELEMTOCELL_H

// Tile includes
#include "TileEvent/TileBeamElemContainer.h"

// Calo includes
#include "CaloEvent/CaloCellContainer.h"

// Atlas includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"


// Avoid pushing dependencies into clients- just fwd declare the following:

class TileTBID;
class TileHWID;
class TileCell;
class TileInfo;
class TileDetDescrManager;

// C++ STL includes
#include <string>
#include <vector>

class TileBeamElemToCell: public AthAlgorithm {
  public:
    // Constructor
    TileBeamElemToCell(const std::string& name, ISvcLocator* pSvcLocator);

    //Destructor 
    virtual ~TileBeamElemToCell();

    //Gaudi Hooks
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

  private:

    SG::ReadHandleKey<TileBeamElemContainer> m_beamElemContainerKey{this,"TileBeamElemContainer",
                                                                     "TileBeamElemCnt",
                                                                     "Input Tile beam elements container key"};

    SG::WriteHandleKey<CaloCellContainer> m_cellContainerKey{this,"TileTBCellContainer",
                                                             "TileTBCellCnt", "Output Calo cell container key"};


    std::string m_infoName;

    const TileTBID* m_tileTBID;
    const TileHWID* m_tileHWID;
    const TileInfo* m_tileInfo;
    const TileDetDescrManager* m_tileMgr;

//    std::vector<TileCell *> m_allCells;

// Compute calibrated energy, time, etc. for TileCell and adjust it.
    void correctCell(TileCell* pCell, int correction, int pmt, int gain,
                       double ener, double time, double qual);
};

#endif // TILERECALGS_TILEBEAMELEMTOCELL_H

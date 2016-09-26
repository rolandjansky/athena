/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileCellIDCToCell.h
// Author   : Alexandre Solodkov
// Created  : Jan 2003
//
// DESCRIPTION
// 
//    TileCellIDCToCell copies all TileCells from TileIDC container
// to TileCellContainer without any corrections
//
// Properties (JobOption Parameters):
//
//    TileCellIDC               string   Name of TileIDC container to read
//    TileCellContainer         string   Name of CaloCellContainer to write
//    TileInfoName              string   Name of object in TDS with all parameters
//
// BUGS:
//  
// History:
//   02Mar02  Created from TileRawChannelToCell.
//  
//****************************************************************************

#ifndef TILERECALGS_TILECELLIDCTOCELL_H
#define TILERECALGS_TILECELLIDCTOCELL_H

#include "AthenaBaseComps/AthAlgorithm.h"

// Avoid pushing dependencies into clients- just fwd declare the following:

class TileID;
class TileCell;
class TileInfo;
class TileDetDescrManager;

// C++ STL includes
#include <string>

class TileCellIDCToCell: public AthAlgorithm {
  public:
    // Constructor
    TileCellIDCToCell(const std::string& name, ISvcLocator* pSvcLocator);

    // Destructor
    virtual ~TileCellIDCToCell();

    // Gaudi Hooks
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

  private:
    std::string m_cellIDC;
    std::string m_cellContainer;
    std::string m_infoName;

    const TileID* m_tileID;
    const TileInfo* m_tileInfo;
    const TileDetDescrManager* m_tileMgr;
};

#endif // TILERECALGS_TILECELLIDCTOCELL_H

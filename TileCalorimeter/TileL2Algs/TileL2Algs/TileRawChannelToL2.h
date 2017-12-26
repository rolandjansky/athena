/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileRawChannelToL2.h
// Author   : Aranzazu Ruiz
// Created  : February 2007
//
// DESCRIPTION
//    TileL2 from TileRawChannel.
//
// BUGS:
//  
// History:
//  
//****************************************************************************

#ifndef TILEL2ALGS_TILERAWCHANNELTOL2_H
#define TILEL2ALGS_TILERAWCHANNELTOL2_H

// Tile includes
#include "TileEvent/TileContainer.h"

// Athena includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/WriteHandleKey.h"

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"

// C++ STL includes
#include <string>
#include <vector>

// Forward declaration
class TileL2Builder;

/**
 * @class TileRawChannelToL2
 * @brief This class emulates the algorithms processed at the TileCal ROD DSP level to contribute to the LVL2 trigger:
 * a low Pt muon tagging algorithm and the Et calculation per superdrawer.
 * @author Aranzazu Ruiz
 * @author Carlos Solans
 *
 * This class retrieves the TileRawChannelContainer, processes both algorithms separately in each superdrawer and the results
 * are stored in a TileL2Container in StoreGate.
 */

class TileRawChannelToL2: public AthAlgorithm {

  public:

    /** Constructor */
    TileRawChannelToL2(const std::string& name, ISvcLocator* pSvcLocator);

    /** Destructor */
    virtual ~TileRawChannelToL2();

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

  private:

    /** Pointer to TileL2Builder */
    ToolHandle<TileL2Builder> m_tileL2Builder;

    /** TileL2Container in detector store */
    SG::WriteHandleKey<TileL2Container> m_l2ContainerKey{this,"TileL2Container","TileL2Cnt",
                                                         "Output Tile L2 container key"};


};

#endif // TILEL2ALGS_TILERAWCHANNELTOL2_H

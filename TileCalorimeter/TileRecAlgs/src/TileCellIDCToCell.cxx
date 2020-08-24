/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileCellIDCToCell.cxx
//  Author   : Alexandre Solodkov
//  Created  : Jan, 2003
//
//  DESCRIPTION:
//     Implementation comments only.  Class level comments go in .h file.
//
//  HISTORY:
//
//  BUGS:
//
//*****************************************************************************


#include "TileCellIDCToCell.h"


// access all Cells inside container
#include "AthenaKernel/errorcheck.h"
#include "EventContainers/SelectAllObject.h" 

// Calo includes
#include "CaloIdentifier/TileID.h"
#include "CaloEvent/CaloCellContainer.h"

// Tile includes
#include "TileDetDescr/TileDetDescrManager.h"
#include "TileConditions/TileInfo.h"
#include "TileEvent/TileCellIDC.h"
#include "TileEvent/TileCell.h"


//
// Constructor
//
TileCellIDCToCell::TileCellIDCToCell(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_tileID(0)
  , m_tileInfo(0)
  , m_tileMgr(0)
{
  declareProperty("TileCellIDC", m_cellIDC = "TileCellIDC");                    // Name of input container
  declareProperty("TileCellContainer", m_cellContainer = "TileCellContainer");        // Name of output container
  declareProperty("TileInfoName", m_infoName = "TileInfo");                  // name of TileInfo store
}

TileCellIDCToCell::~TileCellIDCToCell()
{}

//****************************************************************************
//* Initialization
//****************************************************************************

StatusCode TileCellIDCToCell::initialize() 
{ 

  // retrieve Tile detector manager, TileID helper and TileIfno from det store
  CHECK( detStore()->retrieve(m_tileMgr) );
  CHECK( detStore()->retrieve(m_tileID) );
  CHECK( detStore()->retrieve(m_tileInfo, m_infoName) );

  ATH_MSG_INFO( "TileCellIDCToCell initialization completed" );

  return StatusCode::SUCCESS;
}


//****************************************************************************
//* Execution
//****************************************************************************

StatusCode TileCellIDCToCell::execute()
{

  /* zero all counters and sums */
  int nCell = 0;
  float eCellTot = 0.0;

  CaloCellContainer* cells = new CaloCellContainer;
 
  //**
  //* Get TileCells from TileIDC
  //**
  const TileCellIDC* inputCells;

  if (evtStore()->retrieve(inputCells, m_cellIDC).isFailure() ) {

    ATH_MSG_WARNING( " Could not find container " << m_cellIDC );
    ATH_MSG_WARNING( " creating empty cell container " );

  } else {

    SelectAllObject<TileCellIDC> selAll(inputCells); 
    SelectAllObject<TileCellIDC>::const_iterator inpItr=selAll.begin();
    SelectAllObject<TileCellIDC>::const_iterator end=selAll.end();

    TileCell * c;
    const TileCell * cinp;

    //**
    //* Iterate over cells, creating new TileCells 
    //* Add each TileCell to the CaloCellContainer.
    //**

    for( ; inpItr != end; ++inpItr) {
      
      cinp = (*inpItr);

      ++nCell;
      eCellTot += cinp->energy();

      c = new TileCell( cinp );
      cells->push_back( c );

      ATH_MSG_VERBOSE( "TileCellIDCToCell:  cell_id="
          << m_tileID->to_string(cinp->subcalo_hash(),-2)
          << " nCell=" << nCell << " energy=" << cinp->energy());
    }
  }
  
  /* Register the set of TileCells to the event store. */

  CHECK( evtStore()->record(cells, m_cellContainer, false ) );

  // Execution completed.
  ATH_MSG_DEBUG( "TileCellIDCToCell execution completed." );
  ATH_MSG_DEBUG( " nCell=" << nCell << " eneTot=" << eCellTot );

  ATH_MSG_VERBOSE( "TileCell container registered to the TES with name"
      << m_cellContainer );

  return StatusCode::SUCCESS;
}

//************************************************************************


//****************************************************************************
//* Finalize
//****************************************************************************

StatusCode TileCellIDCToCell::finalize()
{
  
  ATH_MSG_INFO( "TileCellIDCToCell::finalize() end" );

  return StatusCode::SUCCESS;
}


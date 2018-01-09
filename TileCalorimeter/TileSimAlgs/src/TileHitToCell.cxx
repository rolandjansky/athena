/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileHitToCell.cxx
//  Author   : Zhifang
//  Created  : April, 2002
//
//  DESCRIPTION:
//     Implement the TileHitToCell class
//
//  HISTORY:
//
//  BUGS:
//
//*****************************************************************************

// Tile includes
#include "TileSimAlgs/TileHitToCell.h"
#include "TileDetDescr/TileDetDescrManager.h"
#include "TileConditions/TileInfo.h"
#include "TileEvent/TileCell.h"

// Calo includes
#include "CaloIdentifier/TileID.h"
#include "CaloDetDescr/CaloDetDescrElement.h"

// Atlas includes
// access all Hits inside container
#include "EventContainers/SelectAllObject.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"


//
// Constructor
//
TileHitToCell::TileHitToCell(std::string name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_tileID(0)
  , m_tileInfo(0)
  , m_tileMgr(0)
{
  m_infoName = "TileInfo";
  declareProperty("TileInfoName", m_infoName);
}

TileHitToCell::~TileHitToCell() {
}

//
// Alg standard initialize function
//
StatusCode TileHitToCell::initialize() {

  // retrieve Tile detector manager, TileID helper and TileInfo from det store

  CHECK( detStore()->retrieve(m_tileMgr) );

  CHECK( detStore()->retrieve(m_tileID) );

  CHECK( detStore()->retrieve(m_tileInfo, m_infoName) );

  ATH_CHECK( m_hitContainerKey.initialize() );
  ATH_CHECK( m_cellContainerKey.initialize() );

  ATH_MSG_INFO( "TileHitToCell initialisation completed" );

  return StatusCode::SUCCESS;
}

//
// Alg standard execute function
//
StatusCode TileHitToCell::execute() {

  ATH_MSG_DEBUG( "Executing TileHitToCell" );

  // step1: read hits from TES
  SG::ReadHandle<TileHitContainer> hitContainer(m_hitContainerKey);
  ATH_CHECK( hitContainer.isValid() );

  //Zero sums for monitoring.
  int nTwo = 0;
  int nChan = 0;
  int nCell = 0;
  double eHitTot = 0.0;
  double eCellTot = 0.0;

  // step2: to figure out the cell energy from the hits obtained above
  unsigned int nCellMax = m_tileMgr->tile_cell_size();
  std::vector<double> enePmt1;
  std::vector<double> enePmt2;
  enePmt1.resize(nCellMax);
  enePmt2.resize(nCellMax);

  SelectAllObject<TileHitContainer> selAll(hitContainer.cptr());
  SelectAllObject<TileHitContainer>::const_iterator hitItr = selAll.begin();
  SelectAllObject<TileHitContainer>::const_iterator lastHit = selAll.end();

  IdentifierHash cellHash_id;
  IdContext cellContext = m_tileID->cell_context();

  for (; hitItr != lastHit; ++hitItr) {

    // Get hit id (logical pmt id)
    Identifier pmt_id = (*hitItr)->identify();
    int pmt = m_tileID->pmt(pmt_id); // 0 or 1 - first or second PMT of the cell

    // Get logical ID of cell
    Identifier cell_id = m_tileID->cell_id(pmt_id);
    m_tileID->get_hash(cell_id, cellHash_id, &cellContext);

    // Convert hit energy deposit to cell energy
    double eHit = (*hitItr)->energy();
    double fact = m_tileInfo->HitCalib(pmt_id);
    double eCell = eHit * fact;

    ++nChan;
    eHitTot += eHit;
    eCellTot += eCell;

    // Add energy to existing entry for this cell, or
    // make new cell entry if there is no previous entry for this cell
    if (0 == pmt) {
      ++nCell;
      enePmt1[cellHash_id] = eCell;
    } else {
      ++nTwo;
      enePmt2[cellHash_id] = eCell;
    }

    ATH_MSG_VERBOSE( "TileHitToCell: "
                    << " nChan=" << nChan
                    << " pmt_id=" << m_tileID->to_string(pmt_id, -1)
                    << " eHit=" << eHit
                    << " nCell=" << nCell
                    << " cell_id=" << m_tileID->to_string(cell_id, -2)
                    << " eCell=" << eCell );
  }

  // step3: form cells, and put them in container

  SG::WriteHandle<CaloCellContainer> cellContainer(m_cellContainerKey);
  ATH_CHECK( cellContainer.record(std::make_unique<CaloCellContainer>()) );
  ATH_MSG_VERBOSE( "TileCell container registered to the TES with name" << m_cellContainerKey.key() );

  for (unsigned int i = 0; i < nCellMax; ++i) {
    const CaloDetDescrElement* caloDDE = m_tileMgr->get_cell_element(i);
    TileCell* pCell = new TileCell(caloDDE, enePmt1[i], enePmt2[i], 0.0, 0.0, 0, 0
                                   , TileCell::MASK_AMPL, TileCell::MASK_AMPL
                                   , CaloGain::INVALIDGAIN, CaloGain::INVALIDGAIN); // gain is not known
    cellContainer->push_back(pCell);
  }

  // Execution completed.
  if (msgLvl(MSG::DEBUG))  {
    msg(MSG::DEBUG) << "TileHitToCell execution completed." << endmsg;
    msg(MSG::DEBUG) << " nChan=" << nChan
                    << " eHitTot=" << eHitTot
                    << " nCell=" << nCell
                    << " n2=" << nTwo
                    << " eneTot=" << eCellTot << endmsg;
  }



  return StatusCode::SUCCESS;
}

StatusCode TileHitToCell::finalize() {

  ATH_MSG_INFO( "TileHitToCell::finalize() end" );

  return StatusCode::SUCCESS;
}


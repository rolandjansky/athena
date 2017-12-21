/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileCellToTTL1.cxx
//  Author   : Monica Dunford
//  Created  : Oct, 2009
//
//  DESCRIPTION:
//      This is a tool which builds the L1 tower energy from the energy measured
//      by the tile cells. This tool is useful for L1Calo to compare their
//      tower energy to that measured by the digital readout in tile
//
//
//  HISTORY:
//
//  BUGS:
//
//*****************************************************************************

// Tile includes
#include "TileRecAlgs/TileCellToTTL1.h"
#include "TileConditions/TileCablingService.h"

// Calo includes
#include "CaloIdentifier/TileID.h"
#include "CaloIdentifier/CaloLVL1_ID.h"

// Atlas includes
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "AthenaKernel/errorcheck.h"

//C++ STL includes
#include <vector>

//
// Constructor
//
TileCellToTTL1::TileCellToTTL1(std::string name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_tileID(0)
  , m_TT_ID(0)
  , m_tileCablingService(0)
{

}

TileCellToTTL1::~TileCellToTTL1() {
}

//
// Alg standard initialize function
//
StatusCode TileCellToTTL1::initialize() {

  // retrieve CaloLVL1_ID, TileID, from det store
  CHECK( detStore()->retrieve(m_TT_ID));
  CHECK( detStore()->retrieve(m_tileID));

  m_tileCablingService = TileCablingService::getInstance();

  ATH_CHECK( m_cellContainerKey.initialize() );
  ATH_CHECK( m_ttl1CellContainerKey.initialize() );

  ATH_MSG_INFO( "TileCellToTTL1 initialization completed");

  return StatusCode::SUCCESS;
}
/*==========================================================================*/
//
// Begin Execution Phase.
//
StatusCode TileCellToTTL1::execute() {

  ATH_MSG_DEBUG( "Executing TileCellToTTL1");

  // -------------------------------------------------
  // Load the TileCell container
  // -------------------------------------------------

  SG::ReadHandle<CaloCellContainer> cellContainer(m_cellContainerKey);
  ATH_CHECK( cellContainer.isValid() );

  // -------------------------------------------------
  // Create TTL1 container and other arrays
  // -------------------------------------------------
  SG::WriteHandle<TileTTL1CellContainer> ttl1CellContainer(m_ttl1CellContainerKey);

  // Register the TTL1 container in the TES
  ATH_CHECK( ttl1CellContainer.record(std::make_unique<TileTTL1CellContainer>()) );
  ATH_MSG_DEBUG( "TileTTL1Container registered successfully (" << m_ttl1CellContainerKey.key() << ")");

  int ttNpmt[32][64];       // array of TT occupancy
  Identifier ttId[32][64];  // array of TT identifiers
  float ttAmp[32][64];      // array of all TT amplitudes
  uint16_t ttStatusCells[32][64];   // array of TT status of cells
  uint16_t ttStatusChans[32][64];   // array of TT status of channels
  float ttTimeAve[32][64];      // array of TT time average
  float ttCorrFact[32][64];     // array of TT correction factor

  // clear the arrays
  for (int i = 0; i < 32; i++) {
    for (int j = 0; j < 64; j++) {
      ttNpmt[i][j] = 0;
      ttId[i][j] = 0;
      ttAmp[i][j] = 0.0;
      ttStatusCells[i][j] = 0;
      ttStatusChans[i][j] = 0;
      ttTimeAve[i][j] = 0.0;
      ttCorrFact[i][j] = 1.0; // this is a place holder for now, set to 1.0
    }
  }

  // -------------------------------------------------
  // Loop over all cells
  // -------------------------------------------------

  for (const CaloCell* cell : *cellContainer) {

    // keep only cells from TileCal calorimeter barrel or extended barrel
    Identifier cell_id = cell->ID();
    if (!(m_tileID->is_tile(cell_id))) continue;

    const TileCell* tilecell = dynamic_cast<const TileCell*>(cell);
    if (!tilecell) continue;

    float cell_ene = cell->energy();
    float cell_time = cell->time();

    int bad_cell = tilecell->badcell();
    int bad_chan[2];
    bad_chan[0] = tilecell->badch1();
    bad_chan[1] = tilecell->badch2();

    // In order to make sure that the D-cells are correctly added
    // across two towers. Loop over the two PMTs in each cell
    // for each PMT reduce the cell energy by 50%. 

    for (int ipmt = 0; ipmt < 2; ipmt++) {
      Identifier pmt_id = m_tileID->pmt_id(cell_id, ipmt);
      Identifier tt_id = m_tileCablingService->pmt2tt_id(pmt_id);

      // remove the E-cells
      int sample = m_tileID->sample(pmt_id);
      if (sample == TileID::SAMP_E) continue;

      // if in the negative eta region add 16 to the ieta offset arrays
      int eta_offset = 0;
      if (m_tileID->is_negative(pmt_id))
        eta_offset = 16;

      // the D0 cell is not being split correctly across cells
      int ieta = m_TT_ID->eta(tt_id);
      if (sample == TileID::SAMP_D && ieta == 0 && ipmt == 1)
        eta_offset = 16;

      ieta += eta_offset;
      int iphi = m_TT_ID->phi(tt_id);

      // Sum the tower energy
      // already exists - just add charge
      // reduce cell energy by 50% because we are loop over both pmts in cell
      if (ttNpmt[ieta][iphi] > 0) {
        ttAmp[ieta][iphi] += cell_ene * 0.5;
        ttNpmt[ieta][iphi]++;
        ttStatusCells[ieta][iphi] += (uint16_t) bad_cell;
        ttStatusChans[ieta][iphi] += (uint16_t) bad_chan[ipmt];
        ttTimeAve[ieta][iphi] += cell_time;

        // rawChannel in new TT
      } else {
        ttId[ieta][iphi] = tt_id;
        ttNpmt[ieta][iphi]++;
        ttAmp[ieta][iphi] = cell_ene * 0.5;
        ttStatusCells[ieta][iphi] = (uint16_t) bad_cell;
        ttStatusChans[ieta][iphi] = (uint16_t) bad_chan[ipmt];
        ttTimeAve[ieta][iphi] = cell_time;
      }

    } // end of loop over pmts in the cell
  } // end loop over cells

  for (int ieta = 0; ieta < 32; ieta++) {
    for (int iphi = 0; iphi < 64; iphi++) {

      // don't load towers that are empty
      if (ttNpmt[ieta][iphi] == 0) continue;

      float time_ave = ttTimeAve[ieta][iphi] / ((float) ttNpmt[ieta][iphi]);

      uint16_t qual = 0;
      if (ttStatusChans[ieta][iphi] == ttNpmt[ieta][iphi])
        qual += TileTTL1Cell::MASK_BADTOWER;
      if (ttStatusCells[ieta][iphi] > 0)
        qual += TileTTL1Cell::MASK_BADCELL;
      if (ttStatusChans[ieta][iphi] > 0)
        qual += TileTTL1Cell::MASK_BADCHAN;

      ttl1CellContainer->push_back(std::make_unique<TileTTL1Cell>(ttId[ieta][iphi], 
                                                                  ttAmp[ieta][iphi], 
                                                                  time_ave,
                                                                  ttCorrFact[ieta][iphi], 
                                                                  qual));

    }
  }


  // Execution completed
  ATH_MSG_DEBUG( "TileCellToTTL1 execution completed.");

  return StatusCode::SUCCESS;
}

StatusCode TileCellToTTL1::finalize() {

  ATH_MSG_INFO( "TileCellToTTL1::finalize() end");

  return StatusCode::SUCCESS;
}


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/errorcheck.h"

#include "CaloIdentifier/TileID.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloEvent/CaloCell.h"

#include "TileIdentifier/TileHWID.h"
#include "TileConditions/TileCablingService.h" 
#include "TileRecUtils/TileCellFakeProb.h"



// Constructor:

TileCellFakeProb::TileCellFakeProb(const std::string& type,
    const std::string& name, const IInterface* parent)
  : AthAlgTool(type, name, parent)
  , m_caloID(0)
  , m_tileID(0)
  , m_tileHWID(0)
  , m_cabling(0)
{
  declareInterface<ICellWeightTool>(this);
  declareProperty("DeadDrawerList", m_deadDrawerInput);
}

StatusCode TileCellFakeProb::initialize() {

  // Get the CaloCell_ID helper from the detector store
  CHECK( detStore()->retrieve(m_caloID, "CaloCell_ID") );

  // Get the TileID helper from the detector store
  CHECK( detStore()->retrieve(m_tileID, "TileID") );

  // Get the TileHWID helper from the detector store
  CHECK( detStore()->retrieve(m_tileHWID, "TileHWID") );

  // Instantiate Cabling Svc to initialize pointers to helpers there
  m_cabling = TileCablingService::getInstance();
  if (m_cabling == 0) {
    ATH_MSG_FATAL( " Cannot get instance of TileCablingService" );
    return StatusCode::FAILURE;
  }

  // process properties from jobOptions
  CHECK( createMiscalibratedCellList() );

  return StatusCode::SUCCESS;
}

// Desctructor
TileCellFakeProb::~TileCellFakeProb() {
}

// MakeCorrection:  This is called with a pointer to the Cell Object.
double TileCellFakeProb::wtCell(const CaloCell* theCell) {

  Identifier id = theCell->ID();
  double totalweight = 1.0; // default weight

  if (m_tileID->is_tile(id)) {
    std::map<Identifier, double>::iterator cur = m_celllist.find(id);

    if (cur != m_celllist.end()) {
      totalweight = (cur->second);
      ATH_MSG_VERBOSE( "eta = " << theCell->eta()
                      << ", phi = " << theCell->phi()
                      << ", weight = " << totalweight );
    }
  }

  return totalweight;
}

StatusCode TileCellFakeProb::createMiscalibratedCellList() {

//  ros         1 to 4      ReadOutSystem number ( 1,2 = pos/neg Barrel (side A/C)
//                                                 3,4 = pos/neg Ext.Barrel (side A/C) )
//  drawer      0 to 63     64 drawers (modules) in one cylinder (phi-slices)
//  channel     0 to 47     channel number in the drawer
//  adc         0 to 1      ADC number for the channel (0 = low gain, 1 = high gain)

  // read in dead drawers from jopOptions
  std::vector<std::string>::const_iterator itrStringID = m_deadDrawerInput.begin();
  for (; itrStringID != m_deadDrawerInput.end(); ++itrStringID) {
    std::string theString = *itrStringID;
    std::stringstream is;
    is << theString << std::endl;

    int iros, idrawer;
    double weight;
    is >> iros >> idrawer >> weight;

    ATH_MSG_DEBUG( "scale ROS " << iros
                  << " Drawer " << idrawer
                  << " with weight=" << weight );

    // loop over all TileCal cells and check if they belong to dead drawers
    int sub_calo_num = (int) CaloCell_ID::TILE;
    IdentifierHash hmin, hmax;
    m_caloID->calo_cell_hash_range(sub_calo_num, hmin, hmax);

    int nCells = 0;
    for (unsigned int i = hmin; i < hmax; ++i) {
      Identifier id = m_caloID->cell_id((IdentifierHash) i);
      HWIdentifier myhwid = m_cabling->s2h_drawer_id(id);
      int ros = m_tileHWID->ros(myhwid);
      int drawer = m_tileHWID->drawer(myhwid);

      if ((ros == iros) && (drawer == idrawer)) {
        m_celllist[id] = weight;
        ++nCells;
      }
    }
    ATH_MSG_DEBUG( "looped over " << (hmax - hmin)
                  << " cells, " << nCells << " cells matched" );
  }

  return StatusCode::SUCCESS;
}

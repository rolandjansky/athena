/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileCellVerify.cxx
//  Author   : Zhifang
//  Created  : May, 2002
//
//  DESCRIPTION:
//     Implement the TileCellVerify class
//
//  HISTORY:
//
//  BUGS:
//
//*****************************************************************************

// Tile includes
#include "TileEvent/TileCell.h"
#include "TileRecAlgs/TileCellVerify.h"

// Calo includes
#include "CaloIdentifier/TileID.h"

// Atlas includes
#include "StoreGate/ReadHandle.h"
#include "AthenaKernel/errorcheck.h"


//C++ STL includes
#include <vector>
#include <algorithm>

using namespace std;

class CompCell: public binary_function<const CaloCell*, const CaloCell*, bool> {
  public:
    bool operator()(const CaloCell* p1, const CaloCell* p2) {
      return p1->energy() < p2->energy();
    }
};

//==========================================================================
// TileCellVerify's implementations
//==========================================================================

// Constructor
TileCellVerify::TileCellVerify(string name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_tileID(0)
  , m_dumpCells(false)
  , m_sortFlag(false)

{
  declareProperty("DumpCells", m_dumpCells);
  declareProperty("SortFlag", m_sortFlag);
}

TileCellVerify::~TileCellVerify() {
}

// Alg standard interfacw function
StatusCode TileCellVerify::initialize() {

  // retrieve TileID helper from det store
  CHECK( detStore()->retrieve(m_tileID) );

  ATH_CHECK( m_cellContainer1Key.initialize() );
  ATH_CHECK( m_cellContainer2Key.initialize() );

  ATH_MSG_INFO( "TileCellVerify initialization completed" );

  return StatusCode::SUCCESS;
}

StatusCode TileCellVerify::execute() {

  // step1: read two cell containers from TES
  SG::ReadHandle<CaloCellContainer> cellContainer1(m_cellContainer1Key);
  ATH_CHECK(cellContainer1.isValid());

  SG::ReadHandle<CaloCellContainer> cellContainer2(m_cellContainer2Key);
  ATH_CHECK(cellContainer2.isValid());

  // step2: first compare the number of cells in the two containers
  int nCells1 = cellContainer1->size();
  int nCells2 = cellContainer2->size();
  ATH_MSG_INFO( "The number of cells in " << m_cellContainer1Key.key() << " is " << nCells1 );
  ATH_MSG_INFO( "The number of cells in " << m_cellContainer2Key.key() << " is " << nCells2 );

  if (nCells1 != nCells2) {
    ATH_MSG_ERROR( "The number of cells is not equal in the two cell "
                   << "containers: " << m_cellContainer1Key.key() 
                   << " and " << m_cellContainer2Key.key() );

    return (StatusCode::SUCCESS);
  }

  // step3: to sort the cells in the container read above by energy
  vector<const CaloCell*> cells1;
  vector<const CaloCell*> cells2;
  const CaloCell* cell1(nullptr);
  const CaloCell* cell2(nullptr);
  if (m_sortFlag) {

    for (const CaloCell* cell : *cellContainer1) {
      cells1.push_back(cell);
    }

    for (const CaloCell* cell : *cellContainer2) {
      cells2.push_back(cell);
    }

    sort(cells1.begin(), cells1.end(), CompCell());
    sort(cells2.begin(), cells2.end(), CompCell());
  }

  // step4: then compare every cell-pair in the containers
  bool bErrorFlag = false;
  bool bOnlyOnceFlag = false;
  bool lVerbose = msgLvl(MSG::VERBOSE);
  for (int i = 0; i < nCells1; ++i) {

    if (m_sortFlag) {
      cell1 = cells1[i];
      cell2 = cells2[i];
    } else {
      cell1 = (*cellContainer1)[i];
      cell2 = (*cellContainer2)[i];
    }

    Identifier id1 = cell1->ID();
    Identifier id2 = cell2->ID();
    if (id1 != id2) bErrorFlag = true;
    if (lVerbose && (m_dumpCells || bErrorFlag)) {

      if (!bOnlyOnceFlag) {
        msg(MSG::VERBOSE) << "  ===" << m_cellContainer1Key.key() << "===      ===" << m_cellContainer2Key.key() << "===" << endmsg;
        msg(MSG::VERBOSE) << "  Index      e1            id1        |        e2           id2" << endmsg;
        msg(MSG::VERBOSE) << "--------------------------------------------------------------------------------" << endmsg;
        bOnlyOnceFlag = true;
      }

      msg(MSG::VERBOSE) << setw(5) << i << "   " << setw(12) << cell1->energy() << "   [";
      msg(MSG::VERBOSE) << m_tileID->to_string(id1, -2) << "]";
      msg(MSG::VERBOSE) << "  |  " << setw(12) << cell2->energy() << "   [";
      msg(MSG::VERBOSE) << m_tileID->to_string(id2, -2) << "]";

      if (bErrorFlag) msg(MSG::VERBOSE) << " * ";

      msg(MSG::VERBOSE) << endmsg;

    } else if (bErrorFlag){
      break;
    }
  }
  if (bOnlyOnceFlag) {
    msg(MSG::VERBOSE) << "--------------------------------------------------------------------------------" << endmsg;
  }
  if (!bErrorFlag) {
    ATH_MSG_INFO( "The two cellContainers (" << m_cellContainer1Key.key()
                  << " and " << m_cellContainer2Key.key() << ") are same!!!" );
  } else {
    ATH_MSG_INFO( "The two cellContainers (" << m_cellContainer1Key.key()
                  << " and " << m_cellContainer2Key.key() << ") are not same!!!" );
  }

  // Execution completed.
  ATH_MSG_INFO( "TileCellVerify execution completed successfully" );

  return StatusCode::SUCCESS;
}

StatusCode TileCellVerify::finalize() {

  ATH_MSG_INFO( "TileCellVerify::finalize() end" );

  return StatusCode::SUCCESS;
}


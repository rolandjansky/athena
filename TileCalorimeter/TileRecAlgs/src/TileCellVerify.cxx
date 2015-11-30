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

// Atlas includes
#include "AthenaKernel/errorcheck.h"

// Calo includes
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/TileID.h"

// Tile includes
#include "TileEvent/TileCell.h"
#include "TileRecAlgs/TileCellVerify.h"

//C++ STL includes
#include <vector>
#include <algorithm>

using namespace std;

class CompCell: public binary_function<CaloCell*, CaloCell*, bool> {
  public:
    bool operator()(CaloCell* p1, CaloCell* p2) {
      return p1->energy() < p2->energy();
    }
};

//==========================================================================
// TileCellVerify's implementations
//==========================================================================

// Constructor
TileCellVerify::TileCellVerify(string name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_cellContainer1("TileCellContainer1")
  , m_cellContainer2("TileCellContainer2")
  , m_tileID(0)
  , m_dumpCells(false)
  , m_sortFlag(false)

{

  declareProperty("TileCellContainer1", m_cellContainer1);
  declareProperty("TileCellContainer2", m_cellContainer2);
  declareProperty("DumpCells", m_dumpCells);
  declareProperty("SortFlag", m_sortFlag);
}

TileCellVerify::~TileCellVerify() {
}

// Alg standard interfacw function
StatusCode TileCellVerify::initialize() {

  // retrieve TileID helper from det store
  CHECK( detStore()->retrieve(m_tileID) );

  ATH_MSG_INFO( "TileCellVerify initialization completed" );

  return StatusCode::SUCCESS;
}

StatusCode TileCellVerify::execute() {

  // step1: read two cell containers from TES
  const CaloCellContainer* pCellContainer1;
  const CaloCellContainer* pCellContainer2;
  CHECK( evtStore()->retrieve(pCellContainer1, m_cellContainer1) );
  CHECK( evtStore()->retrieve(pCellContainer2, m_cellContainer2) );

  // step2: first compare the number of cells in the two containers
  int nCells1 = pCellContainer1->size();
  int nCells2 = pCellContainer2->size();
  ATH_MSG_INFO( "The number of cells in " << m_cellContainer1 << " is " << nCells1 );
  ATH_MSG_INFO( "The number of cells in " << m_cellContainer2 << " is " << nCells2 );

  if (nCells1 != nCells2) {
    ATH_MSG_ERROR( "The number of cells is not equal in the two cell "
        << "containers: " << m_cellContainer1 << " and " << m_cellContainer2 );

    return (StatusCode::SUCCESS);
  }

  // step3: to sort the cells in the container read above by energy
  vector<CaloCell*> v1;
  vector<CaloCell*> v2;
  const CaloCell* pCell1;
  const CaloCell* pCell2;
  if (m_sortFlag) {
    CaloCellContainer::const_iterator it = pCellContainer1->begin();
    CaloCellContainer::const_iterator itLast = pCellContainer1->end();
    for (; it != itLast; ++it) {
      v1.push_back(*it);
    }
    it = pCellContainer2->begin();
    itLast = pCellContainer2->end();
    for (; it != itLast; ++it) {
      v2.push_back(*it);
    }
    sort(v1.begin(), v1.end(), CompCell());
    sort(v2.begin(), v2.end(), CompCell());
  }

  // step4: then compare every cell-pair in the containers
  bool bErrorFlag = false;
  bool bOnlyOnceFlag = false;
  bool lVerbose = msgLvl(MSG::VERBOSE);
  for (int i = 0; i < nCells1; ++i) {

    if (m_sortFlag) {
      pCell1 = v1[i];
      pCell2 = v2[i];
    } else {
      pCell1 = (*pCellContainer1)[i];
      pCell2 = (*pCellContainer2)[i];
    }

    Identifier id1 = pCell1->ID();
    Identifier id2 = pCell2->ID();
    if (id1 != id2) bErrorFlag = true;
    if (lVerbose && (m_dumpCells || bErrorFlag)) {

      if (!bOnlyOnceFlag) {
        msg(MSG::VERBOSE) << "  ===" << m_cellContainer1 << "===      ===" << m_cellContainer2 << "===" << endmsg;
        msg(MSG::VERBOSE) << "  Index      e1            id1        |        e2           id2" << endmsg;
        msg(MSG::VERBOSE) << "--------------------------------------------------------------------------------" << endmsg;
        bOnlyOnceFlag = true;
      }

      msg(MSG::VERBOSE) << setw(5) << i << "   " << setw(12) << pCell1->energy() << "   [";
      msg(MSG::VERBOSE) << m_tileID->to_string(id1, -2) << "]";
      msg(MSG::VERBOSE) << "  |  " << setw(12) << pCell2->energy() << "   [";
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
    ATH_MSG_INFO( "The two cellContainers (" << m_cellContainer1
        << " and " << m_cellContainer2 << ") are same!!!" );
  } else {
    ATH_MSG_INFO( "The two cellContainers (" << m_cellContainer1
        << " and " << m_cellContainer2 << ") are not same!!!" );
  }

  // Execution completed.
  ATH_MSG_INFO( "TileCellVerify execution completed successfully" );

  return StatusCode::SUCCESS;
}

StatusCode TileCellVerify::finalize() {

  ATH_MSG_INFO( "TileCellVerify::finalize() end" );

  return StatusCode::SUCCESS;
}


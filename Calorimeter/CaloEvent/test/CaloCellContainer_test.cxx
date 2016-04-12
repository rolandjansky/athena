/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloCellContainer_test.cxx,v 1.5 2009-03-18 03:06:33 ssnyder Exp $
/**
 * @file  CaloCellContainer_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2007
 * @brief Component test for CaloCellContainer.
 */

#undef NDEBUG

#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCell.h"
#include "CaloDetDescr/CaloDetectorElements.h"
#include "CaloDetDescr/CaloDetDescriptor.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/LArMiniFCAL_ID.h"
#include "CaloIdentifier/TileID.h"
#include "IdDictParser/IdDictParser.h"
#include "GaudiKernel/Bootstrap.h"
#include "TestTools/initGaudi.h"
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>



typedef std::vector<CaloCell*> CellVector;
typedef CaloCellContainer CellContainer;


#include "CaloCellContainerTestCommon.icc"


class CaloCompactCellTool
{
public:
  static void set_total (CaloCellContainer& cont)
  {
    cont.setHasTotalSize (true);
  }
};


void test_gen (const CaloCell_ID* helper,
               selfcn_t* selfcn,
               const std::vector<CaloCell*>& cells,
               bool total)
{
  std::vector<CaloCell*> selected_cells[CaloCell_ID::NSUBCALO];
  selfcn (cells, selected_cells);
  {
    CaloCellContainer cont (SG::VIEW_ELEMENTS);
    assert (cont.ownPolicy() == SG::VIEW_ELEMENTS);
    fill_cells1 (selected_cells, cont);
    if (total) CaloCompactCellTool::set_total (cont);
    check_cells (helper, cells, selected_cells, cont, total);
  }
  {
    CaloCellContainer cont (SG::VIEW_ELEMENTS);
    fill_cells2 (selected_cells, cont);
    if (total) CaloCompactCellTool::set_total (cont);
    check_cells (helper, cells, selected_cells, cont, total);
  }
}



//============================================================================
// Test handling of duplicate cells.
//


void test5 (const CaloCell_ID* helper, const std::vector<CaloCell*>& cells)
{
  std::cout << "test5\n";
  {
    std::vector<CaloCell*> selected_cells[CaloCell_ID::NSUBCALO];
    select_cells1 (cells, selected_cells);
    CaloCellContainer cont (SG::VIEW_ELEMENTS);
    assert (!selected_cells[0].empty());
    selected_cells[0].push_back (selected_cells[0][0]);
    fill_cells1 (selected_cells, cont);
    CaloCompactCellTool::set_total (cont);
    selected_cells[0].pop_back();
    check_cells (helper, cells, selected_cells, cont, true);
  }
  {
    std::vector<CaloCell*> selected_cells[CaloCell_ID::NSUBCALO];
    select_cells2 (cells, selected_cells);
    CaloCellContainer cont (SG::VIEW_ELEMENTS);
    assert (selected_cells[0].size() > 0);
    assert (!selected_cells[0].empty());
    selected_cells[0].push_back (selected_cells[0][0]);
    fill_cells1 (selected_cells, cont);
    selected_cells[0].pop_back();
    check_cells (helper, cells, selected_cells, cont, false);
  }
  {
    std::vector<CaloCell*> selected_cells[CaloCell_ID::NSUBCALO];
    select_cells2a (cells, selected_cells);
    CaloCellContainer cont (SG::VIEW_ELEMENTS);
    assert (selected_cells[0].size() > 0);
    assert (!selected_cells[0].empty());
    selected_cells[0].push_back (selected_cells[0][0]);
    fill_cells1 (selected_cells, cont);
    selected_cells[0].pop_back();
    check_cells (helper, cells, selected_cells, cont, false);
  }
}


//============================================================================


void test6(const CaloCell_ID* /*helper*/, const std::vector<CaloCell*>& cells)
{
  std::cout << "test6\n";
  CaloCellContainer cont (SG::VIEW_ELEMENTS);
  for (int i=0; i < 10; i++)
    cont.push_back (cells[9-i]);
  assert (cont.findCell(3) == cells[3]);
  for (int i=0; i < 10; i++)
    cont.push_back (cells[19-i]);
  assert (cont.findCell(13) == cells[13]);
}


//============================================================================


int main()
{
  ISvcLocator* pSvcLoc;
  if (!Athena_test::initGaudi("CaloCellContainer_test.txt", pSvcLoc)) {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }  

  TileID* tile_helper = new TileID;
  CaloCell_ID* helper = make_helper (tile_helper);
  std::vector<CaloCell*> cells = make_cells (helper, tile_helper);

  test1 (helper, cells);
  test2 (helper, cells);
  test3 (helper, cells);
  test4 (helper, cells);
  test5 (helper, cells);
  test6 (helper, cells);

  return 0;
}

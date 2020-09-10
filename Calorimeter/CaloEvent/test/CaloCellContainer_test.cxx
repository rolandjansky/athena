/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file  CaloCellContainer_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2007
 * @brief Component test for CaloCellContainer.
 */

#undef NDEBUG

#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloTester.h"
#include "CaloIdentifier/CaloCell_ID.h"
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


void test_gen (const CaloCell_ID& helper,
               selfcn_t* selfcn,
               const std::vector<CaloCell*>& cells,
               bool total,
               Athena_test::URNG& stlrand)
{
  std::vector<CaloCell*> selected_cells[CaloCell_ID::NSUBCALO];
  selfcn (stlrand, cells, selected_cells);
  {
    CaloCellContainer cont (SG::VIEW_ELEMENTS);
    assert (cont.ownPolicy() == SG::VIEW_ELEMENTS);
    fill_cells1 (selected_cells, cont, stlrand);
    if (total) CaloCompactCellTool::set_total (cont);
    check_cells (stlrand, helper, cells, selected_cells, cont, total);
  }
  {
    CaloCellContainer cont (SG::VIEW_ELEMENTS);
    fill_cells2 (selected_cells, cont, stlrand);
    if (total) CaloCompactCellTool::set_total (cont);
    check_cells (stlrand, helper, cells, selected_cells, cont, total);
  }
}



//============================================================================
// Test handling of duplicate cells.
//


void test5 (const CaloCell_ID& helper, const std::vector<CaloCell*>& cells,
            Athena_test::URNG& stlrand)
{
  std::cout << "test5\n";
  {
    std::vector<CaloCell*> selected_cells[CaloCell_ID::NSUBCALO];
    select_cells1 (stlrand, cells, selected_cells);
    CaloCellContainer cont (SG::VIEW_ELEMENTS);
    assert (!selected_cells[0].empty());
    selected_cells[0].push_back (selected_cells[0][0]);
    fill_cells1 (selected_cells, cont, stlrand);
    CaloCompactCellTool::set_total (cont);
    selected_cells[0].pop_back();
    check_cells (stlrand, helper, cells, selected_cells, cont, true);
  }
  {
    std::vector<CaloCell*> selected_cells[CaloCell_ID::NSUBCALO];
    select_cells2 (stlrand, cells, selected_cells);
    CaloCellContainer cont (SG::VIEW_ELEMENTS);
    assert (selected_cells[0].size() > 0);
    assert (!selected_cells[0].empty());
    selected_cells[0].push_back (selected_cells[0][0]);
    fill_cells1 (selected_cells, cont, stlrand);
    selected_cells[0].pop_back();
    check_cells (stlrand, helper, cells, selected_cells, cont, false);
  }
  {
    std::vector<CaloCell*> selected_cells[CaloCell_ID::NSUBCALO];
    select_cells2a (stlrand, cells, selected_cells);
    CaloCellContainer cont (SG::VIEW_ELEMENTS);
    assert (selected_cells[0].size() > 0);
    assert (!selected_cells[0].empty());
    selected_cells[0].push_back (selected_cells[0][0]);
    fill_cells1 (selected_cells, cont, stlrand);
    selected_cells[0].pop_back();
    check_cells (stlrand, helper, cells, selected_cells, cont, false);
  }
}


//============================================================================


void test6(const CaloCell_ID& /*helper*/, const std::vector<CaloCell*>& cells)
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
  if (!Athena_test::initGaudi("CaloEvent/CaloCellContainer_test.txt", pSvcLoc)) {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }  

  CaloTester tester;
  const CaloCell_ID& helper = tester.caloID();
  CellVector cells = tester.get_cells();

  Athena_test::URNG stlrand;

  test1 (helper, cells, stlrand);
  test2 (helper, cells, stlrand);
  test3 (helper, cells, stlrand);
  test4 (helper, cells);
  test5 (helper, cells, stlrand);
  test6 (helper, cells);

  return 0;
}

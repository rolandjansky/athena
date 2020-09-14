/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file CaloEvent/test/CaloConstCellContainer_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2014
 * @brief Component test for CaloConstCellContainer.
 */

#undef NDEBUG

#include "CaloEvent/CaloConstCellContainer.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloTester.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "TestTools/initGaudi.h"
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>
#include <cassert>


typedef std::vector<const CaloCell*> CellVector;
typedef CaloConstCellContainer CellContainer;
#define CONST_CONTAINER

#include "CaloCellContainerTestCommon.icc"


void test_gen (const CaloCell_ID& helper,
               selfcn_t* selfcn,
               const CellVector& cells, 
               bool /*total*/,
               Athena_test::URNG& stlrand)
{
  CellVector selected_cells[CaloCell_ID::NSUBCALO];
  selfcn (stlrand, cells, selected_cells);
  {
    CaloConstCellContainer cont (SG::VIEW_ELEMENTS);
    assert (cont.ownPolicy() == SG::VIEW_ELEMENTS);
    fill_cells1 (selected_cells, cont, stlrand);
    check_cells (stlrand, helper, cells, selected_cells, cont);
  }
  {
    CaloConstCellContainer cont (SG::VIEW_ELEMENTS);
    fill_cells2 (selected_cells, cont, stlrand);
    check_cells (stlrand, helper, cells, selected_cells, cont);
  }
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
  CellVector cells = tester.get_const_cells();

  Athena_test::URNG stlrand;

  test1 (helper, cells, stlrand);
  test2 (helper, cells, stlrand);
  test3 (helper, cells, stlrand);
  test4 (helper, cells);

  return 0;
}

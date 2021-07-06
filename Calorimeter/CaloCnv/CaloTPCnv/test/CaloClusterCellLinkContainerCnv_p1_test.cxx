/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/* @file CaloClusterCellLinkContainerCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2016
 * @brief Regression tests for CaloClusterCellLinkContainerCnv_p1.
 */

#undef NDEBUG
#include "CaloTPCnv/CaloClusterCellLinkContainerCnv_p1.h"
#include "CaloEvent/CaloClusterCellLinkContainer.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <cassert>
#include <iostream>


void compare (const CaloClusterCellLink& p1,
              const CaloClusterCellLink& p2)
{
  assert (p1.getCellContainerLink() == p2.getCellContainerLink());
  assert (p1.size() == p2.size());
  if (p1.size() == 0) return;
  CaloClusterCellLink::const_iterator it1 = p1.begin();
  CaloClusterCellLink::const_iterator it2 = p2.begin();
  for (size_t i = 0; i < p1.size(); i++) {
    assert (it1.index() == it2.index());
    assert (it1.weight() == it2.weight());
  }
}


void compare (const CaloClusterCellLinkContainer& p1,
              const CaloClusterCellLinkContainer& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++)
    compare (*p1.at(i), *p2.at(i));
}


void testit (const CaloClusterCellLinkContainer& trans1)
{
  MsgStream log (0, "test");
  CaloClusterCellLinkContainerCnv_p1 cnv;
  CaloClusterCellLinkContainer_p1 pers;
  cnv.transToPersWithKey (&trans1, &pers, "key", log);
  CaloClusterCellLinkContainer trans2;
  cnv.persToTransWithKey (&pers, &trans2, "key", log);
  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  (void)Gaudi::Hive::currentContext();
  Athena_test::Leakcheck check;

  CaloClusterCellLinkContainer trans1;
  {
    DataLink<CaloCellContainer> link ("cont1");
    auto cccl = std::make_unique<CaloClusterCellLink> (link);
    cccl->addCell (2, 1.5);
    cccl->addCell (3, 2.5);
    trans1.push_back (std::move (cccl));
  }
  {
    DataLink<CaloCellContainer> link ("cont2");
    auto cccl = std::make_unique<CaloClusterCellLink> (link);
    cccl->addCell (12, 11.5);
    cccl->addCell (13, 12.5);
    cccl->addCell (14, 13.5);
    trans1.push_back (std::move (cccl));
  }
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}

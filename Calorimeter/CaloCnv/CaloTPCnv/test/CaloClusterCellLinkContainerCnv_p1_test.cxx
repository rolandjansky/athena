/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
#include "CxxUtils/checker_macros.h"
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
    if (std::abs (it1.weight() - 1.0) < 1e-6) {
      assert (it2.weight() == 1.0);
    }
    else {
      assert (it1.weight() == it2.weight());
    }
    ++it1;
    ++it2;
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

  // Make sure that reading + rewriting doesn't change the persistent form.
  CaloClusterCellLinkContainer_p1 pers2;
  cnv.transToPersWithKey (&trans2, &pers2, "key", log);
  assert (pers.m_indices == pers2.m_indices);
  assert (pers.m_weights == pers2.m_weights);
  assert (pers.m_nCellsPerCluster == pers2.m_nCellsPerCluster);
  assert (pers.m_cellCont.m_SGKeyHash == pers2.m_cellCont.m_SGKeyHash);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
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
    cccl->addCell (4, 1.0);
    cccl->addCell (5, 1.0 - 1e-9);
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


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}

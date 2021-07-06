/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrigCaloEventTPCnv/test/TrigRNNOutputCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for TrigRNNOutputCnv_p1.
 */


#undef NDEBUG
#include "TrigCaloEventTPCnv/TrigRNNOutputCnv_p1.h"
#include "SGTools/TestStore.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <cassert>
#include <iostream>


void compare (const TrigRNNOutput& p1,
              const TrigRNNOutput& p2)
{
  assert (p1.output() == p2.output());
  assert (p1.clusterLink() == p2.clusterLink());
}


void testit (const TrigRNNOutput& trans1)
{
  MsgStream log (0, "test");
  TrigRNNOutputCnv_p1 cnv;
  TrigRNNOutput_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigRNNOutput trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  (void)Gaudi::Hive::currentContext();
  // Get proxy created outside of leak check.
  ElementLink<TrigEMClusterContainer> foo ("foo", 10);
  Athena_test::Leakcheck check;

  TrigRNNOutput trans1 (std::vector<float> {1.5, 2.5, 3.5});
  trans1.setCluster (ElementLink<TrigEMClusterContainer> ("foo", 10));
  
  testit (trans1);
}


int main()
{
  SGTest::initTestStore();
  test1();
  return 0;
}

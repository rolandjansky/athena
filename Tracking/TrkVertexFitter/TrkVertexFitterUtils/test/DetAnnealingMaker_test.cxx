/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file TrkVertexFitterUtils/test/DetAnnealingMaker_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2019
 * @brief Simple unit test for DetAnnealingMaker.
 */


#undef NDEBUG
#include "TrkVertexFitterInterfaces/IVertexAnnealingMaker.h"
#include "TestTools/initGaudi.h"
#include "TestTools/FLOATassert.h"
#include "CxxUtils/ubsan_suppress.h"
#include "GaudiKernel/ToolHandle.h"
#include "TInterpreter.h"
#include <iostream>
#include <cassert>
#include <cmath>


void test1 (Trk::IVertexAnnealingMaker& tool)
{
  std::cout << "test1\n";

  Trk::IVertexAnnealingMaker::AnnealingState astate;
  tool.reset(astate);
  assert (!tool.isEquilibrium(astate));
  assert (tool.actualTemp(astate) == 3);
  assert (Athena_test::isEqual (tool.getWeight (astate, 1), 0.791391));
  assert (Athena_test::isEqual (tool.getWeight (astate,
                                                1, std::vector<double>{1,2,3}),
                                0.353781));

  tool.anneal(astate);
  assert (!tool.isEquilibrium(astate));
  assert (tool.actualTemp(astate) == 2);
  assert (Athena_test::isEqual (tool.getWeight (astate, 1), 0.880797));
  assert (Athena_test::isEqual (tool.getWeight (astate,
                                                1, std::vector<double>{1,2,3}),
                                0.39672));

  tool.anneal(astate);
  assert (!tool.isEquilibrium(astate));
  assert (tool.actualTemp(astate) == 1);
  assert (Athena_test::isEqual (tool.getWeight (astate, 1), 0.982014));
  assert (Athena_test::isEqual (tool.getWeight (astate,
                                                1, std::vector<double>{1,2,3}),
                                0.501825));
  tool.anneal(astate);
  assert (tool.isEquilibrium(astate));
}


int main()
{
  std::cout << "TrkVertexFitterUtils/DetAnnealingMaker_test\n";
  CxxUtils::ubsan_suppress ([]() { TInterpreter::Instance(); });
  ISvcLocator* svcloc = nullptr;
  Athena_test::initGaudi ("TrkVertexFitterUtils/TrkVertexFitterUtils_tests.txt", svcloc);

  ToolHandle<Trk::IVertexAnnealingMaker> tool ("Trk::DetAnnealingMaker");
  assert( tool.retrieve().isSuccess() );

  test1 (*tool);

  return 0;
}

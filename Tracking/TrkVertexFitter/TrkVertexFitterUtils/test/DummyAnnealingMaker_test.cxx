/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file TrkVertexFitterUtils/test/DummyAnnealingMaker_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2019
 * @brief Simple unit test for DummyAnnealingMaker.
 */


#undef NDEBUG
#include "TrkVertexFitterInterfaces/IVertexAnnealingMaker.h"
#include "TestTools/initGaudi.h"
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
  assert (tool.isEquilibrium(astate));
  tool.anneal(astate);
  assert (tool.isEquilibrium(astate));
  assert (tool.actualTemp(astate) == 0);
  assert (tool.getWeight (astate, 1) == 0.5);
  assert (tool.getWeight (astate, 1, std::vector<double>{1, 2, 3}) == 0.5);
}


int main()
{
  std::cout << "TrkVertexFitterUtils/DummyAnnealingMaker_test\n";
  CxxUtils::ubsan_suppress ([]() { TInterpreter::Instance(); });
  ISvcLocator* svcloc = nullptr;
  Athena_test::initGaudi (svcloc);

  ToolHandle<Trk::IVertexAnnealingMaker> tool ("Trk::DummyAnnealingMaker");
  assert( tool.retrieve().isSuccess() );

  test1 (*tool);

  return 0;
}

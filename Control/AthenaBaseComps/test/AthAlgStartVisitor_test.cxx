/*
 * Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file AthenaBaseComps/test/AthAlgStartVisitor_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2018
 * @brief Unit test for AthAlgStartVisitor.
 */

#undef NDEBUG
#include "AthenaBaseComps/AthAlgStartVisitor.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TestTools/initGaudi.h"
#include <iostream>
#include <cassert>


class TestAlg
  : public AthAlgorithm
{
public:
  using AthAlgorithm::AthAlgorithm;
  StatusCode execute() override { return StatusCode::SUCCESS; }
};


class TestTool
  : public AthAlgTool
{
public:
  using AthAlgTool::AthAlgTool;
};


class TestHandleKey
  : public SG::VarHandleKey
{
public:
  TestHandleKey (const std::string& key)
    : SG::VarHandleKey (1234, key,
                        Gaudi::DataHandle::Reader,
                        "EventStore", true),
      m_started (0)
  {
  }

  virtual StatusCode start() override
  {
    ++m_started;
    return StatusCode::SUCCESS;
  }

  int m_started;
};


void test1 (ISvcLocator* svcLoc)
{
  std::cout << "test1\n";

  TestHandleKey h1 ("h1");
  TestHandleKey h2 ("h2");
  TestHandleKey h3 ("h3");
  TestHandleKey h4 ("h4");
  TestHandleKey h5 ("h5");

  TestAlg alg ("TestAlg", svcLoc);
  alg.addRef();
  alg.declare (h1);

  TestTool tool1 ("TestTool", "tool1", &alg);
  alg.registerTool (&tool1);
  tool1.declare (h2);

  TestTool tool2 ("TestTool", "tool2", &alg);
  alg.registerTool (&tool2);
  tool1.declare (h3);
  tool1.declare (h4);

  TestTool tool3 ("TestTool", "tool3", &tool2);
  tool2.registerTool (&tool3);
  tool1.declare (h5);

  assert (h1.m_started == 0);
  assert (h2.m_started == 0);
  assert (h3.m_started == 0);
  assert (h4.m_started == 0);
  assert (h5.m_started == 0);

  AthAlgStartVisitor v( &alg );
  alg.acceptDHVisitor (&v);

  assert (h1.m_started == 1);
  assert (h2.m_started == 1);
  assert (h3.m_started == 1);
  assert (h4.m_started == 1);
  assert (h5.m_started == 1);
}


int main()
{
  ISvcLocator* svcLoc = nullptr;
  if (!Athena_test::initGaudi ("propertyHandling_test.txt", svcLoc))
    return 1;

  std::cout << "AthAlgStartVisitor_test\n";
  test1 (svcLoc);
  return 0;
}


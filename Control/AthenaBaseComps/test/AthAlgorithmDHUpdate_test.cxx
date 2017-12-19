/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */

// $Id$
/**
 * @file AthenaBaseComps/test/AthAlgorithmDHUpdate_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2017
 * @brief Test property handling for AthReentrantAlgorithm.
 */


#undef NDEBUG
#include "../src/AthAlgorithmDHUpdate.h"
#include "SGTools/CLASS_DEF.h"
#include "SGTools/BaseInfo.h"
#include "TestTools/initGaudi.h"
#include <cassert>
#include <iostream>


class A2 {};
class A3 {};
class A1 : public A2, public A3 {};
CLASS_DEF (A1, 132102064, 0)
CLASS_DEF (A2, 132102065, 0)
CLASS_DEF (A3, 132102066, 0)
SG_BASES2 (A1, A2, A3);

class B2 {};
class B1 : public B2 {};
CLASS_DEF (B1, 132102074, 0)
CLASS_DEF (B2, 132102075, 0)
SG_BASE (B1, B2);

class C2 {};
class C1 : public C2 {};
CLASS_DEF (C1, 132102084, 0)
CLASS_DEF (C2, 132102085, 0)
SG_BASE (C1, C2);

class TestHolder
  : public IDataHandleHolder
{
public:
  virtual std::vector<Gaudi::DataHandle*> inputHandles() const override
  { std::abort(); }
  virtual const DataObjIDColl& extraInputDeps() const override
  { std::abort(); }
  virtual void acceptDHVisitor(IDataHandleVisitor*) const override
  { std::abort(); }
  virtual void commitHandles() override
  { std::abort(); }
  virtual const DataObjIDColl& inputDataObjs() const override
  { std::abort(); }
  virtual void addDependency(const DataObjID&, const Gaudi::DataHandle::Mode&) override
  { std::abort(); }
  virtual void declare(Gaudi::DataHandle&) override
  { std::abort(); }
  virtual void renounce(Gaudi::DataHandle&) override
  { std::abort(); }
  virtual unsigned long addRef() override
  { std::abort(); }
  virtual unsigned long release() override
  { std::abort(); }
  virtual StatusCode queryInterface(const InterfaceID&, void**) override
  { std::abort(); }
  virtual const std::string& name() const override
  { std::abort(); }

  virtual std::vector<Gaudi::DataHandle*> outputHandles() const override;
  virtual const DataObjIDColl& extraOutputDeps() const override;
  virtual const DataObjIDColl& outputDataObjs() const override;


  std::vector<Gaudi::DataHandle*> handle_ptrs;
  DataObjIDColl outDeps;
  DataObjIDColl extraOutDeps;
  DataObjIDColl extendedExtraOutDeps;
};


std::vector<Gaudi::DataHandle*> TestHolder::outputHandles() const
{
  return handle_ptrs;
}


const DataObjIDColl& TestHolder::extraOutputDeps() const
{
  if (!extendedExtraOutDeps.empty())
    return extendedExtraOutDeps;
  return extraOutDeps;
}


const DataObjIDColl& TestHolder::outputDataObjs() const
{
  return outDeps;
}


class TestChain
  : public IDataHandleVisitor
{
public:
  virtual void visit(const IDataHandleHolder*) override;

  const IDataHandleHolder* visited = nullptr;
};


void TestChain::visit (const IDataHandleHolder* dhh) 
{
  visited = dhh;
}


void test1()
{
  std::cout << "test1\n";

  TestHolder h;
  Gaudi::DataHandle h1 (DataObjID (ClassID_traits<A1>::ID(), "a1"));
  h.handle_ptrs.push_back (&h1);

  h.outDeps.emplace (ClassID_traits<B1>::ID(), "b1");
  h.outDeps.emplace (ClassID_traits<C1>::ID(), "c1");
  h.extraOutDeps.emplace (ClassID_traits<C1>::ID(), "c1");
  h.extraOutDeps.emplace (ClassID_traits<C1>::ID(), "d1");

  DataObjIDColl& linkedObjs = h.extendedExtraOutDeps;
  auto chain = std::make_unique<TestChain>();
  TestChain* tc = chain.get();
  AthenaBaseComps::AthAlgorithmDHUpdate dhu (linkedObjs, std::move (chain));
  dhu.visit (&h);
  assert (tc->visited == &h);

  DataObjIDColl exp = {
    { ClassID_traits<A2>::ID(), "a1" },
    { ClassID_traits<A3>::ID(), "a1" },
    { ClassID_traits<B2>::ID(), "b1" },
    { ClassID_traits<C1>::ID(), "c1" },
    { ClassID_traits<C2>::ID(), "c1" },
    { ClassID_traits<C1>::ID(), "d1" },
    { ClassID_traits<C2>::ID(), "d1" },
  };

  if (linkedObjs != exp) {
    for (const DataObjID& o : linkedObjs) {
        std::cout << "linked " << o.clid() << " " << o.key() << "\n";
    }
  }

  // Quick test with null chain.
  std::unique_ptr<IDataHandleVisitor> chain2;
  AthenaBaseComps::AthAlgorithmDHUpdate dhu2 (linkedObjs, std::move (chain2));
  dhu2.visit (&h);
}


int main()
{
  ISvcLocator* svcLoc = nullptr;
  if (!Athena_test::initGaudi ("propertyHandling_test.txt", svcLoc)) {
    return 1;
  }

  test1();
  return 0;
}

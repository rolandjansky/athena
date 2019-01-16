/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @brief Compile tests for WriteCondHandleKeyArray
 * @TODO extend to also test functionality
 */


#undef NDEBUG
#include "StoreGate/CondHandleKeyArray.h"
#include "StoreGate/exceptions.h"
#include "SGTools/TestStore.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/StorableConversions.h"
#include "SGTools/DataProxy.h"
#include "TestTools/initGaudi.h"
#include "TestTools/expect_exception.h"
#include "AthContainersInterfaces/IConstAuxStore.h"
#include "AthContainers/DataVector.h"
#include "AthContainers/ConstDataVector.h"
#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "CxxUtils/unused.h"
#include "CxxUtils/checker_macros.h"
#include <cassert>
#include <iostream>

#define DEBUG_VHB 1


class MyObjAux
  : public SG::IConstAuxStore, public ILockable
{
public:
  MyObjAux(int x=0) : x(x) {}
  ~MyObjAux() { deleted.push_back (x); }
  int x;
  bool m_locked = false;

  virtual const void* getData (SG::auxid_t /*auxid*/) const override { return 0; }
  virtual void* getDecoration (SG::auxid_t /*auxid*/, size_t /*size*/, size_t /*capacity*/) override { return 0; }
  virtual const SG::auxid_set_t& getAuxIDs() const override { std::abort(); }
  virtual bool isDecoration(SG::auxid_t /*auxid*/) const override { std::abort(); }
  virtual void lock() override { m_locked = true; }
  virtual bool clearDecorations() override { return false; }
  virtual size_t size() const override { return 0; }
  virtual void lockDecoration (SG::auxid_t) override { std::abort(); }

  static std::vector<int> deleted ATLAS_THREAD_SAFE;
};
std::vector<int> MyObjAux::deleted;
CLASS_DEF (MyObjAux, 293847296, 1)

class MyObj
{
public:
  MyObj(int x=0) : x(x) {}
  ~MyObj() { deleted.push_back (x); }
  SG::IAuxStore* getStore() const { return nullptr; }
  void setStore (SG::IConstAuxStore* store) {aux = dynamic_cast<MyObjAux*>(store); }
  int x;
  MyObjAux* aux {nullptr};

  static std::vector<int> deleted ATLAS_THREAD_SAFE;
};
std::vector<int> MyObj::deleted;
CLASS_DEF (MyObj, 293847295, 1)

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF(MyObj, 223019562 );


CLASS_DEF (DataVector<MyObj>, 293847495, 1)


class MyObj2 {};
CLASS_DEF (MyObj2, 293847395, 1)


class MyDObj : public DataObject
{
public:
  MyDObj(int x=0) : x(x) {}
  ~MyDObj() { deleted.push_back (x); }
  int x;

  static std::vector<int> deleted ATLAS_THREAD_SAFE;
};
std::vector<int> MyDObj::deleted;
CLASS_DEF (MyDObj, 293847297, 1)


std::pair<std::unique_ptr<MyObj>, std::unique_ptr<MyObjAux> >
makeWithAux (int x=0)
{
  auto obj = std::make_unique<MyObj>(x);
  auto aux = std::make_unique<MyObjAux>(x+100);
  obj->setStore (aux.get());
  return std::make_pair (std::move(obj), std::move(aux));
}




// Ctors.
void test1()
{
  std::cout << "test1\n";

  SG::WriteCondHandleKeyArray<MyObj> k1 {"MyObj"};

  assert ( k1.size() == 1);
  assert ( k1.mode() == Gaudi::DataHandle::Writer);
  assert ( k1[0].key() == "MyObj");
  // need to setup conditions store
  /* assert ( */ k1[0].initialize() /* .isSuccess())*/ ;

  try {
    std::vector<SG::WriteCondHandle<MyObj> > h1( k1.makeHandles() );
    assert (h1[0].key() == "MyObj");
  }
  catch(...) {
  }

}

int main()
{
  ISvcLocator* svcloc;
  //need MessageSvc
  if (!Athena_test::initGaudi("CondHandleKeyArray_test.txt", svcloc)) {
    return 1;
  }

  test1();
  return 0;
}

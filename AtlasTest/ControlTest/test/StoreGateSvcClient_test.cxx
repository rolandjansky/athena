/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 test the client (algorithm) interface of SG
 -------------------------------------------
 ATLAS Collaboration
 ***************************************************************************/

// $Id: StoreGateSvcClient_test.cxx,v 1.8 2008-07-09 20:47:31 calaf Exp $

#undef NDEBUG

#include <iostream>
#include <string>
#include <cassert>


#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/SGtests.h"
#include "AthenaKernel/ILockable.h"

#ifndef NOGAUDI
#include "SGTools/BaseInfo.h"
#include "AthContainers/DataVector.h"
#include "AthContainers/ConstDataVector.h"

#include "TestTools/initGaudi.h"
using namespace Athena_test;
using namespace std;

namespace Athena_test {
} //end namespace
//-----------------------------------

////////////////////////////////////////////////////////////////////////////
// Test DataVector conversions.
// (This can't go in StoreGate since that doesn't have a DataModel dependency.
//



namespace Athena_test {

struct A
{
  A(int x) : aa(x) {}
  virtual ~A(){}
  int aa;
};
struct B
{
  B(int x) : bb(x) {}
  virtual ~B(){}
  int bb;
};
struct C
  : public A, public B
{
  C(int x) : A(x+100), B(x+1000), cc(x) {}
  int cc;
};

} // namespace Athena_test

CLASS_DEF(Athena_test::A, 82734609, 1)

DATAVECTOR_BASE(Athena_test::C, Athena_test::B);

namespace Athena_test {

struct CV
  : public DataVector<C>
{
public:
  CV() {}
  CV(SG::OwnershipPolicy ownPolicy) : DataVector<C> (ownPolicy) {}
};

} // namespace Athena_test


SG_BASES2 (Athena_test::C, Athena_test::B, Athena_test::A);
SG_BASE(Athena_test::CV, DataVector<Athena_test::C>);

CLASS_DEF(DataVector<A>, 82734619, 1)
CLASS_DEF(DataVector<B>, 82734621, 1)
CLASS_DEF(DataVector<C>, 82734623, 1)
CLASS_DEF(CV, 82734625, 1)

struct Lockable 
  : public ILockable
{
  Lockable() : m_locked (false) {}
  void lock() override { m_locked = true; }
  bool m_locked;
};
CLASS_DEF(Lockable, 82734636, 1)


namespace Athena_test {


class X
{
public:
  int a;
};


class Y
{
public:
  int a;
  int b;
};

class XCopyConversion
  : public SG::CopyConversion<X, Y>
{
public:
  void convert (const X& src, Y& dst) const override
  {
    dst.a = src.a+10;
    dst.b = src.a+20;
  }
};

} // namespace Athena_test


CLASS_DEF(Athena_test::X, 82734627, 1)
CLASS_DEF(Athena_test::Y, 82734628, 1)
SG_ADD_COPY_CONVERSION(Athena_test::X, Athena_test::XCopyConversion);



template <class CONTA, class CONTB, class CONTC, class CONTD>
void test_DVL_conversions1 (StoreGateSvc& sg)
{
  CONTD* contd = new CONTD;
  for (int i=0; i < 10; i++)
    contd->push_back (new C(i));

  assert (sg.record (contd, "contd").isSuccess());

  CONTC* contc = nullptr;
  assert (sg.retrieve (contc, "contd").isSuccess());
  assert (contc == contd);

  CONTB* contb = nullptr;
  assert (sg.retrieve (contb, "contd").isSuccess());
  assert (contb == contd);

  CONTA* conta = nullptr;
  assert (sg.symLink (contd, conta).isSuccess());
  assert (sg.retrieve (conta, "contd").isSuccess());
  assert (conta->size() == 10);
  typename CONTA::iterator it = conta->begin();
  for (int i=0; i < 10; i++) {
    assert ((*it)->aa == i+100);
    ++it;
  }

  for (int i=10; i < 20; i++)
    contd->push_back (new C(i));

  assert (sg.retrieve (conta, "contd").isSuccess());
  assert (conta->size() == 20);
  it = conta->begin();
  for (int i=0; i < 20; i++) {
    assert ((*it)->aa == i+100);
    ++it;
  }
}


void test_DVL_conversions (StoreGateSvc& sg)
{
  cout << "*** test_DVL_conversions\n";
  test_DVL_conversions1<DataVector<A>,
                        DataVector<B>,
                        DataVector<C>,
                        CV>(sg);
}


template <class DVL, class ConstContainer>
void test_const_DVL1 (StoreGateSvc& sg)
{
  ConstContainer* cdvl1 = new ConstContainer;
  for (int i=0; i < 10; i++)
    cdvl1->push_back (new C(i));

  assert (sg.record (cdvl1, "dvl").isSuccess());

  // This should give an error.
  {
    DVL* dvl0 = nullptr;
    assert (sg.retrieve (dvl0, "dvl").isFailure());
  }

  // But this shold work.
  const DVL* dvl1 = nullptr;
  assert (sg.retrieve (dvl1, "dvl").isSuccess());

  ConstContainer* cdvl2 = new ConstContainer (SG::VIEW_ELEMENTS);
  assert (sg.record (cdvl2, "dvl2").isSuccess());
  int i = 0;
  for (typename DVL::const_iterator it = dvl1->begin();
       it != dvl1->end();
       ++it, ++i)
  {
    assert ((*it)->cc == i);
    cdvl2->push_back (*it);
  }

  const DVL* dvl2 = nullptr;
  assert (sg.retrieve (dvl2, "dvl2").isSuccess());
  i = 0;
  for (typename DVL::const_iterator it = dvl2->begin();
       it != dvl2->end();
       ++it, ++i)
  {
    assert ((*it)->cc == i);
  }
}


void test_const_DVL (StoreGateSvc& sg)
{
  cout << "*** test_const_DVL\n";
  test_const_DVL1<CV, ConstDataVector<CV> > (sg);
}


void test_copy_conversions (StoreGateSvc& sg)
{
  cout << "*** test_copy_conversions\n";
  Athena_test::X* x = new Athena_test::X;
  x->a = 10;
  assert (sg.record (x, "x").isSuccess());

  Athena_test::Y* y = nullptr;
  assert (sg.retrieve (y, "x").isFailure());

  assert (sg.setConst (x).isSuccess());

  const Athena_test::Y* cy = nullptr;
  assert (sg.retrieve (cy, "x").isSuccess());
  assert ((char*)cy != (char*)x);
  assert (cy->a == 20);
  assert (cy->b == 30);

  const Athena_test::A* ca = nullptr;
  assert (sg.retrieve (ca, "x").isFailure());
}

void test_lock (StoreGateSvc& sg)
{
  cout << "*** test_lock\n";
  Lockable* l = new Lockable;
  assert (sg.record (l, "lockable1").isSuccess());
  assert (l->m_locked == false);
  assert (sg.setConst (l).isSuccess());
  assert (l->m_locked == true);

  sg.clearStore();
  Lockable* l2 = new Lockable;
  assert (sg.record (l2, "lockable1").isSuccess());
}


////////////////////////////////////////////////////////////////////////////

int main() {
  ISvcLocator* pSvcLoc;
  if (!initGaudi("StoreGateSvcClient_test.txt", pSvcLoc)) {
    cerr << "This test can not be run" << endl;
    return 0;
  }  
  assert(pSvcLoc);

  StoreGateSvc* pStore(nullptr);
  static const bool CREATE(true);
  assert((pSvcLoc->service("StoreGateSvc", pStore, CREATE)).isSuccess());
  assert(pStore);

  testRecord(*pStore);
  testContains(*pStore);
  testRetrieve(*pStore);
  testTryRetrieve(*pStore);
  testSymLink(*pStore);
  testReadPrivateCopy(*pStore);
  testRetrievePrivateCopy(*pStore);
  testRemove(*pStore);
  testCreate(*pStore);
  testBind(*pStore); 
#ifdef FOLDERS_ASDOBJ
  testFolders(*pStore);
#endif

  cout << "Testing dump: \n -------->>\n" 
       << pStore->dump() 
       << "\n<<--------" << endl;
  
  pStore->clearStore(/*force=*/true).ignore();
   
  test_symlink2 (*pStore);
  test_symlink3 (*pStore);
  pStore->clearStore(/*force=*/true).ignore();

  testTransientSwap(*pStore);

  testVersionedKey(*pStore);
  pStore->clearStore(/*force=*/true).ignore();

  test_DVL_conversions (*pStore);
  test_const_DVL (*pStore);

  test_copy_conversions (*pStore);

  testKeys(*pStore);

  testRetrieveAux(*pStore);
  testBoundReset(*pStore);

  testRecordObject(*pStore);

  testWriteAux(*pStore);

  test_lock (*pStore);

  pStore->finalize().ignore();

  ///////

  StoreGateSvc* detStore(nullptr);
  assert((pSvcLoc->service("DetectorStore", detStore, CREATE)).isSuccess());
  assert(detStore);

  testRecord(*detStore);

  cout << "*** StoreGateSvcClient_test OK ***" <<endl;
  return 0;
}
#endif /*NOGAUDI*/

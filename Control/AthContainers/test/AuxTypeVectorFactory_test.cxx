/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/test/AuxTypeVectorFactory_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2014
 * @brief Regression tests for AuxTypeVectorFactory.
 */


#undef NDEBUG


#include "AthContainers/tools/AuxTypeVectorFactory.h"
#include <iostream>
#include <cassert>

#ifndef XAOD_STANDALONE
#include "AthLinks/ElementLink.h"
#include "SGTools/TestStore.h"
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF (std::vector<int*>, 28374627, 0)
#endif


template <class T>
T makeT(int x=0) { return T(x); }

bool makeT(int x=0) { return (x&1) != 0; }


template <class T>
void test_vector()
{
  SG::AuxTypeVectorFactory<T> fac;
  assert (fac.getEltSize() == sizeof(T));
  assert (!fac.isDynamic());
  if (typeid(T) == typeid(bool))
    assert (fac.tiVec() == &typeid (std::vector<char>));
  else
    assert (fac.tiVec() == &typeid (std::vector<T>));

  std::unique_ptr<SG::IAuxTypeVector> v = fac.create (10, 20);
  T* ptr = reinterpret_cast<T*> (v->toPtr());
  ptr[0] = makeT(20);
  ptr[1] = makeT(2);

  std::unique_ptr<SG::IAuxTypeVector> v2 = fac.create (10, 20);
  T* ptr2 = reinterpret_cast<T*> (v2->toPtr());
  fac.copy (ptr2, 0, ptr, 1);
  fac.copy (ptr2, 1, ptr, 0);
  assert (ptr2[0] == makeT(2));
  assert (ptr2[1] == makeT(20));

  ptr2[0] = makeT(10);
  ptr2[1] = makeT(11);

  fac.swap (ptr2, 0, ptr, 1);
  assert (ptr[0] == makeT(20));
  assert (ptr[1] == makeT(10));
  assert (ptr2[0] == makeT(2));
  assert (ptr2[1] == makeT(11));

  fac.clear (ptr2, 0);
  assert (ptr2[0] == makeT());
  assert (ptr2[1] == makeT(11));

  typedef typename SG::AuxTypeVectorHolder<T>::vector_type vector_type;
  vector_type* vec3 = new vector_type;
  vec3->push_back (makeT(3));
  vec3->push_back (makeT(2));
  vec3->push_back (makeT(1));
  std::unique_ptr<SG::IAuxTypeVector> v3 = fac.createFromData (vec3, false, true);
  assert (v3->size() == 3);
  T* ptr3 = reinterpret_cast<T*> (v3->toPtr());
  assert (ptr3[0] == makeT(3));
  assert (ptr3[1] == makeT(2));
  assert (ptr3[2] == makeT(1));
}


template <class T>
void test_vector2()
{
  SG::AuxTypeVectorFactory<T> fac;
  SG::PackedContainer<T>* vec4 = new SG::PackedContainer<T>;
  vec4->push_back (makeT(4));
  vec4->push_back (makeT(3));
  vec4->push_back (makeT(2));
  vec4->push_back (makeT(1));
  std::unique_ptr<SG::IAuxTypeVector> v4 = fac.createFromData (vec4, true, true);
  assert (v4->size() == 4);
  T* ptr4 = reinterpret_cast<T*> (v4->toPtr());
  assert (ptr4[0] == makeT(4));
  assert (ptr4[1] == makeT(3));
  assert (ptr4[2] == makeT(2));
  assert (ptr4[3] == makeT(1));
}


void test1()
{
  std::cout << "test1\n";
  test_vector<int>();
  test_vector2<int>();
  test_vector<bool>();
  test_vector<float>();
  test_vector2<float>();
}


// testing copyForOutput.
void test2()
{
  std::cout << "test2\n";

#ifndef XAOD_STANDALONE
  std::unique_ptr<SGTest::TestStore> store = SGTest::getTestStore();

  typedef ElementLink<std::vector<int*> > EL;
  EL elv[10];
  elv[1] = EL (123, 10);

  SG::AuxTypeVectorFactory<EL> ve1;
  SG::AuxTypeVectorFactory<std::vector<EL> > ve2;

  ve1.copyForOutput (elv, 2, elv, 1);
  assert (elv[2].key() == 123);
  assert (elv[2].index() == 10);

  std::vector<EL> velv[10];
  velv[1].push_back (EL (123, 5));
  velv[1].push_back (EL (123, 6));
  ve2.copyForOutput (velv, 2, velv, 1);
  assert (velv[2][0].key() == 123);
  assert (velv[2][0].index() == 5);
  assert (velv[2][1].key() == 123);
  assert (velv[2][1].index() == 6);

  store->remap (123, 456, 10, 20);

  ve1.copyForOutput (elv, 5, elv, 1);
  assert (elv[5].key() == 456);
  assert (elv[5].index() == 20);

  store->remap (123, 456, 6, 12);
  ve2.copyForOutput (velv, 5, velv, 1);
  assert (velv[5][0].key() == 123);
  assert (velv[5][0].index() == 5);
  assert (velv[5][1].key() == 456);
  assert (velv[5][1].index() == 12);
#endif
}


int main()
{
  std::cout << "AuxTypeVectorFactory_test\n";
  test1();
  test2();
  return 0;
}

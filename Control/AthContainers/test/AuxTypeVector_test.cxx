/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/test/AuxTypeVector_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2013
 * @brief Regression tests for AuxTypeVector.
 */


#undef NDEBUG


#include "AthContainers/tools/AuxTypeVector.h"
#include <iostream>
#include <cassert>


template <class T>
T makeT(int x=0) { return T(x); }

bool makeT(int x=0) { return (x&1) != 0; }


template <class T>
void test_vector()
{
  SG::IAuxTypeVector* v = new SG::AuxTypeVector<T> (10, 20);
  T* ptr = reinterpret_cast<T*> (v->toPtr());
  ptr[0] = makeT(1);
  ptr[1] = makeT(2);
  assert (v->size() == 10);

  v->reserve (50);
  ptr = reinterpret_cast<T*> (v->toPtr());
  v->resize (40);
  T* ptr2 = reinterpret_cast<T*> (v->toPtr());
  assert (ptr == ptr2);
  assert (ptr[0] == makeT(1));
  assert (ptr[1] == makeT(2));
  assert (v->size() == 40);

  v->shift (1, 1);
  assert (ptr[0] == makeT(1));
  assert (ptr[1] == makeT());
  assert (ptr[2] == makeT(2));

  ptr[1] = makeT(20);
  v->shift (1, -1);
  assert (ptr[0] == makeT(20));
  assert (ptr[1] == makeT(2));

  SG::IAuxTypeVector* v2 = new SG::AuxTypeVector<T> (10, 20);
  ptr2 = reinterpret_cast<T*> (v2->toPtr());
  SG::AuxTypeVector<T>::copy (ptr2, 0, ptr, 1);
  SG::AuxTypeVector<T>::copy (ptr2, 1, ptr, 0);
  assert (ptr2[0] == makeT(2));
  assert (ptr2[1] == makeT(20));

  ptr2[0] = makeT(10);
  ptr2[1] = makeT(11);

  SG::AuxTypeVector<T>::swap (ptr2, 0, ptr, 1);
  assert (ptr[0] == makeT(20));
  assert (ptr[1] == makeT(10));
  assert (ptr2[0] == makeT(2));
  assert (ptr2[1] == makeT(11));

  SG::AuxTypeVector<T>::clear (ptr2, 0);
  assert (ptr2[0] == makeT());
  assert (ptr2[1] == makeT(11));

  SG::IAuxTypeVector* v3 = v->clone();
  assert (v3->size() == v->size());
  T* ptr3 = reinterpret_cast<T*> (v3->toPtr());
  for (size_t i = 0; i < v->size(); i++)
    assert (ptr[i] == ptr3[i]);

  v->resize (0);
  assert (v->toPtr() == 0);

  delete v;
  delete v2;
  delete v3;
}


void test1()
{
  std::cout << "test1\n";
  test_vector<int>();
  test_vector<bool>();
  test_vector<float>();
}


int main()
{
  test1();
  return 0;
}

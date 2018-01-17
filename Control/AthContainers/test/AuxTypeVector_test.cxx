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
#include "CxxUtils/checker_macros.h"
#include <vector>
#include <iostream>
#include <cassert>
#include <memory>


template <class T>
T makeT1(int x, T*) { return T(x); }

bool makeT1(int x, bool) { return (x&1) != 0; }

template <class T>
T makeT(int x=0) { return makeT1(x, static_cast<T*>(nullptr)); }


struct MoveTest
{
  MoveTest(int x=0) : m_v(x) {}
  MoveTest(const MoveTest& other): m_v (other.m_v) {}
  MoveTest(MoveTest&& other): m_v (std::move(other.m_v)) {}
  MoveTest& operator= (const MoveTest& other) {
    if (this != &other) m_v = other.m_v;
    return *this;
  }
  MoveTest& operator= (MoveTest&& other) {
    if (this != &other) m_v = std::move(other.m_v);
    return *this;
  }
  std::vector<int> m_v;
  bool operator== (const MoveTest& other) const { return m_v.size() == other.m_v.size(); }
};


template <class T>
bool wasMoved (const T&) { return true; }

bool wasMoved (const MoveTest& x) { return x.m_v.empty(); }


template <class T>
void test_vector1()
{
  SG::AuxTypeVector<T>* vconcrete = new SG::AuxTypeVector<T> (10, 20);
  SG::IAuxTypeVector* v = vconcrete;
  T* ptr = reinterpret_cast<T*> (v->toPtr());
  ptr[0] = makeT<T>(1);
  ptr[1] = makeT<T>(2);
  assert (v->size() == 10);

  assert (vconcrete->vec()[0] == makeT<T>(1));
  assert (vconcrete->vec()[1] == makeT<T>(2));
  assert (&vconcrete->vec() == v->toVector());

  v->reserve (50);
  ptr = reinterpret_cast<T*> (v->toPtr());
  assert (v->resize (40) == true);
  T* ptr2 = reinterpret_cast<T*> (v->toPtr());
  assert (ptr == ptr2);
  assert (ptr[0] == makeT<T>(1));
  assert (ptr[1] == makeT<T>(2));
  assert (v->size() == 40);

  v->shift (1, 1);
  assert (ptr[0] == makeT<T>(1));
  assert (ptr[1] == makeT<T>());
  assert (ptr[2] == makeT<T>(2));

  ptr[1] = makeT<T>(20);
  v->shift (1, -1);
  assert (ptr[0] == makeT<T>(20));
  assert (ptr[1] == makeT<T>(2));

  SG::IAuxTypeVector* v2 = new SG::AuxTypeVector<T> (10, 20);
  ptr2 = reinterpret_cast<T*> (v2->toPtr());
  SG::AuxTypeVector<T>::copy (ptr2, 0, ptr, 1);
  SG::AuxTypeVector<T>::copy (ptr2, 1, ptr, 0);
  assert (ptr2[0] == makeT<T>(2));
  assert (ptr2[1] == makeT<T>(20));

  ptr2[0] = makeT<T>(10);
  ptr2[1] = makeT<T>(11);

  SG::AuxTypeVector<T>::swap (ptr2, 0, ptr, 1);
  assert (ptr[0] == makeT<T>(20));
  assert (ptr[1] == makeT<T>(10));
  assert (ptr2[0] == makeT<T>(2));
  assert (ptr2[1] == makeT<T>(11));

  SG::AuxTypeVector<T>::clear (ptr2, 0);
  assert (ptr2[0] == makeT<T>());
  assert (ptr2[1] == makeT<T>(11));

  SG::IAuxTypeVector* v3 = v->clone();
  assert (v3->size() == v->size());
  T* ptr3 = reinterpret_cast<T*> (v3->toPtr());
  for (size_t i = 0; i < v->size(); i++)
    assert (ptr[i] == ptr3[i]);

  assert (v->resize (0) == true);
  assert (v->toPtr() == 0);

  if (typeid(T) == typeid(bool))
    assert (v->objType() == &typeid(std::vector<char>));
  else
    assert (v->objType() == &typeid(std::vector<T>));

  assert (v->resize (1000) == false);
  assert (v->resize (500) == true);
  assert (v->resize (1000) == true);

  delete v;
  delete v2;
  delete v3;
}


template <class T>
void test_vector2()
{
  SG::AuxTypeVector<T> v1 (10, 10);
  T* ptr1 = reinterpret_cast<T*> (v1.toPtr());
  ptr1[0] = makeT<T>(1);
  ptr1[1] = makeT<T>(2);

  SG::AuxTypeVector<T> v2 (v1);
  T* ptr2 = reinterpret_cast<T*> (v2.toPtr());
  assert (v1.size() == 10);
  assert (v2.size() == 10);
  assert (ptr1[0] == makeT<T>(1));
  assert (ptr1[1] == makeT<T>(2));
  assert (ptr2[0] == makeT<T>(1));
  assert (ptr2[1] == makeT<T>(2));

  SG::AuxTypeVector<T> v3 (0, 0);
  v3 = v1;
  T* ptr3 = reinterpret_cast<T*> (v3.toPtr());
  assert (v1.size() == 10);
  assert (v3.size() == 10);
  assert (ptr1[0] == makeT<T>(1));
  assert (ptr1[1] == makeT<T>(2));
  assert (ptr3[0] == makeT<T>(1));
  assert (ptr3[1] == makeT<T>(2));

  v3.resize (3);
  ptr3[0] = makeT<T>(3);
  ptr3[1] = makeT<T>(2);
  ptr3[2] = makeT<T>(1);

  SG::AuxTypeVector<T> v4 (std::move (v3));
  T* ptr4 = reinterpret_cast<T*> (v4.toPtr());
  assert (v4.size() == 3);
  assert (v3.size() == 0);
  assert (ptr4[0] == makeT<T>(3));
  assert (ptr4[1] == makeT<T>(2));
  assert (ptr4[2] == makeT<T>(1));

  v3 = std::move(v4);
  assert (v3.size() == 3);
  assert (v4.size() == 0);
  assert (ptr3[0] == makeT<T>(3));
  assert (ptr3[1] == makeT<T>(2));
  assert (ptr3[2] == makeT<T>(1));
}


template <class T>
void test_vector3()
{
  typedef typename SG::AuxTypeVectorHolder<T>::vector_type vector_type;

  vector_type* vptr1 = new vector_type;
  vptr1->push_back (makeT<T>(1));
  vptr1->push_back (makeT<T>(2));
  vptr1->push_back (makeT<T>(3));

  SG::AuxTypeVectorHolder<T> v1 (vptr1, true);
  assert (v1.size() == 3);
  T* ptr1 = reinterpret_cast<T*> (v1.toPtr());
  assert (ptr1[0] == makeT<T>(1));
  assert (ptr1[1] == makeT<T>(2));
  assert (ptr1[2] == makeT<T>(3));

  SG::AuxTypeVectorHolder<T> v2 (v1);
  assert (v1.size() == 3);
  assert (v2.size() == 3);
  T* ptr2 = reinterpret_cast<T*> (v2.toPtr());
  assert (ptr2[0] == makeT<T>(1));
  assert (ptr2[1] == makeT<T>(2));
  assert (ptr2[2] == makeT<T>(3));

  v2.resize(2);
  ptr2[0] = makeT<T>(2);
  ptr2[1] = makeT<T>(1);
  v1 = v2;
  assert (v1.size() == 2);
  assert (v2.size() == 2);
  assert (ptr2[0] == makeT<T>(2));
  assert (ptr2[1] == makeT<T>(1));

  SG::AuxTypeVectorHolder<T> v3 (std::move(v2));
  assert (v2.size() == 2);
  assert (v3.size() == 2);
  T* ptr3 = reinterpret_cast<T*> (v3.toPtr());
  assert (ptr3[0] == makeT<T>(2));
  assert (ptr3[1] == makeT<T>(1));

  v1 = std::move(v3);
  ptr1 = reinterpret_cast<T*> (v1.toPtr());
  assert (v1.size() == 2);
  assert (v3.size() == 2);
  assert (ptr1[0] == makeT<T>(2));
  assert (ptr1[1] == makeT<T>(1));
}


// Testing insertMove
template <class T>
void test_vector4 (bool isPOD)
{
  SG::AuxTypeVector<T> v1 (10, 20);
  T* ptr1 = reinterpret_cast<T*> (v1.toPtr());
  for (int i=0; i<10; i++)
    ptr1[i] = makeT<T>(i);

  SG::AuxTypeVector<T> v2 (5, 5);
  T* ptr2 = reinterpret_cast<T*> (v2.toPtr());
  for (int i=0; i<5; i++)
    ptr2[i] = makeT<T>(i+10);

  assert (v1.insertMove (3, ptr2, ptr2+5));
  assert (v1.size() == 15);
  for (int i=0; i<3; i++)
    assert (ptr1[i] == makeT<T>(i));
  for (int i=0; i<5; i++)
    assert (ptr1[3+i] == makeT<T>(10+i));
  for (int i=0; i<7; i++)
    assert (ptr1[8+i] == makeT<T>(3+i));
  assert (v2.size() == 5);
  for (int i=0; i<5; i++)
    assert (wasMoved (ptr2[i]));

  for (int i=0; i<5; i++)
    ptr2[i] = makeT<T>(i+20);
  for (int i=0; i<5; i++)
    assert (isPOD || !wasMoved (ptr2[i]));

  assert (v1.insertMove (15, ptr2, ptr2+5));
  assert (v1.size() == 20);
  for (int i=0; i<3; i++)
    assert (ptr1[i] == makeT<T>(i));
  for (int i=0; i<5; i++)
    assert (ptr1[3+i] == makeT<T>(10+i));
  for (int i=0; i<7; i++)
    assert (ptr1[8+i] == makeT<T>(3+i));
  for (int i=0; i<5; i++)
    assert (ptr1[15+i] == makeT<T>(20+i));
  assert (v2.size() == 5);
  for (int i=0; i<5; i++)
    assert (wasMoved (ptr2[i]));

  SG::AuxTypeVector<T> v3 (1000, 1000);
  T* ptr3 = reinterpret_cast<T*> (v3.toPtr());
  assert ( ! v1.insertMove (20, ptr3, ptr3 + v3.size()) );
}


template <class T>
void test_vector()
{
  test_vector1<T>();
  test_vector2<T>();
  test_vector3<T>();
  test_vector4<T>(true);
}


void test1()
{
  std::cout << "test1\n";
  test_vector<int>();
  test_vector<bool>();
  test_vector<float>();
  test_vector4<MoveTest>(false);
}


class TestContainer
  : public std::vector<int>, public SG::IAuxSetOption
{
public:
  virtual bool setOption (const SG::AuxDataOption& option) 
  { lastopt = option; return true; }

  static SG::AuxDataOption lastopt ATLAS_THREAD_SAFE;
};


SG::AuxDataOption TestContainer::lastopt ("", 0);


// test setOption
void test2()
{
  std::cout << "test2\n";

  SG::AuxTypeVector<int> v1 (10, 20);
  assert (!v1.setOption (SG::AuxDataOption ("opt", 1)));

  SG::AuxTypeVector<int, TestContainer> v2 (10, 20);
  assert (v2.setOption (SG::AuxDataOption ("opt", 1)));
  assert (TestContainer::lastopt.name() == "opt");
  assert (TestContainer::lastopt.intVal() == 1);
}


// test toPacked
void test3()
{
  std::cout << "test3\n";
  SG::AuxTypeVector<int> v1 (0, 0);
  v1.vec().push_back(1);
  v1.vec().push_back(2);

  assert (v1.objType() == &typeid(std::vector<int>));

  void* ptr = v1.toPtr();
  int* iptr = reinterpret_cast<int*>(ptr);
  assert (v1.size() == 2);
  assert (iptr[0] == 1);
  assert (iptr[1] == 2);

  SG::IAuxTypeVector* v2 = v1.toPacked();
  assert (v2 != 0);
  assert (ptr == v2->toPtr());
  assert (v2->size() == 2);
  assert (iptr[0] == 1);
  assert (iptr[1] == 2);
  assert (v1.size() == 0);
  assert (v1.toPtr() == 0);

  assert (v2->objType() == &typeid(SG::PackedContainer<int>));
  SG::PackedContainer<int>* pptr =
    reinterpret_cast<SG::PackedContainer<int>*> (v2->toVector());
  assert (typeid(*pptr) == typeid(SG::PackedContainer<int>));

  SG::AuxTypeVector<std::string> v3 (0, 0);
  v3.vec().push_back("1");
  v3.vec().push_back("2");

  assert (v3.toPacked() == 0);
  assert (v3.size() == 2);
}


int main()
{
  test1();
  test2();
  test3();
  return 0;
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainersRoot/test/RootAuxVectorFactory_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2014
 * @brief Regression test for RootAuxVectorFactory.
 */


#undef NDEBUG
#include "AthContainersRoot/RootAuxVectorFactory.h"
#include "CxxUtils/StrFormat.h"
#include "TClass.h"
#include <iostream>
#include <cassert>


std::string str (int x)
{
  return CxxUtils::strformat ("%d", x);
}


void test1()
{
  std::cout << "test1\n";
  TClass* cl = TClass::GetClass ("vector<int>");
  SG::RootAuxVectorFactory fac (cl);
  SG::IAuxTypeVector* vec = new SG::RootAuxVector (&fac, 10, 20);
  assert (vec->size() == 10);
  int* ptr = reinterpret_cast<int*> (vec->toPtr());
  for (int i=0; i < 10; i++)
    ptr[i] = i+1;
  assert (vec->resize (100) == false);
  vec->reserve (100);
  ptr = reinterpret_cast<int*> (vec->toPtr());
  for (int i=0; i < 10; i++)
    assert (ptr[i] == i+1);
  std::vector<int>* pvec = reinterpret_cast<std::vector<int>*> (vec->toVector());
  assert (pvec->size() == 100);
  for (int i=0; i < 10; i++)
    assert ((*pvec)[i] == i+1);

  // 1 2 3 4 5 6 7 8 9 10

  assert (vec->resize (10) == true);
  vec->shift (7, -3);
  assert (vec->size() == 7);
  // 1 2 3 4 8 9 10
  ptr = reinterpret_cast<int*> (vec->toPtr());
  assert (ptr[0] == 1);
  assert (ptr[1] == 2);
  assert (ptr[2] == 3);
  assert (ptr[3] == 4);
  assert (ptr[4] == 8);
  assert (ptr[5] == 9);
  assert (ptr[6] == 10);

  vec->shift (3, 2);
  assert (vec->size() == 9);
  // 1 2 3 0 0 4 8 9 10
  ptr = reinterpret_cast<int*> (vec->toPtr());
  assert (ptr[0] == 1);
  assert (ptr[1] == 2);
  assert (ptr[2] == 3);
  assert (ptr[3] == 0);
  assert (ptr[4] == 0);
  assert (ptr[5] == 4);
  assert (ptr[6] == 8);
  assert (ptr[7] == 9);
  assert (ptr[8] == 10);

  SG::IAuxTypeVector* vec2 = vec->clone();
  int* ptr2 = reinterpret_cast<int*> (vec2->toPtr());
  assert (ptr != ptr2);
  assert (ptr2[0] == 1);
  assert (ptr2[1] == 2);
  assert (ptr2[2] == 3);
  assert (ptr2[3] == 0);
  assert (ptr2[4] == 0);
  assert (ptr2[5] == 4);
  assert (ptr2[6] == 8);
  assert (ptr2[7] == 9);
  assert (ptr2[8] == 10);

  std::vector<int> vec3 { 20, 21, 22, 23, 24 };
  assert (vec->insertMove (3, vec3.data(), vec3.data() + 5));
  assert (vec->size() == 14);
  assert (ptr[0] == 1);
  assert (ptr[1] == 2);
  assert (ptr[2] == 3);
  assert (ptr[3] == 20);
  assert (ptr[4] == 21);
  assert (ptr[5] == 22);
  assert (ptr[6] == 23);
  assert (ptr[7] == 24);
  assert (ptr[8] == 0);
  assert (ptr[9] == 0);
  assert (ptr[10] == 4);
  assert (ptr[11] == 8);
  assert (ptr[12] == 9);
  assert (ptr[13] == 10);

  std::vector<int> vec4 { 30, 31, 32, 33, 34 };
  assert (vec->insertMove (14, vec4.data(), vec4.data() + 5));
  assert (vec->size() == 19);
  assert (ptr[0] == 1);
  assert (ptr[1] == 2);
  assert (ptr[2] == 3);
  assert (ptr[3] == 20);
  assert (ptr[4] == 21);
  assert (ptr[5] == 22);
  assert (ptr[6] == 23);
  assert (ptr[7] == 24);
  assert (ptr[8] == 0);
  assert (ptr[9] == 0);
  assert (ptr[10] == 4);
  assert (ptr[11] == 8);
  assert (ptr[12] == 9);
  assert (ptr[13] == 10);
  assert (ptr[14] == 30);
  assert (ptr[15] == 31);
  assert (ptr[16] == 32);
  assert (ptr[17] == 33);
  assert (ptr[18] == 34);

  assert (vec->resize (0) == false);
  assert (vec->toPtr() == 0);

  delete vec;
}


void test2()
{
  std::cout << "test2\n";
  TClass* cl = TClass::GetClass ("vector<std::string>");
  SG::RootAuxVectorFactory fac (cl);
  SG::IAuxTypeVector* vec = new SG::RootAuxVector (&fac, 10, 10);
  assert (vec->size() == 10);
  std::string* ptr = reinterpret_cast<std::string*> (vec->toPtr());
  for (int i=0; i < 10; i++)
    ptr[i] = str(i+1);
  vec->resize (100);
  vec->reserve (100);
  ptr = reinterpret_cast<std::string*> (vec->toPtr());
  for (int i=0; i < 10; i++)
    assert (ptr[i] == str(i+1));
  std::vector<std::string>* pvec = reinterpret_cast<std::vector<std::string>*> (vec->toVector());
  assert (pvec->size() == 100);
  for (int i=0; i < 10; i++)
    assert ((*pvec)[i] == str(i+1));

  // 1 2 3 4 5 6 7 8 9 10

  vec->resize (10);
  vec->shift (7, -3);
  assert (vec->size() == 7);
  // 1 2 3 4 8 9 10
  ptr = reinterpret_cast<std::string*> (vec->toPtr());
  assert (ptr[0] == str(1));
  assert (ptr[1] == str(2));
  assert (ptr[2] == str(3));
  assert (ptr[3] == str(4));
  assert (ptr[4] == str(8));
  assert (ptr[5] == str(9));
  assert (ptr[6] == str(10));

  vec->shift (3, 2);
  assert (vec->size() == 9);
  // 1 2 3 "" "" 4 8 9 10
  ptr = reinterpret_cast<std::string*> (vec->toPtr());
  assert (ptr[0] == str(1));
  assert (ptr[1] == str(2));
  assert (ptr[2] == str(3));
  assert (ptr[3] == "");
  assert (ptr[4] == "");
  assert (ptr[5] == str(4));
  assert (ptr[6] == str(8));
  assert (ptr[7] == str(9));
  assert (ptr[8] == str(10));

  std::vector<std::string> vec3 { str(20), str(21), str(22), str(23), str(24) };
  assert (vec->insertMove (3, vec3.data(), vec3.data() + 5));
  assert (vec->size() == 14);
  assert (ptr[0] == str(1));
  assert (ptr[1] == str(2));
  assert (ptr[2] == str(3));
  assert (ptr[3] == str(20));
  assert (ptr[4] == str(21));
  assert (ptr[5] == str(22));
  assert (ptr[6] == str(23));
  assert (ptr[7] == str(24));
  assert (ptr[8] == "");
  assert (ptr[9] == "");
  assert (ptr[10] == str(4));
  assert (ptr[11] == str(8));
  assert (ptr[12] == str(9));
  assert (ptr[13] == str(10));

  std::vector<std::string> vec4 { str(30), str(31), str(32), str(33), str(34) };
  assert (vec->insertMove (14, vec4.data(), vec4.data() + 5));
  assert (vec->size() == 19);
  assert (ptr[0] == str(1));
  assert (ptr[1] == str(2));
  assert (ptr[2] == str(3));
  assert (ptr[3] == str(20));
  assert (ptr[4] == str(21));
  assert (ptr[5] == str(22));
  assert (ptr[6] == str(23));
  assert (ptr[7] == str(24));
  assert (ptr[8] == "");
  assert (ptr[9] == "");
  assert (ptr[10] == str(4));
  assert (ptr[11] == str(8));
  assert (ptr[12] == str(9));
  assert (ptr[13] == str(10));
  assert (ptr[14] == str(30));
  assert (ptr[15] == str(31));
  assert (ptr[16] == str(32));
  assert (ptr[17] == str(33));
  assert (ptr[18] == str(34));

  delete vec;
}


void test3()
{
  std::cout << "test3\n";
  TClass* cl = TClass::GetClass ("vector<int>");
  SG::RootAuxVectorFactory fac (cl);
  assert (fac.vecClass() == cl);
  assert (fac.getEltSize() == sizeof(int));
  assert (fac.tiVec() == &typeid(std::vector<int>));
  assert (fac.isDynamic());
  SG::IAuxTypeVector* vec = fac.create (10, 10);
  assert (vec->size() == 10);

  int* ptr = reinterpret_cast<int*> (vec->toPtr());
  ptr[0] = 1;
  ptr[1] = 2;
  fac.copy (ptr, 5, ptr, 1);
  assert (ptr[1] == 2);
  assert (ptr[5] == 2);

  fac.clear (ptr, 1);
  assert (ptr[1] == 0);

  fac.swap (ptr, 0, ptr, 5);
  assert (ptr[0] == 2);
  assert (ptr[5] == 1);

  delete vec;
}


void test4()
{
  std::cout << "test4\n";
  TClass* cl = TClass::GetClass ("vector<std::string>");
  SG::RootAuxVectorFactory fac (cl);
  assert (fac.vecClass() == cl);
  assert (fac.getEltSize() == sizeof(std::string));
  assert (fac.tiVec() == &typeid(std::vector<std::string>));
  assert (fac.isDynamic());
  SG::IAuxTypeVector* vec = fac.create (10, 10);
  assert (vec->size() == 10);

  std::string* ptr = reinterpret_cast<std::string*> (vec->toPtr());
  ptr[0] = "1";
  ptr[1] = "2";
  fac.copy (ptr, 5, ptr, 1);
  assert (ptr[1] == "2");
  assert (ptr[5] == "2");

  fac.clear (ptr, 1);
  assert (ptr[1] == "");

  fac.swap (ptr, 0, ptr, 5);
  assert (ptr[0] == "2");
  assert (ptr[5] == "1");

  delete vec;
}


void test5()
{
  std::cout << "test5\n";

  TClass* cl = TClass::GetClass ("vector<int>");
  SG::RootAuxVectorFactory fac (cl);

  std::vector<int>* vec1 = new std::vector<int>;
  vec1->push_back(3);
  vec1->push_back(2);
  vec1->push_back(1);
  SG::IAuxTypeVector* v1 = fac.createFromData (vec1, false, true);
  assert (v1->size() == 3);
  int* ptr1 = reinterpret_cast<int*> (v1->toPtr());
  assert (ptr1[0] == 3);
  assert (ptr1[1] == 2);
  assert (ptr1[2] == 1);
  delete v1;

  std::vector<int>* vec2 = new std::vector<int>;
  vec2->push_back(4);
  vec2->push_back(5);
  SG::IAuxTypeVector* v2 = fac.createFromData (vec2, false, false);
  assert (v2->size() == 2);
  int* ptr2 = reinterpret_cast<int*> (v2->toPtr());
  assert (ptr2[0] == 4);
  assert (ptr2[1] == 5);
  delete v2;
  delete vec2;
}


int main()
{
  test1();
  test2();
  test3();
  test4();
  test5();
  return 0;
}

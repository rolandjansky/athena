/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file xAODRootAccess/test/ut_xaodrootaccess_tauxvector_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2017
 * @brief Unit tests for TAuxVector.  (sadly incomplete)
 */


#undef NDEBUG
#include "xAODRootAccess/tools/TAuxVector.h"
#include "xAODRootAccess/tools/TAuxVectorFactory.h"
#include "CxxUtils/StrFormat.h"
#include "TClass.h"
#include <iostream>
#include <sstream>
#include <cassert>


std::string str (int x)
{
  return CxxUtils::strformat ("%d", x);
}


// Test insertMove.
void test1()
{
  std::cout << "test1\n";

  TClass* cl_int = TClass::GetClass ("vector<int>");
  xAOD::TAuxVectorFactory fac_int (cl_int);
  xAOD::TAuxVector vec_int = xAOD::TAuxVector (&fac_int, cl_int, 5, 5);
  int* ptr_int = reinterpret_cast<int*> (vec_int.toPtr());

  for (int i=0; i < 5; i++)
    ptr_int[i] = i;

  std::vector<int> v2_int { 10, 11, 12, 13, 14 };
  vec_int.insertMove (3, v2_int.data(), v2_int.data() + 5);
  assert (vec_int.size() == 10);
  ptr_int = reinterpret_cast<int*> (vec_int.toPtr());
  for (int i=0; i < 3; i++)
    assert (ptr_int[i] == i);
  for (int i=0; i < 5; i++)
    assert (ptr_int[3+i] == 10+i);
  for (int i=3; i < 5; i++)
    assert (ptr_int[5+i] == i);

  std::vector<int> v3_int { 20, 21, 22, 23, 24 };
  vec_int.insertMove (10, v3_int.data(), v3_int.data() + 5);
  assert (vec_int.size() == 15);
  ptr_int = reinterpret_cast<int*> (vec_int.toPtr());
  for (int i=0; i < 3; i++)
    assert (ptr_int[i] == i);
  for (int i=0; i < 5; i++)
    assert (ptr_int[3+i] == 10+i);
  for (int i=3; i < 5; i++)
    assert (ptr_int[5+i] == i);
  for (int i=0; i < 5; i++)
    assert (ptr_int[10+i] == 20+i);

  //*********************************

  TClass* cl_str = TClass::GetClass ("vector<std::string>");
  xAOD::TAuxVectorFactory fac_str (cl_str);
  xAOD::TAuxVector vec_str = xAOD::TAuxVector (&fac_str, cl_str, 5, 5);
  std::string* ptr_str = reinterpret_cast<std::string*> (vec_str.toPtr());

  for (int i=0; i < 5; i++)
    ptr_str[i] = str(i);

  std::vector<std::string> v2_str { str(10), str(11), str(12), str(13), str(14) };
  vec_str.insertMove (3, v2_str.data(), v2_str.data() + 5);
  assert (vec_str.size() == 10);
  ptr_str = reinterpret_cast<std::string*> (vec_str.toPtr());
  for (int i=0; i < 3; i++)
    assert (ptr_str[i] == str(i));
  for (int i=0; i < 5; i++)
    assert (ptr_str[3+i] == str(10+i));
  for (int i=3; i < 5; i++)
    assert (ptr_str[5+i] == str(i));

  std::vector<std::string> v3_str { str(20), str(21), str(22), str(23), str(24) };
  vec_str.insertMove (10, v3_str.data(), v3_str.data() + 5);
  assert (vec_str.size() == 15);
  ptr_str = reinterpret_cast<std::string*> (vec_str.toPtr());
  for (int i=0; i < 3; i++)
    assert (ptr_str[i] == str(i));
  for (int i=0; i < 5; i++)
    assert (ptr_str[3+i] == str(10+i));
  for (int i=3; i < 5; i++)
    assert (ptr_str[5+i] == str(i));
  for (int i=0; i < 5; i++)
    assert (ptr_str[10+i] == str(20+i));
}


int main()
{
  test1();
  return 0;
}

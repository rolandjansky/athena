/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file RootUtils/test/Type_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2014
 * @brief Regression tests for Type.
 */


#undef NDEBUG
#include "RootUtils/Type.h"
#include "TClass.h"
#include "TDataType.h"
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cassert>


template <class T> T fromInt (size_t x);
template <> int fromInt<int> (size_t x) { return x; }
template <> double fromInt<double> (size_t x) { return x; }
template <> std::string fromInt<std::string> (size_t x)
{
  std::ostringstream ss;
  ss << x;
  return ss.str();
}


template <class T>
void checkit (const std::vector<T>& v,
              std::vector<int> v2)
{
  assert (v.size() == v2.size());
  for (size_t i = 0; i < v.size(); i++)
    assert (v[i] == fromInt<T>(v2[i]));
}


template <class T>
void testit (const RootUtils::Type& type)
{
  size_t n = 10;
  std::vector<T> v(n);
  for (size_t i = 0; i < n; i++)
    v[i] = fromInt<T>(i);

  void* p = type.create();
  assert (*reinterpret_cast<T*>(p) == T());
  type.destroy(p);

  type.clear (&v[5]);
  assert (v[5] == T());
  type.clear (v.data(), 7);
  assert (v[7] == T());
  assert (v[4] == fromInt<T>(4));
  assert (v[6] == fromInt<T>(6));
  assert (v[8] == fromInt<T>(8));

  type.assign (&v[5], &v[1]);
  assert (v[5] == fromInt<T>(1));
  type.assign (v.data(), 7, v.data(), 2);
  assert (v[7] == fromInt<T>(2));
  assert (v[4] == fromInt<T>(4));
  assert (v[6] == fromInt<T>(6));
  assert (v[8] == fromInt<T>(8));

  type.swap (&v[5], &v[7]);
  assert (v[5] == fromInt<T>(2));
  assert (v[7] == fromInt<T>(1));
  type.swap (v.data(), 6, v.data(), 8);
  assert (v[6] == fromInt<T>(8));
  assert (v[8] == fromInt<T>(6));

  // 0 1 2 3 4 2 8 1 6 9
  type.copyRange (&v[1], &v[3], 5);
  checkit (v, {0, 3, 4, 2, 8, 1, 8, 1, 6, 9});
  type.copyRange (&v[3], &v[1], 5);
  checkit (v, {0, 3, 4, 3, 4, 2, 8, 1, 6, 9});
}


void test1()
{
  std::cout << "test1\n";

  RootUtils::Type itype (kInt_t);
  assert (itype.getTypeName() == "Int_t");
  assert (itype.getClass() == 0);
  assert (itype.getDataType() == kInt_t);
  assert (itype.getSize() == sizeof(int));
  assert (itype.getTypeInfo() == &typeid(int));
  testit<int> (itype);

  RootUtils::Type itype_x (itype);
  testit<int> (itype_x);

  RootUtils::Type dtype (kDouble_t);
  assert (dtype.getTypeName() == "Double_t");
  assert (dtype.getClass() == 0);
  assert (dtype.getDataType() == kDouble_t);
  assert (dtype.getSize() == sizeof(double));
  assert (dtype.getTypeInfo() == &typeid(double));
  testit<double> (dtype);

  itype_x = dtype;
  testit<double> (itype_x);

  RootUtils::Type stype (TClass::GetClass ("std::string"));
  assert (stype.getTypeName() == "string");
  assert (std::string(stype.getClass()->GetName()) == "string");
  assert (stype.getDataType() == kNoType_t);
  assert (stype.getSize() == sizeof(std::string));
  assert (stype.getTypeInfo() == &typeid(std::string));
  testit<std::string> (stype);

  RootUtils::Type stype_x (stype);
  testit<std::string> (stype_x);
  itype_x = stype;
  testit<std::string> (itype_x);

  RootUtils::Type dtype2 ("double");
  assert (dtype2.getClass() == 0);
  assert (dtype2.getDataType() == kDouble_t);
  assert (dtype2.getSize() == sizeof(double));

  RootUtils::Type stype2 ("std::string");
  assert (std::string(stype2.getClass()->GetName()) == "string");
  assert (stype2.getDataType() == kNoType_t);
  assert (stype2.getSize() == sizeof(std::string));

  RootUtils::Type dtype3;
  dtype3.init (kDouble_t);
  assert (dtype3.getClass() == 0);
  assert (dtype3.getDataType() == kDouble_t);
  assert (dtype3.getSize() == sizeof(double));

  RootUtils::Type stype3;
  stype3.init (TClass::GetClass ("std::string"));
  assert (std::string(stype3.getClass()->GetName()) == "string");
  assert (stype3.getDataType() == kNoType_t);
  assert (stype3.getSize() == sizeof(std::string));

  RootUtils::Type xtype1 ("uint32_t");
  assert (xtype1.getClass() == 0);
  assert (xtype1.getDataType() == kUInt_t);
  assert (xtype1.getSize() == sizeof(unsigned int));
  assert (xtype1.getTypeInfo() == &typeid(unsigned int));
}


// Test fromString
void test2()
{
  std::cout << "test2\n";

  RootUtils::Type itype1 ("int");
  int ix = 0;
  itype1.fromString (&ix, "42");
  assert (ix == 42);

  RootUtils::Type ftype1 ("float");
  float fx = 0;
  ftype1.fromString (&fx, "42.5");
  assert (fx == 42.5);

  RootUtils::Type sctype1 ("signed char");
  signed char scx = 0;
  sctype1.fromString (&scx, "-45");
  assert (scx == -45);

  RootUtils::Type ctype1 ("char");
  char cx = 0;
  ctype1.fromString (&cx, "45");
  assert (cx == 45);

  RootUtils::Type uctype1 ("unsigned char");
  unsigned char ucx = 0;
  uctype1.fromString (&ucx, "45");
  assert (ucx == 45);

  RootUtils::Type stype1 ("std::string");
  std::string sx;
  stype1.fromString (&sx, "lkjlkj");
  assert (sx == "lkjlkj");

  RootUtils::Type xtype1 ("TH1F");
  char xx;
  bool caught = false;
  try {
    xtype1.fromString (&xx, "lkjlkj");
  }
  catch (std::runtime_error&) {
    caught = true;
  }
  assert (caught);
}


// Test unique_ptr + deleter.
void test3()
{
  std::cout << "test3\n";

  RootUtils::Type stype ("std::string");
  RootUtils::Type::unique_ptr up (new std::string ("asd"), stype);
  RootUtils::Type::unique_ptr up2;
}


int main()
{
  test1();
  test2();
  test3();
  return 0;
}

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Array_test.cxx,v 1.2 2009-04-08 21:12:45 ssnyder Exp $
/**
 * @file  Array_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date June, 2004
 * @brief Regression tests for the Array class.
 */


#undef NDEBUG


#include "CxxUtils/Array.h"
#include <cassert>


using namespace CxxUtils;


const int shape[] = {2, 4, 3};
const Arrayelt data[] = { 1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
                          13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24};

#define ARSZ(x) (sizeof(x)/sizeof(x[0]))


void test1()
{
  Array<2> a2x;
  assert (!a2x.valid());
  assert (a2x.size() == 0);
  Array<0> a0x;
  assert (!a0x.valid());

  Arrayrep rep;
  rep.m_shape.assign (std::begin(shape), std::end(shape));
  rep.m_data.assign (data, data + ARSZ(data));
  rep.init_sizes();

  Array<3> a3 (rep);
  assert (a3.valid());
  assert (a3.NDIM == 3);
  std::vector<unsigned int> shape (a3.shape());
  assert (shape.size() == 3);
  assert (shape[0] == 2);
  assert (shape[1] == 4);
  assert (shape[2] == 3);
  assert (a3.size() == 2);
  assert (a3.size(1) == 4);
  assert (*a3.ptr() == 1);

  Array<2> a2 (a3[1]);
  assert (a2.NDIM == 2);
  shape = a2.shape();
  assert (shape.size() == 2);
  assert (shape[0] == 4);
  assert (shape[1] == 3);
  assert (a2.size() == 4);
  assert (a2.size(1) == 3);
  assert (*a2.ptr() == 13);

  Array<1> a1 (a2[1]);
  assert (a1.NDIM == 1);
  shape = a1.shape();
  assert (shape.size() == 1);
  assert (shape[0] == 3);
  assert (a1.size() == 3);
  assert (*a1.ptr() == 16);

  Array<0> a0 (a1[1]);
  assert (a0.valid());
  assert (a0.NDIM == 0);
  shape = a0.shape();
  assert (shape.size() == 0);
  assert (a0.size() == 1);
  assert (a0 == 17);

  assert (a3[1][1][1] == 17);
  assert (*a3[1][1].ptr() == 16);

  Arrayrep rep0;
  rep0.m_data.push_back (101);
  Array<0> a0a (rep0);
  assert (a0a == 101);
}


void test2()
{
  Arrayrep rep;
  rep.m_shape.assign (shape, shape + ARSZ(shape));
  rep.m_data.assign (data, data + ARSZ(data));
  rep.init_sizes();

  WritableArray<3> w3 (rep);
  assert (w3[1][1][1] == 17);
  w3[1][1][1] = 99;
  assert (w3[1][1][1] == 99);

  *w3[1][1].ptr() = 98;
  assert (w3[1][1][0] == 98);

  Array<3> a3 = w3;
  assert (a3[1][1][0] == 98);

  Arrayrep rep0;
  rep0.m_data.push_back (101);
  WritableArray<0> w0 (rep0);
  assert (w0 == 101);
  w0 = 102;
  assert (w0 == 102);
}


void test3()
{
  Arrayrep ar1 (" [[ 1.5, 2.5], [3.5, 4.5]] ");
  Array<2> a1 (ar1);
  assert (a1.size (0) == 2);
  assert (a1.size (1) == 2);
  assert (a1[0][0] == 1.5);
  assert (a1[0][1] == 2.5);
  assert (a1[1][0] == 3.5);
  assert (a1[1][1] == 4.5);
}


void test4()
{
  unsigned int shape1[] = {2, 3};
  WritableArrayData<2> w1 (shape1);
  w1[1][2] = 3;
  w1[0][0] = 2;
  assert (w1[0][0] == 2);
  assert (w1[1][2] == 3);

  std::vector<unsigned int> shape2;
  shape2.push_back (3);
  shape2.push_back (2);
  WritableArrayData<2> w2 (shape2);
  w2[2][1] = 3;
  w2[0][0] = 2;
  assert (w2[0][0] == 2);
  assert (w2[2][1] == 3);
}


// Test iterators.
void test5()
{
  Arrayrep rep;
  rep.m_shape.assign (shape, shape + ARSZ(shape));
  rep.m_data.assign (data, data + ARSZ(data));
  rep.init_sizes();

  Array<3> a3 (rep);

  Array<3>::const_iterator i1 = a3.begin();
  Array<3>::const_iterator i2 = a3.end();
  Array<3>::const_iterator i3;

  assert (i1 == i1);
  assert (i1 != i2);
  assert (i1 < i2);
  assert (i1 <= i2);
  assert (i1 <= i1);
  assert (i2 > i1);
  assert (i2 >= i1);
  assert (i1 >= i1);
  assert (i2 - i1 == 2);

  assert (i3 != i1);
  i3 = i1;
  assert (i3 == i1);

  assert ((*i1)[0][0] == 1);
  assert (i1->size() == 4);

  Array<2>::const_iterator i11 = i1->begin();
  Array<2>::const_iterator i12 = i1->end();
  assert (i11 != i12);
  assert (i12 - i11 == 4);

  assert (i11[0][0] == 1);
  assert (i11[1][0] == 4);

  ++i11;
  assert (i11[0][0] == 4);
  // cppcheck-suppress postfixOperator
  i11++;
  assert (i11[0][0] == 7);

  --i11;
  assert (i11[0][0] == 4);
  // cppcheck-suppress postfixOperator
  i11--;
  assert (i11[0][0] == 1);

  i11 += 3;
  assert (i11[0][0] == 10);
  i11 -= 2;
  assert (i11[0][0] == 4);

  Array<2>::const_iterator i13 = i11 + 2;
  assert (i13[0][0] == 10);
  i13 = i11 - 1;
  assert (i13[0][0] == 1);
  i13 += 4;
  assert (i13 == i12);

  Array<1>::const_iterator j1 = i11->begin();
  Array<1>::const_iterator j2 = i11->end();

  assert (j1 + 3 == j2);
  assert (j1[0] == 4);
  assert (j1[1] == 5);
  assert (j1[2] == 6);
  assert (j2 - j1 == 3);
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

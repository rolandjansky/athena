/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/test/AuxElementComplete_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2013
 * @brief Regression tests for AuxElementComplete
 */


#undef NDEBUG
#include "AthContainers/AuxElementComplete.h"
#include <iostream>
#include <cassert>


class Elt
  : public SG::AuxElement
{
public:
  Elt() : x(0) {}
  Elt(int the_x) : x(the_x) {}
  Elt(const Elt& other)
    : SG::AuxElement(other),
      x(other.x)
  {}
  
  Elt& operator= (const Elt&) = default;

  int& ityp1()
  {
    static const Accessor<int> acc ("anInt");
    return acc (*this);
  }

  int ityp1() const
  {
    static const Accessor<int> acc ("anInt");
    return acc (*this);
  }

  float& ftyp1()
  {
    static const Accessor<float> acc ("aFloat");
    return acc (*this);
  }

  float ftyp1() const
  {
    static const Accessor<float> acc ("aFloat");
    return acc (*this);
  }

  int x;
};


typedef SG::AuxElementComplete<Elt> EltComplete;


void test1()
{
  std::cout << "test1\n";
  EltComplete elt;
  assert (elt.x == 0);

  EltComplete elt2 (10);
  assert (elt2.x == 10);

  Elt::Accessor<int> ityp2 ("anotherInt");

  elt.ityp1() = 1;
  ityp2(elt) = 2;
  elt.ftyp1() = 1.5;

  assert (elt.ityp1() == 1);
  assert (ityp2(elt) == 2);
  assert (elt.ftyp1() == 1.5);

  elt.x = 20;
  EltComplete elt3 (elt);
  assert (elt3.x == 20);
  assert (elt3.ityp1() == 1);
  assert (ityp2(elt3) == 2);
  assert (elt3.ftyp1() == 1.5);
  assert (elt3.container() != elt.container());

  elt.ityp1() = 31;
  assert (elt3.ityp1() == 1);
  elt3 = elt;
  assert (elt3.ityp1() == 31);
  assert (elt3.container() != elt.container());

  elt.ityp1() = 41;
  assert (elt3.ityp1() == 31);
  elt3 = std::move(elt);
  assert (elt3.ityp1() == 41);
  assert (elt3.container() != elt.container());

  EltComplete elt4 (std::move(elt));
  assert (elt4.x == 20);
  assert (elt4.ityp1() == 41);
  assert (ityp2(elt4) == 2);
  assert (elt4.ftyp1() == 1.5);
  assert (elt4.container() != elt.container());

  elt3.ityp1() = 41;
  elt.releasePrivateStore();
  assert (elt.container() == 0);
  assert (elt3.x == 20);
  assert (elt3.ityp1() == 41);
  assert (ityp2(elt3) == 2);
  assert (elt3.ftyp1() == 1.5);
}


int main()
{
  test1();
  return 0;
}

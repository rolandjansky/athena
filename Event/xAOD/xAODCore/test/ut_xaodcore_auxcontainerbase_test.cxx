/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file xAODCore/test/AuxContainerBase_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Unit tests for AuxContainerBase.  (sadly incomplete)
 */


#undef NDEBUG
#include "xAODCore/AuxContainerBase.h"
#include <iostream>
#include <sstream>
#include <cassert>


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


bool wasMoved (const MoveTest& x) { return x.m_v.empty(); }


class AuxContainerTest
  : public xAOD::AuxContainerBase
{
public:
  AuxContainerTest();

  std::vector<int> i1;
  std::vector<MoveTest> m1;
};


AuxContainerTest::AuxContainerTest()
{
  AUX_VARIABLE(i1);
  AUX_VARIABLE(m1);
}


bool operator== (const SG::auxid_set_t& s1,
                 const SG::auxid_set_t& s2)
{
  std::set<SG::auxid_t> x1 (s1.begin(), s1.end());
  std::set<SG::auxid_t> x2 (s2.begin(), s2.end());
  return x1 == x2;
}


// Test insertMove.
void test1()
{
  std::cout << "test1\n";

  SG::auxid_t ityp1 = SG::AuxTypeRegistry::instance().getAuxID<int> ("i1");
  SG::auxid_t ityp2 = SG::AuxTypeRegistry::instance().getAuxID<int> ("i2");
  SG::auxid_t ityp3 = SG::AuxTypeRegistry::instance().getAuxID<int> ("i3");
  SG::auxid_t ityp4 = SG::AuxTypeRegistry::instance().getAuxID<int> ("i4");
  SG::auxid_t mtyp1 = SG::AuxTypeRegistry::instance().getAuxID<MoveTest> ("m1");
  AuxContainerTest s1;
  s1.reserve(20);
  s1.resize(5);

  int* i1 = reinterpret_cast<int*> (s1.getData(ityp1, 5, 20));
  int* i2 = reinterpret_cast<int*> (s1.getData(ityp2, 5, 20));
  MoveTest* m1 = reinterpret_cast<MoveTest*> (s1.getData(mtyp1, 5, 20));

  for (int i=0; i<5; i++) {
    i1[i] = i;
    i2[i] = i+100;
    m1[i] = MoveTest(i);
  }

  AuxContainerTest s2;
  s2.resize(5);

  int* i1_2 = reinterpret_cast<int*> (s2.getData(ityp1, 5, 5));
  int* i3_2 = reinterpret_cast<int*> (s2.getData(ityp3, 5, 5));
  int* i4_2 = reinterpret_cast<int*> (s2.getData(ityp4, 5, 5));
  MoveTest* m1_2 = reinterpret_cast<MoveTest*> (s2.getData(mtyp1, 5, 5));
  for (int i=0; i<5; i++) {
    i1_2[i] = i+10;
    i3_2[i] = i+110;
    i4_2[i] = i+210;
    m1_2[i] = MoveTest(i+10);
  }

  SG::auxid_set_t exp1;
  exp1.insert (ityp1);
  exp1.insert (ityp2);
  exp1.insert (mtyp1);
  assert (s1.getAuxIDs() == exp1);

  SG::auxid_set_t exp2 = exp1;
  exp2.insert (ityp3);

  SG::auxid_set_t ignore;
  ignore.insert (ityp4);
  assert (! s1.insertMove (3, s2, ignore)); // false due to added vbl
  assert (s1.getAuxIDs() == exp2);
  assert (s1.size() == 10);
  s1.reserve(20);
  assert (s1.getData(ityp4) == nullptr);
  const int* i3 = reinterpret_cast<const int*> (s1.getData(ityp3));
  assert (i3 != 0);
  for (int i=0; i<3; i++) {
    assert (i1[i] == i);
    assert (i2[i] == i+100);
    assert (i3[i] == 0);
    assert (m1[i] == MoveTest(i));
  }
  for (int i=0; i<5; i++) {
    assert (i1[3+i] == i+10);
    assert (i2[3+i] == 0);
    assert (i3[3+i] == i+110);
    assert (m1[3+i] == MoveTest(i+10));
  }
  for (int i=3; i<5; i++) {
    assert (i1[5+i] == i);
    assert (i2[5+i] == i+100);
    assert (i3[5+i] == 0);
    assert (m1[5+i] == MoveTest(i));
  }
  for (int i=0; i<5; i++) {
    assert (wasMoved (m1_2[i]));
  }

  for (int i=0; i<5; i++) {
    i1_2[i] = i+20;
    i3_2[i] = i+120;
    m1_2[i] = MoveTest(i+20);
  }
  assert (s1.insertMove (10, s2, ignore));
  assert (s1.size() == 15);
  for (int i=0; i<3; i++) {
    assert (i1[i] == i);
    assert (i2[i] == i+100);
    assert (i3[i] == 0);
    assert (m1[i] == MoveTest(i));
  }
  for (int i=0; i<5; i++) {
    assert (i1[3+i] == i+10);
    assert (i2[3+i] == 0);
    assert (i3[3+i] == i+110);
    assert (m1[3+i] == MoveTest(i+10));
  }
  for (int i=3; i<5; i++) {
    assert (i1[5+i] == i);
    assert (i2[5+i] == i+100);
    assert (i3[5+i] == 0);
    assert (m1[5+i] == MoveTest(i));
  }
  for (int i=0; i<5; i++) {
    assert (i1[10+i] == i+20);
    assert (i2[10+i] == 0);
    assert (i3[10+i] == i+120);
    assert (m1[10+i] == MoveTest(i+20));
  }
  for (int i=0; i<5; i++) {
    assert (wasMoved (m1_2[i]));
  }

  SG::auxid_set_t set2;
  set2.insert (ityp2);
  set2.insert (ityp3);
  assert (s1.getStore()->getAuxIDs() == set2);
}


int main()
{
  test1();
  return 0;
}

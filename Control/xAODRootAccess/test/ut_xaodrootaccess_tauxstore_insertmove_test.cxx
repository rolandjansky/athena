/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file xAODRootAccess/test/ut_xaodrootaccess_tauxstore_insertmove_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2017
 * @brief Unit tests TAuxStore::insertMove.
 */


#undef NDEBUG
#include "xAODRootAccess/TAuxStore.h"
#include "xAODRootAccess/tools/ReturnCheck.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/AuxStoreInternal.h"
#include "TTree.h"
#include <iostream>
#include <sstream>
#include <cassert>


const char* APP_NAME = "ut_xaodrootaccess_tauxstore_insertmove_test";


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


int test1()
{
  std::cout << "test1\n";

  SG::auxid_t ityp1 = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt");
  SG::auxid_t ityp2 = SG::AuxTypeRegistry::instance().getAuxID<int> ("anotherInt");
  SG::auxid_t ityp3 = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt3");
  SG::auxid_t ityp4 = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt4");
  SG::auxid_t mtyp1 = SG::AuxTypeRegistry::instance().getAuxID<MoveTest> ("moveTest");

  TTree tree ("t", "t");
  xAOD::TAuxStore s1( "fooAux." );
  RETURN_CHECK( APP_NAME, s1.readFrom (&tree) );
  s1.resize(5);

  int* i1 = reinterpret_cast<int*> (s1.getData(ityp1, 5, 20));
  int* i2 = reinterpret_cast<int*> (s1.getData(ityp2, 5, 20));
  MoveTest* m1 = reinterpret_cast<MoveTest*> (s1.getData(mtyp1, 5, 20));

  for (int i=0; i<5; i++) {
    i1[i] = i;
    i2[i] = i+100;
    m1[i] = MoveTest(i);
  }

  SG::AuxStoreInternal s2;
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

  SG::auxid_set_t ignore;
  ignore.insert (ityp4);

  s1.insertMove (3, s2, ignore);
  assert (s1.size() == 10);
  s1.reserve(20);
  assert (s1.getData(ityp4) == nullptr);
  const int* i3 = reinterpret_cast<const int*> (s1.getData(ityp3));
  assert (i3 != 0);
  i1 = reinterpret_cast<int*> (s1.getData(ityp1, 5, 20));
  i2 = reinterpret_cast<int*> (s1.getData(ityp2, 5, 20));
  m1 = reinterpret_cast<MoveTest*> (s1.getData(mtyp1, 5, 20));
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
  s1.insertMove (10, s2, ignore);
  assert (s1.size() == 15);
  i1 = reinterpret_cast<int*> (s1.getData(ityp1, 5, 20));
  i2 = reinterpret_cast<int*> (s1.getData(ityp2, 5, 20));
  i3 = reinterpret_cast<int*> (s1.getData(ityp3, 5, 20));
  m1 = reinterpret_cast<MoveTest*> (s1.getData(mtyp1, 5, 20));
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

  return 0;
}


int main()
{
  test1();
  return 0;
}

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file xAODRootAccess/test/ut_xaodtrigger_bytestreamauxcontainer_v1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2017
 * @brief Unit tests for ByteStreamAuxContainer_v1 (sadly incomplete).
 */


#undef NDEBUG
#include "xAODTrigger/versions/ByteStreamAuxContainer_v1.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/AuxStoreInternal.h"
#include <iostream>
#include <sstream>
#include <cassert>


class AuxContainerTest
  : public xAOD::ByteStreamAuxContainer_v1
{
public:
  AuxContainerTest();

  std::vector<int> anInt;
};


AuxContainerTest::AuxContainerTest()
{
  AUX_VARIABLE(anInt);
}


void test1()
{
  std::cout << "test1\n";

  SG::auxid_t ityp1 = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt");
  SG::auxid_t ityp2 = SG::AuxTypeRegistry::instance().getAuxID<int> ("anotherInt");
  SG::auxid_t ityp3 = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt3");
  SG::auxid_t ityp4 = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt4");

  AuxContainerTest s1;
  s1.resize(5);
  s1.reserve(20);

  int* i1 = reinterpret_cast<int*> (s1.getData(ityp1, 5, 20));
  int* i2 = reinterpret_cast<int*> (s1.getData(ityp2, 5, 20));

  for (int i=0; i<5; i++) {
    i1[i] = i;
    i2[i] = i+100;
  }

  SG::AuxStoreInternal s2;
  s2.resize(5);

  int* i1_2 = reinterpret_cast<int*> (s2.getData(ityp1, 5, 5));
  int* i3_2 = reinterpret_cast<int*> (s2.getData(ityp3, 5, 5));
  int* i4_2 = reinterpret_cast<int*> (s2.getData(ityp4, 5, 5));
  for (int i=0; i<5; i++) {
    i1_2[i] = i+10;
    i3_2[i] = i+110;
    i4_2[i] = i+210;
  }

  SG::auxid_set_t ignore { ityp4 };

  s1.insertMove (3, s2, ignore);
  assert (s1.size() == 10);
  s1.reserve(20);
  assert (s1.getData(ityp4) == nullptr);
  const int* i3 = reinterpret_cast<const int*> (s1.getData(ityp3));
  assert (i3 != 0);
  i1 = reinterpret_cast<int*> (s1.getData(ityp1, 10, 20));
  i2 = reinterpret_cast<int*> (s1.getData(ityp2, 10, 20));
  for (int i=0; i<3; i++) {
    assert (i1[i] == i);
    assert (i2[i] == i+100);
    assert (i3[i] == 0);
  }
  for (int i=0; i<5; i++) {
    assert (i1[3+i] == i+10);
    assert (i2[3+i] == 0);
    assert (i3[3+i] == i+110);
  }
  for (int i=3; i<5; i++) {
    assert (i1[5+i] == i);
    assert (i2[5+i] == i+100);
    assert (i3[5+i] == 0);
  }

  for (int i=0; i<5; i++) {
    i1_2[i] = i+20;
    i3_2[i] = i+120;
  }
  s1.insertMove (10, s2, ignore);
  assert (s1.size() == 15);
  i1 = reinterpret_cast<int*> (s1.getData(ityp1, 15, 20));
  i2 = reinterpret_cast<int*> (s1.getData(ityp2, 15, 20));
  i3 = reinterpret_cast<int*> (s1.getData(ityp3, 15, 20));
  for (int i=0; i<3; i++) {
    assert (i1[i] == i);
    assert (i2[i] == i+100);
    assert (i3[i] == 0);
  }
  for (int i=0; i<5; i++) {
    assert (i1[3+i] == i+10);
    assert (i2[3+i] == 0);
    assert (i3[3+i] == i+110);
  }
  for (int i=3; i<5; i++) {
    assert (i1[5+i] == i);
    assert (i2[5+i] == i+100);
    assert (i3[5+i] == 0);
  }
  for (int i=0; i<5; i++) {
    assert (i1[10+i] == i+20);
    assert (i2[10+i] == 0);
    assert (i3[10+i] == i+120);
  }

  s1.lock();
  
  SG::auxid_t ityp5 = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt5");
  int* i5 = reinterpret_cast<int*> (s1.getDecoration(ityp5, 15, 20));
  assert (i5 != nullptr);
  assert (!s1.isDecoration (ityp1));
  assert ( s1.isDecoration (ityp5));

  assert (s1.clearDecorations());
  assert (!s1.isDecoration (ityp1));
  assert (!s1.isDecoration (ityp5));
}


int main()
{
  test1();
  return 0;
}

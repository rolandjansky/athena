/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file LArRecConditions/test/LArRoIMap_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2021
 * @brief Unit test for LArRoIMap.
 */


#undef NDEBUG
#include "LArRecConditions/LArRoIMap.h"
#include <cassert>
#include <iostream>


void test1()
{
  std::cout << "test1\n";

  LArRoIMap m (100);
  assert (m.trigTowerID (Identifier (123)) == 101);
  assert (m.trigTowerID (HWIdentifier (123)) == 101);
  assert (m.collectionID (123).empty());

  m.addEntry (Identifier(123),
              HWIdentifier(456),
              HWIdentifier(991),
              1);
  m.addEntry (Identifier(124),
              HWIdentifier(457),
              HWIdentifier(992),
              1);
  m.addEntry (Identifier(125),
              HWIdentifier(458),
              HWIdentifier(991),
              1);
  m.addEntry (Identifier(126),
              HWIdentifier(459),
              HWIdentifier(993),
              2);

  assert (m.trigTowerID (Identifier (123)) == 1);
  assert (m.trigTowerID (Identifier (124)) == 1);
  assert (m.trigTowerID (Identifier (125)) == 1);
  assert (m.trigTowerID (Identifier (126)) == 2);
  assert (m.trigTowerID (Identifier (122)) == 101);
  assert (m.trigTowerID (Identifier (127)) == 101);

  assert (m.trigTowerID (HWIdentifier (456)) == 1);
  assert (m.trigTowerID (HWIdentifier (457)) == 1);
  assert (m.trigTowerID (HWIdentifier (458)) == 1);
  assert (m.trigTowerID (HWIdentifier (459)) == 2);
  assert (m.trigTowerID (HWIdentifier (455)) == 101);
  assert (m.trigTowerID (HWIdentifier (460)) == 101);

  assert (m.collectionID (1) == (std::vector<LArRoIMap::COLL_ID> {HWIdentifier(991), HWIdentifier(992)}));
  assert (m.collectionID (2) == (std::vector<LArRoIMap::COLL_ID> {HWIdentifier(993)}));
  assert (m.collectionID (0).empty());
  assert (m.collectionID (3).empty());

  assert (m.validID (1));
  assert (m.validID (2));
  assert (!m.validID (0));
  assert (!m.validID (3));
}


int main()
{
  std::cout << "LArRecConditions/LArRoIMap_test\n";
  test1();
  return 0;
}


/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file LArRawConditions/test/LArConditionsContainerDB_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2017
 * @brief Unit test for LArConditionsContainerDB.
 */

#undef NDEBUG
#include "LArRawConditions/LArConditionsContainerDB.h"
#include <vector>
#include <iostream>
#include <cassert>


void test1()
{
  std::cout << "test1\n";

  LArConditionsContainerDB<int> db1 (1);
  assert (db1.size() == 0);
  assert (db1.getGain() == 1);
  db1.setGain(2);
  assert (db1.getGain() == 2);

  std::vector<int> feb1 { 1, 4, 7, 10 };
  std::vector<int> feb2 { 100, 105, 107, 176, 189, 199};
  std::vector<int> feb3 { 200, 214, 219, 237, 256 };

  db1.add (1, &feb1);
  db1.add (2, &feb2);
  db1.add (3, &feb3);
  assert (db1.size() == 3);

  assert (db1.exist (1));
  assert (db1.exist (3));
  assert (!db1.exist (4));

  assert (db1.get (2, 1) == 105);
  db1.set (2, 1, 300);
  assert (db1.get (2, 1) == 300);
  assert (db1.getNonConst (3, 2) == 219);
  db1.getNonConst (3, 2) = 555;
  assert (db1.getNonConst (3, 2) == 555);

  std::vector<int> feb4 { 313, 356 };
  std::vector<int> feb1a { 412, 456, 467 };
  LArConditionsContainerDB<int> db2;
  db2.add (1, &feb1a);
  db2.add (4, &feb4);
  db1.add (&db2);
  assert (db1.size() == 4);
  assert (db1.exist (4));
  assert (!db1.exist (5));
  assert (db1.get (1, 1) == 456);
  assert (db1.get (4, 1) == 356);

  db1.erase (2);
  assert (db1.size() == 3);
  assert (!db1.exist(2));
  assert (db1.get (2, 0) == 0);

  const LArConditionsContainerDB<int>& cdb1 = db1;

  {
    std::cout << "it1:\n";
    LArConditionsContainerDB<int>::const_iterator it = cdb1.begin (nullptr);
    LArConditionsContainerDB<int>::const_iterator it_end = cdb1.end (nullptr);
    for (; it != it_end; ++it) {
      std::cout << "  " << it.getFebId() << " (" << it.febId().get_compact() << ") "
                << it.getChannel() << ": " << *it << "\n";
    }
  }

  {
    std::cout << "it2:\n";
    LArConditionsContainerDB<int>::FebIdVec febs { 1, 4 };
    LArConditionsContainerDB<int>::const_iterator it = cdb1.begin (nullptr, febs);
    LArConditionsContainerDB<int>::const_iterator it_end = cdb1.end (nullptr);
    for (; it != it_end; it++) {
      std::cout << "  " << it.getFebId() << " (" << it.febId().get_compact() << ") "
                << it.getChannel() << ": " << *it << "\n";
    }
  }

  {
    std::cout << "it3:\n";
    LArConditionsContainerDB<int>::FebIdVec febs { 3, 4 };
    LArConditionsContainerDB<int>::const_iterator it = cdb1.begin (nullptr, febs);
    LArConditionsContainerDB<int>::const_iterator it_end = cdb1.end (nullptr);
    for (; it != it_end; ++it) {
      std::cout << "  " << it.getFebId() << " (" << it.febId().get_compact() << ") "
                << it.getChannel() << ": " << *it << "\n";
    }
  }

  LArConditionsContainerDB<int> db3;
  const LArConditionsContainerDB<int>& cdb3 = db3;
  assert (db3.begin(nullptr) == db3.end(nullptr));
  assert (cdb3.begin(nullptr) == cdb3.end(nullptr));

  {
    LArConditionsContainerDB<int>::const_iterator it1 = cdb1.begin(nullptr);
    assert (it1 == cdb1.begin(nullptr));
    assert (it1 != cdb1.end(nullptr));
    assert (*it1 == 412);
    LArConditionsContainerDB<int>::const_iterator it2 = ++it1;
    assert (it1 == it2);
    assert (*it1 == 456);
    assert (*it2 == 456);
    LArConditionsContainerDB<int>::const_iterator it3 = it1++;
    assert (it1 != it3);
    assert (*it1 == 467);
    assert (*it3 == 456);
  }

  {
    std::cout << "it4:\n";
    LArConditionsContainerDB<int>::iterator it = db1.begin (nullptr);
    LArConditionsContainerDB<int>::iterator it_end = db1.end (nullptr);
    for (; it != it_end; ++it) {
      std::cout << "  " << it.getFebId() << " (" << it.febId().get_compact() << ") "
                << it.getChannel() << ": " << *it << "\n";
    }
  }

  {
    std::cout << "it5:\n";
    LArConditionsContainerDB<int>::FebIdVec febs { 1, 4 };
    LArConditionsContainerDB<int>::iterator it = db1.begin (nullptr, febs);
    LArConditionsContainerDB<int>::iterator it_end = db1.end (nullptr);
    for (; it != it_end; it++) {
      std::cout << "  " << it.getFebId() << " (" << it.febId().get_compact() << ") "
                << it.getChannel() << ": " << *it << "\n";
    }
  }

  {
    LArConditionsContainerDB<int>::iterator it1 = db1.begin(nullptr);
    assert (it1 == db1.begin(nullptr));
    assert (it1 != db1.end(nullptr));
    assert (*it1 == 412);
    LArConditionsContainerDB<int>::iterator it2 = ++it1;
    assert (it1 == it2);
    assert (*it1 == 456);
    assert (*it2 == 456);
    *it1 = 999;
    assert (*it2 == 999);
    LArConditionsContainerDB<int>::iterator it3 = it1++;
    assert (it1 != it3);
    assert (*it1 == 467);
    assert (*it3 == 999);
  }
}


int main()
{
  test1();
  return 0;
}

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file  SGTools/StringPool_test.cxx
 * @author scott snyder
 * @date Mar 2007
 * @brief Regression test for StringPool.
 */


#undef NDEBUG

#include "SGTools/StringPool.h"
#include "SGTools/exceptions.h"
#include "CxxUtils/crc64.h"
#include "TestTools/expect_exception.h"
#include <vector>
#include <string>
#include <cstdlib>
#include <cassert>
#include <iostream>


const char* const teststrings[] = {
  "",
  "MNIIQGNLVGTGLKIGIVVGRFNDFITSKLLSGAEDALLRHGVDTNDIDVAWVPGAFEIPFAAKKMAETKKYDAIITLGTVIRGATTSYDYVCNEAAKGIAQAANTTGVPVIFGIVTTENIEQAIERAGTKAGNKGVDCAVSAIEMANLNRSFE",
  "MNIIQGNLVGTGLKIGIVVGRFNDFITSKLLSGAEDALLRHGVDTNDIDVAWVPGAFEIPFAAKKMAETKKYDAIITLGDVIRGATTHYDYVCNEAAKGIAQAANTTGVPVIFGIVTTENIEQAIERAGTKAGNKGVDCAVSAIEMANLNRSFE",
  "ajsdla lkjasd lkjasd ",
  "paks  aspdk pok  asd",
  "asd  laskjkd lkajsd",
};


std::string randstring()
{
  char buf[256];
  int len = std::rand() % 256;
  for (int i = 0; i < len; i++)
    buf[i] = (char)(std::rand() % 256);
  return std::string (buf, len);
}


void checkPool (const SG::StringPool& sp,
                const std::vector<SG::StringPool::sgkey_t>& keys,
                const std::vector<SG::StringPool::sgaux_t>& aux,
                const std::vector<std::string>& strings,
                bool test2)
                
{
  for (size_t i = 0; i < keys.size(); i++) {
    SG::StringPool::sgaux_t auxout = 0;
    assert (*sp.keyToString (keys[i]) == strings[i]);
    assert (*sp.keyToString (keys[i], auxout) == strings[i]);
    assert (auxout == aux[i]);
    auxout = 0;
    if (test2) {
      assert (*sp.keyToString (keys[i]+1, auxout) == strings[i]);
      assert (auxout == aux[i]);
    }
    else
      assert (!sp.keyToString (keys[i]+1));
  }
}


void test1()
{
  std::cout << "test1\n";
  
  SG::StringPool sp;
  std::vector<std::string> strings;
  std::vector<SG::StringPool::sgkey_t> keys;
  std::vector<SG::StringPool::sgaux_t> aux;
  size_t ntest = sizeof (teststrings) / sizeof (teststrings[0]);
  for (size_t i = 0; i < ntest; i++) {
    SG::StringPool::sgkey_t key = sp.stringToKey (teststrings[i], i);
    assert (key <= SG::StringPool::sgkey_t_max);
    strings.push_back (teststrings[i]);
    keys.push_back (key);
    aux.push_back (i);
    std::cout << CxxUtils::crc64format (key) << " " << teststrings[i] << "\n";
  }
  size_t nstore = ntest;
  assert (sp.size() == nstore);

  std::cout << "pool dump\n";
  sp.dump();

  for (int i=0; i < 1000; i++) {
    SG::StringPool::sgaux_t auxkey = std::rand();
    std::string str = randstring();
    SG::StringPool::sgkey_t key = sp.stringToKey (str, auxkey);
    assert (key <= SG::StringPool::sgkey_t_max);
    strings.push_back (str);
    keys.push_back (key);
    aux.push_back (auxkey);
  }

  nstore += 1000;
  assert (sp.size() == nstore);
  checkPool (sp, keys, aux, strings, false);

  for (size_t i = 0; i < keys.size(); i++) {
    assert (sp.registerKey (keys[i]+1, strings[i], aux[i]));
  }

  nstore *= 2;
  assert (sp.size() == nstore);
  checkPool (sp, keys, aux, strings, true);

  assert (sp.registerKey (keys[10], strings[10], aux[10]));
  assert (!sp.registerKey (keys[10], strings[11], aux[10]));
  assert (!sp.registerKey (keys[10], strings[10], aux[11]));
  nstore += 2;

  SG::StringPool sp2 = sp;

  sp.clear();
  assert (sp.size() == 0);
  std::cout << "pool dump2\n";
  sp.dump();

  assert (sp2.size() == nstore);
  checkPool (sp2, keys, aux, strings, true);

  SG::StringPool sp3 (std::move (sp2));
  assert (sp2.size() == 0);
  assert (sp3.size() == nstore);
  checkPool (sp3, keys, aux, strings, true);

  sp2 = sp3;
  assert (sp2.size() == nstore);
  assert (sp3.size() == nstore);
  checkPool (sp2, keys, aux, strings, true);
  checkPool (sp3, keys, aux, strings, true);

  sp = std::move (sp3);
  assert (sp.size() == nstore);
  assert (sp3.size() == 0);
  checkPool (sp, keys, aux, strings, true);
}


// Test collisions.
void test2()
{
  std::cout << "test2\n";

  SG::StringPool sp;
  SG::StringPool::sgkey_t key = sp.stringToKey ("HLTNav_ComboHypo_merged_Step3_Step3_1muEFSA_Step3_1muEFSA_MuonEFSAHypoAux.", 187169987);
  assert (key == 522984112);
  EXPECT_EXCEPTION (SG::ExcSgkeyCollision,
                    sp.stringToKey ("IMfastCalo_view_159_EMCaloRoIs", 33347479););
}


int main()
{
  test1();
  test2();
  return 0;
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: StringPool_test.cxx,v 1.2 2007-12-10 22:20:04 ssnyder Exp $
/**
 * @file  SGTools/StringPool_test.cxx
 * @author scott snyder
 * @date Mar 2007
 * @brief Regression test for StringPool.
 */


#undef NDEBUG

#include "SGTools/StringPool.h"
#include "SGTools/crc64.h"
#include <vector>
#include <string>
#include <cstdlib>
#include <cassert>
#include <iostream>


const char* teststrings[] = {
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


void test1()
{
  SG::StringPool sp;
  std::vector<std::string> strings;
  std::vector<SG::StringPool::sgkey_t> keys;
  std::vector<SG::StringPool::sgaux_t> aux;
  int ntest = sizeof (teststrings) / sizeof (teststrings[0]);
  for (int i = 0; i < ntest; i++) {
    SG::StringPool::sgkey_t key = sp.stringToKey (teststrings[i], i);
    assert (key <= SG::StringPool::sgkey_t_max);
    strings.push_back (teststrings[i]);
    keys.push_back (key);
    aux.push_back (i);
    std::cout << SG::crc64format (key) << " " << teststrings[i] << "\n";
  }

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

  for (size_t i = 0; i < keys.size(); i++) {
    SG::StringPool::sgaux_t auxout;
    assert (*sp.keyToString (keys[i]) == strings[i]);
    assert (*sp.keyToString (keys[i], auxout) == strings[i]);
    assert (auxout == aux[i]);
    assert (!sp.keyToString (keys[i]+1));
  }

  for (size_t i = 0; i < keys.size(); i++) {
    assert (sp.registerKey (keys[i]+1, strings[i], aux[i]));
  }

  for (size_t i = 0; i < keys.size(); i++) {
    SG::StringPool::sgaux_t auxout = 0;
    assert (*sp.keyToString (keys[i], auxout) == strings[i]);
    assert (auxout == aux[i]);
    auxout = 0;
    assert (*sp.keyToString (keys[i]+1, auxout) == strings[i]);
    assert (auxout == aux[i]);
  }

  assert (sp.registerKey (keys[10], strings[10], aux[10]));
  assert (!sp.registerKey (keys[10], strings[11], aux[10]));
  assert (!sp.registerKey (keys[10], strings[10], aux[11]));
}


int main()
{
  test1();
  return 0;
}

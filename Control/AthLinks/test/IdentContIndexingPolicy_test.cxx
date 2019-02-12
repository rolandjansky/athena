/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthLinks/test/IdentContIndexingPolicy_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2014
 * @brief Regression tests for IdentContIndexingPolicy.
 */


#undef NDEBUG
#include "AthLinks/tools/IdentContIndexingPolicy.h"
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <cassert>


#include "TestTools/expect_exception.h"


class Cont
{
public:
  struct IDENTIFIABLE : public std::vector<std::string>
  {
    unsigned identifyHash() const { return m_hash; }
    unsigned m_hash;
  };

  typedef std::map<int, IDENTIFIABLE> map_t;

  class const_iterator
  {
  public:
    typedef map_t::const_iterator base_t;

    const_iterator (base_t it) : m_it (it) {}
    const IDENTIFIABLE* operator*() { return &m_it->second; }
    bool operator== (const_iterator other) const
    { return m_it == other.m_it; }
    bool operator!= (const_iterator other) const
    { return m_it != other.m_it; }

    void operator++() { ++m_it; }

  private:
    base_t m_it;
  };

  const_iterator begin() const { return const_iterator (m_map.begin()); }
  const_iterator end() const { return const_iterator (m_map.end()); }
  const_iterator indexFind(int hash) const
  { return const_iterator (m_map.find (hash)); }

  void set (unsigned hash, unsigned idx, const std::string& val)
  {
    IDENTIFIABLE& i = m_map[hash];
    i.m_hash = hash;
    if (i.size() <= idx)
      i.resize (idx+1);
    i[idx] = val;
  }

private:
  map_t m_map;
};


void test1()
{
  std::cout << "test1\n";

  typedef SG::IdentContIndexingPolicy<Cont> Pol;

  Pol::stored_index_type idx;
  Pol::reset (idx);
  assert (!Pol::isValid(idx));
  idx = Pol::stored_index_type(123*0x10000 + 456);
  assert (Pol::isValid(idx));
  Pol::index_type idx2 = idx;
  assert (idx2 == 123*0x10000 + 456);
  Pol::reset (idx);
  assert (!Pol::isValid(idx));

  Cont c;
  c.set (0, 1, "one");
  c.set (2, 3, "two");
  c.set (2, 4, "three");

  Pol::ElementType elt = (**c.indexFind(0))[1];
  assert (elt == "one");
  Pol::ElementConstReference r_elt = elt;
  assert (r_elt == "one");
  Pol::ElementConstPointer p_elt = &elt;
  assert (*p_elt == "one");

  Pol::reset (idx);
  assert (Pol::lookup (2*0x10000 + 3, c) == "two");
  EXPECT_EXCEPTION (SG::ExcInvalidIndex, Pol::lookup (idx, c));
  EXPECT_EXCEPTION (SG::ExcIndexNotFound, Pol::lookup (2*0x10000 + 10, c));
  EXPECT_EXCEPTION (SG::ExcIndexNotFound, Pol::lookup (5*0x10000 + 1, c));

  Pol::reset (idx2);
  Pol::reverseLookup (c, "three", idx2);
  assert (idx2 == 2*0x10000 + 4);
  Pol::reverseLookup (c, "three", idx2);
  assert (idx2 == 2*0x10000 + 4);
  idx2 = 2*0x10000 + 5;
  Pol::reverseLookup (c, "three", idx2);
  assert (idx2 == 2*0x10000 + 4);
  idx2 = 1*0x10000 + 4;
  Pol::reverseLookup (c, "three", idx2);
  assert (idx2 == 2*0x10000 + 4);
  EXPECT_EXCEPTION (SG::ExcElementNotFound, Pol::reverseLookup (c, "ten", idx2));

  assert (Pol::storedToExternal (Pol::stored_index_type(123)) == 123);
}


int main()
{
  test1();
  return 0;
}

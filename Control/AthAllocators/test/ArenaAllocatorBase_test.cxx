/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaAllocatorBase_test.cxx 470529 2011-11-24 23:54:22Z ssnyder $
/**
 * @file AthAllocators/test/ArenaAllocatorBase_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2007
 * @brief Regression tests for ArenaAllocatorBase.
 */

#undef NDEBUG
#include "AthAllocators/ArenaAllocatorBase.h"
#include <cassert>
#include <sstream>


int testcount = 0;

class Test
{
public:
  Test() { ++testcount; }
  ~Test() { --testcount; }

  int x = 0;
};

class Test2
  : public Test
{
public:
  void clear() { testcount += 10; }
};


class ArenaTestAllocator
  : public SG::ArenaAllocatorBase
{
public:
  virtual void reset() {}
  virtual void erase() {}
  virtual void reserve (size_t /*size*/) { }
  virtual const ArenaAllocatorBase::Stats& stats() const
  { return m_stats; }
  virtual const std::string& name() const { return m_name; }
  ArenaAllocatorBase::Stats m_stats;
  std::string m_name;
};


void set_stat (SG::ArenaAllocatorBase::Stats::Stat& stat, int x)
{
  stat.inuse = x;
  stat.free = x+1;
  stat.total = x+2;
}

void set_stats (SG::ArenaAllocatorBase::Stats& stats, int x)
{
  set_stat (stats.elts, x);
  set_stat (stats.bytes, x + 1000);
  set_stat (stats.blocks, x + 2000);
}


void check_stat_sum (const SG::ArenaAllocatorBase::Stats::Stat& stat1,
                     const SG::ArenaAllocatorBase::Stats::Stat& stat2,
                     const SG::ArenaAllocatorBase::Stats::Stat& stat3)
{
  assert (stat1.inuse + stat2.inuse == stat3.inuse);
  assert (stat1.free +  stat2.free  == stat3.free);
  assert (stat1.total + stat2.total == stat3.total);
}
void check_stats_sum (const SG::ArenaAllocatorBase::Stats& stats1,
                      const SG::ArenaAllocatorBase::Stats& stats2,
                      const SG::ArenaAllocatorBase::Stats& stats3)
{
  check_stat_sum (stats1.elts, stats2.elts, stats3.elts);
  check_stat_sum (stats1.bytes, stats2.bytes, stats3.bytes);
  check_stat_sum (stats1.blocks, stats2.blocks, stats3.blocks);
}

void check_stat_zero (const SG::ArenaAllocatorBase::Stats::Stat& stat)
{
  assert (stat.inuse == 0);
  assert (stat.free  == 0);
  assert (stat.total == 0);
}
void check_stats_zero (const SG::ArenaAllocatorBase::Stats& stats)
{
  check_stat_zero (stats.elts);
  check_stat_zero (stats.bytes);
  check_stat_zero (stats.blocks);
}


int main()
{
  ArenaTestAllocator ata;
  ata.reset();
  ata.erase();
  ata.reserve(0);
  assert (ata.stats().elts.inuse == 0);
  assert (ata.name() == "");

  SG::ArenaAllocatorBase::Params params =
    SG::ArenaAllocatorBase::initParams<Test>(500, "foo");
  assert (params.name == "foo");
  assert (params.nblock == 500);
  assert (params.eltSize == sizeof (Test));
  char* p = new char[sizeof(Test)];
  assert (testcount == 0);
  params.constructor (p);
  assert (testcount == 1);
  params.destructor (p);
  assert (testcount == 0);

  params = SG::ArenaAllocatorBase::initParams<Test2>(500);
  assert (params.clear == 0);
  params = SG::ArenaAllocatorBase::initParams<Test2, true>(500);
  assert (params.clear != 0);
  params.clear (p);
  assert (testcount == 10);

  params = SG::ArenaAllocatorBase::initParams<int>(500);
  assert (params.constructor == 0);
  assert (params.destructor == 0);

  SG::ArenaAllocatorBase::Stats stats1;
  SG::ArenaAllocatorBase::Stats stats2;

  set_stats (stats1, 1);
  set_stats (stats2, 20);

  SG::ArenaAllocatorBase::Stats stats3 = stats1;
  stats3 += stats2;
  check_stats_sum (stats1, stats2, stats3);
  stats3.clear();
  check_stats_zero (stats3);

  ata.m_name = "foo";
  assert (ata.name() == "foo");
  set_stats (ata.m_stats, 1);
  std::ostringstream os;
  ata.report (os);
  assert (os.str() == "       1/      2/      3    1001/   1002/   1003    2001/   2002/   2003  foo\n");
  std::ostringstream os2;
  stats1.header (os2);
  assert (os2.str() == "Elts InUse/Free/Total   Bytes InUse/Free/Total  Blocks InUse/Free/Total");

  delete  [] p;
  return 0;
}

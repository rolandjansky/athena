/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */

// $Id$
/**
 * @file  CxxUtils/test/ConcurrentRangeMap_test.cxx
 * @author scott snyder
 * @date Nov, 2017
 * @brief Regression test for ConcurrentRangeMap.
 */


#undef NDEBUG
#include "CxxUtils/ConcurrentRangeMap.h"
#include "TestTools/random.h"
#include <mutex>
#include <thread>
#include <shared_mutex>
#include <unordered_set>
#include <cassert>
#include <iostream>
#include <cstdlib>


const int nslots = 4;
const int nwrites = 10000;
const int ninflight = 10;


typedef unsigned int Time;


struct Range
{
  Range (Time begin=0, Time end=0) : m_begin(begin), m_end(end) {}
  Time m_begin;
  Time m_end;
};


struct RangeCompare
{
  bool operator() (Time t, const Range& r) const
  { return t < r.m_begin; }
  bool operator() (const Range& r1, const Range& r2) const
  { return r1.m_begin < r2.m_begin; }
};


struct Payload
{
  typedef std::unordered_set<int> Hist;
  Payload (int x, Hist* hist = nullptr)
    : m_x(x), m_hist (hist) { if (m_hist) m_hist->insert (x); }
  ~Payload()
  {
    if (m_hist) {
      assert (m_hist->count (m_x) == 1);
      m_hist->erase (m_x);
    }
  }
  int m_x;
  Hist* m_hist;
};


template <class T>
class TestUpdater
{
public:
  typedef int Context_t;

  TestUpdater()
    : m_p (nullptr),
      m_inGrace (0)
  {
  }

  TestUpdater (TestUpdater&& other)
    : m_p (static_cast<T*> (other.m_p))
  {
  }

  ~TestUpdater()
  {
    delete m_p;
    for (T* p : m_garbage) delete p;
  }

  void update (std::unique_ptr<T> p, int slot = 0)
  {
    std::lock_guard<std::mutex> g (m_mutex);
    if (m_p) m_garbage.push_back (m_p);
    m_p = p.release();
    m_inGrace = (~(1<<slot)) & ((1<<nslots)-1);
  }

  const T& get() const { return *m_p; }

  void quiescent (int slot)
  {
    std::lock_guard<std::mutex> g (m_mutex);
    m_inGrace &= ~(1<<slot);
    if (!m_inGrace) {
      for (T* p : m_garbage) delete p;
      m_garbage.clear();
    }
  }

  static int defaultContext() { return 0; }


private:
  std::mutex m_mutex;
  std::atomic<T*> m_p;
  std::vector<T*> m_garbage;
  unsigned int m_inGrace;
};


typedef CxxUtils::ConcurrentRangeMap<Range, Time, Payload, RangeCompare,
                                     TestUpdater>
  TestMap;


void test1()
{
  std::cout << "test1\n";
  Payload::Hist phist;
  {
    TestMap map (TestMap::Updater_t(), 3);

    assert (map.size() == 0);
    assert (map.empty());
    assert (map.range().empty());
    assert (map.capacity() == 3);

    //======

    map.emplace (Range (10, 20), std::make_unique<Payload> (100, &phist));
    // 10..20->100 - -
    assert (map.capacity() == 3);
    assert (map.size() == 1);
    assert (!map.empty());
    TestMap::const_iterator_range r = map.range();
    assert (r.size() == 1);
    assert (r.begin()->first.m_begin == 10);
    assert (r.begin()->first.m_end == 20);
    assert (r.begin()->second->m_x == 100);
    assert (r.begin()+1 == r.end());

    assert (map.find (15)->second->m_x == 100);
    assert (map.find (5) == nullptr);
    assert (map.find (25)->second->m_x == 100);

    //======

    map.emplace (Range (25, 30), std::make_unique<Payload> (200, &phist));
    // 10..20->100 25..30->200 -
    assert (map.capacity() == 3);
    assert (map.size() == 2);
    r = map.range();
    assert (r.size() == 2);
    assert (r.begin()->second->m_x == 100);
    assert ((r.begin()+1)->second->m_x == 200);

    assert (map.find (5) == nullptr);
    assert (map.find (10)->second->m_x == 100);
    assert (map.find (15)->second->m_x == 100);
    assert (map.find (20)->second->m_x == 100);
    assert (map.find (25)->second->m_x == 200);
    assert (map.find (30)->second->m_x == 200);

    //======

    map.emplace (Range (30, 40), std::make_unique<Payload> (300, &phist));
    // 10..20->100 25..30->200 30..40->300
    assert (map.capacity() == 3);
    assert (map.size() == 3);
    r = map.range();
    assert (r.size() == 3);
    assert (r.begin()->second->m_x == 100);
    assert ((r.begin()+1)->second->m_x == 200);
    assert ((r.begin()+2)->second->m_x == 300);

    assert (map.find (5) == nullptr);
    assert (map.find (10)->second->m_x == 100);
    assert (map.find (15)->second->m_x == 100);
    assert (map.find (20)->second->m_x == 100);
    assert (map.find (25)->second->m_x == 200);
    assert (map.find (30)->second->m_x == 300);
    assert (map.find (35)->second->m_x == 300);

    //======

    map.emplace (Range (50, 60), std::make_unique<Payload> (400, &phist));
    // 10..20->100 25..30->200 30..40->300 50..60->400 - -
    assert (map.capacity() == 6);
    assert (map.size() == 4);
    r = map.range();
    assert (r.size() == 4);
    assert (r.begin()->second->m_x == 100);
    assert ((r.begin()+1)->second->m_x == 200);
    assert ((r.begin()+2)->second->m_x == 300);
    assert ((r.begin()+3)->second->m_x == 400);

    assert (map.find (5) == nullptr);
    assert (map.find (10)->second->m_x == 100);
    assert (map.find (15)->second->m_x == 100);
    assert (map.find (20)->second->m_x == 100);
    assert (map.find (25)->second->m_x == 200);
    assert (map.find (30)->second->m_x == 300);
    assert (map.find (35)->second->m_x == 300);
    assert (map.find (55)->second->m_x == 400);

    //======

    map.emplace (Range (40, 45), std::make_unique<Payload> (500, &phist));
    // 10..20->100 25..30->200 30..40->300 40..45->500 50..60->400 - - -
    assert (map.capacity() == 8);
    assert (map.size() == 5);
    r = map.range();
    assert (r.size() == 5);
    assert (r.begin()->second->m_x == 100);
    assert ((r.begin()+1)->second->m_x == 200);
    assert ((r.begin()+2)->second->m_x == 300);
    assert ((r.begin()+3)->second->m_x == 500);
    assert ((r.begin()+4)->second->m_x == 400);

    assert (map.find (5) == nullptr);
    assert (map.find (10)->second->m_x == 100);
    assert (map.find (15)->second->m_x == 100);
    assert (map.find (20)->second->m_x == 100);
    assert (map.find (25)->second->m_x == 200);
    assert (map.find (30)->second->m_x == 300);
    assert (map.find (35)->second->m_x == 300);
    assert (map.find (40)->second->m_x == 500);
    assert (map.find (55)->second->m_x == 400);

    //======

    map.erase (15);
    // - 25..30->200 30..40->300 40..45->500 50..60->400 - - -
    assert (map.capacity() == 8);
    assert (map.size() == 4);
    r = map.range();
    assert (r.size() == 4);
    assert (r.begin()->second->m_x == 200);
    assert ((r.begin()+1)->second->m_x == 300);
    assert ((r.begin()+2)->second->m_x == 500);
    assert ((r.begin()+3)->second->m_x == 400);

    assert (map.find (5)  == nullptr);
    assert (map.find (10) == nullptr);
    assert (map.find (15) == nullptr);
    assert (map.find (20) == nullptr);
    assert (map.find (25)->second->m_x == 200);
    assert (map.find (30)->second->m_x == 300);
    assert (map.find (35)->second->m_x == 300);
    assert (map.find (40)->second->m_x == 500);
    assert (map.find (55)->second->m_x == 400);

    //======

    map.erase (25);
    // - - 30..40->300 40..45->500 50..60->400 - - -
    assert (map.capacity() == 8);
    assert (map.size() == 3);
    r = map.range();
    assert (r.size() == 3);
    assert (r.begin()->second->m_x == 300);
    assert ((r.begin()+1)->second->m_x == 500);
    assert ((r.begin()+2)->second->m_x == 400);

    assert (map.find (5)  == nullptr);
    assert (map.find (10) == nullptr);
    assert (map.find (15) == nullptr);
    assert (map.find (20) == nullptr);
    assert (map.find (25) == nullptr);
    assert (map.find (30)->second->m_x == 300);
    assert (map.find (35)->second->m_x == 300);
    assert (map.find (40)->second->m_x == 500);
    assert (map.find (55)->second->m_x == 400);

    //======

    map.erase (40);
    // 30..40->300 50..60->400 - - - - - -
    assert (map.capacity() == 8);
    assert (map.size() == 2);
    r = map.range();
    assert (r.size() == 2);
    assert (r.begin()->second->m_x == 300);
    assert ((r.begin()+1)->second->m_x == 400);

    assert (map.find (5)  == nullptr);
    assert (map.find (10) == nullptr);
    assert (map.find (15) == nullptr);
    assert (map.find (20) == nullptr);
    assert (map.find (25) == nullptr);
    assert (map.find (30)->second->m_x == 300);
    assert (map.find (35)->second->m_x == 300);
    assert (map.find (40)->second->m_x == 300);
    assert (map.find (55)->second->m_x == 400);

    //======

    map.erase (35);
    // - 50..60->400 - - - - - -
    assert (map.capacity() == 8);
    assert (map.size() == 1);
    r = map.range();
    assert (r.size() == 1);
    assert (r.begin()->second->m_x == 400);

    assert (map.find (40) == nullptr);
    assert (map.find (55)->second->m_x == 400);

    //======

    map.erase (50);
    // - - + - - - - - -
    assert (map.capacity() == 8);
    assert (map.size() == 0);
    r = map.range();
    assert (r.size() == 0);

    assert (map.find (55) == nullptr);

    //======

    map.emplace (Range (70, 75), std::make_unique<Payload> (600, &phist));
    // - - 70..75->600 - - - - -
    assert (map.capacity() == 8);
    assert (map.size() == 1);
    r = map.range();
    assert (r.size() == 1);
    assert (r.begin()->second->m_x == 600);
    assert (map.find (55) == nullptr);
    assert (map.find (75)->second->m_x == 600);

    //======

    map.emplace (Range (75, 80), std::make_unique<Payload> (610, &phist));
    map.emplace (Range (80, 85), std::make_unique<Payload> (620, &phist));
    map.emplace (Range (85, 90), std::make_unique<Payload> (630, &phist));
    map.emplace (Range (90, 93), std::make_unique<Payload> (640, &phist));
    map.emplace (Range (93, 96), std::make_unique<Payload> (650, &phist));
    // - - 70..75->600 75..80->610 80..85->620 85..90->630 90..93->640 93..96->650
    assert (map.capacity() == 8);
    assert (map.size() == 6);
    r = map.range();
    assert (r.size() == 6);
    assert (r.begin()->second->m_x == 600);
    assert ((r.begin()+1)->second->m_x == 610);
    assert ((r.begin()+2)->second->m_x == 620);
    assert ((r.begin()+3)->second->m_x == 630);
    assert ((r.begin()+4)->second->m_x == 640);
    assert ((r.begin()+5)->second->m_x == 650);

    assert (map.find (60) == nullptr);
    assert (map.find (70)->second->m_x == 600);
    assert (map.find (77)->second->m_x == 610);
    assert (map.find (84)->second->m_x == 620);
    assert (map.find (85)->second->m_x == 630);
    assert (map.find (92)->second->m_x == 640);
    assert (map.find (94)->second->m_x == 650);
    assert (map.find (99)->second->m_x == 650);

    //======

    map.erase (70);
    map.erase (75);
    map.erase (80);
    map.erase (85);
    map.erase (90);
    // - - - - - - - 93..96->650
    assert (map.capacity() == 8);
    assert (map.size() == 1);
    r = map.range();
    assert (r.size() == 1);
    assert (r.begin()->second->m_x == 650);

    assert (map.find (92) == nullptr);
    assert (map.find (94)->second->m_x == 650);
    assert (map.find (99)->second->m_x == 650);

    //======

    map.emplace (Range (97, 99), std::make_unique<Payload> (660, &phist));
    // 93..96->650 97..99->660 - - - - - -
    assert (map.capacity() == 8);
    assert (map.size() == 2);
    r = map.range();
    assert (r.size() == 2);
    assert (r.begin()->second->m_x == 650);
    assert ((r.begin()+1)->second->m_x == 660);
    assert (map.find (92) == nullptr);
    assert (map.find (94)->second->m_x == 650);
    assert (map.find (99)->second->m_x == 660);

    assert (!phist.empty());
  }

  assert (phist.empty());
}


std::shared_timed_mutex start_mutex;


class test2_Base
{
public:
  test2_Base (int slot);
  int ctx() const { return m_slot; }
  void setContext();

  
private:
  int m_slot;
};


test2_Base::test2_Base (int slot)
  : m_slot (slot)
{
}


void test2_Base::setContext()
{
}



class test2_Writer
  : public test2_Base
{
public:
  test2_Writer (int slot, TestMap& map);
  void operator()();
  Range makeRange (int i);

private:
  TestMap& m_map;
};


test2_Writer::test2_Writer (int slot, TestMap& map)
  : test2_Base (slot),
    m_map (map)
{
}


void test2_Writer::operator()()
{
  setContext();
  std::shared_lock<std::shared_timed_mutex> lock (start_mutex);

  for (int i=0; i < nwrites; i++) {
    if (i >= ninflight) {
      Range rr = makeRange(i-ninflight);
      m_map.erase (rr.m_begin, ctx());
    }
    Range r = makeRange(i);
    m_map.emplace (r, std::make_unique<Payload> (i), ctx());
    m_map.quiescent (ctx());
    if (((i+1)%128) == 0) {
      usleep (1000);
    }
  }
}


Range test2_Writer::makeRange (int i)
{
  return Range (i*10, (i+1)*10);
}


class test2_Iterator
  : public test2_Base
{
public:
  test2_Iterator (int slot, TestMap& map);
  void operator()();

private:
  TestMap& m_map;
};


test2_Iterator::test2_Iterator (int slot, TestMap& map)
  : test2_Base (slot),
    m_map (map)
{
}


void test2_Iterator::operator()()
{
  setContext();
  std::shared_lock<std::shared_timed_mutex> lock (start_mutex);

  bool full = false;
  while (true) {
    int sz = static_cast<int>(m_map.size());
    if (full) {
      assert (std::abs (sz - ninflight) <= 1);
    }
    TestMap::const_iterator_range r = m_map.range();
    sz = r.size();
    if (full) {
      assert (std::abs (sz - ninflight) <= 1);
    }

    if (sz >= ninflight) {
      full = true;
    }

    for (const TestMap::value_type& v : r) {
      assert (static_cast<int>(v.first.m_begin) == static_cast<int>(v.second->m_x)*10);
    }

    if (sz > 0 && (r.end()-1)->second->m_x == nwrites-1) break;

    m_map.quiescent (ctx());
  }
}


class test2_Reader
  : public test2_Base
{
public:
  test2_Reader (int slot, TestMap& map);
  void operator()();

private:
  TestMap& m_map;
  uint32_t m_seed;
};


test2_Reader::test2_Reader (int slot, TestMap& map)
  : test2_Base (slot),
    m_map (map),
    m_seed (slot * 123)
{
}


void test2_Reader::operator()()
{
  setContext();
  std::shared_lock<std::shared_timed_mutex> lock (start_mutex);

  while (true) {
    TestMap::const_iterator_range r = m_map.range();
    if (r.empty()) continue;
    int key = Athena_test::randi_seed (m_seed, (r.end()-1)->first.m_end-1, r.begin()->first.m_begin);
    TestMap::const_iterator it = m_map.find (key);
    if (!it) {
      assert (key < static_cast<int> (m_map.range().begin()->first.m_begin));
    }
    else {
      assert (key >= static_cast<int> (it->first.m_begin) &&
              key < static_cast<int> (it->first.m_end));
      assert (static_cast<int>(it->first.m_begin) == it->second->m_x*10);
    }
    
    if ((r.end()-1)->second->m_x == nwrites-1) break;

    m_map.quiescent (ctx());
  }
}


void test2_iter()
{
  TestMap map (TestMap::Updater_t(), 20);

  const int nthread = 4;
  std::thread threads[nthread];
  start_mutex.lock();

  threads[0] = std::thread (test2_Writer (0, map));
  threads[1] = std::thread (test2_Iterator (1, map));
  threads[2] = std::thread (test2_Reader (2, map));
  threads[3] = std::thread (test2_Reader (3, map));

  // Try to get the threads starting as much at the same time as possible.
  start_mutex.unlock();
  for (int i=0; i < nthread; i++)
    threads[i].join();
}


void test2()
{
  std::cout << "test2\n";

  for (int i=0; i < 5; i++) {
    test2_iter();
  }
}


int main()
{
  test1();
  test2();
  return 0;
}

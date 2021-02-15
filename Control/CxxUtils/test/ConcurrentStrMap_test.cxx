/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CxxUtils/test/ConcurrentStrMap_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2020
 * @brief Tests for ConcurrenStrMap.
 */


#undef NDEBUG
#include "CxxUtils/ConcurrentStrMap.h"
#include "CxxUtils/StrFormat.h"
#include "CxxUtils/MurmurHash2.h"
#include "CxxUtils/checker_macros.h"
#include "TestTools/expect_exception.h"
#include "TestTools/random.h"
#include "tbb/concurrent_unordered_map.h"
#include "boost/timer/timer.hpp"
#ifdef HAVE_CK
extern "C" {
#include "ck_ht.h"
}
#endif
#include <unordered_map>
#include <mutex>
#include <thread>
#include <shared_mutex>
#include <vector>
#include <memory>
#include <iostream>
#include <sstream>
#include <cassert>


const int nslots = 4;

struct Context_t
{
  Context_t (int the_slot = 0) : slot (the_slot) {}
  int slot;
};


template <class T>
class TestUpdater
{
public:
  using Context_t = ::Context_t;

  TestUpdater()
    : m_p (nullptr),
      m_inGrace (0)
  {
  }

  TestUpdater (TestUpdater&& other)
    : m_p (static_cast<T*> (other.m_p)),
      m_inGrace (0)
  {
  }

  TestUpdater& operator= (const TestUpdater&) = delete; // coverity

  ~TestUpdater()
  {
    delete m_p;
    for (T* p : m_garbage) delete p;
  }

  void update (std::unique_ptr<T> p, const Context_t& ctx)
  {
    std::lock_guard<std::mutex> g (m_mutex);
    if (m_p) m_garbage.push_back (m_p);
    m_p = p.release();
    m_inGrace = (~(1<<ctx.slot)) & ((1<<nslots)-1);
  }

  void discard (std::unique_ptr<T> p)
  {
    std::lock_guard<std::mutex> g (m_mutex);
    m_garbage.push_back (p.release());
    m_inGrace = ((1<<nslots)-1);
  }

  const T& get() const { return *m_p; }

  void quiescent (const Context_t& ctx)
  {
    unsigned int mask = (1<<ctx.slot);
    std::lock_guard<std::mutex> g (m_mutex);
    if ((m_inGrace & mask) == 0) return;
    m_inGrace &= ~mask;
    if (!m_inGrace) {
      for (T* p : m_garbage) delete p;
      m_garbage.clear();
    }
  }

  static Context_t defaultContext() { return 0; }


private:
  std::mutex m_mutex;
  std::atomic<T*> m_p;
  std::vector<T*> m_garbage;
  unsigned int m_inGrace;
};


using TestMap = CxxUtils::ConcurrentStrMap<size_t, TestUpdater>;


void test1()
{
  std::cout << "test1\n";
  TestMap map {TestMap::Updater_t()};

  const size_t MAXKEYS = 1000;
  std::vector<std::string> keys;

  for (size_t i = 0; i < MAXKEYS; i++) {
    std::ostringstream ss;
    ss << i;
    keys.push_back (ss.str());
  }

  assert (map.size() == 0);
  assert (map.capacity() == 64);
  assert (map.empty());

  for (size_t i = 0; i < MAXKEYS; i++) {
    auto [it, flag] = map.emplace (keys[i], i);
    assert (flag);
    assert (it.valid());
    assert (it->first == keys[i]);
    assert (it->second == i);
  }

  assert (map.size() == MAXKEYS);
  assert (map.capacity() == 1024);
  assert (!map.empty());

  for (size_t i = 0; i < MAXKEYS; i++) {
    TestMap::const_iterator it = map.find (keys[i]);
    assert (it.valid());
    assert (it != map.end());
    assert (it->first == keys[i]);
    assert (it->second == i);
  }
  assert (map.count (keys[10]) == 1);
  assert (map.count ("foobar") == 0);
  assert (map.contains (keys[10]));
  assert (!map.contains ("foobar"));
  assert (map.find ("foobar") == map.end());
  assert (!map.find ("foobar").valid());

  {
    auto [i1, i2] = map.equal_range (keys[10]);
    assert (i1.valid());
    assert (i1 != i2);
    assert (i1->first == keys[10]);
    assert (i1->second == 10);
    ++i1;
    assert (i1 == i2);
  }

  {
    auto [i1, i2] = map.equal_range ("foobar");
    assert (!i1.valid());
    assert (i1 == map.end());
    assert (i1 == i2);
  }

  assert (map.at (keys[10]) == 10);
  EXPECT_EXCEPTION (std::out_of_range, map.at ("fooabr"));

  for (size_t i = 0; i < MAXKEYS; i++) {
    auto [it, flag] = map.insert_or_assign (keys[i], i+10000);
    assert (!flag);
    assert (it.valid());
    assert (it->first == keys[i]);
    assert (it->second == i+10000);
  }

  assert (map.size() == MAXKEYS);
  assert (map.capacity() == 1024);

  for (size_t i = 0; i < MAXKEYS; i++) {
    TestMap::const_iterator it = map.find (keys[i]);
    assert (it.valid());
    assert (it != map.end());
    assert (it->first == keys[i]);
    assert (it->second == i+10000);
  }

  std::vector<size_t> exp;
  for (size_t i = 0; i < MAXKEYS; i++) {
    exp.push_back (i);
  }

  std::vector<size_t> seen;
  // not using reference, because our iterator doesn't return a reference
  for (const auto p : map.range()) {
    size_t i = p.second - 10000;
    assert (i < MAXKEYS);
    assert (p.first == keys[i]);
    seen.push_back (i);
  }

  std::sort (seen.begin(), seen.end());
  assert (seen == exp);

  seen.clear();
  // not using reference, because our iterator doesn't return a reference
  for (const auto p : map) {
    size_t i = p.second - 10000;
    assert (i < MAXKEYS);
    assert (p.first == keys[i]);
    seen.push_back (i);
  }

  std::sort (seen.begin(), seen.end());
  assert (seen == exp);

  {
    auto [it, flag] = map.insert (std::make_pair ("baz", 99999));
    assert (flag);
    assert (it.valid());
    assert (it->first == "baz");
    assert (it->second == 99999);
  }    
  {
    auto [it, flag] = map.insert (std::make_pair ("baz", 99998));
    assert (!flag);
    assert (it.valid());
    assert (it->first == "baz");
    assert (it->second == 99999);
  }    
}


// Bulk copy / insert.
void test2()
{
  std::cout << "test2\n";
  std::vector<std::pair<std::string, size_t> > data
    { {"zero",  0},
      {"one",   1},
      {"two",   2},
      {"three", 3},
      {"four",  4},
      {"five",  5},
      {"six",   6},
      {"seven", 7},
      {"eight", 8},
      {"nine",  9},
    };
  
  TestMap map1 (data.begin(), data.end(), TestMap::Updater_t());
  assert (map1.size() == 10);
  {
    TestMap::const_iterator it = map1.find ("four");
    assert (it.valid());
    assert (it != map1.end());
    assert (it->first == "four");
    assert (it->second == 4);
  }

  TestMap map2 (map1, TestMap::Updater_t());
  assert (map2.size() == 10);
  {
    TestMap::const_iterator it = map2.find ("six");
    assert (it.valid());
    assert (it != map2.end());
    assert (it->first == "six");
    assert (it->second == 6);
  }

  TestMap map3 {TestMap::Updater_t()};
  assert (map3.capacity() == 64);
  assert (map3.size() == 0);

  map3.reserve (200);
  assert (map3.capacity() == 256);
  map3.reserve (100);
  assert (map3.capacity() == 256);
  map3.quiescent (Context_t());
  assert (map3.size() == 0);

  map3.insert (std::begin(data), std::end(data));
  assert (map3.size() == 10);
  assert (map3.at ("five") == 5);
}


// With a pointer value.
void test3()
{
  std::cout << "test3\n";

  using PtrMap = CxxUtils::ConcurrentStrMap<const int*, TestUpdater>;
  PtrMap map {PtrMap::Updater_t()};

  int v[10] = {0};

  map.emplace ("one", &v[1]);
  map.emplace ("three", &v[3]);
  map.emplace ("five", &v[5]);

  assert (map.at("three") == &v[3]);
  assert (map.find("three")->first == "three");
  assert (map.find("three")->second == &v[3]);
}


//***************************************************************************
// Threaded test.
//


std::shared_timed_mutex start_mutex;


class test4_Base
{
public:
  static constexpr size_t nwrites = 10000;
  
  test4_Base (int slot);
  int ctx() const { return m_slot; }
  const std::string& key (size_t i) const { return m_keys[i]; }
  void setContext();


private:
  int m_slot;
  std::vector<std::string> m_keys;
};


test4_Base::test4_Base (int slot)
  : m_slot (slot)
{
  for (size_t i = 0; i < nwrites; i++) {
    std::ostringstream ss;
    ss << i;
    m_keys.push_back (ss.str());
  }
}


void test4_Base::setContext()
{
}



class test4_Writer
  : public test4_Base
{
public:
  test4_Writer (int slot, TestMap& map);
  void operator()();

private:
  TestMap& m_map;
};


test4_Writer::test4_Writer (int slot, TestMap& map)
  : test4_Base (slot),
    m_map (map)
{
}


void test4_Writer::operator()()
{
  setContext();
  std::shared_lock<std::shared_timed_mutex> lock (start_mutex);

  for (size_t i=0; i < nwrites; i++) {
    assert (m_map.emplace (key(i), i).second);
    m_map.quiescent (ctx());
    if (((i+1)%128) == 0) {
      usleep (1000);
    }
  }

  for (size_t i=0; i < nwrites; i++) {
    assert (!m_map.insert_or_assign (key(i), i + 10*nwrites).second);
    m_map.quiescent (ctx());
    if (((i+1)%128) == 0) {
      usleep (1000);
    }
  }

  for (size_t i=0; i < nwrites; i++) {
    assert (!m_map.insert_or_assign (key(i), i + 20*nwrites).second);
    m_map.quiescent (ctx());
    if (((i+1)%128) == 0) {
      usleep (1000);
    }
  }

  assert (m_map.emplace ("fin", nwrites).second);
}


class test4_Iterator
  : public test4_Base
{
public:
  test4_Iterator (int slot, TestMap& map);
  void operator()();

private:
  TestMap& m_map;
};


test4_Iterator::test4_Iterator (int slot, TestMap& map)
  : test4_Base (slot),
    m_map (map)
{
}


void test4_Iterator::operator()()
{
  setContext();
  std::shared_lock<std::shared_timed_mutex> lock (start_mutex);

  while (true) {
    // not using reference, because our iterator doesn't return a reference
    for (const auto p : m_map) {
      if (p.second == nwrites) continue;
      size_t i = p.second % nwrites;
      assert (key(i) == p.first);
      assert (p.second == i || p.second == i+10*nwrites || p.second == i +20*nwrites);
    }

    TestMap::const_iterator_range range = m_map.range();
    TestMap::const_iterator begin2 = range.begin();
    TestMap::const_iterator end2 = range.end();
    while (begin2 != end2) {
      --end2;
      if (end2->second == nwrites) continue;
      size_t i = end2->second % nwrites;
      assert (key(i) == end2->first);
      assert (end2->second == i || end2->second == i+10*nwrites || end2->second == i +20*nwrites);
    }

    m_map.quiescent (ctx());
    if (m_map.size() > nwrites) break;
  }
}


class test4_Reader
  : public test4_Base
{
public:
  test4_Reader (int slot, TestMap& map);
  void operator()();

private:
  TestMap& m_map;
};


test4_Reader::test4_Reader (int slot, TestMap& map)
  : test4_Base (slot),
    m_map (map)
{
}


void test4_Reader::operator()()
{
  setContext();
  std::shared_lock<std::shared_timed_mutex> lock (start_mutex);

  while (true) {
    for (size_t i = 0; ; ++i) {
      TestMap::const_iterator it = m_map.find (key(i));
      if (it == m_map.end()) break;
      assert(it->second == i || it->second == i+10*nwrites || it->second == i+20*nwrites);
    }

    m_map.quiescent (ctx());
    if (m_map.size() > nwrites) break;
  }
}


void test4_iter()
{
  TestMap map {TestMap::Updater_t()};

  const int nthread = 4;
  std::thread threads[nthread];
  start_mutex.lock();

  threads[0] = std::thread (test4_Writer (0, map));
  threads[1] = std::thread (test4_Iterator (1, map));
  threads[2] = std::thread (test4_Reader (2, map));
  threads[3] = std::thread (test4_Reader (3, map));

  // Try to get the threads starting as much at the same time as possible.
  start_mutex.unlock();
  for (int i=0; i < nthread; i++)
    threads[i].join();
}


void test4()
{
  std::cout << "test4\n";

  for (int i=0; i < 5; i++) {
    test4_iter();
  }
}


//***************************************************************************
// Optional performance test test.
//


class ConcurrentStrMapAdapter
  : public TestMap
{
public:
  ConcurrentStrMapAdapter()
    : TestMap (TestMap::Updater_t(), 512)
  {
  }
  static std::string name() { return "ConcurrentStrMap"; }

  uintptr_t get (const std::string& s) const
  {
    auto it = find (s);
    if (it.valid()) return it->second;
    return 0;
  }

  static const std::string& key (const const_iterator& i)
  { return i->first; }
  static uintptr_t value (const const_iterator& i)
  { return i->second; }
};


class UnorderedMapAdapter
{
public:
  typedef std::unordered_map<std::string, uintptr_t> map_t;
  typedef map_t::const_iterator const_iterator;
  
  static std::string name() { return "UnorderedMap"; }

  void emplace (const std::string& s, uintptr_t p)
  {
    lock_t lock (m_mutex);
    m_map.emplace (s, p);
  }


  uintptr_t get (const std::string& s) const
  {
    lock_t lock (m_mutex);
    auto it = m_map.find (s);
    if (it != m_map.end()) return it->second;
    return 0;
  }

  const_iterator begin() const { return m_map.begin(); }
  const_iterator end() const { return m_map.end(); }
  static const std::string& key (const const_iterator& i)
  { return i->first; }
  static uintptr_t value (const const_iterator& i)
  { return i->second; }

  
private:
  map_t m_map;

  typedef std::mutex mutex_t;
  typedef std::lock_guard<mutex_t> lock_t;
  mutable mutex_t m_mutex;
};


class ConcurrentUnorderedMapAdapter
{
public:
  typedef tbb::concurrent_unordered_map<std::string, uintptr_t> map_t;
  typedef map_t::const_iterator const_iterator;

  static std::string name() { return "concurrent_unordered_map"; }

  void emplace (const std::string& s, uintptr_t p)
  {
    m_map.emplace (s, p);
  }


  uintptr_t get (const std::string& s) const
  {
    auto it = m_map.find (s);
    if (it != m_map.end()) return it->second;
    return 0;
  }

  const_iterator begin() const { return m_map.begin(); }
  const_iterator end() const { return m_map.end(); }
  static const std::string& key (const const_iterator& i)
  { return i->first; }
  static uintptr_t value (const const_iterator& i)
  { return i->second; }

private:
  map_t m_map;
};


#ifdef HAVE_CK
class CKHTAdapter
{
public:
  CKHTAdapter();
  static std::string name() { return "ck_ht"; }

  struct const_iterator
  {
    const_iterator (ck_ht_t* ht)
      : m_ht (ht)
    {
      if (m_ht) {
        ck_ht_iterator_init (&m_it);
        if (!ck_ht_next (m_ht, &m_it, &m_entry)) {
          m_entry = nullptr;
        }
      }
      else {
        m_entry = nullptr;
      }
    }

    const_iterator& operator++()
    {
      if (!ck_ht_next (m_ht, &m_it, &m_entry)) {
        m_entry = nullptr;
      }
      return *this;
    }

    bool operator!= (const const_iterator& other) const
    {
      return m_entry != other.m_entry;
    }

    ck_ht_t* m_ht;
    ck_ht_iterator_t m_it;
    ck_ht_entry_t* m_entry;
  };

  void emplace (const std::string& s, uintptr_t p)
  {
    ck_ht_entry_t entry;
    ck_ht_hash_t h;
    ck_ht_hash (&h, &m_ht, s.c_str(), s.size());
    ck_ht_entry_set (&entry, h, s.c_str(), s.size(),
                     reinterpret_cast<const void*>(p));
    ck_ht_put_spmc (&m_ht, h, &entry);
  }


  uintptr_t get (const std::string& s) const
  {
    ck_ht_entry_t entry;
    ck_ht_hash_t h;
    ck_ht_hash (&h, &m_ht, s.c_str(), s.size());
    ck_ht_entry_key_set (&entry, s.c_str(), s.size());
    if (ck_ht_get_spmc (&m_ht, h, &entry)) {
      return reinterpret_cast<uintptr_t>(ck_ht_entry_value (&entry));
    }
    return 0;
  }

  static const std::string key (const const_iterator& i)
  {
    void* key = ck_ht_entry_key (i.m_entry);
    uint16_t len = ck_ht_entry_key_length (i.m_entry);
    return std::string (reinterpret_cast<const char*>(key), len);
  }
  static uintptr_t value (const const_iterator& i)
  { return reinterpret_cast<uintptr_t>(ck_ht_entry_value(i.m_entry)); }

  const_iterator begin() const
  {
    return const_iterator (&m_ht);
  }

  const_iterator end() const
  {
    return const_iterator (nullptr);
  }


private:
  static void hash (ck_ht_hash_t* h,
                    const void* key,
                    size_t key_length,
                    uint64_t seed)
  {
    // Same as used by libstdc++.
    h->value = CxxUtils::MurmurHash64A (key, key_length, seed);
  }

  static void ht_free (void *p, size_t /*b*/, bool /*r*/) { free(p); }

  ck_malloc m_alloc;
  mutable ck_ht_t m_ht ATLAS_THREAD_SAFE;
};


CKHTAdapter::CKHTAdapter()
{
  m_alloc.malloc = malloc;
  m_alloc.free = ht_free;
  if (!ck_ht_init (&m_ht,
                   CK_HT_MODE_BYTESTRING,
                   hash, // ck_ht_hash_cb_t
                   &m_alloc, // ck_malloc*
                   128, // initial size
                   6602834))
  {
    std::cout << "ck_hs_init error\n";
  }
}
#endif // HAVE_CK


class Timer
{
public:
  Timer();

  class RunTimer
  {
  public:
    RunTimer (boost::timer::cpu_timer& timer) : m_timer (&timer)
    { timer.resume(); }
    RunTimer (RunTimer&& other) : m_timer (other.m_timer) { other.m_timer = nullptr; }
    ~RunTimer() { if (m_timer) m_timer->stop(); }
  private:
    boost::timer::cpu_timer* m_timer;
  };
  RunTimer run() { return RunTimer (m_timer); }

  std::string format() const { return m_timer.format(3); }

private:
  boost::timer::cpu_timer m_timer;
};


Timer::Timer()
{
  m_timer.stop();
}


class TesterBase
{
public:
  TesterBase();

  Timer::RunTimer run_lookup_timer() { return m_lookup_timer.run(); }
  Timer::RunTimer run_iterate_timer() { return m_iterate_timer.run(); }

  void report();

private:
  Timer m_lookup_timer;
  Timer m_iterate_timer;
};


TesterBase::TesterBase()
{
}


void TesterBase::report()
{
  std::cout << "lookup:  " << m_lookup_timer.format();
  std::cout << "iterate: " << m_iterate_timer.format();
}


template <class CONT>
class Tester
  : public TesterBase
{
public:
  static constexpr size_t NCONT = 1000;
  static constexpr size_t NEACH = 10000;
  static constexpr size_t LOG_NENT = 11;
  static constexpr size_t NENT = 1<<LOG_NENT;
  static constexpr size_t ENT_MASK = NENT-1;
  
  Tester();
  void lookup_test();
  void iterate_test();

  void test();
  std::string name() { return CONT::name(); }

private:
  std::vector<std::string> m_keys[NCONT];
  CONT m_cont[NCONT];
  uint32_t m_seed;
};


template <class CONT>
Tester<CONT>::Tester()
  : m_seed (1235)
{
  for (size_t j=0; j < NCONT; j++) {
    for (size_t i = 0; i < NENT; i++) {
      m_keys[j].push_back (CxxUtils::strformat ("key%03lu%08lu", j, i));
    }
    for (size_t i = 0; i < NENT; i++) {
      m_cont[j].emplace (m_keys[j][i],
                         reinterpret_cast<uintptr_t>(&m_keys[j][i]));
    }
  }
}


template <class CONT>
void Tester<CONT>::lookup_test()
{
  auto timer = run_lookup_timer();
  for (size_t irep = 0; irep < NEACH; irep++) {
    for (size_t icont = 0; icont < NCONT; icont++) {
      uint32_t ient = Athena_test::rng_seed (m_seed) & ENT_MASK;
      const std::string& key = m_keys[icont][ient];
      uintptr_t val = m_cont[icont].get (key);
      assert (val == reinterpret_cast<uintptr_t>(&key));
    }
  }
}


template <class CONT>
void Tester<CONT>::iterate_test()
{
  auto timer = run_iterate_timer();
  int icount = 0;
  for (size_t irep = 0; irep < 100; irep++) {
    for (size_t icont = 0; icont < NCONT; icont++) {
      const CONT& cont = m_cont[icont];
      typename CONT::const_iterator it = cont.begin();
      typename CONT::const_iterator end = cont.end();
      while (it != end) {
        const std::string& key = CONT::key (it);
        const uintptr_t  val= CONT::value (it);
        if (((++icount) % 128) == 0) {
          assert (key == *reinterpret_cast<const std::string*>(val));
        }
        ++it;
      }
    }
  }
}


template <class CONT>
void Tester<CONT>::test()
{
  lookup_test();
  iterate_test();
}


template <class CONT>
void perftest_one()
{
  Tester<CONT> tester;
  std::cout << tester.name() << "\n";
  tester.test();
  tester.report();
}


void perftest()
{
  perftest_one<ConcurrentStrMapAdapter>();
  perftest_one<UnorderedMapAdapter>();
  perftest_one<ConcurrentUnorderedMapAdapter>();
#ifdef HAVE_CK
  perftest_one<CKHTAdapter>();
#endif
}


int main (int argc, char** argv)
{
  if (argc >= 2 && strcmp (argv[1], "--perf") == 0) {
    perftest();
    return 0;
  }

  std::cout << "CxxUtils/ConcurrentStrMap_test\n";
  test1();
  test2();
  test3();
  test4();
  return 0;
}

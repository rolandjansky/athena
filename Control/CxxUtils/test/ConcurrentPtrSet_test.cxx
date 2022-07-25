/*
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CxxUtils/test/ConcurrentPtrSet_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2022
 * @brief Tests for ConcurrentStrMap.
 */


#undef NDEBUG
#include "CxxUtils/ConcurrentPtrSet.h"
#include "CxxUtils/MurmurHash2.h"
#include "CxxUtils/checker_macros.h"
#include "TestTools/random.h"
#include "tbb/concurrent_unordered_set.h"
#include "boost/timer/timer.hpp"
#ifdef HAVE_CK
extern "C" {
#include "ck_ht.h"
}
#endif
#include <unordered_set>
#include <mutex>
#include <thread>
#include <shared_mutex>
#include <vector>
#include <memory>
#include <iostream>
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


using TestSet = CxxUtils::ConcurrentPtrSet<int, TestUpdater>;
using TestConstSet = CxxUtils::ConcurrentPtrSet<const int, TestUpdater>;


void test1()
{
  std::cout << "test1\n";
  TestSet set {TestSet::Updater_t()};

  const size_t MAXKEYS = 1000;
  std::vector<int> keys;

  for (size_t i = 0; i < MAXKEYS; i++) {
    keys.push_back (i);
  }

  assert (set.size() == 0);
  assert (set.capacity() == 64);
  assert (set.empty());

  for (size_t i = 0; i < MAXKEYS; i++) {
    auto [it, flag] = set.emplace (&keys[i]);
    assert (flag);
    assert (it.valid());
    assert (*it == &keys[i]);
  }

  assert (set.size() == MAXKEYS);
  assert (set.capacity() == 1024);
  assert (!set.empty());

  for (size_t i = 0; i < MAXKEYS; i++) {
    TestSet::const_iterator it = set.find (&keys[i]);
    assert (it.valid());
    assert (it != set.end());
    assert (*it == &keys[i]);
  }
  assert (set.count (&keys[10]) == 1);
  int xtmp = 0;
  assert (set.count (&xtmp) == 0);
  assert (set.contains (&keys[10]));
  assert (!set.contains (&xtmp));
  assert (set.find (&xtmp) == set.end());
  assert (!set.find (&xtmp).valid());

  {
    auto [i1, i2] = set.equal_range (&keys[10]);
    assert (i1.valid());
    assert (i1 != i2);
    assert (*i1 == &keys[10]);
    ++i1;
    assert (i1 == i2);
  }

  {
    auto [i1, i2] = set.equal_range (&xtmp);
    assert (!i1.valid());
    assert (i1 == set.end());
    assert (i1 == i2);
  }

  std::vector<const int*> exp;
  for (size_t i = 0; i < MAXKEYS; i++) {
    exp.push_back (&keys[i]);
  }

  std::vector<const int*> seen;
  for (const int* p : set.range()) {
    seen.push_back (p);
  }

  std::sort (seen.begin(), seen.end());
  assert (seen == exp);

  seen.clear();
  for (const int* p : set) {
    seen.push_back (p);
  }

  std::sort (seen.begin(), seen.end());
  assert (seen == exp);

  int xtmp2 = 0;
  {
    auto [it, flag] = set.insert (&xtmp2);
    assert (flag);
    assert (it.valid());
    assert (*it == &xtmp2);
  }    
  {
    auto [it, flag] = set.insert (&xtmp2);
    assert (!flag);
    assert (it.valid());
    assert (*it == &xtmp2);
  }

  assert (set.size() > MAXKEYS);
  assert (set.capacity() == 1024);
  assert (!set.empty());
  set.clear();
  assert (set.size() == 0);
  assert (set.capacity() == 1024);
  assert (set.empty());
  set.clear(2048);
  assert (set.size() == 0);
  assert (set.capacity() == 2048);
  assert (set.empty());
}


// Bulk copy / insert.
void test2()
{
  std::cout << "test2\n";
  std::vector<int> vdata { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  std::vector<int*> data;
  for (int i = 0; i < 10; i++) {
    data.push_back (&vdata[i]);
  }
  
  TestSet set1 (data.begin(), data.end(), TestSet::Updater_t());
  assert (set1.size() == 10);
  {
    TestSet::const_iterator it = set1.find (data[4]);
    assert (it.valid());
    assert (it != set1.end());
    assert (**it == 4);
  }

  TestSet set2 (set1, TestSet::Updater_t());
  assert (set2.size() == 10);
  {
    TestSet::const_iterator it = set2.find (data[6]);
    assert (it.valid());
    assert (it != set2.end());
    assert (**it == 6);
  }

  TestSet set3 {TestSet::Updater_t()};
  assert (set3.capacity() == 64);
  assert (set3.size() == 0);

  set3.reserve (200);
  assert (set3.capacity() == 256);
  set3.reserve (100);
  assert (set3.capacity() == 256);
  set3.quiescent (Context_t());
  assert (set3.size() == 0);

  set3.insert (std::begin(data), std::end(data));
  assert (set3.size() == 10);
  assert (set3.count (data[5]) == 1);
}


// Const
void test3()
{
  std::cout << "test3\n";

  TestConstSet set {TestConstSet::Updater_t()};

  const size_t MAXKEYS = 1000;
  std::vector<int> keys;

  for (size_t i = 0; i < MAXKEYS; i++) {
    keys.push_back (i);
  }

  for (size_t i = 0; i < MAXKEYS; i++) {
    assert (set.emplace (&keys[i]).second);
  }

  assert (set.size() == MAXKEYS);

  for (size_t i = 0; i < MAXKEYS; i++) {
    TestConstSet::const_iterator it = set.find (&keys[i]);
    assert (it.valid());
    assert (it != set.end());
    assert (*it == &keys[i]);
  }
  assert (set.count (&keys[10]) == 1);

  std::vector<const int*> exp;
  for (size_t i = 0; i < MAXKEYS; i++) {
    exp.push_back (&keys[i]);
  }

  std::vector<const int*> seen;
  for (const int* p : set.range()) {
    seen.push_back (p);
  }

  std::sort (seen.begin(), seen.end());
  assert (seen == exp);

  seen.clear();
  for (const int* p : set) {
    seen.push_back (p);
  }

  std::sort (seen.begin(), seen.end());
  assert (seen == exp);
}


//***************************************************************************
// Threaded test.
//


std::shared_timed_mutex start_mutex;


class test4_Base
{
public:
  static constexpr size_t nwrites = 10000;
  
  test4_Base (int slot, std::vector<int>& keys);
  int ctx() const { return m_slot; }
  int* key (size_t i) { return &m_keys[i]; }
  void setContext();


private:
  int m_slot;
  std::vector<int>& m_keys;
};


test4_Base::test4_Base (int slot, std::vector<int>& keys)
  : m_slot (slot),
    m_keys (keys)
{
}


void test4_Base::setContext()
{
}



class test4_Writer
  : public test4_Base
{
public:
  test4_Writer (int slot, TestSet& set, std::vector<int>& keys);
  void operator()();

private:
  TestSet& m_set;
  int m_fin = -1;
};


test4_Writer::test4_Writer (int slot, TestSet& set, std::vector<int>& keys)
  : test4_Base (slot, keys),
    m_set (set)
{
}


void test4_Writer::operator()()
{
  setContext();
  std::shared_lock<std::shared_timed_mutex> lock (start_mutex);

  for (size_t i=0; i < nwrites; i++) {
    assert (m_set.emplace (key(i)).second);
    m_set.quiescent (ctx());
    if (((i+1)%128) == 0) {
      usleep (1000);
    }
  }

  assert (m_set.insert (&m_fin).second);
}


class test4_Iterator
  : public test4_Base
{
public:
  test4_Iterator (int slot, TestSet& set, std::vector<int>& keys);
  void operator()();

private:
  TestSet& m_set;
};


test4_Iterator::test4_Iterator (int slot, TestSet& set, std::vector<int>& keys)
  : test4_Base (slot, keys),
    m_set (set)
{
}


void test4_Iterator::operator()()
{
  setContext();
  std::shared_lock<std::shared_timed_mutex> lock (start_mutex);

  while (true) {
    for (const int* p : m_set) {
      assert (*p >= -1 && *p < static_cast<int>(nwrites));
    }

    TestSet::const_iterator_range range = m_set.range();
    TestSet::const_iterator begin2 = range.begin();
    TestSet::const_iterator end2 = range.end();
    while (begin2 != end2) {
      --end2;
      assert (**end2 >= -1 && **end2 < static_cast<int>(nwrites));
    }

    m_set.quiescent (ctx());
    if (m_set.size() > nwrites) break;
  }
}


class test4_Reader
  : public test4_Base
{
public:
  test4_Reader (int slot, TestSet& set, std::vector<int>& keys);
  void operator()();

private:
  TestSet& m_set;
};


test4_Reader::test4_Reader (int slot, TestSet& set, std::vector<int>& keys)
  : test4_Base (slot, keys),
    m_set (set)
{
}


void test4_Reader::operator()()
{
  setContext();
  std::shared_lock<std::shared_timed_mutex> lock (start_mutex);

  while (true) {
    for (int i = 0; ; ++i) {
      TestSet::const_iterator it = m_set.find (key(i));
      if (it == m_set.end()) break;
      assert(**it == i);
    }

    m_set.quiescent (ctx());
    if (m_set.size() > nwrites) break;
  }
}


void test4_iter()
{
  TestSet set {TestSet::Updater_t()};

  const int nthread = 4;
  std::thread threads[nthread];
  start_mutex.lock();

  std::vector<int> keys;
  for (size_t i = 0; i < test4_Base::nwrites; i++) {
    keys.push_back (i);
  }

  threads[0] = std::thread (test4_Writer (0, set, keys));
  threads[1] = std::thread (test4_Iterator (1, set, keys));
  threads[2] = std::thread (test4_Reader (2, set, keys));
  threads[3] = std::thread (test4_Reader (3, set, keys));

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
// Optional performance test.
//


class ConcurrentPtrSetAdapter
  : public TestSet
{
public:
  ConcurrentPtrSetAdapter()
    : TestSet (TestSet::Updater_t(), 512)
  {
  }
  static std::string name() { return "ConcurrentPtrSet"; }
  static const int* key (const const_iterator& i) { return *i; }
};


class UnorderedSetAdapter
{
public:
  typedef std::unordered_set<int*> set_t;
  typedef set_t::const_iterator const_iterator;
  
  static std::string name() { return "UnorderedSet"; }

  void emplace (int* p)
  {
    lock_t lock (m_mutex);
    m_set.emplace (p);
  }


  size_t count (int* p) const
  {
    lock_t lock (m_mutex);
    return m_set.count (p);
  }

  // FIXME: No locking for iteration.
  const_iterator begin() const { return m_set.begin(); }
  const_iterator end() const { return m_set.end(); }
  static const int* key (const const_iterator& i) { return *i; }


private:
  set_t m_set;

  typedef std::mutex mutex_t;
  typedef std::lock_guard<mutex_t> lock_t;
  mutable mutex_t m_mutex;
};


class ConcurrentUnorderedSetAdapter
{
public:
  typedef tbb::concurrent_unordered_set<int*> set_t;
  typedef set_t::const_iterator const_iterator;

  static std::string name() { return "concurrent_unordered_set"; }

  void emplace (int* p)
  {
    m_set.emplace (p);
  }


  size_t count (int* p) const
  {
    return m_set.count (p);
  }

  const_iterator begin() const { return m_set.begin(); }
  const_iterator end() const { return m_set.end(); }
  static const int* key (const const_iterator& i) { return *i; }

private:
  set_t m_set;
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

  void emplace (int* p)
  {
    uintptr_t pint = reinterpret_cast<uintptr_t>(p);
    ck_ht_entry_t entry;
    ck_ht_hash_t h;
    ck_ht_hash_direct (&h, &m_ht, pint);
    ck_ht_entry_set_direct (&entry, h, pint, 0);
    ck_ht_put_spmc (&m_ht, h, &entry);
  }


  size_t count (int* p) const
  {
    uintptr_t pint = reinterpret_cast<uintptr_t>(p);
    ck_ht_entry_t entry;
    ck_ht_hash_t h;
    ck_ht_hash_direct (&h, &m_ht, pint);
    ck_ht_entry_key_set_direct (&entry, pint);
    if (ck_ht_get_spmc (&m_ht, h, &entry)) {
      return 1;
    }
    return 0;
  }

  static const int* key (const const_iterator& i)
  {
    return reinterpret_cast<const int*> (ck_ht_entry_key_direct (i.m_entry));
  }

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
                    size_t /*key_length*/,
                    uint64_t /*seed*/)
  {
    static const std::hash<const void*> hasher;
    h->value = hasher (*reinterpret_cast<const void* const *>(key));
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
                   CK_HT_MODE_DIRECT,
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
  std::vector<int> m_keys[NCONT];
  CONT m_cont[NCONT];
  uint32_t m_seed;
};


template <class CONT>
Tester<CONT>::Tester()
  : m_seed (1235)
{
  for (size_t j=0; j < NCONT; j++) {
    for (size_t i = 0; i < NENT; i++) {
      m_keys[j].push_back (j*NENT + i);
    }
    for (size_t i = 0; i < NENT; i++) {
      m_cont[j].emplace (&m_keys[j][i]);
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
      assert (m_cont[icont].count (&m_keys[icont][ient]) == 1);
      int dum = 0;
      assert (m_cont[icont].count (&dum) == 0);
    }
  }
}


template <class CONT>
void Tester<CONT>::iterate_test()
{
  auto timer = run_iterate_timer();
  for (size_t irep = 0; irep < 100; irep++) {
    for (size_t icont = 0; icont < NCONT; icont++) {
      const CONT& cont = m_cont[icont];
      typename CONT::const_iterator it = cont.begin();
      typename CONT::const_iterator end = cont.end();
      while (it != end) {
        const int* p = CONT::key (it);
        assert (*p >= static_cast<int>(icont*NENT) &&
                *p < static_cast<int>((icont+1)*NENT));
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
  perftest_one<ConcurrentPtrSetAdapter>();
  perftest_one<UnorderedSetAdapter>();
  perftest_one<ConcurrentUnorderedSetAdapter>();
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

  std::cout << "CxxUtils/ConcurrentPtrSet_test\n";
  test1();
  test2();
  test3();
  test4();
  return 0;
}

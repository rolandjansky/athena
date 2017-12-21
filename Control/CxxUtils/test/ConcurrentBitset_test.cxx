/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */

// $Id$
/**
 * @file CxxUtils/test/ConcurrentBitset_test.cxx
 * @author scott snyder
 * @date Nov, 2017
 * @brief Regression test for ConcurrentBitset.
 */


#undef NDEBUG
#include "CxxUtils/ConcurrentBitset.h"
#include "TestTools/random.h"
#include "tbb/concurrent_unordered_set.h"
#include "boost/timer/timer.hpp"
#ifdef HAVE_CK
extern "C" {
#include "ck_hs.h"
#include "ck_bitmap.h"
}
#endif
#include <mutex>
#include <thread>
#include <bitset>
#include <shared_mutex>
#include <algorithm>
#include <vector>
#include <string>
#include <set>
#include <unordered_set>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cassert>
#include <cstring>


// Basic tests.
void test_basic()
{
  std::cout << "test_basic\n";

  CxxUtils::ConcurrentBitset bs0;
  assert (bs0.capacity() == 0);
  assert (bs0.size() == 0);
  assert (bs0.count() == 0);
  assert (!bs0.test(0));
  assert (bs0.empty());

  CxxUtils::ConcurrentBitset bs (128);
  assert (bs.capacity() == 128);
  assert (bs.size() == 0);
  assert (bs.count() == 0);
  assert (bs.empty());

  // set, test, reset, etc.
  assert (!bs.test (10));
  assert (!bs.test (20));
  assert (!bs.test (30));
  bs.set (10);
  bs.set (30);
  assert (bs.test (10));
  assert (!bs.test (20));
  assert (bs.test (30));
  assert (bs.size() == 2);
  assert (!bs.empty());

  bs.insert (100);
  assert (bs.count (100) == 1);
  assert (bs.count (101) == 0);
  assert (bs.count() == 3);

  bs.reset (30);
  assert (!bs.test (30));
  assert (bs.size() == 2);

  bs.flip (110);
  assert (bs.test (110));
  assert (bs.size() == 3);
  bs.flip (10);
  assert (!bs.test (10));
  assert (bs.size() == 2);

  bs.set (10, true);
  assert (bs.test (10));
  assert (bs.size() == 3);
  bs.set (10, false);
  assert (!bs.test (10));
  assert (bs.size() == 2);

  // Set to all / clear.
  bs.reset();
  assert (bs.size() == 0);
  assert (!bs.test (10));
  assert (!bs.test (50));
  assert (!bs.test (100));
  assert (!bs.test (101));

  bs.set();
  assert (bs.size() == 128);
  assert (bs.test (10));
  assert (bs.test (50));
  assert (bs.test (100));
  assert (bs.test (101));

  bs.clear();
  assert (bs.size() == 0);
  assert (!bs.test (10));
  assert (!bs.test (50));
  assert (!bs.test (100));
  assert (!bs.test (101));
}


// Reference.
void test_reference()
{
  std::cout << "test_reference\n";

  CxxUtils::ConcurrentBitset bs (128);
  const CxxUtils::ConcurrentBitset& cbs = bs;

  assert (!bs[10]);
  assert (!cbs[10]);
  assert (~bs[10]);

  bs[10] = true;
  assert (cbs[10]);
  assert (bs.count() == 1);

  bs[20] = bs[10];
  assert (cbs[10]);
  assert (cbs[20]);
  assert (bs.count() == 2);

  bs[20] = bs[21];
  assert (cbs[10]);
  assert (!cbs[20]);
  assert (bs.count() == 1);

  bs[30].flip();
  assert (cbs[10]);
  assert (!cbs[20]);
  assert (cbs[30]);
  assert (bs.count() == 2);

  bs[10] = false;
  assert (!cbs[10]);
  assert (!cbs[20]);
  assert (cbs[30]);
  assert (bs.count() == 1);
}


// Iterate.
void test_iterate()
{
  std::cout << "test_iterate\n";

  CxxUtils::ConcurrentBitset bs0;
  CxxUtils::ConcurrentBitset::const_iterator beg = bs0.begin();
  CxxUtils::ConcurrentBitset::const_iterator end = bs0.end();
  for (; beg != end; ++beg)
    std::cout << "elt\n";
  
  //std::vector<size_t> v0 (bs0.begin(), bs0.end());
  //assert (v0.empty());

  CxxUtils::ConcurrentBitset bs (2048);
  bs.set (100);
  bs.set (200);
  bs.set (300);
  bs.set (400);

  std::vector<size_t> v1 (bs.begin(), bs.end());
  assert (v1 == (std::vector<size_t> {100, 200, 300, 400}));

  bs.set (1000);
  std::vector<size_t> v2 (bs.begin(), bs.end());
  assert (v2 == (std::vector<size_t> {100, 200, 300, 400, 1000}));

  bs.reset (200);
  std::vector<size_t> v3 (bs.begin(), bs.end());
  assert (v3 == (std::vector<size_t> {100, 300, 400, 1000}));

  bs.reset (1000);
  std::vector<size_t> v4 (bs.begin(), bs.end());
  assert (v4 == (std::vector<size_t> {100, 300, 400}));

  bs.set (0);
  bs.set (2047);
  std::vector<size_t> v5 (bs.begin(), bs.end());
  assert (v5 == (std::vector<size_t> {0, 100, 300, 400, 2047}));

  bs.set (101);
  bs.set (102);
  bs.set (103);
  std::vector<size_t> v6 (bs.begin(), bs.end());
  assert (v6 == (std::vector<size_t> {0, 100, 101, 102, 103, 300, 400, 2047}));

  bs.set (3);
  std::vector<size_t> v7 (bs.begin(), bs.end());
  assert (v7 == (std::vector<size_t> {0, 3, 100, 101, 102, 103, 300, 400, 2047}));

  bs.set (1);
  std::vector<size_t> v8 (bs.begin(), bs.end());
  assert (v8 == (std::vector<size_t> {0, 1, 3, 100, 101, 102, 103, 300, 400, 2047}));

  bs.set (64);
  std::vector<size_t> v9 (bs.begin(), bs.end());
  assert (v9 == (std::vector<size_t> {0, 1, 3, 64, 100, 101, 102, 103, 300, 400, 2047}));

  bs.set (65);
  std::vector<size_t> v10 (bs.begin(), bs.end());
  assert (v10 == (std::vector<size_t> {0, 1, 3, 64, 65, 100, 101, 102, 103, 300, 400, 2047}));

  bs.set (63);
  std::vector<size_t> v11 (bs.begin(), bs.end());
  assert (v11 == (std::vector<size_t> {0, 1, 3, 63, 64, 65, 100, 101, 102, 103, 300, 400, 2047}));

  bs.reset (0);
  bs.reset (1);
  std::vector<size_t> v12 (bs.begin(), bs.end());
  assert (v12 == (std::vector<size_t> {3, 63, 64, 65, 100, 101, 102, 103, 300, 400, 2047}));
}


// Find.
void test_find()
{
  std::cout << "test_find\n";

  CxxUtils::ConcurrentBitset bs (2048);
  assert (bs.find (123) == bs.end());

  bs.set (0);
  bs.set (63);
  bs.set (64);
  bs.set (65);
  bs.set (2047);
  std::vector<size_t> v0 (bs.find(0), bs.end());
  assert (v0 == (std::vector<size_t> {0, 63, 64, 65, 2047}));
  std::vector<size_t> v1 (bs.find(63), bs.end());
  assert (v1 == (std::vector<size_t> {63, 64, 65, 2047}));
  std::vector<size_t> v2 (bs.find(64), bs.end());
  assert (v2 == (std::vector<size_t> {64, 65, 2047}));
  std::vector<size_t> v3 (bs.find(65), bs.end());
  assert (v3 == (std::vector<size_t> {65, 2047}));
  std::vector<size_t> v4 (bs.find(2047), bs.end());
  assert (v4 == (std::vector<size_t> {2047}));
}


// Insert.
void test_insert()
{
  std::cout << "test_insert\n";

  CxxUtils::ConcurrentBitset bs (3);
  assert (bs.capacity() == 3);

  assert (!bs.test (35));
  bs.set (35);
  assert (!bs.test (35));
  assert (bs.capacity() == 3);

  bs.insert (35);
  assert (bs.test (35));
  assert (bs.capacity() == 128);

  assert (!bs.test (300));
  bs.set (300);
  assert (!bs.test (300));
  assert (bs.capacity() == 128);

  bs.insert (300);
  assert (bs.test (300));
  assert (bs.capacity() == 640);

  bs.emptyGarbage();

  CxxUtils::ConcurrentBitset bs2 (1024);
  std::vector<size_t> v2 { 4, 10, 100, 500 };
  bs2.insert (v2.begin(), v2.end());
  assert (bs2.size() == 4);
  assert (bs2.test (4));
  assert (bs2.test (10));
  assert (bs2.test (100));
  assert (bs2.test (500));

  bs2.insert ({ 10, 11, 12});
  std::vector<size_t> v3 (bs2.begin(), bs2.end());
  assert (v3 == (std::vector<size_t> {4, 10, 11, 12, 100, 500}));

  bs2.insert ({ 3000, 3001 });
  std::vector<size_t> v4 (bs2.begin(), bs2.end());
  assert (v4 == (std::vector<size_t> {4, 10, 11, 12, 100, 500, 3000, 3001}));

  CxxUtils::ConcurrentBitset bs3 (128);
  bs3.insert ({3, 10, 100});
  CxxUtils::ConcurrentBitset bs4 (256);
  bs4.insert ({2, 10, 200});

  bs3.insert (bs4);
  std::vector<size_t> v5 (bs3.begin(), bs3.end());
  assert (v5 == (std::vector<size_t> {2, 3, 10, 100, 200}));
}


// Copy/assign
void test_copy()
{
  std::cout << "test_copy\n";

  CxxUtils::ConcurrentBitset bs1 (128);
  assert (bs1.capacity() == 128);
  bs1.set (5);
  bs1.set (45);
  bs1.set (87);

  CxxUtils::ConcurrentBitset bs2 (bs1);
  assert (bs2.capacity() == 128);
  assert (bs2.count() == 3);
  assert (bs2.test(5));
  assert (bs2.test(45));
  assert (!bs2.test(46));
  assert (bs2.test(87));

  bs2.insert (500, 1024);
  assert (bs2.capacity() == 1024);
  assert (bs2.test(500));

  bs2 = bs1;
  assert (bs2.capacity() == 1024);
  assert (bs2.test(5));
  assert (bs2.test(45));
  assert (!bs2.test(46));
  assert (bs2.test(87));
  assert (!bs2.test(500));

  bs2.set (501);
  bs1 = bs2;
  assert (bs1.capacity() == 1024);
  assert (bs1.test(5));
  assert (bs1.test(45));
  assert (!bs1.test(46));
  assert (bs1.test(87));
  assert (!bs1.test(500));
  assert (bs1.test(501));

  CxxUtils::ConcurrentBitset bs3;
  bs3 = bs1;
  std::vector<size_t> v3 (bs3.begin(), bs3.end());
  assert (v3 == (std::vector<size_t> {5, 45, 87, 501}));
}


// Initializer list
void test_initlist()
{
  std::cout << "test_initlist\n";

  CxxUtils::ConcurrentBitset bs1 { 10, 50 };
  assert (bs1.size() == 2);
  assert (bs1.test (10));
  assert (!bs1.test (9));
  assert (bs1.test (50));
  assert (bs1.capacity() == 64);

  CxxUtils::ConcurrentBitset bs2 { 10, 40, 90, 100 };
  assert (bs2.size() == 4);
  assert (bs2.test (10));
  assert (bs2.test (40));
  assert (bs2.test (90));
  assert (bs2.test (100));
  assert (!bs2.test (9));
  assert (!bs2.test (91));
  assert (bs2.capacity() == 128);
}


// Comparison
void test_compare()
{
  std::cout << "test_compare\n";

  CxxUtils::ConcurrentBitset bs1 { 10, 40, 50, 60 };
  CxxUtils::ConcurrentBitset bs2 { 10, 40, 50, 60 };

  assert (bs1 == bs2);
  assert (!(bs1 != bs2));

  bs1.set (15);
  assert (!(bs1 == bs2));
  assert (bs1 != bs2);

  bs2.set (15);
  assert (bs1 == bs2);
  assert (!(bs1 != bs2));

  bs1.insert (1000);
  assert (!(bs1 == bs2));
  assert (bs1 != bs2);
  bs1.reset (1000);
  assert (bs1 == bs2);
  assert (!(bs1 != bs2));

  bs2.insert (2000);
  assert (!(bs1 == bs2));
  assert (bs1 != bs2);
  bs2.reset (2000);
  assert (bs1 == bs2);
  assert (!(bs1 != bs2));

  bs1.insert (2000);
  bs2.insert (2000);
  assert (bs1 == bs2);
  assert (!(bs1 != bs2));
}


// Operators
void test_operators()
{
  std::cout << "test_operators\n";

  CxxUtils::ConcurrentBitset bs1 (256);
  CxxUtils::ConcurrentBitset bs2 (128);

  bs1.insert ({2, 10, 50, 70, 100, 150, 200});
  bs2.insert ({2, 12, 48, 70, 100});

  assert ((bs1 &= bs2).size() == 3);
  std::vector<size_t> v1 (bs1.begin(), bs1.end());
  assert (v1 == (std::vector<size_t> {2, 70, 100}));

  bs1.insert ({2, 10, 50, 70, 100, 150, 200});
  assert ((bs1 |= bs2).size() == 9);
  std::vector<size_t> v2 (bs1.begin(), bs1.end());
  assert (v2 == (std::vector<size_t> {2, 10, 12, 48, 50, 70, 100, 150, 200}));

  assert ((bs1 ^= bs2).size() == 4);
  std::vector<size_t> v3 (bs1.begin(), bs1.end());
  assert (v3 == (std::vector<size_t> {10, 50, 150, 200}));

  bs2.insert ({50, 150});
  assert ((bs1 -= bs2).size() == 2);
  std::vector<size_t> v4 (bs1.begin(), bs1.end());
  assert (v4 == (std::vector<size_t> {10, 200}));

  bs1.flip();
  std::vector<size_t> v5;
  for (size_t i = 0; i < 256; i++) {
    if (!bs1.test (i)) v5.push_back (i);
  }
  assert (v5 == (std::vector<size_t> {10, 200}));

  CxxUtils::ConcurrentBitset bs4 = ~bs1;
  std::vector<size_t> v6 (bs4.begin(), bs4.end());
  assert (v6 == (std::vector<size_t> {10, 200}));
}


// any/all/none/empty
void test_anyall()
{
  std::cout << "test_anyall\n";

  CxxUtils::ConcurrentBitset bs (128);
  assert (bs.none());
  assert (bs.empty());
  assert (!bs.all());
  assert (!bs.any());
}


//************************ MT test.


//  Have N threads, M bitsets.
//  Within each bitset, thread i accesses bits b where b mod N == i.
//  Thread i can expand bitset s where s mod N == i.
//  Each thread has a set of random bitstrings.
//  It fills all bitsets with strings, checks them, then rotates the strings.


std::shared_timed_mutex start_mutex;


class test_mt_Thread
{
public:
  static constexpr const size_t maxbit = 65536;
  static constexpr const float prob = 0.2;
  static const int niter = 500;

  test_mt_Thread (int slot, int nsets, int nslots,
                std::vector<CxxUtils::ConcurrentBitset>& sets,
                std::mutex* mutexes);
  void operator()();
  void dowrite (int iset, int irand,
                CxxUtils::ConcurrentBitset& flipsave);

  
private:
  int m_slot;
  int m_nsets;
  int m_nslots;
  std::vector<CxxUtils::ConcurrentBitset>& m_sets;
  std::mutex* m_mutexes;
  typedef std::bitset<maxbit> randset_t;
  std::vector<randset_t> m_rands;
};
constexpr const size_t test_mt_Thread::maxbit;


test_mt_Thread::test_mt_Thread (int slot,
                            int nsets,
                            int nslots,
                            std::vector<CxxUtils::ConcurrentBitset>& sets,
                            std::mutex* mutexes)
  : m_slot (slot),
    m_nsets (nsets),
    m_nslots (nslots),
    m_sets (sets),
    m_mutexes (mutexes)
{
  uint32_t seed = (slot+1) * 2531;
  m_rands.resize (nsets);
  for (int i=0; i < nsets; i++) {
    for (size_t j=0; j < maxbit; j++) {
      if (Athena_test::randf_seed (seed, 1.0) < prob) {
        m_rands[i].set (j);
      }
    }
  }
}


void test_mt_Thread::dowrite (int iset, int irand,
                            CxxUtils::ConcurrentBitset& flipsave)
{
  CxxUtils::ConcurrentBitset& set = m_sets[iset];
  randset_t& rand = m_rands[irand];
  size_t nbits = set.capacity();
  if (irand == 0) {
    flipsave = set;
    for (size_t ibit = m_slot; ibit < nbits; ibit += m_nslots) {
      set.flip (ibit);
    }
  }
  else {
    for (size_t ibit = m_slot; ibit < nbits; ibit += m_nslots) {
      if (rand.test(ibit)) {
        set.set (ibit);
      }
      else {
        set.reset (ibit);
      }
    }
  }
}


void test_mt_Thread::operator()()
{
  std::shared_lock<std::shared_timed_mutex> lock (start_mutex);

  int randoff = 0;
  int expoff = m_slot;
  for (int iter = 0; iter < niter; iter++) {

    std::vector<size_t> sizes (m_nsets);
    CxxUtils::ConcurrentBitset flipsave (maxbit);

    // set
    for (int iset = 0; iset < m_nsets; iset++) {
      sizes[iset] = m_sets[iset].capacity();
      int irand = (iset + randoff) % m_nsets;
      std::lock_guard<std::mutex> lock (m_mutexes[iset]);
      dowrite (iset, irand, flipsave);
    }

    // check
    for (int iset = 0; iset < m_nsets; iset++) {
      CxxUtils::ConcurrentBitset& set = m_sets[iset];
      int irand = (iset + randoff) % m_nsets;
      randset_t& rand = m_rands[irand];
      size_t nbits = sizes[iset];
      assert (nbits <= set.capacity());
      if (irand == 0) {
        for (size_t ibit = m_slot; ibit < nbits; ibit += m_nslots) {
          if (flipsave.test (ibit)) {
            assert (!set.test (ibit));
          }
          else {
            assert (set.test (ibit));
          }
        }
      }
      else {
        for (size_t ibit = m_slot; ibit < nbits; ibit += m_nslots) {
          if (rand.test(ibit)) {
            assert (set.test (ibit));
          }
          else {
            assert (!set.test (ibit));
          }
        }
      }
    }

    {
      CxxUtils::ConcurrentBitset& set = m_sets[expoff];
      size_t nbits = set.capacity();
      if (nbits < maxbit) {
        size_t newsize = std::min (nbits + 128, maxbit);
        std::lock_guard<std::mutex> lock (m_mutexes[expoff]);
        set.insert (newsize-1, newsize);
      }

      expoff += m_nslots;
      if (expoff >= m_nsets) {
        expoff = m_slot;
      }
    }

    randoff++;
    if (randoff >= m_nsets) randoff = 0;
  }
}


void test_mt_iter()
{
  const int nthread = 4;
  const int nsets = 100;

  std::vector<CxxUtils::ConcurrentBitset> bitsets
    (nsets, CxxUtils::ConcurrentBitset (128));
  std::mutex mutexes[nsets];

  std::thread threads[nthread];
  start_mutex.lock();

  for (int i=0; i < nthread; i++) {
    threads[i] = std::thread (test_mt_Thread (i, nsets, nthread, bitsets, mutexes));
  }

  // Try to get the threads starting as much at the same time as possible.
  start_mutex.unlock();
  for (int i=0; i < nthread; i++) {
    threads[i].join();
  }
}


void test_mt()
{
  std::cout << "test_mt\n";

  for (int i=0; i < 5; i++) {
    test_mt_iter();
  }
}


//************************ performance testing.


class TestVectors
{
public:
  typedef std::vector<size_t> vec_t;

  void read (const std::string& fname);
  size_t size() const { return m_vecs.size(); }
  const vec_t& vec (size_t i) const
  {
    if (i >= m_vecs.size()) std::abort();
    return m_vecs[i];
  }

  std::vector<vec_t>::const_iterator begin() const { return m_vecs.begin(); }
  std::vector<vec_t>::const_iterator end() const { return m_vecs.end(); }

private:
  std::vector<vec_t> m_vecs;
};


void TestVectors::read (const std::string& fname)
{
  std::ifstream s (fname);
  std::string str;
  while (std::getline (s, str)) {
    std::istringstream ss (str);
    size_t id;
    std::vector<size_t> v;
    while (ss >> id) {
      v.push_back (id);
    }
    m_vecs.push_back (std::move (v));
  }
}


class SetAdapter
  : public std::set<size_t>
{
public:
  bool test (size_t id) const
  {
    return find(id) != end();
  }
  static std::string name() { return "set"; }
};


class USetAdapter
  : public std::unordered_set<size_t>
{
public:
  bool test (size_t id) const
  {
    return find(id) != end();
  }
  static std::string name() { return "unordered_set"; }
};


class CUSetAdapter
  : public tbb::concurrent_unordered_set<size_t>
{
public:
  bool test (size_t id) const
  {
    return find(id) != end();
  }
  static std::string name() { return "concurrent_unordered_set"; }
};


#ifdef HAVE_CK
class CKHSAdapter
{
public:
  CKHSAdapter();
  CKHSAdapter (const CKHSAdapter& other);

  static std::string name() { return "ck_hs"; }

  void insert (size_t id)
  {
    if (!ck_hs_put (&m_hs, id, reinterpret_cast<const void*> (id))) {
      std::cout << "ck_hs_put error\n";
    }
  }

  bool test (size_t id) const
  {
    return ck_hs_get (const_cast<ck_hs_t*>(&m_hs), id, reinterpret_cast<const void*> (id)) != nullptr;
  }


  struct const_iterator
    : public ck_hs_iterator_t
  {
    const_iterator(const ck_hs_t* hs) : m_hs (const_cast<ck_hs_t*>(hs))
    { cursor = nullptr; offset = 0; }
    const_iterator() : const_iterator(nullptr) {}
    size_t operator* () const { return reinterpret_cast<size_t> (m_elt); }
    const_iterator& operator++()
    {
      if (!ck_hs_next (m_hs, this, &m_elt)) {
        cursor = nullptr;
        offset = 0;
      }
      return *this;
    }

    bool operator== (const const_iterator& other) const
    {
      return cursor == other.cursor && offset == other.offset;
    }
    bool operator!= (const const_iterator& other) const
    {
      return !(*this == other);
    }

    ck_hs_t* m_hs;
    void* m_elt = nullptr;
  };


  const_iterator begin() const
  {
    const_iterator it (&m_hs);
    ++it;
    return it;
  }
  const_iterator end() const { return const_iterator(); }


private:
  static unsigned long hash (const void* p, unsigned long /*seed*/)
  { return reinterpret_cast<unsigned long> (p); }

  static bool compare (const void* p1, const void* p2)
  { return p1 == p2; }

  static void hs_free (void *p, size_t /*b*/, bool /*r*/) { free(p); }

  ck_malloc m_alloc;
  ck_hs_t m_hs;
};


CKHSAdapter::CKHSAdapter()
{
  m_alloc.malloc = malloc;
  m_alloc.free = hs_free;
  if (!ck_hs_init (&m_hs,
                   CK_HS_MODE_SPMC | CK_HS_MODE_DIRECT,
                   hash, // ck_hs_hash_cb_t
                   compare, // ck_hs_compare_cb_t
                   &m_alloc, // ck_malloc*
                   128, // initial size
                   6602834))
  {
    std::cout << "ck_hs_init error\n";
  }
}


CKHSAdapter::CKHSAdapter (const CKHSAdapter& other)
  : CKHSAdapter()
{
  ck_hs_iterator it { nullptr, 0};
  void* obj;
  while (ck_hs_next (const_cast<ck_hs_t*>(&other.m_hs), &it, &obj)) {
    insert (reinterpret_cast<size_t> (obj));
  }
}


class CKBitmapAdapter
{
public:
  typedef CK_BITMAP_INSTANCE(2048) Bitmap_t;

  CKBitmapAdapter();
  CKBitmapAdapter (const CKBitmapAdapter& other);

  static std::string name() { return "ck_bitmap"; }

  void insert (size_t id)
  { CK_BITMAP_SET (&m_map, id); }

  bool test (size_t id) const
  { return CK_BITMAP_TEST (&m_map, id); }

  struct const_iterator
    : public ck_bitmap_iterator
  {
    const_iterator() : m_map(nullptr), m_bit(-1) {}
    const_iterator (const Bitmap_t* map) : m_map(map), m_bit(0)
    { CK_BITMAP_ITERATOR_INIT (this, map); }
    size_t operator*() const { return m_bit; }
    const_iterator& operator++()
    {
      if (!CK_BITMAP_NEXT (m_map, this, &m_bit)) {
        m_bit = -1;
      }
      return *this;
    }

    bool operator!= (const const_iterator& /*other*/)
    { return m_bit != static_cast<unsigned int>(-1); }


  private:
    const Bitmap_t* m_map;
    unsigned int m_bit;
  };

  const_iterator begin() const
  {
    const_iterator it(&m_map);
    ++it;
    return it;
  }
  const_iterator end() const { return const_iterator(); }

private:
  Bitmap_t m_map;
};


CKBitmapAdapter::CKBitmapAdapter()
{
  CK_BITMAP_INIT (&m_map, 2048, false);
}


CKBitmapAdapter::CKBitmapAdapter (const CKBitmapAdapter& other)
  : CKBitmapAdapter()
{
  CK_BITMAP_UNION (&m_map, &other.m_map);
}
#endif


class ConcurrentBitsetAdapter
  : public CxxUtils::ConcurrentBitset
{
public:
  ConcurrentBitsetAdapter() : CxxUtils::ConcurrentBitset (2048) {}
  static std::string name() { return "ConcurrentBitset"; }
};


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
  TesterBase (const TestVectors& tv);

  const TestVectors& tv() const { return m_tv; }

  Timer::RunTimer run_fill_timer() { return m_fill_timer.run(); }
  Timer::RunTimer run_copy_timer() { return m_copy_timer.run(); }
  Timer::RunTimer run_iterate_timer() { return m_iterate_timer.run(); }
  Timer::RunTimer run_lookup_timer() { return m_lookup_timer.run(); }

  void report();

private:
  const TestVectors& m_tv;
  Timer m_fill_timer;
  Timer m_copy_timer;
  Timer m_iterate_timer;
  Timer m_lookup_timer;
};


TesterBase::TesterBase (const TestVectors& tv)
  : m_tv (tv)
{
}


void TesterBase::report()
{
  std::cout << "fill:    " << m_fill_timer.format();
  std::cout << "copy:    " << m_copy_timer.format();
  std::cout << "iterate: " << m_iterate_timer.format();
  std::cout << "lookup:  " << m_lookup_timer.format();
}


template <class CONT>
class Tester
  : public TesterBase
{
public:
  static const int NEACH = 1000;

  Tester (const TestVectors& tv);
  void fill_test();
  void copy_test();
  size_t iterate_test();
  int lookup_test();

  void test();
  std::string name() { return CONT::name(); }

private:
  std::vector<CONT> m_conts;
};


template <class CONT>
Tester<CONT>::Tester (const TestVectors& tv)
  : TesterBase (tv),
    m_conts (tv.size())
{
  for (size_t i = 0; i < tv.size(); i++) {
    const TestVectors::vec_t& v = tv.vec(i);
    for (size_t id : v) {
      m_conts[i].insert (id);
    }
  }
}


template <class CONT>
void Tester<CONT>::fill_test()
{
  auto timer = run_fill_timer();
  for (int i=0; i < NEACH; i++) {
    for (const TestVectors::vec_t& vec : tv()) {
      CONT cont;
      for (size_t id : vec) {
        cont.insert (id);
      }
    }
  }
}


template <class CONT>
void Tester<CONT>::copy_test()
{
  auto timer = run_copy_timer();
  for (const CONT& cont : m_conts) {
    for (int i=0; i < NEACH; i++) {
      CONT new_cont (cont);
    }
  }
}


template <class CONT>
size_t Tester<CONT>::iterate_test()
{
  size_t tot = 0;
  auto timer = run_iterate_timer();
  for (int i=0; i < NEACH*10 * 10; i++) {
    for (const CONT& cont : m_conts) {
      for (size_t id : cont) {
        tot += id;
      }
    }
  }
  return tot;
}

template <class CONT>
int Tester<CONT>::lookup_test()
{
  int tot = 0;
  auto timer = run_lookup_timer();
  size_t sz = m_conts.size();
  for (int j=0; j < NEACH; j++) {
    for (size_t icont = 0; icont < sz; icont++) {
      const CONT& cont = m_conts[icont];
      const TestVectors::vec_t& vec = tv().vec(icont);
      for (size_t id : vec) {
        tot += cont.test (id);
        tot += cont.test (id+1);
      }
    }
  }
  return tot;
}


template <class CONT>
void Tester<CONT>::test()
{
  fill_test();
  copy_test();
  iterate_test();
  lookup_test();
}


template <class CONT>
void perftest_one (const TestVectors& tv)
{
  Tester<CONT> tester (tv);
  std::cout << tester.name() << "\n";
  tester.test();
  tester.report();
}


void perftest (const char* setfile)
{
  TestVectors tv;
  tv.read (setfile);

  perftest_one<SetAdapter> (tv);
  perftest_one<USetAdapter> (tv);
  perftest_one<CUSetAdapter> (tv);
#ifdef HAVE_CK 
  perftest_one<CKHSAdapter> (tv);
  perftest_one<CKBitmapAdapter> (tv);
#endif
  perftest_one<ConcurrentBitsetAdapter> (tv);
}


int main (int argc, char** argv)
{
  if (argc > 2 && strcmp (argv[1], "--perf") == 0) {
    perftest (argv[2]);
    return 0;
  }

  test_basic();
  test_reference();
  test_iterate();
  test_find();
  test_insert();
  test_copy();
  test_initlist();
  test_compare();
  test_operators();
  test_anyall();
  test_mt();
  return 0;
}

    

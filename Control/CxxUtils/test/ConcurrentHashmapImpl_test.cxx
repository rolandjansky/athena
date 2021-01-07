/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CxxUtils/test/ConcurrentHashmapImpl_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2020
 * @brief Tests for ConcurrentHashmapImpl.
 */


#undef NDEBUG
#include "CxxUtils/ConcurrentHashmapImpl.h"
#include <mutex>
#include <thread>
#include <shared_mutex>
#include <vector>
#include <memory>
#include <iostream>
#include <cassert>
#include <unistd.h>


const int nslots = 4;


// Needed to access internals of ConcurrentHashmapImpl.
class ConcurrentHashmapImplTest
{
public:
  static void test2();
};


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


struct TestHash
{
  size_t operator() (size_t x) const { return x; }
};


using CHMImpl = CxxUtils::detail::ConcurrentHashmapImpl<TestUpdater, TestHash>;


// Test CHMTableIterator
void test1()
{
  std::cout << "test1\n";
  using TableIterator = CxxUtils::detail::CHMTableIterator<4>;
  // table capacity = 64
  TableIterator it (12345, 0x3f, 6, 16);
  std::vector<size_t> offsets;
  do {
    offsets.push_back (it.offset());
  } while (it.next());
  assert ((std::vector<size_t> {57, 58, 59, 56, 61, 62, 63, 60, 5, 6, 7, 4, 17, 18, 19, 16 }) == offsets);
}


// Test ConcurrentHashmapImpl::Table.
void ConcurrentHashmapImplTest::test2()
{
  std::cout << "test2\n";
  using entry_t = CHMImpl::entry_t;
  using Table = CHMImpl::Table;
  std::unique_ptr<Table> table (new (16) Table (16));

  assert (table->capacity() == 16);

  size_t nstored = 0;
  while (true) {
    size_t key = 23*nstored + 100;
    size_t hash = TestHash() (key);

    bool insert;
    size_t offset = table->probeWrite (key, hash, insert);
    if (offset == CHMImpl::INVALID) {
      break;
    }
    assert (insert);
    entry_t& ent = table->entry (offset);
    ent.m_val = key+1;
    ent.m_key = key;
    const Table* ctable = table.get();
    const entry_t& cent = ctable->entry (offset);
    assert (cent.m_val == key+1);
    assert (cent.m_key == key);
    ++nstored;
  }
  assert (nstored == 16);

  for (size_t i = 0; i < nstored; ++i) {
    size_t key = 23*i + 100;
    size_t hash = TestHash() (key);
    {
      size_t offset = table->probeRead (key, hash);
      assert (offset != CHMImpl::INVALID);
      const entry_t& ent = table->entry (offset);
      assert (ent.m_key == key && ent.m_val == key+1);
    }
    {
      bool insert;
      size_t offset = table->probeWrite (key, hash, insert);
      assert (offset != CHMImpl::INVALID);
      assert (!insert);
      entry_t& ent = table->entry (offset);
      assert (ent.m_key == key && ent.m_val == key+1);
    }
  }

  size_t key = 23*nstored + 100;
  size_t hash = TestHash() (key);
  assert (table->probeRead (key, hash) == CHMImpl::INVALID);
  bool insert;
  assert (table->probeWrite (key, hash, insert) == CHMImpl::INVALID);
}


void test3()
{
  std::cout << "test3\n";
  CHMImpl chm (CHMImpl::Updater_t(), 50,
               CHMImpl::Hasher_t(),
               CHMImpl::Matcher_t(),
               CHMImpl::Context_t());

  assert (chm.size() == 0);
  assert (chm.capacity() == 64);

  assert (chm.hasher()(12345) == CHMImpl::Hasher_t()(12345));
  assert (chm.matcher() (4321, 4321));
  assert (!chm.matcher() (4321, 4322));

  {
    auto [begin, end] = chm.range();
    assert (!(begin != end));
  }

  for (size_t i = 0; i < 1000; i++) {
    size_t key = 23*i + 100;
    size_t hash = TestHash() (key);

    auto [it, flag] = chm.put (key, hash, key+1, true, Context_t());
    assert (flag);
    assert (it.valid());
    assert (it.key() == key);
    assert (it.value() == key+1);
  }
  assert (chm.size() == 1000);
  assert (chm.capacity() == 1024);

  for (size_t i = 0; i < 1000; i++) {
    size_t key = 23*i + 100;
    size_t hash = TestHash() (key);

    CHMImpl::const_iterator it = chm.get (key, hash);
    assert (it.valid());
    assert (it.value() == key+1);
  }

  for (size_t i = 0; i < 1000; i++) {
    size_t key = 23*i + 100;
    size_t hash = TestHash() (key);

    auto [it, flag] = chm.put (key, hash, key+2, true, Context_t());
    assert (!flag);
    assert (it.valid());
    assert (it.key() == key);
    assert (it.value() == key+2);
  }
  assert (chm.size() == 1000);
  assert (chm.capacity() == 1024);

  std::vector<size_t> exp;
  for (size_t i = 0; i < 1000; i++) {
    size_t key = 23*i + 100;
    size_t hash = TestHash() (key);

    CHMImpl::const_iterator it = chm.get (key, hash);
    assert (it.valid());
    assert (it.value() == key+2);

    exp.push_back (key);
  }
  std::sort (exp.begin(), exp.end());

  std::vector<size_t> seen;
  {
    auto [begin, end] = chm.range();
    for (auto it = begin; it != end; it.next()) {
      assert (it.key()+2 == it.value());
      seen.push_back (it.key());
    }
  }
  std::sort (seen.begin(), seen.end());
  assert (exp == seen);

  seen.clear();
  {
    CHMImpl::const_iterator begin = chm.begin();
    CHMImpl::const_iterator it = chm.end();
    assert (it != begin);
    do {
      it.prev();
      assert (it.key()+2 == it.value());
      seen.push_back (it.key());
    } while (it != begin);
  }
    
  std::sort (seen.begin(), seen.end());
  assert (exp == seen);

  {
    size_t key = 23*500 + 100;
    size_t hash = TestHash() (key);
    auto [it, flag] = chm.put (key, hash, 200, false, Context_t());
    assert (!flag);
    assert (it.valid());
    assert (it.key() == key);
    assert (it.value() == key+2);

    CHMImpl::const_iterator it2 = chm.get (key, hash);
    assert (it2.valid());
    assert (it2.value() == key+2);
  }

  {
    size_t key = 23*1500 + 100;
    size_t hash = TestHash() (key);
    auto [it, flag] = chm.put (key, hash, 200, false, Context_t());
    assert (flag);
    assert (it.valid());
    assert (it.key() == key);
    assert (it.value() == 200);

    CHMImpl::const_iterator it2 = chm.get (key, hash);
    assert (it2.valid());
    assert (it2.value() == 200);
  }

  assert (chm.size() == 1001);
  assert (chm.capacity() == 1024);

  {
    size_t iseen = 0;
    CHMImpl::const_iterator it = chm.clear (Context_t());
    while (it.valid()) {
      ++iseen;
      it.next();
    }
    assert (iseen == 1001);
  }
  assert (chm.size() == 0);
  assert (chm.capacity() == 1024);

  chm.clear (200, Context_t());
  assert (chm.size() == 0);
  assert (chm.capacity() == 256);

  chm.reserve (100, Context_t());
  assert (chm.size() == 0);
  assert (chm.capacity() == 256);

  chm.reserve (300, Context_t());
  assert (chm.size() == 0);
  assert (chm.capacity() == 512);
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
  void setContext();

  
private:
  int m_slot;
};


test4_Base::test4_Base (int slot)
  : m_slot (slot)
{
}


void test4_Base::setContext()
{
}



class test4_Writer
  : public test4_Base
{
public:
  test4_Writer (int slot, CHMImpl& map);
  void operator()();

private:
  CHMImpl& m_map;
};


test4_Writer::test4_Writer (int slot, CHMImpl& map)
  : test4_Base (slot),
    m_map (map)
{
}


void test4_Writer::operator()()
{
  setContext();
  std::shared_lock<std::shared_timed_mutex> lock (start_mutex);

  for (size_t i=0; i < nwrites; i++) {
    size_t key = 23*i + 100;
    size_t hash = TestHash() (key);
    assert (m_map.put (key, hash, key+2, true, Context_t()).second);
    m_map.quiescent (ctx());
    if (((i+1)%128) == 0) {
      usleep (1000);
    }
  }

  for (size_t i=0; i < nwrites; i++) {
    size_t key = 23*i + 100;
    size_t hash = TestHash() (key);
    assert (!m_map.put (key, hash, key+3, true, Context_t()).second);
    m_map.quiescent (ctx());
    if (((i+1)%128) == 0) {
      usleep (1000);
    }
  }

  for (size_t i=0; i < nwrites; i++) {
    size_t key = 23*i + 100;
    size_t hash = TestHash() (key);
    assert (!m_map.put (key, hash, key+4, true, Context_t()).second);
    m_map.quiescent (ctx());
    if (((i+1)%128) == 0) {
      usleep (1000);
    }
  }

  {
    size_t key = 23*nwrites + 100;
    size_t hash = TestHash() (key);
    assert (m_map.put (key, hash, key+3, true, Context_t()).second);
  }
}



class test4_Iterator
  : public test4_Base
{
public:
  test4_Iterator (int slot, CHMImpl& map);
  void operator()();

private:
  CHMImpl& m_map;
};


test4_Iterator::test4_Iterator (int slot, CHMImpl& map)
  : test4_Base (slot),
    m_map (map)
{
}


void test4_Iterator::operator()()
{
  setContext();
  std::shared_lock<std::shared_timed_mutex> lock (start_mutex);

  while (true) {
    auto [begin, end] = m_map.range();
    while (begin != end) {
      size_t key = begin.key();
      size_t val = begin.value();
      assert (val == key+2 || val == key+3 || val == key+4);
      begin.next();
    }

    auto [begin2, end2] = m_map.range();
    if (begin2 != end2) {
      do {
        end2.prev();
        size_t key = end2.key();
        size_t val = end2.value();
        assert (val == key+2 || val == key+3 || val == key+4);
      } while (begin2 != end2);
    }

    m_map.quiescent (ctx());
    if (m_map.size() > nwrites) break;
  }
}


class test4_Reader
  : public test4_Base
{
public:
  test4_Reader (int slot, CHMImpl& map);
  void operator()();

private:
  CHMImpl& m_map;
};


test4_Reader::test4_Reader (int slot, CHMImpl& map)
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
      size_t key = 23*i + 100;
      size_t hash = TestHash() (key);
      CHMImpl::const_iterator it = m_map.get (key, hash);
      if (!it.valid()) {
        break;
      }
      assert(it.value() == key+2 || it.value() == key+3 || it.value() == key+4);
    }

    m_map.quiescent (ctx());
    if (m_map.size() > nwrites) break;
  }
}


void test4_iter()
{
  CHMImpl chm (CHMImpl::Updater_t(), 50,
               CHMImpl::Hasher_t(),
               CHMImpl::Matcher_t(),
               CHMImpl::Context_t());

  const int nthread = 4;
  std::thread threads[nthread];
  start_mutex.lock();

  threads[0] = std::thread (test4_Writer (0, chm));
  threads[1] = std::thread (test4_Iterator (1, chm));
  threads[2] = std::thread (test4_Reader (2, chm));
  threads[3] = std::thread (test4_Reader (3, chm));

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


int main()
{
  std::cout << "CxxUtils/ConcurrentHashmapImpl_test\n";
  test1();
  ConcurrentHashmapImplTest::test2();
  test3();
  test4();
  return 0;
}

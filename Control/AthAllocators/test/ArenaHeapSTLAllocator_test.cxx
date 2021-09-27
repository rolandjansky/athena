/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthAllocators/test/ArenaHeapSTLAllocator_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov 2011
 * @brief Regression tests for ArenaHeapSTLAllocator.
 */


#undef NDEBUG
#include "AthAllocators/ArenaHeapSTLAllocator.h"
#include "CxxUtils/checker_macros.h"
#include <list>
#include <vector>
#include <cassert>
#include <iostream>
#include <atomic>
#include <setjmp.h>
#include <signal.h>


//==========================================================================

struct Payload
{
  Payload(int = 0);
  Payload(const Payload& p);
  Payload& operator= (const Payload&) = default;
  ~Payload();
  void clear();

  int x;
  int y;
  static std::atomic<int> n;
  static std::vector<int> v ATLAS_THREAD_SAFE;
};

// cppcheck-suppress uninitMemberVar  // false positive
Payload::Payload(int the_y)
{
  x = n++;
  y = the_y;
  v.push_back (x);
}

Payload::Payload (const Payload& p)
  : x (p.x),
    y (p.y)
{
  v.push_back (x);
}


Payload::~Payload()
{
  v.push_back (-x);
}

void Payload::clear ()
{
  y = 0;
}

std::atomic<int> Payload::n;
std::vector<int> Payload::v;


struct CTest
{
  typedef SG::ArenaHeapSTLAllocator<int> allocator_type;
  CTest (allocator_type& a) : m_alloc(a) {}
  allocator_type get_allocator() const { return m_alloc; }

  allocator_type& m_alloc;
};


//==========================================================================

// Test generic specialization.
void test1()
{
  Payload::v.clear();
  Payload::n = 0;

  std::cout << "test1\n";

  SG::ArenaHeapSTLAllocator<Payload> a1;
  assert (a1.nblock() == 1000);
  assert (a1.name() == "");

  SG::ArenaHeapSTLAllocator<Payload> a2 (100, "a2");
  assert (a2.nblock() == 100);
  assert (a2.name() == "a2");

  SG::ArenaHeapSTLAllocator<Payload*, int> a3 (100, "a3");
  assert (a3.nblock() == 100);
  assert (a3.name() == "a3");

  SG::ArenaHeapSTLAllocator<Payload, int> a4 (a3);
  assert (a4.nblock() == 100);
  assert (a4.name() == "a3");
  assert (a4.poolptr() != nullptr);

  {
    Payload p;
    Payload& pr = p;
    const Payload& pcr = p;
    assert (a4.address (pr) == &p);
    assert (a4.address (pcr) == &p);
  }
  assert (Payload::n == 1);
  assert (Payload::v.size() == 2);
  Payload::v.clear();

  std::vector<Payload*> pv;
  for (int i=0; i < 10; i++)
    pv.push_back (a4.allocate (1));

  assert (Payload::n == 1);
  assert (Payload::v.size() == 0);

  assert (a4.max_size() == 1);

  a4.construct (pv[0], Payload(10));
  a4.destroy (pv[0]);

  assert (Payload::n == 2);
  std::vector<int> exp1 {1, 1, -1, -1};
  assert (Payload::v == exp1);
  Payload::v.clear();

  for (int i=0; i < 9; i++) {
    if (i > 0)
      assert (pv[i] != pv[i-1]);
    a4.deallocate (pv[i], 1);
  }

  assert (a4.stats().elts.inuse == 1);
  assert (a4.stats().elts.total >= 100);

  a4.reset();
  assert (a4.stats().elts.inuse == 0);
  assert (a4.stats().elts.total >= 100);

  a4.erase();
  assert (a4.stats().elts.inuse == 0);
  assert (a4.stats().elts.total == 0);

  a4.reserve(5000);
  assert (a4.stats().elts.inuse == 0);
  assert (a4.stats().elts.total >= 5000);

  assert (Payload::n == 2);
  assert (Payload::v.size() == 0);

  {
    SG::ArenaHeapSTLAllocator<const Payload, int> a5;
    const Payload* p = a5.allocate (1);
    a5.deallocate (p, 1);
  }
}


// Test vetoed specialization.
void test2()
{
  Payload::v.clear();
  Payload::n = 0;

  std::cout << "test2\n";

  SG::ArenaHeapSTLAllocator<int, int> a1;
  assert (a1.nblock() == 1000);
  assert (a1.name() == "");
  assert (a1.poolptr() == nullptr);

  SG::ArenaHeapSTLAllocator<int, int> a2 (100, "a2");
  assert (a2.nblock() == 100);
  assert (a2.name() == "a2");
  assert (a2.poolptr() == nullptr);

  CTest c2 (a2);
  auto a2_nc = SG::ArenaHeapSTLAllocator<int>::get_allocator (c2);
  a2_nc.reset();
  a2_nc.erase();
  a2_nc.reserve(10);
  assert (a2.stats().elts.total == 0);

  SG::ArenaHeapSTLAllocator<Payload, int> a3 (500, "a3");
  a3.reserve (1000);
  SG::ArenaHeapSTLAllocator<int> a4 (a3);
  assert (a4.nblock() == 500);
  assert (a4.name() == "a3");
  assert (a4.poolptr() == a3.poolptr());

  (void)a3.allocate (1);
  assert (a3.stats().elts.inuse == 1);
  assert (a3.stats().elts.total >= 1000);

  assert (a4.stats().elts.inuse == 1);
  assert (a4.stats().elts.total >= 1000);

  CTest c4 (a4);
  auto a4_nc = SG::ArenaHeapSTLAllocator<int>::get_allocator (c4);
  a4_nc.reset();
  assert (a4.stats().elts.inuse == 0);
  assert (a4.stats().elts.total >= 1000);

  a4_nc.erase();
  assert (a4.stats().elts.inuse == 0);
  assert (a4.stats().elts.total == 0);

  a4_nc.reserve(1000);
  assert (a4.stats().elts.inuse == 0);
  assert (a4.stats().elts.total >= 1000);

  int* p = a4.allocate (2);
  assert (a4.stats().elts.inuse == 0);
  assert (a4.stats().elts.total >= 1000);

  a4.deallocate (p, 2);
  assert (a4.stats().elts.inuse == 0);
  assert (a4.stats().elts.total >= 1000);

  assert (Payload::n == 0);
  assert (Payload::v.size() == 0);
}


// Copy/assign/move.
void test3()
{
  Payload::v.clear();
  Payload::n = 0;

  std::cout << "test3\n";
  SG::ArenaHeapSTLAllocator<Payload, int> b1 (100, "b1");
  assert (b1.nblock() == 100);
  assert (b1.name() == "b1");

  Payload* pp[10];
  pp[0] = b1.allocate(1);
  assert (b1.stats().elts.inuse == 1);
  assert (b1.stats().elts.total >= 100);

  SG::ArenaHeapSTLAllocator<Payload, int> b2 (std::move (b1));
  assert (b1.nblock() == 100);
  assert (b1.name() == "b1");
  assert (b2.nblock() == 100);
  assert (b2.name() == "b1");
  assert (b1.stats().elts.inuse == 0);
  assert (b1.stats().elts.total == 0);
  assert (b2.stats().elts.inuse == 1);
  assert (b2.stats().elts.total >= 100);
  b2.deallocate (pp[0], 1);
  pp[1] = b2.allocate(1);
  pp[0] = b2.allocate(1);
  assert (b2.stats().elts.inuse == 2);
  assert (b2.stats().elts.total >= 100);

  b1 = std::move(b2);
  assert (b1.nblock() == 100);
  assert (b1.name() == "b1");
  assert (b2.nblock() == 100);
  assert (b2.name() == "b1");
  assert (b2.stats().elts.inuse == 0);
  assert (b2.stats().elts.total == 0);
  assert (b1.stats().elts.inuse == 2);
  assert (b1.stats().elts.total >= 100);
  b1.deallocate (pp[0], 1);
  pp[2] = b1.allocate(1);
  pp[0] = b1.allocate(1);
  assert (b1.stats().elts.inuse == 3);
  assert (b1.stats().elts.total >= 100);

  b1.swap(b2);
  assert (b1.nblock() == 100);
  assert (b1.name() == "b1");
  assert (b2.nblock() == 100);
  assert (b2.name() == "b1");
  assert (b1.stats().elts.inuse == 0);
  assert (b1.stats().elts.total == 0);
  assert (b2.stats().elts.inuse == 3);
  assert (b2.stats().elts.total >= 100);

  assert (b1 == b1);
  assert (!(b1 != b1));
  assert (!(b1 == b2));
  assert (b1 != b2);
}


// In-situ test.
void test4()
{
  Payload::v.clear();
  Payload::n = 0;

  std::cout << "test4\n";

  typedef SG::ArenaHeapSTLAllocator<int> allocator_t;
  typedef std::list<int,  allocator_t> list_t;

  list_t::allocator_type allo (500, "allo");
  list_t list (allo);

  for (int i = 0; i < 10; i++)
    list.push_back (i);

  assert (list.size() == 10);
  assert (list.get_allocator().stats().elts.total >= 500);
  assert (list.get_allocator().stats().elts.inuse == 10);

  for (list_t::iterator i = list.begin();
       i != list.end();
       ++i)
  {
    i = list.erase (i);
    if (i == list.end()) break;
  }

  assert (list.size() == 5);
  assert (list.get_allocator().stats().elts.total >= 500);
  assert (list.get_allocator().stats().elts.inuse == 5);

  list.clear();
  assert (list.size() == 0);
  assert (list.get_allocator().stats().elts.total >= 500);
  assert (list.get_allocator().stats().elts.inuse == 0);

  allocator_t::get_allocator(list).reset();
  assert (list.get_allocator().stats().elts.total >= 500);
  assert (list.get_allocator().stats().elts.inuse == 0);

  assert (Payload::n == 0);
  assert (Payload::v.size() == 0);
}


// Copy/move container.
void test5()
{
  std::cout << "test5\n";
  Payload::v.clear();
  Payload::n = 0;

  typedef std::list<int, SG::ArenaHeapSTLAllocator<int> > list_t;

  list_t::allocator_type allo (500, "allo");
  list_t list (allo);

  for (int i = 0; i < 10; i++)
    list.push_back (i);

  assert (list.size() == 10);
  assert (list.get_allocator().stats().elts.total >= 500);
  assert (list.get_allocator().stats().elts.inuse == 10);

  list_t list2 = list;
  assert (list.size() == 10);
  assert (list.get_allocator().stats().elts.total >= 500);
  assert (list.get_allocator().stats().elts.inuse == 10);

  assert (list2.size() == 10);
  assert (list2.get_allocator().stats().elts.total >= 500);
  assert (list2.get_allocator().stats().elts.inuse == 10);

  // This fails in gcc 6.2.
  // The implementation of list assignment relies on this
  // implementation of insert():
  //    insert(const_iterator __position, _InputIterator __first,
  //         _InputIterator __last)
  //    {
  //      list __tmp(__first, __last, get_allocator());
  //      if (!__tmp.empty())
  //      {
  //        iterator __it = __tmp.begin();
  //        splice(__position, __tmp);
  //        return __it;
  //      }
  //      return __position._M_const_cast();
  //    }
  //
  // However, for our case, __tmp will end up with a different allocator
  // than *this, so the splice will fail.  This seems to me like a bug
  // in list::insert(), but needs more checking.
#if 0
  list_t list3;
  list3 = list;
  assert (list.size() == 10);
  assert (list.get_allocator().stats().elts.total >= 500);
  assert (list.get_allocator().stats().elts.inuse == 10);

  assert (list3.size() == 10);
  assert (list3.get_allocator().stats().elts.total >= 1000);
  assert (list3.get_allocator().stats().elts.inuse == 10);
#endif

  list_t list4 (std::move (list2));
  assert (list4.size() == 10);
  assert (list4.get_allocator().stats().elts.total >= 500);
  assert (list4.get_allocator().stats().elts.inuse == 10);
  assert (list2.size() == 0);
  assert (list2.get_allocator().stats().elts.total == 0);
  assert (list2.get_allocator().stats().elts.inuse == 0);

  list2 = std::move(list4);
  assert (list2.size() == 10);
  assert (list2.get_allocator().stats().elts.total >= 500);
  assert (list2.get_allocator().stats().elts.inuse == 10);
  assert (list4.size() == 0);
  assert (list4.get_allocator().stats().elts.total == 0);
  assert (list4.get_allocator().stats().elts.inuse == 0);

  list2.swap (list4);
  assert (list4.size() == 10);
  assert (list4.get_allocator().stats().elts.total >= 500);
  assert (list4.get_allocator().stats().elts.inuse == 10);
  assert (list2.size() == 0);
  assert (list2.get_allocator().stats().elts.total == 0);
  assert (list2.get_allocator().stats().elts.inuse == 0);
}


jmp_buf jmp ATLAS_THREAD_SAFE;
void handler (int)
{
  siglongjmp (jmp, 1);
}
void setsig()
{
  struct sigaction act;
  act.sa_handler = handler;
  sigemptyset (&act.sa_mask);
  act.sa_flags = 0;
  if (sigaction (SIGSEGV, &act, nullptr) != 0) std::abort();
}
void resetsig()
{
  struct sigaction act;
  act.sa_handler = SIG_DFL;
  sigemptyset (&act.sa_mask);
  act.sa_flags = 0;
  if (sigaction (SIGSEGV, &act, nullptr) != 0) std::abort();
  sigset_t sigs;
  if (sigemptyset (&sigs) != 0) std::abort();
  if (sigaddset (&sigs, SIGSEGV) != 0) std::abort();
  if (sigprocmask (SIG_UNBLOCK, &sigs, nullptr) != 0) std::abort();
}

template <typename CALLABLE>
void expect_signal (CALLABLE code)
{
  // volatile to avoid gcc -Wclobbered warning.
  volatile bool handled = false;
  if (sigsetjmp (jmp, 0)) {
    handled = true;
  }
  else {
    setsig();
    code();
  }
  resetsig();
  assert (handled);
}


// protect
void test6()
{
  std::cout << "test6\n";

  SG::ArenaHeapSTLAllocator<Payload, int> b1;
  Payload* p = b1.allocate (1);
  p->x = 42;
  b1.protect();
  assert (p->x == 42);
  expect_signal ([&]() { p->x = 43; });
  b1.unprotect();
  assert (p->x == 42);
  p->x = 43;
  assert (p->x == 43);

  b1.protect();
  SG::maybeUnprotect (b1);
}


int main()
{
  std::cout << "AthAllocators/ArenaHeapSTLAllocator_test\n";
  test1();
  test2();
  test3();
  test4();
  test5();
  test6();
  return 0;
}

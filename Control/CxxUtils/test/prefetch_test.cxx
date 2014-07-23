/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Test application for the prefetch.
//
//------------------------------------------------------------------------

//---------------
// C++ Headers --
//---------------
#include <iostream>

//-------------------------------
// Collaborating Class Headers --
//-------------------------------


// redefine CXXUTILS_PREFETCH_ADDRESS macro to do useful stuff
#define CXXUTILS_PREFETCH_ADDRESS(ADDR) std::cout << "  prefetch address: " << static_cast<const void*>(ADDR) << '\n'

#include "CxxUtils/prefetch.h"

namespace {

template <unsigned SIZE>
struct Data {
   char x[ SIZE ];
};

template <unsigned SIZE>
void prefetch(int address)
{
  const Data< SIZE >* ptr = reinterpret_cast<const Data< SIZE >*>(address);
  std::cout << "prefetch address: " << static_cast<const void*>(ptr) << " size: " << sizeof(Data<SIZE>) << '\n';
  CxxUtils::prefetchObj(ptr);
}

}

void test1() {
  // never need more than one line
  prefetch<1>(0);
  prefetch<1>(1);
  prefetch<1>(63);
  prefetch<1>(64);
}

void test2() {
  // object size is small but may need >1 line
  prefetch<2>(0);
  prefetch<2>(1);
  prefetch<2>(62);
  prefetch<2>(63);
  prefetch<2>(64);
}

void test3() {
  // object size is whole line size
  prefetch<64>(0);
  prefetch<64>(1);
  prefetch<64>(63);
  prefetch<64>(64);
}

void test4() {
  // object slightly larger than line size
  prefetch<67>(0);
  prefetch<67>(1);
  prefetch<67>(61);
  prefetch<67>(62);
  prefetch<67>(63);
  prefetch<67>(64);
}

void test5() {
  // varying object sizes
  prefetch<32>(10);
  prefetch<64>(10);
  prefetch<128>(10);
  prefetch<256>(10);
}


struct Foo { int x[10]; };

void test6() {
  std::cout << "test6\n";
  Foo* foo[10];
  for (int i=0; i < 10; i++) foo[i] = reinterpret_cast<Foo*> (i*128);

  CxxUtils::prefetchNext (foo+1, foo+10);
  CxxUtils::prefetchNext (foo+9, foo+10);
  CxxUtils::prefetchTwo (foo+8, foo+10);
  CxxUtils::prefetchTwo (foo+9, foo+10);
  CxxUtils::prefetchTwo (foo+10, foo+10);
}

int main()
{
  test1();
  test2();
  test3();
  test4();
  test5();
  test6();
}


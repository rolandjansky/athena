/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CxxUtils/test/hexdump_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2021
 * @brief Unit tests for hexdump
 */


#undef NDEBUG
#include "CxxUtils/hexdump.h"
#include <iostream>
#include <cassert>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>


void test1()
{
  std::cout << "test1\n";

  const int n = 1024;
  char buf[n];
  for (int i=0; i < n; i++) {
    buf[i] = (i & 0xff);
  }

  CxxUtils::hexdump (std::cout, buf, 128, reinterpret_cast<uintptr_t> (buf));
  std::cout << "\n";
  CxxUtils::hexdump (std::cout, &buf[5], 128, reinterpret_cast<uintptr_t> (buf));
  std::cout << "\n";
  CxxUtils::hexdump (std::cout, buf, 99, reinterpret_cast<uintptr_t> (buf));
}


void test2()
{
  std::cout << "test2\n";

  // Granularity of this test.  Must be a multiple of the HW page size.
  const unsigned int BLOCK = 65536;

  unsigned int pagesize = sysconf (_SC_PAGESIZE);
  assert ((BLOCK % pagesize) == 0);

  unsigned int len = 3*BLOCK;

  char* buf = reinterpret_cast<char*> (aligned_alloc (BLOCK, len));

  for (unsigned i=0; i < len; i++) {
    buf[i] = (i % 0x7f);
  }

  CxxUtils::safeHexdump (std::cout, buf+BLOCK-10, 20,
                         reinterpret_cast<uintptr_t> (buf));
  CxxUtils::safeHexdump (std::cout, buf+2*BLOCK-10, 20,
                         reinterpret_cast<uintptr_t> (buf));

  mprotect (buf+BLOCK, BLOCK, PROT_NONE);
  std::cout << "\n";
  CxxUtils::safeHexdump (std::cout, buf+BLOCK-10, BLOCK+20,
                         reinterpret_cast<uintptr_t> (buf));
  mprotect (buf+BLOCK, BLOCK, PROT_WRITE);

  free (buf);
}


int main()
{
  std::cout << "CxxUtils/hexdump_test\n";
  test1();
  test2();
  return 0;
}


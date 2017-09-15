/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file CxxUtils/test/MDt5_test.cxx
 * @author scott snyder
 * @date Aug 2017
 * @brief Regression tests for MD5
 */


#undef NDEBUG
#include "CxxUtils/MD5.h"
#include <iostream>
#include <cassert>


// Test vectors from
//  <https://www.nist.gov/itl/ssd/software-quality-group/nsrl-test-data>
void test1()
{
  std::cout << "test1\n";

  {
    MD5 md5 ((const unsigned char*)"abc", 3);
    assert (md5.hex_digest() == "900150983cd24fb0d6963f7d28e17f72");
    unsigned char buf[16];
    md5.raw_digest (buf);
    const unsigned char exp[16] = {
      0x90, 0x01, 0x50, 0x98, 0x3c, 0xd2, 0x4f, 0xb0,
      0xd6, 0x96, 0x3f, 0x7d, 0x28, 0xe1, 0x7f, 0x72,
    };
    for (int i = 0; i < 16; i++) {
      assert (buf[i] == exp[i]);
    }
  }

  {
    MD5 md5 ((const unsigned char*)"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", 56);
    assert (md5.hex_digest() == "8215ef0796a20bcaaae116d3876c664a");
  }

  {
    std::string s (1000000, 'a');
    MD5 md5 ((const unsigned char*)s.c_str(), s.size());
    assert (md5.hex_digest() == "7707d6ae4e027c70eea2a935c2296f21");
  }
}


int main()
{
  test1();
  return 0;
}

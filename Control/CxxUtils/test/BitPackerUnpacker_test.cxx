/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CxxUtils/test/BitPackerUnpacker_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2014
 * @brief Regression test for BitPacker/BitUnpacker class.
 */

#undef NDEBUG

#include "CxxUtils/checker_macros.h"

#include "CxxUtils/BitPacker.h"
#include "CxxUtils/BitUnpacker.h"
#include "TestTools/random.h"
#include <vector>
#include <cassert>
#include <iostream>



template <class T>
class TestStream
{
public:
  TestStream() : pos(0) {}
  TestStream& operator<< (T x) { v.push_back(x); return *this; }
  TestStream& operator>> (T& x) { x = v[pos++]; return *this; }
  std::vector<T> v;
  size_t pos;
};


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";

  std::vector<uint32_t> data;
  data.push_back (0x1eab8);
  data.push_back (0x3ff8);
  data.push_back (0x15f35);
  data.push_back (0xf34e);
  data.push_back (0xad40);
  data.push_back (0x1d54f);
  data.push_back (0xe2d5);
  data.push_back (0x1221b);
  data.push_back (0x1791d);
  data.push_back (0xe1f6);
  const int n = 10;

  //***********************************************

  TestStream<uint32_t> stream;
  {
    CxxUtils::BitPacker<TestStream<uint32_t> > packer (17, stream);
    for (int i=0; i < n; i++)
      packer.pack (data[i]);
  }

  assert (stream.v.size() == 6);
  assert (stream.v[0] == 0x7ff1eab8);
  assert (stream.v[1] == 0x9a757cd4);
  assert (stream.v[2] == 0xa9ead407);
  assert (stream.v[3] == 0xdb8b57a);
  assert (stream.v[4] == 0xed791d91);
  assert (stream.v[5] == 0x1c3);

  {
    CxxUtils::BitUnpacker<TestStream<uint32_t> > unpacker (17, stream);
    for (int i=0; i < n; i++)
      assert (unpacker.unpack() == data[i]);
  }

  //***********************************************

  TestStream<uint32_t> stream2;
  {
    CxxUtils::BitPacker<TestStream<uint32_t> > packer (16, stream2);
    for (int i=0; i < n; i++)
      packer.pack (data[i] & 0xffff);
  }

  assert (stream2.v.size() == 5);
  assert (stream2.v[0] == 0x3ff8eab8);
  assert (stream2.v[1] == 0xf34e5f35);
  assert (stream2.v[2] == 0xd54fad40);
  assert (stream2.v[3] == 0x221be2d5);
  assert (stream2.v[4] == 0xe1f6791d);

  {
    CxxUtils::BitUnpacker<TestStream<uint32_t> > unpacker (16, stream2);
    for (int i=0; i < n; i++)
      assert (unpacker.unpack() == (data[i] & 0xffff));
  }

  //***********************************************

  TestStream<uint32_t> stream8;
  {
    CxxUtils::BitPacker8<TestStream<uint32_t> > packer (stream8);
    for (int i=0; i < n; i++)
      packer.pack (data[i] & 0xff);
  }

  assert (stream8.v.size() == 3);
  assert (stream8.v[0] == 0x4e35f8b8);
  assert (stream8.v[1] == 0x1bd54f40);
  assert (stream8.v[2] == 0xf61d);

  {
    CxxUtils::BitUnpacker8<TestStream<uint32_t> > unpacker (stream8);
    for (int i=0; i < n; i++)
      assert (unpacker.unpack() == (data[i] & 0xff));
  }

  //***********************************************

  TestStream<uint32_t> stream16;
  {
    CxxUtils::BitPacker16<TestStream<uint32_t> > packer (stream16);
    for (int i=0; i < n; i++)
      packer.pack (data[i] & 0xffff);
  }

  assert (stream16.v.size() == 5);
  assert (stream16.v[0] == 0x3ff8eab8);
  assert (stream16.v[1] == 0xf34e5f35);
  assert (stream16.v[2] == 0xd54fad40);
  assert (stream16.v[3] == 0x221be2d5);
  assert (stream16.v[4] == 0xe1f6791d);

  {
    CxxUtils::BitUnpacker16<TestStream<uint32_t> > unpacker (stream16);
    for (int i=0; i < n; i++)
      assert (unpacker.unpack() == (data[i] & 0xffff));
  }
}


template <template<class> class PACKER, template<class> class UNPACKER, class STREAM>
void testit2 ATLAS_NOT_THREAD_SAFE (int nbits)
{
  const int n = 1000;
  STREAM stream;
  std::vector<uint32_t> data;
  for (int i=0; i < n; i++)
    data.push_back (static_cast<uint32_t> (Athena_test::rng()));

  uint32_t mask;
  if (nbits == 32)
    mask = ~0U;
  else
    mask = (1U << nbits) - 1;

  {
    PACKER<STREAM> packer (nbits, stream);
    for (int i=0; i < n; i++)
      packer.pack (data[i] & mask);
  }

  {
    UNPACKER<STREAM> unpacker (nbits, stream);
    for (int i=0; i < n; i++) {
      uint32_t val = unpacker.unpack();
      assert (val == (data[i]&mask));
    }
  }
}


void test2 ATLAS_NOT_THREAD_SAFE ()
{
  using CxxUtils::BitPacker;
  using CxxUtils::BitPacker8;
  using CxxUtils::BitPacker16;
  using CxxUtils::BitUnpacker;
  using CxxUtils::BitUnpacker8;
  using CxxUtils::BitUnpacker16;
  for (int nbits=1; nbits <= 32; ++nbits) {
    testit2<BitPacker, BitUnpacker, TestStream<uint32_t> > (nbits);
  }
  testit2<BitPacker8,  BitUnpacker8,  TestStream<uint32_t> >  (8);
  testit2<BitPacker16, BitUnpacker16, TestStream<uint32_t> > (16);

  std::cout << "test2\n";
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  test2();
  return 0;
}

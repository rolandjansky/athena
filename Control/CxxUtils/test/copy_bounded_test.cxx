/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CxxUtils/test/copy_bounded_test.cxx
 * @author sss
 * @date March 2013
 * @brief Regression tests for copy_bounded
 */


#undef NDEBUG

// Disable this test entirely for the xAOD standalone builds:
#ifndef XAOD_STANDALONE

#include "CxxUtils/copy_bounded.h"
#include "boost/range/iterator_range.hpp"
#include "boost/range/algorithm/fill.hpp"
#include <vector>
#include <list>
#include <cassert>
#include <iostream>


struct arange
{
  static const int N = 10;
  int x[N];
  typedef int* iterator;
  typedef const int* const_iterator;
  typedef int value_type;
  iterator begin() { return x; }
  iterator end() { return x + N; }
  const_iterator begin() const { return x; }
  const_iterator end() const { return x + N; }

  arange(int) :x() {}
};



template <class InputRange, class OutputRange>
void test1b (InputRange& input, OutputRange& output)
{
  typedef typename boost::range_iterator<InputRange>::type  InputIterator;
  typedef typename boost::range_iterator<OutputRange>::type OutputIterator;
  InputIterator begi = boost::begin(input);
  InputIterator endi = boost::end(input);
  OutputIterator bego = boost::begin(output);
  OutputIterator endo = boost::end(output);

  typedef typename std::iterator_traits<InputIterator>::value_type value_type;

  InputIterator midi = begi;
  std::advance (midi, 5);

  OutputIterator mido = bego;
  std::advance (mido, 5);

  boost::iterator_range<InputIterator> rangei (begi, midi);
  boost::iterator_range<OutputIterator> rangeo (bego, mido);

  int i = 0;
  for (value_type& it : input)
    it = i++;

  boost::range::fill (output, 0);
  CxxUtils::copy_bounded (begi, endi, bego, mido);
  i = 0;
  for (value_type& it : output)
    assert (it == (i < 5 ? i++ : 0));

  boost::range::fill (output, 0);
  CxxUtils::copy_bounded (input, rangeo);
  i = 0;
  for (value_type& it : output)
    assert (it == (i < 5 ? i++ : 0));

  boost::range::fill (output, 0);
  CxxUtils::copy_bounded (begi, midi, bego, endo);
  i = 0;
  for (value_type& it : output)
    assert (it == (i < 5 ? i++ : 0));

  boost::range::fill (output, 0);
  CxxUtils::copy_bounded (rangei, output);
  i = 0;
  for (value_type& it : output)
    assert (it == (i < 5 ? i++ : 0));

  boost::range::fill (output, 0);
  CxxUtils::copy_bounded (begi, endi, bego, endo);
  i = 0;
  for (value_type& it : output)
    assert (it == i++);

  boost::range::fill (output, 0);
  CxxUtils::copy_bounded (input, output);
  i = 0;
  for (value_type& it : output)
    assert (it == i++);
}

template <class Cont1, class Cont2=Cont1>
struct test1a
{
  static void test()
  {
    Cont1 c1 (10);
    Cont2 c2 (10);
    test1b (c1, c2);
  }
};


void test1()
{
  std::cout << "test1\n";
  test1a<std::vector<int> >::test();
  test1a<std::list<int> >::test();
  test1a<std::vector<int>, std::list<int> >::test();
  test1a<std::list<int>,   std::vector<int> >::test();

  test1a<arange>::test();
  test1a<arange, std::list<int> >::test();
  test1a<arange, std::vector<int> >::test();
  test1a<std::list<int>, arange>::test();
  test1a<std::vector<int>, arange>::test();
}

#else

void test1() {}

#endif // not XAOD_STANDALONE

int main()
{
  test1();
  return 0;
}



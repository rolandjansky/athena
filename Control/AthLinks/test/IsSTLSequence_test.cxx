/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  AthLinks/test/IsSTLSequence_test.cxx
 * @author scott snyder
 * @date Aug, 2013
 * @brief Regression test for @c IsSTLSequence.
 */


#undef NDEBUG


#include "AthLinks/tools/IsSTLSequence.h"
#include <type_traits>
#include <set>
#include <cassert>
#include <iostream>


class V1 : public std::vector<float> {};
class L1 : public std::list<float> {};
class Q1 : public std::deque<float> {};

struct X1
{
  typedef int value_type;
  typedef size_t size_type;
  typedef int difference_type;
  typedef const int* const_iterator;
  typedef const int* const_pointer;
  typedef const int& const_reference;
  
  size_t size() const;
  size_t max_size() const;
  bool empty() const;
  const_iterator begin() const;
  const_iterator end() const;
};
struct Y1
{
  typedef int value_type;
  typedef size_t size_type;
  typedef int difference_type;
  typedef const int* const_iterator;
  typedef const int* const_pointer;
  typedef const int& const_reference;
  
  size_t size() const;
  size_t max_size() const;
  bool empty() const;
  const_iterator begin() const;
  const_iterator end() const;

  typedef std::true_type isSequence;
};


void test1()
{
  std::cout << "test1\n";
  assert (SG::IsSTLSequence<std::vector<int> >::value);
  assert (SG::IsSTLSequence<V1>::value);
  assert (SG::IsSTLSequence<std::list<int> >::value);
  assert (SG::IsSTLSequence<L1>::value);
  assert (SG::IsSTLSequence<std::deque<int> >::value);
  assert (SG::IsSTLSequence<Q1>::value);
  assert (SG::IsSTLSequence<Y1>::value);

  assert ( ! SG::IsSTLSequence<std::set<int> >::value );
  assert ( ! SG::IsSTLSequence<X1>::value);
}


int main()
{
  test1();
  return 0;
}

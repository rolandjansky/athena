/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <vector>
#include "TrigCompositeUtils/KFromNItr.h"
#include <algorithm>
#include <iterator>
#include "TestTools/expect.h"

using namespace TrigCompositeUtils;
/// Helper functions in anonymous namespace
namespace
{
  template <class T>
  class printable_vector
    : public std::vector<T>
  {
  public:
    using std::vector<T>::vector;
    printable_vector (const std::vector<T>& v) : std::vector<T>(v) {}
  };

  printable_vector<printable_vector<std::size_t>> allCombos(KFromNItr itr)
  {
    printable_vector<printable_vector<std::size_t>> ret;
    std::copy(itr, KFromNItr(), std::back_inserter(ret));
    return ret;
  }

  /// Allow printing
  template <typename T>
  std::ostream &operator<<(std::ostream &os, const printable_vector<T> &v)
  {
    os << "[";
    if (v.size() > 0)
    {
      for (auto itr = v.begin(); itr != v.end() - 1; ++itr)
        os << *itr << ", ";
      os << v.back();
    }
    return os << "]";
  }
} // namespace

/// Make sure picking one gives you the obvious
void pickOne()
{
  std::cout << "Pick 1 from 6" << std::endl;
  KFromNItr itr(1, 6);
  printable_vector<printable_vector<std::size_t>> all = allCombos(itr);
  VALUE(all) EXPECTED({{0}, {1}, {2}, {3}, {4}, {5}});
}

/// Make sure that an empty iterator is always exhausted
void emptyItr()
{
  std::cout << "Empty iterator" << std::endl;
  KFromNItr itr(0, 3);
  VALUE( itr.exhausted() ) EXPECTED( true );
}

/// Check results are correct
void pickMany()
{
  std::cout << "Pick 3 from 5" << std::endl;
  printable_vector<printable_vector<std::size_t>> expected{
    {0, 1, 2},
    {0, 1, 3},
    {0, 1, 4},
    {0, 2, 3},
    {0, 2, 4},
    {0, 3, 4},
    {1, 2, 3},
    {1, 2, 4},
    {1, 3, 4},
    {2, 3, 4}
  };
  VALUE( allCombos(KFromNItr(3, 5)) ) EXPECTED (expected);
}

/// Check that reset works correctly
void checkReset()
{
  std::cout << "Check resetting" << std::endl;
  KFromNItr itr(3, 5);
  // Purposefully exhaust it
  while(!itr.exhausted())
    ++itr;
  VALUE( allCombos(itr) ) EXPECTED( {} );
  itr.reset();
  VALUE( itr == KFromNItr(3, 5) ) EXPECTED( true );
}

int main()
{
  pickOne();
  emptyItr();
  pickMany();
  checkReset();
  return 0;
}

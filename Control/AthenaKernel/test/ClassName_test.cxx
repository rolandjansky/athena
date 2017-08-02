/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cassert>
#include "AthenaKernel/ClassName.h"

class A {};
class B {};
template <class T> class C {};

template <>
class ClassName<B>
{
public:
  static std::string name() { return "fooB"; }
};

int main()
{
  assert (ClassName<A>::name() == "A");
  assert (ClassName<C<C<A> > >::name() == "C<C<A> >");
  assert (ClassName<B>::name() == "fooB");
  return 0;
}

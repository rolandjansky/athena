/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG

#include "AthenaKernel/tools/safe_clid.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "GaudiKernel/DataObject.h"
#include <cassert>

#include <iostream>

struct A {};
struct B {};
CLASS_DEF (B, 98765, 0)
struct C
  : public DataObject
{
  static const CLID& classID() { static const CLID clid =  44444; return clid; }
};

int main()
{
  std::cout << "AthenaKernel/safe_clid_test\n";
  assert (SG::safe_clid<int> () == 0);
  assert (SG::safe_clid<A> () == 0);
  assert (SG::safe_clid<B> () == 98765);
  assert (SG::safe_clid<C> () == 44444);
  return 0;
}


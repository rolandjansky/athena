/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaBase_test.cxx 470529 2011-11-24 23:54:22Z ssnyder $
/**
 * @file AthAllocators/test/ArenaBase_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2007
 * @brief Regression tests for ArenaBase.
 */

#undef NDEBUG
#include "AthAllocators/ArenaBase.h"
#include <ostream>
#include <sstream>
#include <cassert>


std::string xname = "bar";

class TestArena
  : public SG::ArenaBase
{
public:
  virtual void report (std::ostream& os) const { os << "foo"; }
  virtual const std::string& name() const { return xname; }
};


void test1()
{
  TestArena t;
  std::ostringstream s;
  t.report (s);
  assert (s.str() == "foo");
  assert (t.name() == "bar");
}


int main()
{
  test1();
  return 0;
}


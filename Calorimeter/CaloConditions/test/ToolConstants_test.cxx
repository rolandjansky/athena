/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ToolConstants_test.cxx,v 1.4 2009-04-09 14:41:17 ssnyder Exp $
/**
 * @file  ToolConstants_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date June, 2004
 * @brief Regression tests for the ToolConstants class.
 */

#undef NDEBUG

#include "CaloConditions/ToolConstants.h"
#include "GaudiKernel/GaudiException.h"
#include <stdexcept>
#include <cassert>
#include <iostream>


using namespace CaloRec;
using namespace CxxUtils;


#define EXPECT_EXCEPTION(s) \
  { \
    bool caught = false; \
    try { \
      Arrayrep xx (s); \
    } catch (std::runtime_error& c) {                        \
      caught = true; \
    } catch (GaudiException& c) {                            \
      caught = true; \
      c.code().ignore(); \
    } \
    assert (caught); \
  }


void test1()
{
  ToolConstants tc;
  Array<0> a1 (tc.newrep ("foo", "a", "1.25"));
  assert (a1 == 1.25);

  assert (!tc.hasrep ("xxx"));
  assert (tc.hasrep ("a"));

  assert (tc.map().size() == 1);
  assert (tc.map().begin()->first == "a");

  Array<2> a2 (tc.newrep ("foo", "b", "[[1,2],[3,4]]"));
  assert (a2.size() == 2);
  assert (a2[0][1] == 2);

  Array<0> a3 (tc.getrep ("foo", "a"));
  assert (a3 == 1.25);

  EXPECT_EXCEPTION( tc.newrep ("foo", "a", "2.0") );
  EXPECT_EXCEPTION( tc.newrep ("foo", "z", "asdasd") );
  EXPECT_EXCEPTION( tc.getrep ("foo", "x") );

  tc.setrep ("fee", tc.getrep ("foo", "a"));
  Array<0> a4 (tc.getrep ("foo", "fee"));
  assert (a4 == 1.25);

  assert (tc.clsname() == "");
  tc.clsname ("abc");
  assert (tc.clsname() == "abc");

  assert (tc.version() == 0);
  tc.version (10);
  assert (tc.version() == 10);
}


int main()
{
  test1();
  return 0;
}

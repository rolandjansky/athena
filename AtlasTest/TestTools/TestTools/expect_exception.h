/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TestTools/expect_exception.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2014
 * @brief Helper to check that an exception is thrown.
 */


#ifndef TESTTOOLS_EXPECT_EXCEPTION_H
#define TESTTOOLS_EXPECT_EXCEPTION_H


#include <cassert>


/**
 * @brief Helper to check that an exception is thrown.
 *
 * Use like this:
 *
 *@code
 *  EXPECT_EXCEPTION (std::runtime_error, doSomething());
 @endcode
 *
 * This will produce an assertion failure if @c doSomething()
 * does _not_ throw a @c std::runtime_error exception.
 */
#define EXPECT_EXCEPTION(EXC, CODE) do { \
  bool caught = false;                   \
  try {                                  \
    CODE;                                \
  }                                      \
  catch (const EXC&) {                   \
    caught = true;                       \
  }                                      \
  assert (caught);                       \
} while(0)


#endif // not TESTTOOLS_EXPECT_EXCEPTION_H

// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/tools/ATHCONTAINERS_ASSERT.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2010
 * @brief An overridable variant of assert.
 *
 * For purposes of unit tests, it's sometimes useful to change the
 * behavior of assert, so that, for example, it throws an exception
 * rather than aborting.
 *
 * It turns out that it's not feasible to override what the standard
 * assert macro does (because assert.h does not use an include guard).
 * So we instead introduce our own assert macro.  Normally, this is the
 * same as assert, but unit test programs may redefine it.
 */


#ifndef ATHCONTAINERS_TOOLS_ATHCONTAINERS_ASSERT_H
#define ATHCONTAINERS_TOOLS_ATHCONTAINERS_ASSERT_H


#ifndef ATHCONTAINERS_ASSERT
# include <cassert>
# define ATHCONTAINERS_ASSERT(X) assert(X)
#endif


#endif // not ATHCONTAINERS_TOOLS_ATHCONTAINERS_ASSERT_H

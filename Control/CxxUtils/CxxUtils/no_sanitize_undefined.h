// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CxxUtils/no_sanitize_undefined.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2017
 * @brief Helper to disable undefined behavior sanitizer for a function.
 *
 * To disable the undefined behavior sanitizers for a given function,
 * write in the function's declaration:
 *
 *   void f NO_SANITIZE_UNDEFINED (int x)
 */


#ifndef CXXUTILS_NO_SANITIZE_UNDEFINED_H
#define CXXUTILS_NO_SANITIZE_UNDEFINED_H


#if (__GNUC__ >= 6) && !defined(__clang__)
# define NO_SANITIZE_UNDEFINED [[gnu::no_sanitize_undefined]]
#else
# define NO_SANITIZE_UNDEFINED
#endif


#endif // not CXXUTILS_NO_SANITIZE_UNDEFINED_H

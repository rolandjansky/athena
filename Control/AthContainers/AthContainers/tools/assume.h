// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/tools/assume.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2013
 * @brief Macro to provide hints for optimization.
 *
 * ATHCONTAINERS_ASSUME(x) informs the compiler that it can assume
 * that condition x is true.  This can be used to let the compiler
 * know about conditions that are guaranteed to hold but that it
 * cannot deduce itself, such as postconditions for out-of-line
 * function calls.
 */


#ifndef ATHCONTAINERS_ASSUME_H
#define ATHCONTAINERS_ASSUME_H


#if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5))
# define ATHCONTAINERS_ASSUME(x) do { if (!(x)) __builtin_unreachable(); } while(0)
#else
# define ATHCONTAINERS_ASSUME(x) do {  } while(0)
#endif


#endif // not ATHCONTAINERS_ASSUME_H

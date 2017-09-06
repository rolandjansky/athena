// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CxxUtils/exctrace.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2009
 * @brief Generate stack trace backs from a caught exception.
 *
 * When an exception is caught, we'd sometimes want to dump out the
 * stack at the point where the exception was thrown.
 *
 * If you run with libexctrace_collector.so added to your LD_PRELOAD,
 * then we'll remember the backtrace from the last exception thrown.
 * You can then call CxxUtils::exctrace to print it out.
 * This will behave sensibly if the collector module hasn't been
 * preloaded (just print the exception, with no backtrace).
 */


#ifndef CXXUTILS_EXCTRACE_H
#define CXXUTILS_EXCTRACE_H


#include "CxxUtils/SealCommon.h"
#include "CxxUtils/SealDebug.h"
#include "CxxUtils/checker_macros.h"
#include <exception>


namespace CxxUtils {


/**
 * @brief Print out information for the last exception.
 *
 * Prints the supplied exception, plus the backtrace from
 * the last exception, if available.
 *
 * @param e The exception to print.
 * @param fd The file descriptor to which to write.
 */
void exctrace ATLAS_NOT_THREAD_SAFE (const std::exception& e, IOFD fd = IOFD_INVALID);


} // namespace CxxUtils


#endif // not CXXUTILS_EXCTRACE_H

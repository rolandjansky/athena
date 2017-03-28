// This file's extension implies that it's C, but it's really -*- C++ -*-.
// $Id$
/**
 * @file CxxUtils/ubsan_suppress.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2017
 * @brief Helper for suppressing ubsan warnings.
 *
 * With gcc6, not all ubsan warnings can be suppressed via the suppression
 * file (for example, type mismatch errors where the object has an invalid
 * vptr).
 *
 * However, ubsan will report warnings only once for any given source location.
 * So we can effectively suppress warnings by trigging the warning
 * with stderr pointing to /dev/null.  You can do that by calling the
 * @c ubsan_suppress function, passing it a function to call with
 * stderr redirected to /dev/null.
 */


#ifndef CXXUTILS_UBSAN_SUPPRESS_H
#define CXXUTILS_UBSAN_SUPPRESS_H


namespace CxxUtils {


/**
 * @brief Helper for suppressing ubsan warnings.
 * @param func Function to call (may be a lambda).
 *
 * If ubsan is running, temporarily redirect stderr to /dev/null.
 * Then call @c func.
 *
 * For example, we sometimes get a bogus ubsan warning from
 * cling initialization.  This can be suppressed by adding something like:
 *
 *@code
 *  CxxUtils::ubsan_suppress ([]() { TInterpreter::Instance(); });
 @endcode
 */
void ubsan_suppress (void (*func)());


}


#endif // not CXXUTILS_UBSAN_SUPPRESS_H

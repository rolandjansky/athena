// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CxxUtils/checker_macros.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2017
 * @brief Define macros for attributes used to control the static checker.
 *
 * This file defines macros for the attributes used to control
 * the gcc static checker plugins.  They are enabled only if 
 * ATLAS_GCC_CHECKERS is defined, indicating that the checkers are enabled.
 *
 * For functions and variables, the proper placement of the macros
 * are just after the name, for example:
 *
 *@code
 *   void f1 ATLAS_NOT_REENTRANT () { ... }
 *   static int x ATLAS_THREAD_SAFE;
 @endcode
 *
 * Multiple attributes may be given:
 *
 *@code
 *   void f1 ATLAS_CHECK_THREAD_SAFETY ATLAS_NOT_REENTRANT () { ... }
 @endcode
 *
 * For functions, the declaration and definition should have the same
 * set of attributes.
 *
 * For classes, the macros should be placed before the class name:
 *
 *@code
 *   class ATLAS_CHECK_THREAD_SAFETY C  { ... }
 @endcode
 *
 * Implementation note: the attributes are currently all in the `gnu'
 * attribute namespace due to a gcc limitation.  This has been fixed
 * in gcc7, so eventually the attributes should move into an atlas-specific
 * namespace.  Thus, you should always use the macros defined here; don't
 * write the attributes directly.
 */


#ifndef CXXUTILS_CHECKER_MACROS_H
#define CXXUTILS_CHECKER_MACROS_H


#ifdef ATLAS_GCC_CHECKERS


/**
 * @brief Include this at the start of a file to enable thread-safety
 *        checking for everything in a file:
 *
 *@code
 *   ATLAS_CHECK_FILE_THREAD_SAFETY;
 @endcode
*/
#define ATLAS_CHECK_FILE_THREAD_SAFETY _Pragma ("ATLAS check_thread_safety") \
        class ATLAS_CHECK_FILE_THREAD_SAFETY_SWALLOW_SEMICOLON


/**
 * @brief Include this at the start of a file to disable thread-safety
 *        checking for everything in a file:
 *
 *@code
 *   ATLAS_NO_CHECK_FILE_THREAD_SAFETY;
 @endcode
 *
 * This can be used to disable checking for specific files when checking
 * has been enabled on a per-package basis.
 */
#define ATLAS_NO_CHECK_FILE_THREAD_SAFETY _Pragma ("ATLAS no_check_thread_safety") \
        class ATLAS_CHECK_FILE_THREAD_SAFETY_SWALLOW_SEMICOLON
  

/**
 * @brief Request thread-safety checking of a function/class.
 *
 *@code
 *   void f1 ATLAS_CHECK_THREAD_SAFETY () { ... }
 *   class ATLAS_CHECK_THREAD_SAFETY C  { ... }
 @endcode
 */
#define ATLAS_CHECK_THREAD_SAFETY [[gnu::check_thread_safety]]


/**
 * @brief Mark that a static/mutable variable is ok, or that discarding
 *        const on assignment is ok.
 *
 * Usage:
 *
 * To suppress warnings about the use of a static variable:
 *@code
 *   static int x ATLAS_THREAD_SAFE;
 @endcode
 *
 * To suppress warnings about a mutable class member:
 *@code
 *   mutable int x ATLAS_THREAD_SAFE;
 @endcode
 *
 * To suppress warnings about discarding const in assignment:
 *@code
 *   const int * y;
 *   int* yy ATLAS_THREAD_SAFE = const_cast<int*> (y);
 @endcode
 */
#define ATLAS_THREAD_SAFE [[gnu::thread_safe]]


/**
 * @brief Mark that a function is not thread safe.
 *
 * Usage:
 *
 * Add to a function to suppress warnings about uses of static variables,
 * mutable variables, or discarding const.
 *
 * Add to a class to so mark all functions in the class.
 *
 * A function calling an ATLAS_NOT_THREAD_SAFE function must also be marked
 * ATLAS_NOT_THREAD_SAFE.
 */
#define ATLAS_NOT_THREAD_SAFE [[gnu::not_thread_safe]]


/**
 * @brief Mark that a function uses static data in a non-thread-safe manner.
 *
 * Usage:
 *
 * Add to a function to suppress warnings about uses of static variables.
 *
 * A function calling an ATLAS_NOT_REENTRANT function must also be marked
 * ATLAS_NOT_REENTRANT.
 *
 * A function passing a const static object to a function declared
 * ATLAS_ARGUMENT_NOT_CONST_THREAD_SAFE should be declared ATLAS_NOT_REENTRANT.
 */
#define ATLAS_NOT_REENTRANT [[gnu::not_reentrant]]


/**
 * @brief Mark that a function discards const
 *        (from something other than an argument).
 *
 * Usage:
 *
 * Add to a function to suppress warnings about discarding const
 * from something that is not an argument.
 *
 * An ATLAS_NOT_CONST_THREAD_SAFE function should not access a const static
 * object (unless also marked ATLAS_NOT_THREAD_SAFE or ATLAS_NOT_REENTRANT).
 *
 * A const member function calling an ATLAS_NOT_CONST_THREAD_SAFE member
 * function on the same object must also be marked ATLAS_NOT_CONST_THREAD_SAFE.
 */
#define ATLAS_NOT_CONST_THREAD_SAFE [[gnu::not_const_thread_safe]]


/**
 * @brief Mark that a function discards const from an argument.
 *
 * Usage:
 *
 * Add to a function to suppress warnings about discarding const
 * from an argument.
 *
 * A const static object should not be passed to be function marked
 * as ATLAS_ARGUMENT_NOT_CONST_THREAD_SAFE.
 *
 * A function passing one of its arguments to an
 * ATLAS_ARGUMENT_NOT_CONST_THREAD_SAFE function must also be marked
 * ATLAS_ARGUMENT_NOT_CONST_THREAD_SAFE.
 *
 * A const member function function passing member data to an
 * ATLAS_ARGUMENT_NOT_CONST_THREAD_SAFE function must also be marked
 * ATLAS_ARGUMENT_NOT_CONST_THREAD_SAFE.
 */
#define ATLAS_ARGUMENT_NOT_CONST_THREAD_SAFE [[gnu::argument_not_const_thread_safe]]


#else // not ATLAS_GCC_CHECKERS


#define ATLAS_CHECK_FILE_THREAD_SAFETY class ATLAS_CHECK_FILE_THREAD_SAFETY_SWALLOW_SEMICOLON
#define ATLAS_NO_CHECK_FILE_THREAD_SAFETY class ATLAS_CHECK_FILE_THREAD_SAFETY_SWALLOW_SEMICOLON
#define ATLAS_CHECK_THREAD_SAFETY
#define ATLAS_THREAD_SAFE
#define ATLAS_NOT_THREAD_SAFE
#define ATLAS_NOT_REENTRANT
#define ATLAS_NOT_CONST_THREAD_SAFE
#define ATLAS_ARGUMENT_NOT_CONST_THREAD_SAFE


#endif // not ATLAS_GCC_CHECKERS


#endif // not CXXUTILS_CHECKER_MACROS_H

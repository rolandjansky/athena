// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file RootUtils/WithRootErrorHandler.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2021
 * @brief Run a MT piece of code with an alternate root error handler.
 */


#include "TError.h"
#include <functional>
#include <type_traits>


#ifndef ROOTUTILS_WITHROOTERRORHANDLER_H
#define ROOTUTILS_WITHROOTERRORHANDLER_H

namespace RootUtils {


/**
 * @brief Run a MT piece of code with an alternate root error handler.
 *
 * Sometimes we want to run a piece of code with an alternate root
 * error handler.  This is however problematic in MT jobs since the
 * root error handler pointer is just a global variable visible
 * in all threads.  We also frequently need to introduce additional
 * globals if we need to communicate with the handler.
 *
 * This piece of code tries to get around these limitations.
 * It installs a single root error handler during initialization.
 * This handler will then dispatch to other handlers stored
 * in thread-local storage.  This effectively makes changing
 * the root error handler thread-local.  We also manage the handler
 * as a std::function, allowing it to have associated state,
 * and clean up the change using RAII.
 *
 * To change the root error handler, create an instance of this class,
 * passing it a function object, which should have signature:
 *
 *@code
 *  bool hand (int level, bool abort, const char* location, const char* msg)
 @endcode
 *
 * The arguments are the same as for standard root error handlers.
 * The handler should return true if previous handlers should also be called.
 * If a handler returns false, no further handlers will be called.
 *
 * The change in the error handler is visible only in the current thread,
 * and the revious handler will be restored when the object is deleted.
 */
class WithRootErrorHandler
{
public:
  // A ROOT error handler has signature
  //  void hand (int level, Bool_t abort, const char* location, const char* msg)

  /// Type of handler to register.  Like a ROOT error handler, except that
  /// it returns a bool.
  using Handler_t = std::function<bool (int, Bool_t, const char*, const char*)>;
  

  /**
   * @brief Temporarily install a thread-local root error handler.
   * @param errhand The handler to be installed.
   *
   * The installed handler will only run in the current thread, and it will
   * be removed when this object is deleted.  In addition to the usual
   * arguments for a root error handler, it returns a bool.  If the returned
   * value is true, previous handlers will also be executed; otherwise,
   * no further handlers will be executed.
   */
  WithRootErrorHandler (Handler_t errhand);


  /**
   * @brief Destructor.
   *
   * Remove the error handler.
   */
  ~WithRootErrorHandler();


  // Try to prevent misuse.
  WithRootErrorHandler (const WithRootErrorHandler&) = delete;
  WithRootErrorHandler (WithRootErrorHandler&&) = delete;
  WithRootErrorHandler& operator= (const WithRootErrorHandler&) = delete;
  WithRootErrorHandler& operator= (WithRootErrorHandler&&) = delete;

  static void* operator new   (size_t) = delete;
  static void* operator new[] (size_t) = delete;
  static void  operator delete   (void*) = delete;
  static void  operator delete[] (void*) = delete;


private:
  /// For error checking.
  size_t m_size;
};


} // namespace RootUtils

#endif // not ROOTUTILS_WITHROOTERRORHANDLER_H

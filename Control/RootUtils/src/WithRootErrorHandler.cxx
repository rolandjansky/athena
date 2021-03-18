/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file RootUtils/src/WithRootErrorHandler.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2021
 * @brief Run a MT piece of code with an alternate root error handler.
 */


#include "RootUtils/WithRootErrorHandler.h"
#include <vector>


namespace {


/// Thread-local list of registered error handlers, from oldest to newest.
using Handler_t = RootUtils::WithRootErrorHandler::Handler_t;
thread_local std::vector<Handler_t> rootErrorHandlers;


/// Declare our own error handler to root during initialization
/// and save the previous handler.
void errorHandler (int level,
                   Bool_t abort,
                   const char* location,
                   const char* msg);
const ErrorHandlerFunc_t origHandler = ::SetErrorHandler (errorHandler);


/**
 * @brief Global root error handler.
 */
void errorHandler (int level,
                   Bool_t abort,
                   const char* location,
                   const char* msg)
{
  // Execute all the handlers in our thread-local list from newest to oldest.
  // Stop if one returns false.
  for (int i = rootErrorHandlers.size()-1; i >= 0; --i) {
    if (!rootErrorHandlers[i] (level, abort, location, msg)) return;
  }
  // They all returned true.  Call the previous handler.
  origHandler (level, abort, location, msg);
}


} // anonymous namespace


namespace RootUtils {


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
WithRootErrorHandler::WithRootErrorHandler (Handler_t errhand)
  : m_size (rootErrorHandlers.size()+1)
{
  rootErrorHandlers.push_back (errhand);
}


/**
 * @brief Destructor.
 *
 * Remove the error handler.
 */
WithRootErrorHandler::~WithRootErrorHandler()
{
  if (m_size != rootErrorHandlers.size()) std::abort();
  rootErrorHandlers.pop_back();
}


} // namespace RootUtils

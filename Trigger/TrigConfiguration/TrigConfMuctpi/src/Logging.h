// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Logging.h 360889 2011-04-21 15:06:59Z krasznaa $
#ifndef TRIGCONFMUCTPI_LOGGING_H
#define TRIGCONFMUCTPI_LOGGING_H

// STL include(s):
#include <iostream>

// This is a GCC extension for getting the name of the current function.
#if defined( __GNUC__ )
#   define MSGLOGGER_FNAME __PRETTY_FUNCTION__
#else
#   define MSGLOGGER_FNAME ""
#endif

/// Prefix specifying the source of the message
#define MSGLOGGER_REPORT_PREFIX \
   __FILE__ << ":" << __LINE__ << " (" << MSGLOGGER_FNAME << "): "

/// Convenience macro for reporting INFO messages in the code
/**
 * This macro is very similar to the REPORT_MESSAGE macros of Athena. It prints
 * a nicely formatted output that specifies both the exact function name where
 * the message was printed, and also the filename:line combination. It can be used
 * like a regular function inside cycles:
 *
 * <code>
 *   REPORT_INFO( "MyClass", "An information message" );
 * </code>
 */
#define REPORT_INFO( SOURCE, MESSAGE )                         \
   std::cout << SOURCE << "  INFO   " << MSGLOGGER_REPORT_PREFIX \
             << MESSAGE << std::endl

/// Convenience macro for reporting WARNING messages in the code
/**
 * This macro is very similar to the REPORT_MESSAGE macros of Athena. It prints
 * a nicely formatted output that specifies both the exact function name where
 * the message was printed, and also the filename:line combination. It can be used
 * like a regular function inside cycles:
 *
 * <code>
 *   REPORT_WARNING( "MyClass", "A serious error message" );
 * </code>
 */
#define REPORT_WARNING( SOURCE, MESSAGE )                        \
   std::cout << SOURCE << " WARNING " << MSGLOGGER_REPORT_PREFIX \
             << MESSAGE << std::endl

/// Convenience macro for reporting ERROR messages in the code
/**
 * This macro is very similar to the REPORT_MESSAGE macros of Athena. It prints
 * a nicely formatted output that specifies both the exact function name where
 * the message was printed, and also the filename:line combination. It can be used
 * like a regular function inside cycles:
 *
 * <code>
 *   REPORT_ERROR( "MyClass", "A serious error message" );
 * </code>
 */
#define REPORT_ERROR( SOURCE, MESSAGE )                        \
   std::cout << SOURCE << "  ERROR  " << MSGLOGGER_REPORT_PREFIX \
             << MESSAGE << std::endl

/// Convenience macro for reporting FATAL messages in the code
/**
 * This macro is very similar to the REPORT_MESSAGE macros of Athena. It prints
 * a nicely formatted output that specifies both the exact function name where
 * the message was printed, and also the filename:line combination. It can be used
 * like a regular function inside cycles:
 *
 * <code>
 *   REPORT_FATAL( "MyClass", "A *very* serious error message" );
 * </code>
 */
#define REPORT_FATAL( SOURCE, MESSAGE )                        \
   std::cout << SOURCE << "  FATAL  " << MSGLOGGER_REPORT_PREFIX \
             << MESSAGE << std::endl

#endif // TRIGCONFMUCTPI_LOGGING_H

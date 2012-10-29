// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LogWriter.h 457114 2011-09-05 09:35:49Z krasznaa $
#ifndef TRIGBUNCHCROSSINGTOOL_LOGWRITER_H
#define TRIGBUNCHCROSSINGTOOL_LOGWRITER_H

// STL include(s):
#include <string>

// Local include(s):
#include "MsgLogger.h"

namespace Trig {

   /**
    *  @short Base class for all the log writing classes
    *
    *         To make it simple to write nice log messages in any of the classes,
    *         whenever a class wants to output log messages, it has to inherit from
    *         this lightweight class.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 457114 $
    * $Date: 2011-09-05 11:35:49 +0200 (Mon, 05 Sep 2011) $
    */
   class LogWriter {

   public:
      /// Constructor with a class name
      LogWriter( const std::string& name = "UnknownClass" );

      /// Function giving direct access to the log object
      MsgLogger& log() const;

   private:
      mutable MsgLogger m_logger; ///< Internal message logging object

   }; // class LogWriter

} // namespace Trig

// This is a GCC extension for getting the name of the current function.
#if defined( __GNUC__ )
#   define MSGLOGGER_FNAME __PRETTY_FUNCTION__
#else
#   define MSGLOGGER_FNAME ""
#endif

/// Common prefix for the non-usual messages
/**
 * The idea is that a regular user usually only wants to see DEBUG, INFO
 * and some WARNING messages. So those should be reasonably short. On the other
 * hand serious warnings (ERROR, FATAL) or VERBOSE messages should be as precise
 * as possible.
 *
 * So I stole the idea from Athena (what a surprise...) to have a few macros which
 * produce messages with a common formatting. This macro provides the prefix for
 * all the messages.
 */
#define MSGLOGGER_REPORT_PREFIX \
   __FILE__ << ":" << __LINE__ << " (" << MSGLOGGER_FNAME << "): "

/// Generic macro for reporing messages in the code
/**
 * This macro can be used to conveniently print messages with information about
 * where they are coming from in the code. The rest of the macros use this
 * macro internally. You can use this one directly as well with something like:
 *
 * <code>
 *   REPORT_MSG( Trig::INFO, "An information message" );
 * </code>
 */
#define REPORT_MSG( LEVEL, MESSAGE ) \
   log() << LEVEL << MSGLOGGER_REPORT_PREFIX << MESSAGE << Trig::MsgLogger::endmsg

/// Convenience macro for reporting VERBOSE messages in the code
/**
 * This macro is very similar to the REPORT_MESSAGE macros of Athena. It prints
 * a nicely formatted output that specifies both the exact function name where
 * the message was printed, and also the filename:line combination. It can be used
 * like a regular function inside cycles:
 *
 * <code>
 *   REPORT_VERBOSE_MSG( "This is a verbose message with a number: " << number );
 * </code>
 */
#define REPORT_VERBOSE_MSG( MESSAGE ) \
   REPORT_MSG( Trig::VERBOSE, MESSAGE )

/// Convenience macro for reporting VERBOSE messages in the code
/**
 * This macro is very similar to the REPORT_MESSAGE macros of Athena. It prints
 * a nicely formatted output that specifies both the exact function name where
 * the message was printed, and also the filename:line combination. It can be used
 * like a regular function inside cycles:
 *
 * <code>
 *   REPORT_DEBUG_MSG( "This is a debug message with a message: " << message );
 * </code>
 */
#define REPORT_DEBUG_MSG( MESSAGE ) \
   REPORT_MSG( Trig::DEBUG, MESSAGE )

/// Convenience macro for reporting ERROR messages in the code
/**
 * This macro is very similar to the REPORT_MESSAGE macros of Athena. It prints
 * a nicely formatted output that specifies both the exact function name where
 * the message was printed, and also the filename:line combination. It can be used
 * like a regular function inside cycles:
 *
 * <code>
 *   REPORT_ERROR_MSG( "A serious error message" );
 * </code>
 */
#define REPORT_ERROR_MSG( MESSAGE ) \
   REPORT_MSG( Trig::ERROR, MESSAGE )

/// Convenience macro for reporting FATAL messages in the code
/**
 * This macro is very similar to the REPORT_MESSAGE macros of Athena. It prints
 * a nicely formatted output that specifies both the exact function name where
 * the message was printed, and also the filename:line combination. It can be used
 * like a regular function inside cycles:
 *
 * <code>
 *   REPORT_FATAL_MSG( "A very serious error message" );
 * </code>
 */
#define REPORT_FATAL_MSG( MESSAGE ) \
   REPORT_MSG( Trig::FATAL, MESSAGE )

#endif // TRIGBUNCHCROSSINGTOOL_LOGWRITER_H

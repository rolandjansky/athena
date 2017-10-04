// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MsgLogger.h 472835 2011-12-06 21:05:16Z stelzer $
#ifndef TRIGT1MUCTPI_MSGLOGGER_H
#define TRIGT1MUCTPI_MSGLOGGER_H

// STL include(s):
#include <string>
#include <sstream>

#ifndef STANDALONE
// Gaudi/Athena include(s):
#include "GaudiKernel/IMessageSvc.h"
#endif

// Local include(s):
#include "MsgType.h"

namespace LVL1MUCTPI {

   // Forward declaration(s):
   class MsgWriter;

   /**
    * @short  This class should be used to do all printouts in the simulation.
    *
    *         One shouldn't use the standard output in the simulation,
    *         altough it's still used in some places. Instead this class
    *         should be used, as it will automatically redirect the
    *         messages to the correct output.
    *
    * @see    MsgWriter
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 472835 $
    * $Date: 2011-12-06 22:05:16 +0100 (Tue, 06 Dec 2011) $
    */
   class MsgLogger : public std::ostringstream {

   public:
      /// Constructor with the message source defined by 'source'.
      MsgLogger( const std::string& source );
      /// Copy constructor
      MsgLogger( const MsgLogger& parent );

      MsgLogger& operator= ( const MsgLogger& parent );

      /// The message source truncated to the maximum length
      std::string getFormattedSource() const;
      /// The message source in the format that it is printed
      std::string getPrintedSource() const;

      // Stream modifier(s):
      static MsgLogger& endmsg( MsgLogger& logger );

      // Accept stream modifiers:
      MsgLogger& operator<< ( MsgLogger& ( *_f )( MsgLogger& ) );
      MsgLogger& operator<< ( std::ostream& ( *_f )( std::ostream& ) );
      MsgLogger& operator<< ( std::ios& ( *_f )( std::ios& ) );

      // Accept message type specification:
      MsgLogger& operator<< ( MsgType type );

      // For all the "conventional" inputs:
      template < class T > MsgLogger& operator<< ( T arg ) {
         ( * ( std::ostringstream* ) this ) << arg;
         return *this;
      }

      /// Send a message of type 'type' to the output.
      void send( MsgType type, const std::string& message ) const;

      bool msgLvl( MsgType type ) const;
#ifndef STANDALONE
      bool msgLvl( MSG::Level type ) const;
#endif

   private:
      void send();

      std::string m_source;
      MsgWriter*  m_msgWriter;
      MsgType     m_activeType;

   }; // class MsgLogger

   //////////////////////////////////////////////////////////////////////
   //                                                                  //
   //   To speed up the code a bit, the following operators are        //
   //   declared 'inline'.                                             //
   //                                                                  //
   //////////////////////////////////////////////////////////////////////

   inline MsgLogger& MsgLogger::operator<< ( MsgLogger& ( *f )( MsgLogger& ) ) {

      return ( f )( *this );
   }

   inline MsgLogger& MsgLogger::operator<< ( std::ostream& ( *f )( std::ostream& ) ) {

      ( f )( *this );
      return *this;
   }

   inline MsgLogger& MsgLogger::operator<< ( std::ios& ( *f )( std::ios& ) ) {

      ( f )( *this );
      return *this;
   }

   inline MsgLogger& MsgLogger::operator<< ( MsgType type ) {

      m_activeType = type;
      return *this;
   }

} // namespace LVL1MUCTPI

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
#define MSGLOGGER_REPORT_PREFIX                                   \
   __FILE__ << ":" << __LINE__ << " (" << MSGLOGGER_FNAME << "): "

/// Generic macro for reporting messages in the code
/**
 * This macro can be used to conveniently print messages with information on
 * where the message is coming from in the code. The rest of the macros use this
 * macro internally. But you can also use it directly like this:
 *
 * <code>
 *   REPORT_MSG( LVL1MUCTPI::INFO, "Some information message" );
 * </code>
 */
#define REPORT_MSG2( LEVEL, MESSAGE, LOGGER )                   \
   if( LOGGER.msgLvl(LEVEL) ) {                                 \
        LOGGER << LEVEL << MSGLOGGER_REPORT_PREFIX << MESSAGE   \
               << LVL1MUCTPI::MsgLogger::endmsg;                \
   }
#define REPORT_MSG( LEVEL, MESSAGE ) REPORT_MSG2(LEVEL, MESSAGE, m_logger)

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
#define REPORT_VERBOSE_MSG2( MESSAGE, LOGGER )                          \
   if( LOGGER.msgLvl(LVL1MUCTPI::VERBOSE) ) {                         \
      LOGGER << LVL1MUCTPI::VERBOSE << MESSAGE << LVL1MUCTPI::MsgLogger::endmsg; \
   }
#define REPORT_VERBOSE_MSG(MESSAGE ) REPORT_VERBOSE_MSG2(MESSAGE, m_logger)

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
#define REPORT_ERROR_MSG2( MESSAGE, LOGGER )           \
   REPORT_MSG2( LVL1MUCTPI::ERROR, MESSAGE, LOGGER )
#define REPORT_ERROR_MSG( MESSAGE ) \
   REPORT_MSG( LVL1MUCTPI::ERROR, MESSAGE )

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
   REPORT_MSG( LVL1MUCTPI::FATAL, MESSAGE )
#define REPORT_FATAL_MSG2( MESSAGE, LOGGER )           \
   REPORT_MSG2( LVL1MUCTPI::FATAL, MESSAGE, LOGGER )

#endif // TRIGT1MUCTPI_MSGLOGGER_H

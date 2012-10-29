// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MsgLogger.h 457114 2011-09-05 09:35:49Z krasznaa $
#ifndef TRIGBUNCHCROSSINGTOOL_MSGLOGGER_H
#define TRIGBUNCHCROSSINGTOOL_MSGLOGGER_H

// STL include(s):
#include <string>
#include <sstream>
#include <set>
#include <vector>

// Local include(s):
#include "MsgType.h"

namespace Trig {

   // Forward declaration(s):
   class MsgWriter;

   /**
    *  @short Common logger class for the Athena independent classes
    *
    *         All the Athena independent classes use this logger class for
    *         printing messages. It acts just like any other std::ostream.
    *
    *         The point is that this way the classes are independent of
    *         Gaudi's MsgStream, and can be compiled without it.
    *
    *         The logger object prints all the messages through a singleton
    *         MsgWriter object. It's the MsgWriter that decides how the
    *         messages are to be printed depending on how it was compiled.
    *
    *         The class itself is just a new iteration over the SLogger design
    *         from the SFrame code. (And the MsgLogger from TMVA and
    *         GoodRunsLists...)
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 457114 $
    * $Date: 2011-09-05 11:35:49 +0200 (Mon, 05 Sep 2011) $
    */
   class MsgLogger : public std::ostringstream {

   public:
      /// Constructor with a name of the parent object
      MsgLogger( const std::string& source );
      /// Copy constructor
      MsgLogger( const MsgLogger& parent );

      /// Set the message source
      void setSource( const std::string& source );

      /// Copy operator
      MsgLogger& operator= ( const MsgLogger& parent );

      /// Stream modifier to send a message
      static MsgLogger& endmsg( MsgLogger& logger );

      /// Operator accepting MsgLogger stream modifiers
      MsgLogger& operator<< ( MsgLogger& ( *_f )( MsgLogger& ) );
      /// Operator accepting std::ostream stream modifiers
      MsgLogger& operator<< ( std::ostream& ( *_f )( std::ostream& ) );
      /// Operator accepting std::ios stream modifiers
      MsgLogger& operator<< ( std::ios& ( *_f )( std::ios& ) );

      /// Operator accepting message type setting
      MsgLogger& operator<< ( MsgType type );

      /// Generic output operator accepting any kinds of input
      /**
       * We can't just let the base class's output operator to do the work,
       * because that returns an std::ostringstream. So the MsgLogger specific
       * operations would not work after using such an operator...
       */
      template< class T >
      MsgLogger& operator<< ( const T& arg ) {
         ( * ( std::ostringstream* ) this ) << arg;
         return *this;
      }

      /// Output operator specializations for std::set objects
      /**
       * For some reason I was not able to define this operator outside of the
       * class. If it's not defined here, then the generic operator for some
       * reason always takes precedence. Even if I define both of them outside
       * of the class. I'm a bit lost I have to admit...
       */
      template< class T >
      MsgLogger& operator<< ( const std::set< T >& arg ) {
         *this << "[";
         typename std::set< T >::const_iterator itr = arg.begin();
         typename std::set< T >::const_iterator end = arg.end();
         for( ; itr != end; ++itr ) {
            *this << *itr;
            if( ++itr != end ) {
               *this << ", ";
            }
            --itr;
         }
         *this << "]";
         return *this;
      }

      /// Output operator specializations for std::vector objects
      /**
       * Works as the other specialised output operators. It helps to quickly
       * print the contents of vectors.
       */
      template< class T >
      MsgLogger& operator<< ( const std::vector< T >& arg ) {
         *this << "[";
         typename std::vector< T >::const_iterator itr = arg.begin();
         typename std::vector< T >::const_iterator end = arg.end();
         for( ; itr != end; ++itr ) {
            *this << *itr;
            if( ++itr != end ) {
               *this << ", ";
            }
            --itr;
         }
         *this << "]";
         return *this;
      }

      /// Old style message sender function
      void send( MsgType type, const std::string& message ) const;

   private:
      void send();

      std::string   m_source; ///< Source of the messages
      MsgWriter*    m_logWriter; ///< Pointer to the singleton message writer
      MsgType       m_activeType; ///< The currently active message type

   }; // class MsgLogger

   //////////////////////////////////////////////////////////////////////
   //                                                                  //
   //   To speed up the code a bit, the following operators are        //
   //   declared 'inline'.                                             //
   //                                                                  //
   //////////////////////////////////////////////////////////////////////

   /**
    * This operator handles all stream modifiers that have been written
    * to work on MsgLogger objects specifically. Right now there is basically
    * only the MsgLogger::endmsg stream modifier that is such.
    */
   inline MsgLogger& MsgLogger::operator<< ( MsgLogger& ( *_f )( MsgLogger& ) ) {

      return ( _f )( *this );
   }

   /**
    * This operator handles all stream modifiers that have been written
    * to work on std::ostream objects. Most of the message formatting
    * modifiers are such.
    */
   inline MsgLogger& MsgLogger::operator<< ( std::ostream& ( *_f )( std::ostream& ) ) {

      ( _f )( *this );
      return *this;
   }

   /**
    * This operator handles all stream modifiers that have been written
    * to work on std::ios objects. I have to admit I don't remember exactly
    * which operators these are, but some formatting operations need this.
    */
   inline MsgLogger& MsgLogger::operator<< ( std::ios& ( *_f )( std::ios& ) ) {

      ( _f )( *this );
      return *this;
   }

   /**
    * Messages have a type, defined by the SMsgType enumeration. This operator
    * allows the user to write intuitive message lines in the code like this:
    *
    * <code>
    *   logger << INFO << "This is an info message" << MsgLogger::endmsg;
    * </code>
    */
   inline MsgLogger& MsgLogger::operator<< ( MsgType type ) {

      m_activeType = type;
      return *this;
   }

} // namespace Trig

#endif // TRIGBUNCHCROSSINGTOOL_MSGLOGGER_H

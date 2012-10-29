/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MsgWriter.cxx 514416 2012-08-22 11:42:15Z krasznaa $

// Include the message printing system's header:
#ifdef ROOTCORE
#   include "TError.h"
#else
#   include "AthenaKernel/getMessageSvc.h"
#endif // ROOTCORE

// Local include(s):
#include "TrigBunchCrossingTool/MsgWriter.h"

namespace Trig {

   // Initialize the static variable(s):
   MsgWriter* MsgWriter::m_instance = 0;

   /**
    * This function implements the singleton design pattern for the
    * class. Since the constructor of the class is "protected", the
    * user can not create it manually. He/she has to access a (single)
    * instance of the object with this function.
    */
   MsgWriter* MsgWriter::instance() {

      if( ! m_instance ) {
         m_instance = new MsgWriter();
      }

      return m_instance;
   }

   MsgWriter::~MsgWriter() {

#ifndef ROOTCORE
      if( m_stream ) delete m_stream;
#endif // ROOTCORE
   }

#ifdef ROOTCORE

   /**
    * This function sets the minimum message type that should still be
    * displayed. All messages having a higher priority will be displayed
    * as well of course.
    *
    * @param type The value of the minimum type
    * @see MsgWriter::getMinType
    */
   void MsgWriter::setMinType( MsgType type ) {

      m_minType = type;
      return;
   }

   /**
    * Not much to say here.
    *
    * @see MsgWriter::setMinType
    */
   MsgType MsgWriter::getMinType() const {

      return m_minType;
   }

#else

   /**
    * Tell the MsgStream object what is the minimum type to be printed.
    */
   void MsgWriter::setMinType( MSG::Level type ) {

      m_stream->setLevel( type );
      return;
   }

   /**
    * The function just deletes the current MsgStream object and creates one
    * with the new source name. So it shouldn't be called too often.
    */
   void MsgWriter::setSource( const std::string& source ) {

      if( m_stream ) delete m_stream;
      m_stream = new MsgStream( Athena::getMessageSvc(), source );

      return;
   }

#endif // ROOTCORE

   /**
    * The constructor takes care of filling the std::map-s that are
    * used for generating the output.
    */
   MsgWriter::MsgWriter() {

#ifdef ROOTCORE
      m_minType = INFO;
#else
      m_typeMap[ VERBOSE ] = MSG::VERBOSE;
      m_typeMap[ DEBUG ]   = MSG::DEBUG;
      m_typeMap[ INFO ]    = MSG::INFO;
      m_typeMap[ WARNING ] = MSG::WARNING;
      m_typeMap[ ERROR ]   = MSG::ERROR;
      m_typeMap[ FATAL ]   = MSG::FATAL;
      m_typeMap[ ALWAYS ]  = MSG::ALWAYS;

      m_stream = new MsgStream( Athena::getMessageSvc(), "BunchCrossingTool" );
#endif // ROOTCORE
   }

   /**
    * This function is the heavy-lifter of the class. It writes the received
    * message to the console. The function assumes that the message has no
    * line breaks and that it has been formatted by MsgLogger.
    *
    * @param type The message type
    * @param source The exact source of the message
    * @param line A single line of message to be displayed.
    */
   void MsgWriter::write( MsgType type, const std::string& source,
                          const std::string& line ) const {

#ifdef ROOTCORE
      if( type < m_minType ) return;

      // Use one of the ROOT message printing functions:
      switch( type ) {
      case VERBOSE:
         ::Info( source.c_str(), "=Verbose= %s", line.c_str() );
         break;
      case DEBUG:
         ::Info( source.c_str(), "=Debug= %s", line.c_str() );
         break;
      case INFO:
         ::Info( source.c_str(), "%s", line.c_str() );
         break;
      case WARNING:
         ::Warning( source.c_str(), "%s", line.c_str() );
         break;
      case ERROR:
         ::Error( source.c_str(), "%s", line.c_str() );
         break;
      case FATAL:
         ::Fatal( source.c_str(), "%s", line.c_str() );
         break;
      case ALWAYS:
         ::Info( source.c_str(), "=Always= %s", line.c_str() );
         break;
      default:
         ::Error( source.c_str(), "Unknown message level set for message:" );
         ::Error( source.c_str(), "%s", line.c_str() );
         break;
      }
#else
      // "Simply" print the message using Athena's message stream:
      std::map< MsgType, MSG::Level >::const_iterator stype;
      if( ( stype = m_typeMap.find( type ) ) == m_typeMap.end() ) return;
      if( stype->second < m_stream->level() ) return;
      *m_stream << stype->second << source << " : " << line << endreq;
#endif // ROOTCORE

      return;
   }

} // namespace Trig

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MsgLogger.cxx 706709 2015-11-09 12:10:23Z wengler $

// STL include(s):
#include <iomanip>
#include <cstdlib>

// Local include(s):
#include "MsgLogger.h"
#include "MsgWriter.h"

//***********************************************************************
//
//       Version : $Revision: 706709 $
//
//   Description : Implementation of the MsgLogger class
//
//        Author : $Author: krasznaa $
//          Date : $Date: 2015-11-09 13:10:23 +0100 (Mon, 09 Nov 2015) $
//
//***********************************************************************

namespace LVL1MUCTPI {

   //
   // This is the hard-coded maximum length of the source names:
   //
   static const std::string::size_type MAXIMUM_SOURCE_NAME_LENGTH = 20;

   MsgLogger::MsgLogger( const std::string& source )
      : m_source( source ), m_activeType( INFO ) {

      m_msgWriter = MsgWriter::instance();
   }

   MsgLogger::MsgLogger( const MsgLogger& parent )
      : std::basic_ios< MsgLogger::char_type, MsgLogger::traits_type >(),
        std::ostringstream(),m_msgWriter(nullptr) {

      *this = parent;
   }

   MsgLogger& MsgLogger::operator= ( const MsgLogger& parent ) {

     if (this != &parent){
       m_source = parent.m_source;
       m_msgWriter = MsgWriter::instance();
       m_activeType = parent.m_activeType;
     }
     return *this;
   }

   MsgLogger& MsgLogger::endmsg( MsgLogger& logger ) {

      logger.send();
      return logger;
   }

   std::string MsgLogger::getFormattedSource() const {

      // make sure the source name is no longer than fMaxSourceSize:
      std::string source_name = m_source;
      if( source_name.size() > MAXIMUM_SOURCE_NAME_LENGTH ) {
         source_name = source_name.substr( 0, MAXIMUM_SOURCE_NAME_LENGTH - 3 );
         source_name += "...";
      }
   
      return source_name;
   }

   std::string MsgLogger::getPrintedSource() const {

      // the full logger prefix
      std::string source_name = getFormattedSource();
      if( source_name.size() < MAXIMUM_SOURCE_NAME_LENGTH ) {
         for( std::string::size_type i = source_name.size();
              i < MAXIMUM_SOURCE_NAME_LENGTH; ++i ) {
            source_name.push_back( ' ' );
         }
      }

      return source_name + " : "; 
   }

   /**
    * This is the function that actually writes to the output, using MsgWriterSingleton.
    * You can even give multi-line strings to it, it will nicely give each line
    * the appropriate header.
    */
   void MsgLogger::send( MsgType type, const std::string& message ) const {

      std::string::size_type previous_pos = 0, current_pos = 0;

      //
      // Make sure the source name is no longer than MAXIMUM_SOURCE_NAME_LENGTH:
      //
      std::string source_name = getFormattedSource();

      //
      // Slice the recieved message into lines:
      //
      for( ; ; ) {

         current_pos = message.find( '\n', previous_pos );
         std::string line = message.substr( previous_pos, current_pos - previous_pos );

         std::ostringstream message_to_send;
         // I have to call the modifiers like this, otherwise g++ get's confused
         // with the operators...
         message_to_send.setf( std::ios::adjustfield, std::ios::left );
         message_to_send.width( MAXIMUM_SOURCE_NAME_LENGTH );
         message_to_send << source_name << " : " << line;
         m_msgWriter->write( type, message_to_send.str() );

         if( current_pos == message.npos ) break;
         previous_pos = current_pos + 1;

      }

      return;
   }

   void MsgLogger::send() {

      this->send( m_activeType, this->str() );

      //
      // Reset the stream buffer:
      //
      this->str( "" );

      return;
   }

   bool MsgLogger::msgLvl( MsgType type ) const {
      return m_msgWriter->msgLvl((unsigned int)type);
   }
#ifndef STANDALONE
   bool MsgLogger::msgLvl( MSG::Level type ) const {
      return m_msgWriter->msgLvl((unsigned int) type);
   }
#endif

} // namespace LVL1MUCTPI

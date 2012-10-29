/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MsgLogger.cxx 514416 2012-08-22 11:42:15Z krasznaa $

// ROOT include(s):
#ifdef ROOTCORE
#include "TError.h"
#endif // ROOTCORE

// Local include(s):
#include "TrigBunchCrossingTool/MsgLogger.h"
#include "TrigBunchCrossingTool/MsgWriter.h"

namespace Trig {

   MsgLogger::MsgLogger( const std::string& source )
      : m_source( source ), m_activeType( INFO ) {

      m_logWriter = MsgWriter::instance();
   }

   /**
    * This constructor is necessary to be able to freely copy objects
    * using MsgLogger.
    *
    * @param parent The MsgLogger object that should be cloned
    * @see MsgLogger::operator=
    */
   MsgLogger::MsgLogger( const MsgLogger& parent )
      : std::basic_ios< MsgLogger::char_type, MsgLogger::traits_type >(),
        std::ostringstream() {

      *this = parent;
   }

   void MsgLogger::setSource( const std::string& source ) {

      m_source = source;
      return;
   }

   /**
    * Operator for copying the configuration of one MsgLogger object into
    * another. It is mostly used by the copy constructor.
    *
    * @param parent The MsgLogger object that should be cloned
    * @see MsgLogger(const MsgLogger&)
    */
   MsgLogger& MsgLogger::operator= ( const MsgLogger& parent ) {

      m_source = parent.m_source;
      m_logWriter = MsgWriter::instance();

      return *this;
   }

   /**
    * This function does the heavy-lifting of the message sending.
    * It doesn't use any of the std::ostringstream functionality itself.
    * It receives the type of the message and the message itself.
    * If the type is such that it should be displayed, it slices the
    * message into multiple lines and sends it line-by-line to
    * MsgWriter.
    *
    * @param type The type of the message to send
    * @param message The text of the message
    */
   void MsgLogger::send( const MsgType type, const std::string& message ) const {

      // To speed up the code in standalone mode:
#ifdef ROOTCORE
      if( type < m_logWriter->getMinType() ) return;
#endif // ROOTCORE

      std::string::size_type previous_pos = 0, current_pos = 0;

      //
      // Slice the recieved message into lines:
      //
      for( ; ; ) {
         current_pos = message.find( '\n', previous_pos );
         const std::string line = message.substr( previous_pos,
                                                  current_pos - previous_pos );

         m_logWriter->write( type, m_source, line );

         if( current_pos == message.npos ) break;
         previous_pos = current_pos + 1;
      }

      return;
   }

   /**
    * This function sends the text that's been accumulated by the
    * std::ostringstream base class to the output. It actually uses the
    * MsgLogger::send(const MsgType,const string&) function to do the work.
    * After the message is sent, it clears the text buffer of the object.
    */
   void MsgLogger::send() {

      //
      // Call the "other" send(...) function:
      //
      this->send( m_activeType, this->str() );

      //
      // Reset the stream buffer:
      //
      this->str( "" );

      return;
   }

   /**
    * This is just a convenience function to be able to just use the "<<"
    * operator for printing messages. This stream modifier acts very
    * similarly to std::endl. You can use it like:
    *
    * <code>
    *   logger << "Some text" << MsgLogger::endmsg;
    * </code>
    */
   MsgLogger& MsgLogger::endmsg( MsgLogger& logger ) {

      logger.send();
      return logger;
   }

} // namespace Trig

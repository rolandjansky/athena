/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT1CTP/MsgLogger.h"
#include "TrigT1CTP/MsgWriter.h"

namespace LVL1CTP {

   MsgLogger::MsgLogger( const std::string& source )
      : m_source( source ) {

      m_msgWriter = MsgWriter::instance();

   }

   MsgLogger::~MsgLogger() {

   }

   bool
   MsgLogger::msgLvl (const MSG::Level lvl) const
   {
      return m_msgWriter->msgLvl(lvl);
   }


   /**
    * This is the function that actually writes to the output, using MsgWriter.
    * You can even give multi-line strings to it, it will nicely give each line
    * the appropriate header.
    */
   void MsgLogger::send( MSG::Level type, const std::string& message ) const {

      std::string::size_type previous_pos = 0, current_pos = 0;

      // Slice the recieved message into lines:
      for( ; ; ) {

         current_pos = message.find( '\n', previous_pos );
         std::string line = message.substr( previous_pos, current_pos - previous_pos );

         std::string message_to_send = m_source + " : " + line;
         m_msgWriter->write( type, message_to_send );

         if( current_pos == message.npos ) break;
         previous_pos = current_pos + 1;

      }

      return;

   }

} // namespace LVL1CTP

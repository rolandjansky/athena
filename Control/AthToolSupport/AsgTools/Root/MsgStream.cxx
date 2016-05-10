// $Id: MsgStream.cxx 687011 2015-08-03 09:25:07Z krasznaa $

// System include(s):
#include <string>
#include <iostream>
#include <iomanip>

// Local include(s):
#include "AsgTools/MsgStream.h"

// Initialise the source name width to a default:
size_t MsgStream::s_sourceWidth = 25;

MsgStream::MsgStream( const asg::IAsgTool* tool )
   : m_tool( tool ), m_name( "" ),
     m_lvl( MSG::INFO ), m_reqlvl( MSG::NIL ) {

}

MsgStream::MsgStream( const std::string& name )
   : m_tool( 0 ), m_name( name ),
     m_lvl( MSG::INFO ), m_reqlvl( MSG::NIL ) {

}

/// This constructor is here to allow client code to possibly use MsgStream
/// objects directly in some dual-use code. This constructor can in principle
/// be used in the same sort of code where
///
/// <code>
///  MsgStream( IMessageSvc*, const std::string&, int );
/// </code>
///
/// is used in the offline code.
///
MsgStream::MsgStream( void*, const std::string& name, int )
   : MsgStream( name ) {

}

MsgStream& MsgStream::operator<< ( MsgStream& ( *_f )( MsgStream& ) ) {

   return ( _f )( *this );
}

MsgStream& MsgStream::operator<< ( std::ostream& ( *_f )( std::ostream& ) ) {

   ( _f )( *this );
   return *this;
}

MsgStream& MsgStream::operator<< ( std::ios& ( *_f )( std::ios& ) ) {

   ( _f )( *this );
   return *this;
}

MsgStream& MsgStream::operator<< ( MSG::Level lvl ) {

   m_reqlvl = lvl;
   return *this;
}

/// This is the function doing the heavy lifting. It checks whether the payload
/// needs to be printed or not, prints the payload if necessary, and finally
/// resets the std::ostringstream base object.
///
/// @returns A reference to this object
///
MsgStream& MsgStream::doOutput() {

   // Check if anything needs to be printed:
   if( m_reqlvl >= m_lvl ) {

      // Get the name of the parent tool:
      std::string sname = name();
      // Get the string representation of the message level:
      const std::string& sreqlvl = MSG::name( m_reqlvl );

      // A stream to construct the printed message in. In order not to have to
      // manipulate std::cout directly.
      std::ostringstream output;

      // Maximise the width of the message source string:
      if( sname.size() > s_sourceWidth ) {
         sname = sname.substr( 0, s_sourceWidth - 3 );
         sname += "...";
      }

      // Construct the message to be printed:
      output << std::setiosflags( std::ios::left )
             << std::setw( s_sourceWidth ) << sname << " " << std::setw( 7 )
             << sreqlvl << " " << this->str();

      // Print the message payload:
      std::cout << output.str() << std::endl;
   }

   // Reset the stream buffer:
   this->str( "" );

   return *this;
}

void MsgStream::setLevel( MSG::Level lvl ) {

   m_lvl = lvl;
   return;
}

bool MsgStream::msgLevel( MSG::Level lvl ) const {

   return ( lvl >= m_lvl );
}

MSG::Level MsgStream::level() const {

   return m_lvl;
}

MSG::Level& MsgStream::mutable_level() {

   return m_lvl;
}

MSG::Level MsgStream::currentLevel() const {

   return m_reqlvl;
}

const std::string& MsgStream::name() const {

   // If we have a valid tool parent:
   if( m_tool ) {
      return m_tool->name();
   }

   // If not, return the name stored in the object:
   return m_name;
}

void MsgStream::setName( const std::string& name ) {

   // Detach the object from the IAsgTool pointer:
   m_tool = 0;

   // Remember this name:
   m_name = name;

   return;
}

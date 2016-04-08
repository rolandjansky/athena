// $Id: ToolHandle.cxx 659434 2015-04-08 11:53:27Z krasznaa $

// Local include(s):
#include "AsgTools/ToolHandle.h"

ToolHandleBase::ToolHandleBase( const std::string& typeAndName )
: m_typeAndName( typeAndName ), m_type(), m_name() {

   // Decode the received string:
   size_t pos = 0;
   if( ( pos = m_typeAndName.find( '/' ) ) == std::string::npos ) {
      // The user specified a single string, which is used as both the type
      // and instance name.
      m_type = m_typeAndName;
      m_name = m_typeAndName;
   } else {
      // The user specified both a type and an instance name.
      m_type = m_typeAndName.substr( 0, pos );
      m_name = m_typeAndName.substr( pos + 1 );
   }
}

const std::string& ToolHandleBase::typeAndName() const {

   return m_typeAndName;
}

const std::string& ToolHandleBase::type() const {

   return m_type;
}

const std::string& ToolHandleBase::name() const {

   return m_name;
}

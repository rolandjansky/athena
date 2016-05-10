// $Id: Property.cxx 687011 2015-08-03 09:25:07Z krasznaa $

// System include(s):
#include <iostream>

// Local include(s):
#include "AsgTools/Property.h"

/// Macro used in implementing the type->name translation function
#define DECLARE_TYPE( TYPE, NAME )              \
   case TYPE:                                   \
   {                                            \
      static const std::string n( NAME );       \
      return n;                                 \
   }                                            \
   break

const std::string& Property::name( Type type ) {

   switch( type ) {
      DECLARE_TYPE( UNKNOWNTYPE, "unknown" );
      DECLARE_TYPE( BOOL, "bool" );
      DECLARE_TYPE( INT, "int" );
      DECLARE_TYPE( FLOAT, "float" );
      DECLARE_TYPE( DOUBLE, "double" );
      DECLARE_TYPE( STRING, "string" );
      DECLARE_TYPE( INTVECTOR, "vector<int>" );
      DECLARE_TYPE( FLOATVECTOR, "vector<float>" );
      DECLARE_TYPE( STRINGVECTOR, "vector<string>" );
      DECLARE_TYPE( TOOLHANDLE, "ToolHandle" );
      DECLARE_TYPE( TOOLHANDLEARRAY, "ToolHandleArray" );
   default:
      break;
   }

   static const std::string dummy( "<error>" );
   return dummy;
}

Property::Property()
   : m_type( UNKNOWNTYPE ) {

}

Property::Property( Type type )
   : m_type( type ) {

}

bool Property::isValid() const {

   return ( m_type != UNKNOWNTYPE );
}

Property::Type Property::type() const {

   return m_type;
}

const std::string& Property::typeName() const {

   return name( m_type );
}

int Property::setFrom( const Property& ) {

   std::cout << "Property::setFrom: Property not set!" << std::endl;
   return -1;
}

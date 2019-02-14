// $Id: Property.cxx 802972 2017-04-15 18:13:17Z krumnack $

// System include(s):
#include <iostream>

// Local include(s):
#include "AsgTools/Property.h"
#include "AsgTools/StatusCode.h"

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



StatusCode Property::getString (std::string& /*result*/) const {
  std::cout << "Property::getString: method not implemented" << std::endl;
  return StatusCode::FAILURE;
}

StatusCode Property::getCastString (std::string& /*result*/) const {
  std::cout << "Property::getCastString: method not implemented" << std::endl;
  return StatusCode::FAILURE;
}

StatusCode Property::setString (const std::string& /*value*/) {
  std::cout << "Property::setString: method not implemented" << std::endl;
  return StatusCode::FAILURE;
}

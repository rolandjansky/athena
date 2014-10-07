// $Id: PropertyMgr.cxx 615760 2014-09-09 12:50:01Z krasznaa $

// Local include(s):
#include "AsgTools/PropertyMgr.h"

PropertyMgr::PropertyMgr()
   : m_props(), m_msg( "PropertyMgr" ) {

}

PropertyMgr::~PropertyMgr() {

   PropMap_t::iterator itr = m_props.begin();
   PropMap_t::iterator end = m_props.end();
   for( ; itr != end; ++itr ) {
      delete itr->second;
   }
}

/// The function needs to be specialised for char array, because the users
/// will most often want to set string properties simply like:
///
/// <code>
///  mytool.setProperty( "PropName", "PropValue" );
/// </code>
///
/// This code would use "TProperty<const char[9]>" in the generic template
/// function, which doesn't quite work in this setup.
///
/// It's interesting to note that we can't use a constant string reference
/// as the value argument, as a character constant would need translation into
/// such a parameter, and as such, C++ selects the template function instead.
/// So there's a good reason why we use the basic C type here.
///
/// @param name The name of the string property to set
/// @param value The value of the string property to set
/// @returns <code>StatusCode::SUCCESS</code> if the operation was successful,
///          <code>StatusCode::FAILURE</code> otherwise
///
StatusCode PropertyMgr::setProperty( const std::string& name,
                                     const char* value ) {

   // Look for the property:
   PropMap_t::const_iterator iprop = m_props.find( name );
   if( iprop == m_props.end() ) {
      ATH_MSG_ERROR( "Property not found: " << name );
      return StatusCode::FAILURE;
   }

   // Access the property object:
   Property* pprop = iprop->second;
   // Create a helper property object:
   std::string dummy( value );
   TProperty< std::string > pinprop( dummy, Property::STRING );
   // Try setting the managed property using the helper property object:
   if( pprop->setFrom( pinprop ) ) {
      ATH_MSG_ERROR(  "Value assignment failed for " << name );
      return StatusCode::FAILURE;
   }

   // We succeeded:
   return StatusCode::SUCCESS;
}

/// This function is used in the code to get the value of some property out
/// of an existing tool, from the outside. (The tool itself has much easier
/// ways of accessing one of its own properties...)
///
/// @param name The name of the property
/// @returns A pointer to the property object if it exists, or a null pointer
///          if it doesn't
///
const Property* PropertyMgr::getProperty( const std::string& name ) const {

   // Look for the property:
   PropMap_t::const_iterator iprop = m_props.find( name );
   if( iprop == m_props.end() ) {
      ATH_MSG_WARNING( "Property not found: " << name );
      return 0;
   }

   // Return the property:
   return iprop->second;
}

const PropertyMgr::PropMap_t& PropertyMgr::getProperties() const {

   return m_props;
}

MsgStream& PropertyMgr::msg() const {

   return m_msg;
}

MsgStream& PropertyMgr::msg( MSG::Level level ) const {

   m_msg << level;
   return m_msg;
}

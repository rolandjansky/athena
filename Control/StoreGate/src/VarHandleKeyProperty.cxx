/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file StoreGate/src/VarHandleKeyProperty.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Handle Gaudi property setting for VarHandleKey.
 */

// STL includes
#include <sstream>
#include <map>
#include <boost/tokenizer.hpp>

// StoreGate includes
#include "StoreGate/VarHandleKeyProperty.h"

static const char* const storeSeparator = "+";

namespace Gaudi { 
namespace Parsers {


/**
 * @brief Gaudi function used to initialize a property from a string.
 * @param v The object to initialize.
 * @param s The string from which to initialize.
 *
 * Used during Gaudi property handling to set object @c v from the string @c s.
 * Note that @c s is a representation of the property setting; thus, in the
 * case of setting a property from a string, @c s will contain quote marks.
 */
StatusCode 
parse(SG::VarHandleKey& v, const std::string& s)
{
  std::string prop;
  StatusCode sc = Gaudi::Parsers::parse(prop, s);
  if (sc.isSuccess())
    sc = v.assign (prop);
  return sc;
}

} //> ns Parsers

namespace Utils {


/**
 * @brief Gaudi function used to convert a property to a string.
 * @param v The object to convert.
 * @param o Stream to which to do the conversion.
 *
 * Used during Gaudi property handling to get a string representation of @c v.
 * Note that if the representation is a string, it should be surrounded
 * by quote marks.
 */
std::ostream& 
toStream(const SG::VarHandleKey& v, std::ostream& o)
{
  o << "'" << v.storeHandle().name() << storeSeparator << v.key() << "'";
  return o;
}
    
} //> ns Utils

} //> ns Gaudi


namespace SG {


/**
 * @brief Constructor with parameters.
 * @param name Name of the property.
 * @param ref Object which this property is setting.
 */
VarHandleKeyProperty::VarHandleKeyProperty( const std::string& name, 
                                            SG::VarHandleKey& ref )
  : PropertyWithHandlers( name, typeid( SG::VarHandleKey ) ), 
    m_pValue( &ref ) 
{
}


/**
 * @brief Assignment operator.
 * @param value Value from which to assign.
 *
 * Copy the value object which we control from another value object.
 */
VarHandleKeyProperty& 
VarHandleKeyProperty::operator=( const SG::VarHandleKey& value ) 
{
  setValue( value );
  return *this;
}


/**
 * @brief Return a new copy of this Property object.
 *
 * The new object will be associated with the _same_ value object
 * as the original.
 */
VarHandleKeyProperty* 
VarHandleKeyProperty::clone() const 
{
  return new VarHandleKeyProperty( *this );
}


/**
 * @brief Set the value of another Property.
 * @param destination The Property whose value is changed.
 *
 * The value object of this Property is copied to that of @c destination
 * by converting to a string and back again.  Returns true on success,
 * false on failure.
 */
bool
VarHandleKeyProperty::load( Gaudi::Details::PropertyBase& destination ) const 
{
  return destination.assign( *this );
}


/**
 * @brief Set the value of this Property from another.
 * @param destination The Property from which the value should be copied.
 *
 * The value object of this @c source is copied to that of this Property
 * by converting to a string and back again.  Returns true on success,
 * false on failure.
 */
bool
VarHandleKeyProperty::assign( const Gaudi::Details::PropertyBase& source ) 
{
  return fromString( source.toString() ).isSuccess();
}


/**
 * @brief Return a string representation of the value object.
 */
std::string
VarHandleKeyProperty::toString( ) const 
{
  useReadHandler();
  std::ostringstream o;
  Gaudi::Utils::toStream(*m_pValue, o);
  return o.str();
}


/**
 * @brief Write a string representation of the value object to a stream.
 * @param out Stream to which to write.
 */
void
VarHandleKeyProperty::toStream(std::ostream& out) const
{
  useReadHandler();
  out << this->toString();
}


/**
 * @brief Set this value object from a string.
 * @param s String from which to initialize the value.
 *
 * Returns failure if the conversion does not succeed.
 */
StatusCode 
VarHandleKeyProperty::fromString(const std::string& s)
{
  if (!Gaudi::Parsers::parse(*m_pValue, s).isSuccess()) {
    return StatusCode::FAILURE;
  }
  return useUpdateHandler()
    ? StatusCode::SUCCESS
    : StatusCode::FAILURE;
}


/**
 * @brief Return the value object for this Property.
 */
const SG::VarHandleKey& 
VarHandleKeyProperty::value() const 
{
  useReadHandler();
  return *m_pValue;
}


/**
 * @brief Set the value object for this Property.
 * @param value Value from which to copy.
 *
 * Return true on success; false if the update handler failed.
 */
bool
VarHandleKeyProperty::setValue( const SG::VarHandleKey& value ) 
{
  m_pValue->operator=(value);
  return useUpdateHandler();
}


}  // namespace SG

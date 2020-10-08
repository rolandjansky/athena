/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// STL includes
#include <sstream>
#include <map>
#include <boost/tokenizer.hpp>

// StoreGate includes
#include "StoreGate/VarHandleKeyArrayProperty.h"

namespace SG {


/**
 * @brief Constructor with parameters.
 * @param name Name of the property.
 * @param ref Object which this property is setting.
 */
  VarHandleKeyArrayProperty::VarHandleKeyArrayProperty( const std::string& name, 
                                                        SG::VarHandleKeyArray& ref )
    : PropertyWithHandlers( name, typeid( SG::VarHandleKeyArray ) ), 
      m_pValue( &ref ) 
  {
}


/**
 * @brief Assignment operator.
 * @param value Value from which to assign.
 *
 * Copy the value object which we control from another value object.
 */
VarHandleKeyArrayProperty& 
VarHandleKeyArrayProperty::operator=( const SG::VarHandleKeyArray& value ) 
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
VarHandleKeyArrayProperty* 
VarHandleKeyArrayProperty::clone() const 
{
  return new VarHandleKeyArrayProperty( *this );
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
VarHandleKeyArrayProperty::load( Gaudi::Details::PropertyBase& destination ) const 
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
VarHandleKeyArrayProperty::assign( const Gaudi::Details::PropertyBase& source ) 
{
  return fromString( source.toString() ).isSuccess();
}


/**
 * @brief Return a string representation of the value object.
 */
std::string
VarHandleKeyArrayProperty::toString( ) const 
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
VarHandleKeyArrayProperty::toStream(std::ostream& out) const
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
VarHandleKeyArrayProperty::fromString(const std::string& s)
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
const SG::VarHandleKeyArray& 
VarHandleKeyArrayProperty::value() const 
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
VarHandleKeyArrayProperty::setValue( const SG::VarHandleKeyArray& value ) 
{
  m_pValue->operator=(value);
  return useUpdateHandler();
}


}  // namespace SG

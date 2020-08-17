// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file StoreGate/VarHandleKeyProperty.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Handle Gaudi property setting for VarHandleKey.
 *
 * Gaudi property handling can be extended for a new type T by specializing
 * SimplePropertyRef<T> to be something that derives from Property.
 * This class will then handle conversions of T to and from strings.
 */


#ifndef STOREGATE_VARHANDLEKEYPROPERTY_H
#define STOREGATE_VARHANDLEKEYPROPERTY_H


#include "StoreGate/VarHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "StoreGate/UpdateHandleKey.h"
#include "StoreGate/ReadDecorHandleKey.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "Gaudi/Property.h"
#include <iostream>

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
GAUDI_API
StatusCode parse(SG::VarHandleKey& v, const std::string& s);

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
GAUDI_API
std::ostream& toStream(const SG::VarHandleKey& v, std::ostream& o);

} //> ns Utils
} //> ns Gaudi


namespace SG {


/** 
 * @brief VarHandleKeyProperty is the class which wraps a @c SG::VarHandleKey.
 *
 * The Property object refers to an instance of @c SG::VarHandleKey
 * (the value object) and provides generic methods for manipulating it.
 */
  class GAUDI_API VarHandleKeyProperty : public PropertyWithHandlers <>
{
public:

  /**
   * @brief Constructor with parameters.
   * @param name Name of the property.
   * @param ref Object which this property is setting (the value object).
   */
  VarHandleKeyProperty( const std::string& name, SG::VarHandleKey& ref );


  /**
   * @brief Assignment operator.
   * @param value Value from which to assign.
   *
   * Copy the value object which we control from another value object.
   */
  VarHandleKeyProperty& operator=( const SG::VarHandleKey& value );


  /**
   * @brief Return a new copy of this Property object.
   *
   * The new object will be associated with the _same_ value object
   * as the original.
   */
  virtual VarHandleKeyProperty* clone() const override;


  /**
   * @brief Set the value of another Property.
   * @param destination The Property whose value is changed.
   *
   * The value object of this Property is copied to that of @c destination
   * by converting to a string and back again.  Returns true on success,
   * false on failure.
   */
  virtual bool load( Gaudi::Details::PropertyBase& destination ) const override;


  /**
   * @brief Set the value of this Property from another.
   * @param destination The Property from which the value should be copied.
   *
   * The value object of this @c source is copied to that of this Property
   * by converting to a string and back again.  Returns true on success,
   * false on failure.
   */
  virtual bool assign( const Gaudi::Details::PropertyBase& source ) override;


  /**
   * @brief Return a string representation of the value object.
   */
  virtual std::string toString() const override;


  /**
   * @brief Write a string representation of the value object to a stream.
   * @param out Stream to which to write.
   */
  virtual void toStream(std::ostream& out) const override;


  /**
   * @brief Set this value object from a string.
   * @param s String from which to initialize the value.
   *
   * Returns failure if the conversion does not succeed.
   */
  virtual StatusCode fromString(const std::string& s) override;


  /**
   * @brief Return the value object for this Property.
   */
  const SG::VarHandleKey& value() const;


  /**
   * @brief Set the value object for this Property.
   * @param value Value from which to copy.
   *
   * Return true on success; false if the update handler failed.
   */
  bool setValue( const SG::VarHandleKey& value );


private:
  /** Pointer to the real property. Reference would be better, 
   *  but Reflex does not support references yet
   */
  SG::VarHandleKey* m_pValue;
};


} // namespace SG


// ** Specializations of SimplePropertyRef for the HandleKey classes.

namespace Gaudi {


template<>
class Property<::SG::VarHandleKey&> : public ::SG::VarHandleKeyProperty {
public:
  using ::SG::VarHandleKeyProperty::VarHandleKeyProperty;
};


template<typename T>
class Property<::SG::ReadHandleKey<T>&> : public ::SG::VarHandleKeyProperty {
public:
  using ::SG::VarHandleKeyProperty::VarHandleKeyProperty;
};


template<typename T>
class Property<::SG::WriteHandleKey<T>&> : public ::SG::VarHandleKeyProperty {
public:
  using ::SG::VarHandleKeyProperty::VarHandleKeyProperty;
};


template<typename T>
class Property<::SG::ReadDecorHandleKey<T>&> : public ::SG::VarHandleKeyProperty {
public:
  using ::SG::VarHandleKeyProperty::VarHandleKeyProperty;
};


template<typename T>
class Property<::SG::WriteDecorHandleKey<T>&> : public ::SG::VarHandleKeyProperty {
public:
  using ::SG::VarHandleKeyProperty::VarHandleKeyProperty;
};


template<typename T>
class Property<::SG::UpdateHandleKey<T>&> : public ::SG::VarHandleKeyProperty {
public:
  using ::SG::VarHandleKeyProperty::VarHandleKeyProperty;
};


template<typename T>
class Property<::SG::ReadCondHandleKey<T>&> : public ::SG::VarHandleKeyProperty {
public:
  using ::SG::VarHandleKeyProperty::VarHandleKeyProperty;
};


template<typename T>
class Property<::SG::WriteCondHandleKey<T>&> : public ::SG::VarHandleKeyProperty {
public:
  using ::SG::VarHandleKeyProperty::VarHandleKeyProperty;
};


} // namespace Gaudi


#endif // not STOREGATE_VARHANDLEKEYPROPERTY_H

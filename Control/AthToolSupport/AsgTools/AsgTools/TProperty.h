// TProperty.h

#ifndef asg_TProperty_H
#define asg_TProperty_H

// David Adams
// January 2014
//
// Support classe for PropertyMgr.

#include <string>
#include <vector>
#include "AsgTools/ToolHandle.h"
#include "AsgTools/ToolHandleArray.h"
#include "AsgTools/Property.h"

template<typename T>
class TProperty : public Property {

public:

  // Ctor from a reference.
  TProperty(const T& val, Type type);

  // Return address.
  const T* pointer() const;

  // Set value using that from another property.
  int setFrom(const Property& rhs);

private:

  const T* m_ptr;

};

//**********************************************************************

// Specializations for TProperty::setFrom.

// Special handling so float can be assigned from double.
template<>
int TProperty<float>::setFrom(const Property& rhs);

// Special handling so string can be assigned from string.
template<>
int TProperty<std::string>::setFrom(const Property& rhs);

// Speccial handling for arrays.
template<>
int TProperty<char*>::setFrom(const Property& rhs);

// Special handling for arrays.
template<>
int TProperty<char* const>::setFrom(const Property& rhs);

//**********************************************************************

// Property creator.
template<typename T>
Property* createProperty(const T& rval);

template<> Property* createProperty<bool>(const bool& rval);
template<> Property* createProperty<int>(const int& rval);
template<> Property* createProperty<float>(const float& rval);
template<> Property* createProperty<double>(const double& rval);
template<> Property* createProperty<std::string>(const std::string& rval);

// Handle string constants.
Property* createProperty(const char* const & val);

//**********************************************************************

template<typename T>
TProperty<T>::TProperty(const T& val, Type type) : Property(type), m_ptr(&val) { }

//**********************************************************************

template<typename T>
const T* TProperty<T>::pointer() const { return m_ptr; }

//**********************************************************************

template<typename T>
int TProperty<T>::setFrom(const Property& rhs) {
  if ( this->pointer() == 0 ) return 1;
  const TProperty<T>* pprop = dynamic_cast<const TProperty<T>*>(&rhs);
  if ( pprop == 0 ) return 1;
  T* ptr = const_cast<T*>(pointer());
  *ptr = *pprop->pointer();
  return 0;
}

//**********************************************************************

template<typename T>
Property* createProperty(const T& rval) {
  return new TProperty<T>(rval, Property::UNKNOWNTYPE);
}

//**********************************************************************

#endif

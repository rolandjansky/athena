// TProperty.cxx

#include "AsgTools/TProperty.h"

using std::string;
typedef Property::Type Type;
using asg::ToolHandle;
using asg::ToolHandleArray;

//**********************************************************************

template<>
Property* createProperty(const bool& rval) {
  return new TProperty<bool>(rval, Property::BOOL);
}

//**********************************************************************

template<>
Property* createProperty(const int& rval) {
  return new TProperty<int>(rval, Property::INT);
}

//**********************************************************************

template<>
Property* createProperty(const float& rval) {
  return new TProperty<float>(rval, Property::FLOAT);
}

//**********************************************************************

template<>
Property* createProperty(const double& rval) {
  return new TProperty<double>(rval, Property::DOUBLE);
}

//**********************************************************************

template<>
Property* createProperty(const string& rval) {
  return new TProperty<string>(rval, Property::STRING);
}

//**********************************************************************

Property* createProperty(const char* const & val) {
  return new TProperty<const char*>(val, Property::UNKNOWNTYPE);
}

//**********************************************************************

template<>
int TProperty<char*>::setFrom(const Property&) {
  return 2;
}

//**********************************************************************

template<>
int TProperty<char* const>::setFrom(const Property&) {
  return 2;
}

//**********************************************************************

template<>
int TProperty<float>::setFrom(const Property& rhs) {
  if ( this->pointer() == 0 ) return 1;
  const TProperty<float>* pprop = dynamic_cast<const TProperty<float>*>(&rhs);
  float* ptr = const_cast<float*>(pointer());
  if ( pprop == 0 ) {
    const TProperty<double>* pprop = dynamic_cast<const TProperty<double>*>(&rhs);
    if ( pprop == 0 ) {
      const TProperty<int>* pprop = dynamic_cast<const TProperty<int>*>(&rhs);
      if ( pprop == 0 ) return 1;
      *ptr = *pprop->pointer();
      return 0;
    }
    *ptr = *pprop->pointer();
    return 0;
  }
  *ptr = *pprop->pointer();
  return 0;
}


//**********************************************************************

template<>
int TProperty<string>::setFrom(const Property& rhs) {
  if ( this->pointer() == 0 ) return 1;
  const TProperty<string>* pprop = dynamic_cast<const TProperty<string>*>(&rhs);
  string* ptr = const_cast<string*>(pointer());
  if ( pprop == 0 ) {
    const TProperty<char*>* pprop = dynamic_cast<const TProperty<char*>*>(&rhs);
    if ( pprop == 0 ) {
      const TProperty<const char*>* pprop = dynamic_cast<const TProperty<const char*>*>(&rhs);
      if ( pprop == 0 ) return 1;
      *ptr = *pprop->pointer();
      return 0;
    }
    *ptr = *pprop->pointer();
    return 0;
  }
  *ptr = *pprop->pointer();
  return 0;
}


//**********************************************************************


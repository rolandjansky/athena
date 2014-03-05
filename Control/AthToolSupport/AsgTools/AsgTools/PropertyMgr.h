// PropertyMgr.h

#ifndef asg_PropertyMgr_H
#define asg_PropertyMgr_H

// David Adams
// January 2014
//
// Replacement for Gaudi property manager.

#include "AsgTools/AsgToolsConf.h"
#ifdef ASGTOOL_ATHENA
#include "GaudiKernel/PropertyMgr.h"
#else

#include <string>
#include <map>
#include <iostream>
#include "AsgTools/TProperty.h"

class PropertyMgr {

public:

  typedef Property::Type Type;

public:

  // Map of properties indexed by name.
  typedef std::map<std::string, Property*> PropMap;

  // Debug flag: set nonzero to get extra log messsges.
  static int dbg() { return 0; }

  // Dtor;
  ~PropertyMgr();

  // Declare a property, i.e. assign address and type to name.
  template<typename T>
  Property* declareProperty(const std::string& name, T& loc);

  // Set the value for a property.
  template<class T>
  void setProperty(const std::string& name, const T& val);

  // Retrieve a property.
  template<class T>
  void getProperty(const std::string& name, T& val) const;

  // Return the map of properties.
  const PropMap& getProperties() const;

private:

  PropMap m_props;

};

//**********************************************************************

template<typename T>
Property* PropertyMgr::declareProperty(const std::string& name, T& loc) {
  const std::string myname = "PropertyMgr::declareProperty: ";
  Property* pprop = createProperty(loc);
  if ( dbg() ) {
    std::cout << myname << "Declaring " << pprop->typeName()
              << " property: " << name << std::endl;
  }
  m_props[name] = pprop;
  return 0;
}

//**********************************************************************

template<class T>
void PropertyMgr::setProperty(const std::string& name, const T& rval) {
  const std::string myname = "PropertyMgr::setProperty: ";
  PropMap::const_iterator iprop = m_props.find(name);
  if ( iprop == m_props.end() ) {
    std::cout << myname << "Property not found: " << name << std::endl;
    return;
  }
  Property* pprop = iprop->second;
  Property* pinprop = createProperty(rval);
  if ( pprop->setFrom(*pinprop) ) {
    std::cout << myname << "Value assignment failed." << std::endl;
  }
}

//**********************************************************************

template<class T>
void PropertyMgr::getProperty(const std::string& name, T& valout) const {
  const std::string myname = "PropertyMgr::getProperty: ";
  PropMap::const_iterator iprop = m_props.find(name);
  if ( iprop == m_props.end() ) {
    std::cout << myname << "Property not found: " << name << std::endl;
    return;
  }
  Property* pprop = iprop->second;
  Property* pretprop = createProperty(valout);
  if ( pretprop->setFrom(*pprop) ) {
    std::cout << myname << "Value retrieval failed." << std::endl;
  }
}

//**********************************************************************

#endif
#endif

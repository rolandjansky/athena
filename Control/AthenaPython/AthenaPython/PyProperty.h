///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PyProperty.h 
// Header file for class PyProperty
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENAPYTHON_PYPROPERTY_H
#define ATHENAPYTHON_PYPROPERTY_H 1

// python includes
//#include "Python.h"

// STL includes
#include <typeinfo>

// Gaudi includes
#include "GaudiKernel/Property.h"

// Forward declaration
struct _object; 
typedef _object PyObject;


class PyProperty
  : public Property
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /// factory helper function
  template <typename Klass>
  static
  PyProperty* create (const std::string& name, Klass* o);

  /// Constructor with parameters: 
  PyProperty (const std::string& name,
	      PyObject* obj);

  /// Destructor: 
  virtual ~PyProperty(); 

  /// @c Property implementation
  ///@{
  /// export the property value to the destination
  virtual bool load (Property& dest) const;

  /// import the property value from source
  virtual bool assign (const Property& src);

  /// export the property value as a @c std::string
  virtual std::string toString() const;

  /// export the property value into a std::stream
  virtual void toStream(std::ostream& out) const;

  /// import the property value from a @c std::string
  virtual StatusCode fromString (const std::string& value);

  /// clone: the usual "virtual constructor" pattern
  virtual Property* clone() const;

  /// access underlying wrapped object
  virtual void* object() const;
  ///@}

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /// access underlying wrapped object as a python object
  /// @return a new reference
  PyObject* value() const;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Protected methods 
  /////////////////////////////////////////////////////////////////// 
private: 

  /// Default constructor: not implemented
  PyProperty();

  /// Copy constructor: 
  PyProperty( const PyProperty& rhs );

  /// Assignment operator: 
  PyProperty& operator=( const PyProperty& rhs ); 

  /// Constructor with parameters: 
  PyProperty (const std::string& name,
	      const std::type_info& type,
	      void* obj);

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
private: 

  /// the python object we are wrapping 
  /// (might be a genuine PyObject or a PyRoot one (ObjectProxy))
  PyObject *m_pyobj;
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
//std::ostream& operator<<( std::ostream& out, const PyProperty& o );

template <typename Klass>
inline
PyProperty* 
PyProperty::create (const std::string& name, Klass* o)
{
  return new PyProperty (name, typeid(o), (void*)o);
}

#endif //> !ATHENAPYTHON_PYPROPERTY_H

///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// VarHandleProperty.h 
// Header file for class VarHandleProperty
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef STOREGATE_VARHANDLEPROPERTY_H
#define STOREGATE_VARHANDLEPROPERTY_H 1

// stl
#include <iostream>

// framework includes
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Parsers.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/ToStream.h"

#include "StoreGate/RVar.h"
#include "StoreGate/WVar.h"
#include "StoreGate/RWVar.h"

namespace Gaudi { 
namespace Parsers {

GAUDI_API
StatusCode
parse(SG::VarHandleBase&, const std::string&);

} //> ns Parsers

namespace Utils {

GAUDI_API
std::ostream& 
toStream(const SG::VarHandleBase& v, std::ostream& o);

} //> ns Utils
} //> ns Gaudi



/** @brief VarHandleProperty is the class which wraps a @c SG::VarHandleBase
 *
 *  VarHandleProperty allows any SG::VarHandleBase instance (ie SG::RVar<T>,
 *  SG::WVar<T> and SG::RWVar<T>) to be modified at the job option level.
 */
class GAUDI_API VarHandleProperty : 
    public ::Property 
{
 public:

  /// Constructor with parameters: 
  VarHandleProperty( const std::string& name, SG::VarHandleBase& ref );

  /// Assignment operator: 
  VarHandleProperty& operator=( const SG::VarHandleBase& value );

  /// Destructor: 
  virtual ~VarHandleProperty();

  virtual VarHandleProperty* clone() const;
    
  virtual bool load( Property& destination ) const;

  virtual bool assign( const Property& source );

  virtual std::string toString() const;

  virtual void toStream(std::ostream& out) const;

  virtual StatusCode fromString(const std::string& s);

  const SG::VarHandleBase& value() const;

  bool setValue( const SG::VarHandleBase& value );

 private:
  /** Pointer to the real property. Reference would be better, 
   *  but Reflex does not support references yet
   */
  SG::VarHandleBase* m_pValue;
};

/** @brief Specialization for SG::RVar<T>
 *
 *  Needed drudgery for the @c PropertyMgr and @c Property classes so
 *  @c ::VarHandleBase (and derived) classes are known to the property handling
 *  infrastructure of Gaudi
 */
template<typename T>
class SimplePropertyRef< SG::RVar<T> > :
  public ::VarHandleProperty
{
public:

  /// Constructor from the name and the handle
  SimplePropertyRef(const std::string& name, SG::RVar<T>& value) :
    ::VarHandleProperty(name, value)
  {}

  /// virtual Destructor
  virtual ~SimplePropertyRef() {}
};


/** @brief Specialization for SG::RWVar<T>
 *
 *  Needed drudgery for the @c PropertyMgr and @c Property classes so
 *  @c ::VarHandleBase (and derived) classes are known to the property handling
 *  infrastructure of Gaudi
 */
template<typename T>
class SimplePropertyRef< SG::RWVar<T> > :
  public ::VarHandleProperty
{
public:

  /// Constructor from the name and the handle
  SimplePropertyRef( const std::string& name, SG::RWVar<T>& value ) :
    ::VarHandleProperty(name, value)
  {}

  /// virtual Destructor
  virtual ~SimplePropertyRef() {}
};

/** @brief Specialization for SG::WVar<T>
 *
 *  Needed drudgery for the @c PropertyMgr and @c Property classes so
 *  @c ::VarHandleBase (and derived) classes are known to the property handling
 *  infrastructure of Gaudi
 */
template<typename T>
class SimplePropertyRef< SG::WVar<T> > :
  public ::VarHandleProperty
{
public:

  /// Constructor from the name and the handle
  SimplePropertyRef( const std::string& name, SG::WVar<T>& value ) :
    ::VarHandleProperty(name, value)
  {}

  /// virtual Destructor
  virtual ~SimplePropertyRef() {}
};


inline
VarHandleProperty& 
VarHandleProperty::operator=( const SG::VarHandleBase& value ) 
{
  setValue( value );
  return *this;
}

inline
VarHandleProperty* 
VarHandleProperty::clone() const 
{
  return new VarHandleProperty( *this );
}

inline
bool
VarHandleProperty::load( Property& destination ) const 
{
  return destination.assign( *this );
}

inline
bool
VarHandleProperty::assign( const Property& source ) 
{
  return fromString( source.toString() ).isSuccess();
}

inline
const SG::VarHandleBase& 
VarHandleProperty::value() const 
{
  useReadHandler();
  return *m_pValue;
}

#endif /* !STOREGATE_VARHANDLEPROPERTY_H */

